// ============================================================
// js/robotarm.js — Roboterarm MQTT Dashboard Modul
// ============================================================
// Verbindet sich per MQTT-over-WebSocket (via Nginx WSS Proxy)
// mit dem Mosquitto Broker und zeigt Live-Sensordaten an.
//
// Abhaengigkeiten:
//   - mqtt.js (global: window.mqtt)
//   - Chart.js (global: window.Chart)
//   - PHP-Variablen: MQTT_WS_USER, MQTT_WS_PASS, MQTT_WS_TOPICS
//
// Alle DOM-IDs beginnen mit "ra-" (robotarm) um Kollisionen
// mit dem bestehenden Dashboard zu vermeiden.
// ============================================================

(function() {
    'use strict';

    // ============================================================
    // MQTT CLIENT
    // ============================================================

    var client = null;
    var mqttConnected = false;
    var mqttMsgCount  = 0;
    var reconnectTimer = null;

    // Letzte empfangene Daten (fuer 3D-Modul)
    window._raLastImu    = null;
    window._raLastStatus = null;
    window._raLastKalib  = null;

    /**
     * MQTT-Verbindung aufbauen.
     * Wird nur aufgerufen wenn der Robotarm-Tab aktiv ist.
     */
    function mqttConnect() {
        if (client && mqttConnected) return;
        if (typeof window.mqtt === 'undefined') {
            console.warn('[RA-MQTT] mqtt.js nicht geladen');
            raSetBanner('error', 'MQTT-Bibliothek nicht geladen');
            return;
        }

        // WebSocket-URL: wss://host/mqtt (Nginx Reverse Proxy → Mosquitto:9001)
        var proto = (location.protocol === 'https:') ? 'wss' : 'ws';
        var wsUrl = proto + '://' + location.host + '/mqtt';

        console.log('[RA-MQTT] Verbinde mit ' + wsUrl);
        raSetBanner('connecting', 'MQTT: Verbinde mit ' + location.host + '...');

        try {
            client = window.mqtt.connect(wsUrl, {
                username: MQTT_WS_USER,
                password: MQTT_WS_PASS,
                clientId: 'dashboard_' + Math.random().toString(36).substring(2, 8),
                clean: true,
                reconnectPeriod: 5000,
                connectTimeout: 10000
            });
        } catch (e) {
            console.error('[RA-MQTT] Verbindungsfehler:', e);
            raSetBanner('error', 'MQTT: Verbindungsfehler — ' + e.message);
            return;
        }

        client.on('connect', function() {
            mqttConnected = true;
            console.log('[RA-MQTT] Verbunden');
            raSetBanner('online', 'MQTT: Verbunden');

            // Alle Topics subscriben
            MQTT_WS_TOPICS.forEach(function(topic) {
                client.subscribe(topic, {qos: 0}, function(err) {
                    if (err) {
                        console.error('[RA-MQTT] Subscribe-Fehler:', topic, err);
                    } else {
                        console.log('[RA-MQTT] Subscribt auf:', topic);
                    }
                });
            });
        });

        client.on('message', function(topic, message) {
            mqttMsgCount++;
            var payload = message.toString();
            var data;

            try {
                data = JSON.parse(payload);
            } catch (e) {
                raConsoleLog(topic, payload, true);
                return;
            }

            // Dispatch nach Topic
            if (topic === 'robotarm/imu') {
                onImuData(data);
            } else if (topic === 'robotarm/status') {
                onStatusData(data);
            } else if (topic === 'robotarm/kalib') {
                onKalibData(data);
            }

            // Debug-Konsole
            raConsoleLog(topic, payload, false);
        });

        client.on('error', function(err) {
            console.error('[RA-MQTT] Fehler:', err);
            raSetBanner('error', 'MQTT-Fehler: ' + (err.message || err));
        });

        client.on('close', function() {
            mqttConnected = false;
            raSetBanner('offline', 'MQTT: Getrennt — Reconnect...');
        });

        client.on('offline', function() {
            mqttConnected = false;
            raSetBanner('offline', 'MQTT: Offline');
        });
    }

    function mqttDisconnect() {
        if (client) {
            client.end(true);
            client = null;
            mqttConnected = false;
        }
    }

    // ============================================================
    // BANNER-STATUS
    // ============================================================

    function raSetBanner(state, text) {
        var banner = document.getElementById('ra-mqtt-banner');
        var label  = document.getElementById('ra-mqtt-status-text');
        if (!banner || !label) return;

        label.textContent = text;
        banner.className = 'alert';
        if (state === 'online')       banner.className += ' alert-success';
        else if (state === 'error')   banner.className += ' alert-danger';
        else if (state === 'offline') banner.className += ' alert-danger';
        else                          banner.className += ' alert-info';

        // MQTT-Nachrichtenzaehler aktualisieren
        var el = document.getElementById('ra-stat-mqttcount');
        if (el) el.textContent = mqttMsgCount;
    }

    // ============================================================
    // IMU-DATEN VERARBEITEN (robotarm/imu, 20Hz)
    // ============================================================

    var imuThrottle = 0;
    var IMU_THROTTLE_MS = 100; // UI max 10Hz updaten (20Hz Daten sind zu schnell)

    function onImuData(d) {
        window._raLastImu = d;

        var now = Date.now();
        if (now - imuThrottle < IMU_THROTTLE_MS) return;
        imuThrottle = now;

        // 3 Sensoren updaten
        for (var i = 0; i < 3; i++) {
            if (d.s && d.s[i]) {
                setText('ra-s' + i + '-h', d.s[i].h.toFixed(1) + '\u00B0');
                setText('ra-s' + i + '-r', d.s[i].r.toFixed(1) + '\u00B0');
                setText('ra-s' + i + '-p', d.s[i].p.toFixed(1) + '\u00B0');
            }
            if (d.k && d.k[i]) {
                setKalib(i, d.k[i]);
            }
        }

        // Flex-Sensor
        if (typeof d.f === 'number') {
            setText('ra-flex', d.f.toFixed(1) + ' %');
            var bar = document.getElementById('ra-flex-bar');
            if (bar) bar.style.width = Math.max(0, Math.min(100, d.f)) + '%';
        }

        // System-Felder
        if (typeof d.z === 'number') setText('ra-counter', d.z);
        if (typeof d.v === 'number') setText('ra-proto', 'v' + d.v);
        if (typeof d.fl === 'number') setText('ra-flags', '0x' + d.fl.toString(16).toUpperCase().padStart(2, '0'));

        // Notaus
        var notaus = d.notaus === true;
        setText('ra-notaus', notaus ? 'AKTIV' : 'aus');
        var notausEl = document.getElementById('ra-notaus');
        if (notausEl) notausEl.style.color = notaus ? 'var(--accent-red)' : 'var(--accent-green)';

        // Notaus-Banner
        var banner = document.getElementById('ra-notaus-banner');
        if (banner) banner.style.display = notaus ? 'flex' : 'none';

        // Stats Notaus
        var statNotaus = document.getElementById('ra-stat-notaus');
        if (statNotaus) {
            statNotaus.textContent = notaus ? 'AKTIV' : 'AUS';
            statNotaus.style.color = notaus ? 'var(--accent-red)' : 'var(--accent-green)';
        }

        // Heading-Chart updaten
        if (d.s && d.s.length >= 3) {
            pushHeadingData(d.s[0].h, d.s[1].h, d.s[2].h);
        }
    }

    // ============================================================
    // STATUS-DATEN VERARBEITEN (robotarm/status, 1Hz)
    // ============================================================

    function onStatusData(d) {
        window._raLastStatus = d;

        setText('ra-stat-pps', typeof d.pps === 'number' ? d.pps.toFixed(1) : '---');
        setText('ra-stat-total', typeof d.pakete_gesamt === 'number' ? d.pakete_gesamt.toLocaleString() : '---');
        setText('ra-stat-errors', typeof d.fehler_gesamt === 'number' ? d.fehler_gesamt.toLocaleString() : '---');
        setText('ra-stat-errorrate', typeof d.fehlerrate === 'number' ? (d.fehlerrate * 100).toFixed(2) + ' %' : '---');
        setText('ra-stat-rssi', typeof d.wifi_rssi === 'number' ? d.wifi_rssi + ' dBm' : '---');

        if (typeof d.uptime_s === 'number') {
            var h = Math.floor(d.uptime_s / 3600);
            var m = Math.floor((d.uptime_s % 3600) / 60);
            var s = d.uptime_s % 60;
            setText('ra-stat-uptime', h + ':' + pad2(m) + ':' + pad2(s));
        }

        setText('ra-stat-mqttcount', mqttMsgCount);

        // PPS-Chart updaten
        if (typeof d.pps === 'number') {
            pushPpsData(d.pps);
        }
    }

    // ============================================================
    // KALIBRIERUNGS-DATEN (robotarm/kalib, on-change)
    // ============================================================

    function onKalibData(d) {
        window._raLastKalib = d;
        if (!Array.isArray(d)) return;
        for (var i = 0; i < Math.min(d.length, 3); i++) {
            setKalib(i, d[i]);
        }
    }

    function setKalib(idx, k) {
        if (!k) return;
        setKalibVal('ra-k' + idx + '-s', k.s);
        setKalibVal('ra-k' + idx + '-g', k.g);
        setKalibVal('ra-k' + idx + '-a', k.a);
        setKalibVal('ra-k' + idx + '-m', k.m);

        // Badge: Kalibrierungsqualitaet
        var badge = document.getElementById('ra-kalib-badge-' + idx);
        if (badge) {
            var total = (k.s || 0) + (k.g || 0) + (k.a || 0) + (k.m || 0);
            if (total >= 10) {
                badge.className = 'status-badge badge-good';
                badge.textContent = 'Kalibriert';
            } else if (total >= 5) {
                badge.className = 'status-badge badge-warning';
                badge.textContent = 'Teilweise';
            } else {
                badge.className = 'status-badge badge-critical';
                badge.textContent = 'Unkalibriert';
            }
        }
    }

    function setKalibVal(id, val) {
        var el = document.getElementById(id);
        if (!el) return;
        el.textContent = (typeof val === 'number') ? val : '-';
        if (typeof val === 'number') {
            el.style.color = val === 3 ? 'var(--accent-green)'
                           : val >= 1 ? 'var(--accent-yellow)'
                           :            'var(--accent-red)';
        }
    }

    // ============================================================
    // CHARTS (Chart.js)
    // ============================================================

    var ppsChart = null;
    var headingChart = null;
    var MAX_CHART_POINTS = 60;

    function initCharts() {
        if (typeof Chart === 'undefined') {
            console.warn('[RA] Chart.js nicht geladen, Charts deaktiviert');
            return;
        }

        // PPS Chart
        var ppsCtx = document.getElementById('ra-chart-pps');
        if (ppsCtx && !ppsChart) {
            ppsChart = new Chart(ppsCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'PPS',
                        data: [],
                        borderColor: '#10b981',
                        backgroundColor: 'rgba(16, 185, 129, 0.1)',
                        borderWidth: 2,
                        fill: true,
                        tension: 0.3,
                        pointRadius: 0
                    }]
                },
                options: chartOptions('Pakete/Sek', 0, 25)
            });
        }

        // Heading Chart
        var headCtx = document.getElementById('ra-chart-heading');
        if (headCtx && !headingChart) {
            headingChart = new Chart(headCtx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        {label: 'IMU 0', data: [], borderColor: '#3b82f6', borderWidth: 2, pointRadius: 0, tension: 0.3},
                        {label: 'IMU 1', data: [], borderColor: '#f59e0b', borderWidth: 2, pointRadius: 0, tension: 0.3},
                        {label: 'IMU 2', data: [], borderColor: '#8b5cf6', borderWidth: 2, pointRadius: 0, tension: 0.3}
                    ]
                },
                options: chartOptions('Heading (\u00B0)', 0, 360)
            });
        }
    }

    function chartOptions(yLabel, sugMin, sugMax) {
        return {
            responsive: true,
            maintainAspectRatio: false,
            animation: {duration: 0},
            scales: {
                x: {
                    display: true,
                    ticks: {color: '#64748b', maxTicksLimit: 8},
                    grid: {color: 'rgba(255,255,255,0.05)'}
                },
                y: {
                    display: true,
                    suggestedMin: sugMin,
                    suggestedMax: sugMax,
                    ticks: {color: '#64748b'},
                    grid: {color: 'rgba(255,255,255,0.05)'}
                }
            },
            plugins: {
                legend: {labels: {color: '#94a3b8'}},
                tooltip: {mode: 'index', intersect: false}
            }
        };
    }

    function pushPpsData(val) {
        if (!ppsChart) return;
        var ts = new Date().toLocaleTimeString('de-DE', {hour: '2-digit', minute: '2-digit', second: '2-digit'});
        ppsChart.data.labels.push(ts);
        ppsChart.data.datasets[0].data.push(val);
        if (ppsChart.data.labels.length > MAX_CHART_POINTS) {
            ppsChart.data.labels.shift();
            ppsChart.data.datasets[0].data.shift();
        }
        ppsChart.update('none');
    }

    function pushHeadingData(h0, h1, h2) {
        if (!headingChart) return;
        var ts = new Date().toLocaleTimeString('de-DE', {hour: '2-digit', minute: '2-digit', second: '2-digit'});
        headingChart.data.labels.push(ts);
        headingChart.data.datasets[0].data.push(h0);
        headingChart.data.datasets[1].data.push(h1);
        headingChart.data.datasets[2].data.push(h2);
        if (headingChart.data.labels.length > MAX_CHART_POINTS) {
            headingChart.data.labels.shift();
            headingChart.data.datasets.forEach(function(ds) { ds.data.shift(); });
        }
        headingChart.update('none');
    }

    // ============================================================
    // DEBUG-KONSOLE
    // ============================================================

    var consoleMaxLines = 500;

    function raConsoleLog(topic, payload, isError) {
        var paused = document.getElementById('ra-console-pause');
        if (paused && paused.checked) return;

        var term = document.getElementById('ra-console');
        if (!term) return;

        var ts = new Date().toLocaleTimeString('de-DE', {hour: '2-digit', minute: '2-digit', second: '2-digit', fractionalSecondDigits: 3});
        var line = document.createElement('div');
        line.className = 'log-line';

        var shortTopic = topic.replace('robotarm/', '');
        var color = isError ? 'var(--accent-red)' : 'var(--accent-green)';

        // Payload kuerzen wenn zu lang
        var display = payload.length > 200 ? payload.substring(0, 200) + '...' : payload;

        line.innerHTML = '<span style="color:var(--text-tertiary)">' + escHtml(ts) + '</span> '
            + '<span style="color:var(--accent-blue);font-weight:600">[' + escHtml(shortTopic) + ']</span> '
            + '<span style="color:' + color + '">' + escHtml(display) + '</span>';

        term.appendChild(line);

        // Scroll nach unten
        term.scrollTop = term.scrollHeight;

        // Alte Zeilen entfernen
        while (term.children.length > consoleMaxLines) {
            term.removeChild(term.firstChild);
        }
    }

    // ============================================================
    // SUB-TAB NAVIGATION
    // ============================================================

    function raSubTab(tabName) {
        var panels = ['live', 'stats', '3d', 'console'];
        panels.forEach(function(p) {
            var panel = document.getElementById('ra-panel-' + p);
            if (panel) panel.style.display = (p === tabName) ? 'block' : 'none';
        });

        // Sub-Tab Buttons aktualisieren
        var buttons = document.querySelectorAll('.ra-sub-tab');
        buttons.forEach(function(btn) {
            btn.classList.toggle('active', btn.dataset.arg === tabName);
        });

        // Charts initialisieren wenn Stats angezeigt werden
        if (tabName === 'stats') initCharts();

        // 3D initialisieren wenn Tab zum ersten Mal geoeffnet wird
        if (tabName === '3d' && typeof window.raInit3D === 'function') {
            window.raInit3D();
        }
    }

    // ============================================================
    // ROBOTARM TAB LIFECYCLE
    // ============================================================

    /**
     * Wird von app.js aufgerufen wenn der Robotarm-Tab aktiviert wird.
     * Startet die MQTT-Verbindung.
     */
    window.raOnTabActivate = function() {
        mqttConnect();
    };

    /**
     * Wird aufgerufen wenn der Tab verlassen wird.
     * MQTT bleibt verbunden (Reconnect-Overhead vermeiden).
     */
    window.raOnTabDeactivate = function() {
        // MQTT nicht trennen — nur UI-Updates stoppen
        // Das spart den erneuten Handshake beim Zurueckwechseln
    };

    // ============================================================
    // EVENT-HANDLER (via app.js Dispatcher)
    // ============================================================

    window.raSubTab = raSubTab;

    window.raClearConsole = function() {
        var term = document.getElementById('ra-console');
        if (term) term.innerHTML = '';
    };

    window.raToggleMount = function() {
        if (typeof window.raToggleWallMount === 'function') {
            window.raToggleWallMount();
        }
    };

    window.raResetCamera = function() {
        if (typeof window.raResetCam === 'function') {
            window.raResetCam();
        }
    };

    // ============================================================
    // HILFSFUNKTIONEN
    // ============================================================

    function setText(id, text) {
        var el = document.getElementById(id);
        if (el) el.textContent = text;
    }

    function pad2(n) { return n < 10 ? '0' + n : '' + n; }

    function escHtml(s) {
        var div = document.createElement('div');
        div.textContent = s;
        return div.innerHTML;
    }

    // ============================================================
    // INIT
    // ============================================================

    console.log('[RA] Robotarm-Modul geladen. MQTT-User:', typeof MQTT_WS_USER !== 'undefined' ? MQTT_WS_USER : 'NICHT GESETZT');

})();
