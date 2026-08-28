#!/usr/bin/env python3
"""
MQTT MCP Server — Live-Zugriff auf Roboterarm-Sensordaten fuer Claude Code.

Tools:
  mqtt_subscribe    — Subscribt auf ein Topic und gibt N Nachrichten zurueck
  mqtt_read_latest  — Gibt die letzte retained Message eines Topics zurueck
  mqtt_list_topics  — Listet alle aktiven Topics unter robotarm/#
  mqtt_publish      — Publisht eine Nachricht auf ein Topic (nur fuer Tests)
  mqtt_status       — Zeigt Verbindungsstatus und Statistiken

Konfiguration:
  mqtt_config.local.py (gitignoriert) mit Broker-Adresse und Credentials.
  Kopiervorlage: mqtt_config.template.py
"""

import asyncio
import json
import os
import sys
import time
import threading
from collections import defaultdict

import paho.mqtt.client as mqtt
from mcp.server.fastmcp import FastMCP

# ============================================================
# Konfiguration laden
# ============================================================

SKRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# Versuche lokale Config zu laden
config_path = os.path.join(SKRIPT_DIR, "mqtt_config.local.py")
if not os.path.exists(config_path):
    print(f"FEHLER: {config_path} nicht gefunden.", file=sys.stderr)
    print("Kopiere mqtt_config.template.py als mqtt_config.local.py und passe die Werte an.", file=sys.stderr)
    sys.exit(1)

# Config als Modul laden
config = {}
with open(config_path) as f:
    exec(f.read(), config)

MQTT_BROKER = config.get("MQTT_BROKER", "localhost")
MQTT_PORT = config.get("MQTT_PORT", 1883)
MQTT_USER = config.get("MQTT_USER", "mcp_reader")
MQTT_PASSWORT = config.get("MQTT_PASSWORT", "")
MQTT_TOPICS = config.get("MQTT_TOPICS", ["robotarm/#"])

# ============================================================
# MQTT Client (laeuft in eigenem Thread)
# ============================================================

class MqttBridge:
    def __init__(self):
        self.client = mqtt.Client(
            client_id="claude-mcp-reader",
            protocol=mqtt.MQTTv311,
            callback_api_version=mqtt.CallbackAPIVersion.VERSION2
        )
        self.client.username_pw_set(MQTT_USER, MQTT_PASSWORT)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.on_disconnect = self._on_disconnect

        # Nachrichten-Speicher (thread-safe via Lock)
        self.lock = threading.Lock()
        self.latest = {}          # topic -> letzte Nachricht
        self.history = defaultdict(list)  # topic -> liste der letzten N
        self.max_history = 100
        self.msg_count = 0
        self.connected = False
        self.connect_time = None
        self.last_msg_time = None

        # Warteschlange fuer subscribe-Tool
        self.waiters = []  # list of (topic_filter, asyncio.Queue)

    def start(self):
        try:
            self.client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
            self.client.loop_start()
        except Exception as e:
            print(f"MQTT Verbindungsfehler: {e}", file=sys.stderr)

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()

    def _on_connect(self, client, userdata, flags, reason_code, properties=None):
        self.connected = True
        self.connect_time = time.time()
        print(f"[MQTT] Verbunden mit {MQTT_BROKER}:{MQTT_PORT}", file=sys.stderr)
        for topic in MQTT_TOPICS:
            client.subscribe(topic)
            print(f"[MQTT] Subscribt auf: {topic}", file=sys.stderr)

    def _on_disconnect(self, client, userdata, flags, reason_code, properties=None):
        self.connected = False
        print(f"[MQTT] Getrennt (rc={reason_code})", file=sys.stderr)

    def _on_message(self, client, userdata, msg):
        payload = msg.payload.decode("utf-8", errors="replace")
        timestamp = time.time()

        with self.lock:
            self.latest[msg.topic] = {
                "topic": msg.topic,
                "payload": payload,
                "timestamp": timestamp,
                "retained": msg.retain
            }
            self.history[msg.topic].append({
                "payload": payload,
                "timestamp": timestamp
            })
            # History begrenzen
            if len(self.history[msg.topic]) > self.max_history:
                self.history[msg.topic] = self.history[msg.topic][-self.max_history:]
            self.msg_count += 1
            self.last_msg_time = timestamp

        # Waiter benachrichtigen
        for topic_filter, queue in self.waiters:
            if mqtt.topic_matches_sub(topic_filter, msg.topic):
                try:
                    queue.put_nowait({
                        "topic": msg.topic,
                        "payload": payload,
                        "timestamp": timestamp
                    })
                except asyncio.QueueFull:
                    pass

    def get_latest(self, topic):
        with self.lock:
            return self.latest.get(topic)

    def get_all_latest(self):
        with self.lock:
            return dict(self.latest)

    def get_topics(self):
        with self.lock:
            return list(self.latest.keys())

    def get_history(self, topic, count=10):
        with self.lock:
            return self.history.get(topic, [])[-count:]

    def get_stats(self):
        with self.lock:
            return {
                "connected": self.connected,
                "broker": f"{MQTT_BROKER}:{MQTT_PORT}",
                "user": MQTT_USER,
                "subscriptions": MQTT_TOPICS,
                "topics_aktiv": len(self.latest),
                "nachrichten_gesamt": self.msg_count,
                "uptime_s": int(time.time() - self.connect_time) if self.connect_time else 0,
                "letzte_nachricht_vor_s": round(time.time() - self.last_msg_time, 1) if self.last_msg_time else None
            }

# ============================================================
# MCP Server
# ============================================================

bridge = MqttBridge()
mcp = FastMCP("mqtt-robotarm")

@mcp.tool()
async def mqtt_subscribe(topic: str = "robotarm/#", count: int = 5, timeout_s: int = 10) -> str:
    """Subscribt auf ein MQTT-Topic und wartet auf N Nachrichten.

    Args:
        topic: MQTT-Topic (z.B. 'robotarm/imu', 'robotarm/#' fuer alle)
        count: Anzahl Nachrichten die gesammelt werden sollen
        timeout_s: Maximale Wartezeit in Sekunden
    """
    if not bridge.connected:
        return json.dumps({"error": "MQTT nicht verbunden"})

    queue = asyncio.Queue(maxsize=count + 10)
    bridge.waiters.append((topic, queue))

    messages = []
    try:
        deadline = time.time() + timeout_s
        while len(messages) < count and time.time() < deadline:
            remaining = deadline - time.time()
            if remaining <= 0:
                break
            try:
                msg = await asyncio.wait_for(
                    asyncio.get_event_loop().run_in_executor(None, queue.get, True, min(remaining, 1.0)),
                    timeout=min(remaining, 1.5)
                )
                messages.append(msg)
            except (asyncio.TimeoutError, Exception):
                continue
    finally:
        bridge.waiters.remove((topic, queue))

    return json.dumps({
        "topic_filter": topic,
        "empfangen": len(messages),
        "angefragt": count,
        "nachrichten": messages
    }, indent=2)


@mcp.tool()
async def mqtt_read_latest(topic: str = "robotarm/imu") -> str:
    """Gibt die letzte empfangene Nachricht eines Topics zurueck.

    Args:
        topic: Exaktes MQTT-Topic (z.B. 'robotarm/imu', 'robotarm/status')
    """
    if not bridge.connected:
        return json.dumps({"error": "MQTT nicht verbunden"})

    msg = bridge.get_latest(topic)
    if msg is None:
        return json.dumps({"error": f"Kein Wert fuer Topic '{topic}' vorhanden"})

    # Versuche JSON zu parsen fuer bessere Lesbarkeit
    try:
        msg["payload_parsed"] = json.loads(msg["payload"])
    except (json.JSONDecodeError, TypeError):
        pass

    msg["alter_s"] = round(time.time() - msg["timestamp"], 1)
    return json.dumps(msg, indent=2)


@mcp.tool()
async def mqtt_list_topics() -> str:
    """Listet alle aktiven MQTT-Topics die seit Serverstart empfangen wurden."""
    if not bridge.connected:
        return json.dumps({"error": "MQTT nicht verbunden"})

    topics = bridge.get_topics()
    result = {}
    for t in sorted(topics):
        latest = bridge.get_latest(t)
        if latest:
            result[t] = {
                "alter_s": round(time.time() - latest["timestamp"], 1),
                "retained": latest.get("retained", False),
                "payload_vorschau": latest["payload"][:100]
            }

    return json.dumps({
        "topics_aktiv": len(result),
        "topics": result
    }, indent=2)


@mcp.tool()
async def mqtt_history(topic: str = "robotarm/imu", count: int = 10) -> str:
    """Gibt die letzten N Nachrichten eines Topics zurueck (aus dem lokalen Puffer).

    Args:
        topic: Exaktes MQTT-Topic
        count: Anzahl Nachrichten (max 100)
    """
    if not bridge.connected:
        return json.dumps({"error": "MQTT nicht verbunden"})

    count = min(count, 100)
    history = bridge.get_history(topic, count)

    # JSON parsen wo moeglich
    for msg in history:
        try:
            msg["payload_parsed"] = json.loads(msg["payload"])
        except (json.JSONDecodeError, TypeError):
            pass

    return json.dumps({
        "topic": topic,
        "anzahl": len(history),
        "nachrichten": history
    }, indent=2)


@mcp.tool()
async def mqtt_status() -> str:
    """Zeigt MQTT-Verbindungsstatus und Statistiken."""
    return json.dumps(bridge.get_stats(), indent=2)


@mcp.tool()
async def mqtt_publish(topic: str, message: str, retain: bool = False) -> str:
    """Publisht eine Nachricht auf ein MQTT-Topic (nur fuer Tests/Debugging).

    Args:
        topic: MQTT-Topic (z.B. 'robotarm/test')
        message: Nachricht als String
        retain: Ob die Nachricht retained werden soll
    """
    if not bridge.connected:
        return json.dumps({"error": "MQTT nicht verbunden"})

    # Sicherheit: nur robotarm/ Topics erlauben
    if not topic.startswith("robotarm/"):
        return json.dumps({"error": "Nur Topics unter 'robotarm/' erlaubt"})

    result = bridge.client.publish(topic, message, retain=retain)
    return json.dumps({
        "topic": topic,
        "message": message,
        "retain": retain,
        "rc": result.rc,
        "success": result.rc == 0
    })


# ============================================================
# Main
# ============================================================

def main():
    bridge.start()
    try:
        mcp.run(transport="stdio")
    finally:
        bridge.stop()

if __name__ == "__main__":
    main()
