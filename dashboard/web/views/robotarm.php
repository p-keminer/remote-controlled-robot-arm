            <!-- ========================================================
                 ROBOTARM VIEW - Live-Sensordaten, Statistiken, 3D-Modell
                 Modular: Dieses File kann entfernt werden ohne den Rest
                 des Dashboards zu beeinflussen.
                 ======================================================== -->
            <div id="view-robotarm" class="view-section">
                <div class="page-header">
                    <h1 class="page-title">Roboterarm Control</h1>
                    <p class="page-subtitle">Live-Sensordaten, Kalibrierung &amp; 3D-Visualisierung via MQTT</p>
                </div>

                <!-- MQTT Verbindungsstatus-Banner -->
                <div id="ra-mqtt-banner" class="alert alert-info" style="display:flex;align-items:center;gap:12px;">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M1 1l22 22"/><path d="M16.72 11.06A10.94 10.94 0 0 1 19 12.55"/><path d="M5 12.55a10.94 10.94 0 0 1 5.17-2.39"/><path d="M10.71 5.05A16 16 0 0 1 22.56 9"/><path d="M1.42 9a15.91 15.91 0 0 1 4.7-2.88"/><path d="M8.53 16.11a6 6 0 0 1 6.95 0"/><line x1="12" y1="20" x2="12.01" y2="20"/></svg>
                    <span id="ra-mqtt-status-text">MQTT: Verbinde...</span>
                </div>

                <!-- Notaus-Banner (nur sichtbar wenn aktiv) -->
                <div id="ra-notaus-banner" class="alert alert-danger" style="display:none;">
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15"/><line x1="9" y1="9" x2="15" y2="15"/></svg>
                    <strong>NOTAUS AKTIV</strong> — Bewegung blockiert. Toggle-Button am Controller druecken zum Aufheben.
                </div>

                <!-- Sub-Tab Navigation innerhalb Robotarm -->
                <div class="ra-sub-tabs">
                    <button class="ra-sub-tab active" data-click="raSubTab" data-arg="live">
                        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
                        Live-Daten
                    </button>
                    <button class="ra-sub-tab" data-click="raSubTab" data-arg="stats">
                        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><line x1="18" y1="20" x2="18" y2="10"/><line x1="12" y1="20" x2="12" y2="4"/><line x1="6" y1="20" x2="6" y2="14"/></svg>
                        Statistiken
                    </button>
                    <button class="ra-sub-tab" data-click="raSubTab" data-arg="3d">
                        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 2L2 7l10 5 10-5-10-5z"/><path d="M2 17l10 5 10-5"/><path d="M2 12l10 5 10-5"/></svg>
                        3D-Modell
                    </button>
                    <button class="ra-sub-tab" data-click="raSubTab" data-arg="console">
                        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="4 17 10 11 4 5"/><line x1="12" y1="19" x2="20" y2="19"/></svg>
                        Debug-Konsole
                    </button>
                </div>

                <!-- ====== SUB-VIEW: LIVE-DATEN ====== -->
                <div id="ra-panel-live" class="ra-panel">
                    <div class="grid">
                        <!-- Sensor 0: Handgelenk -->
                        <div class="card" id="ra-card-sensor0">
                            <div class="card-header">
                                <div class="card-title">
                                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 6v6l4 2"/></svg>
                                    IMU 0 — Handgelenk
                                </div>
                                <div id="ra-kalib-badge-0" class="status-badge badge-offline">--</div>
                            </div>
                            <div class="card-content">
                                <div class="info-row"><span class="info-label">Heading</span><span class="info-value" id="ra-s0-h">---</span></div>
                                <div class="info-row"><span class="info-label">Roll</span><span class="info-value" id="ra-s0-r">---</span></div>
                                <div class="info-row"><span class="info-label">Pitch</span><span class="info-value" id="ra-s0-p">---</span></div>
                            </div>
                            <div class="ra-kalib-grid">
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">SYS</span><span id="ra-k0-s" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">GYR</span><span id="ra-k0-g" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">ACC</span><span id="ra-k0-a" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">MAG</span><span id="ra-k0-m" class="ra-kalib-val">-</span></div>
                            </div>
                        </div>

                        <!-- Sensor 1: Unterarm -->
                        <div class="card" id="ra-card-sensor1">
                            <div class="card-header">
                                <div class="card-title">
                                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 6v6l4 2"/></svg>
                                    IMU 1 — Unterarm
                                </div>
                                <div id="ra-kalib-badge-1" class="status-badge badge-offline">--</div>
                            </div>
                            <div class="card-content">
                                <div class="info-row"><span class="info-label">Heading</span><span class="info-value" id="ra-s1-h">---</span></div>
                                <div class="info-row"><span class="info-label">Roll</span><span class="info-value" id="ra-s1-r">---</span></div>
                                <div class="info-row"><span class="info-label">Pitch</span><span class="info-value" id="ra-s1-p">---</span></div>
                            </div>
                            <div class="ra-kalib-grid">
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">SYS</span><span id="ra-k1-s" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">GYR</span><span id="ra-k1-g" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">ACC</span><span id="ra-k1-a" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">MAG</span><span id="ra-k1-m" class="ra-kalib-val">-</span></div>
                            </div>
                        </div>

                        <!-- Sensor 2: Oberarm -->
                        <div class="card" id="ra-card-sensor2">
                            <div class="card-header">
                                <div class="card-title">
                                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 6v6l4 2"/></svg>
                                    IMU 2 — Oberarm
                                </div>
                                <div id="ra-kalib-badge-2" class="status-badge badge-offline">--</div>
                            </div>
                            <div class="card-content">
                                <div class="info-row"><span class="info-label">Heading</span><span class="info-value" id="ra-s2-h">---</span></div>
                                <div class="info-row"><span class="info-label">Roll</span><span class="info-value" id="ra-s2-r">---</span></div>
                                <div class="info-row"><span class="info-label">Pitch</span><span class="info-value" id="ra-s2-p">---</span></div>
                            </div>
                            <div class="ra-kalib-grid">
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">SYS</span><span id="ra-k2-s" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">GYR</span><span id="ra-k2-g" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">ACC</span><span id="ra-k2-a" class="ra-kalib-val">-</span></div>
                                <div class="ra-kalib-cell"><span class="ra-kalib-label">MAG</span><span id="ra-k2-m" class="ra-kalib-val">-</span></div>
                            </div>
                        </div>

                        <!-- Flex-Sensor + System -->
                        <div class="card">
                            <div class="card-header">
                                <div class="card-title">
                                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18 20V10"/><path d="M12 20V4"/><path d="M6 20v-6"/></svg>
                                    Flex-Sensor &amp; System
                                </div>
                            </div>
                            <div class="card-content">
                                <div class="info-row"><span class="info-label">Flex</span><span class="info-value" id="ra-flex">--- %</span></div>
                                <div class="info-row"><span class="info-label">Paketzaehler</span><span class="info-value" id="ra-counter">---</span></div>
                                <div class="info-row"><span class="info-label">Protokoll</span><span class="info-value" id="ra-proto">---</span></div>
                                <div class="info-row"><span class="info-label">Flags</span><span class="info-value" id="ra-flags">---</span></div>
                                <div class="info-row"><span class="info-label">Notaus</span><span class="info-value" id="ra-notaus">---</span></div>
                            </div>
                            <!-- Flex-Sensor Balken -->
                            <div class="ra-flex-bar-wrap">
                                <div class="ra-flex-bar" id="ra-flex-bar" style="width:0%"></div>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- ====== SUB-VIEW: STATISTIKEN ====== -->
                <div id="ra-panel-stats" class="ra-panel" style="display:none;">
                    <div class="stats-grid">
                        <div class="stat-box">
                            <div class="stat-label">Pakete/Sek</div>
                            <div class="stat-value" id="ra-stat-pps" style="color:var(--accent-green);">---</div>
                        </div>
                        <div class="stat-box">
                            <div class="stat-label">Pakete Gesamt</div>
                            <div class="stat-value" id="ra-stat-total">---</div>
                        </div>
                        <div class="stat-box">
                            <div class="stat-label">Fehler Gesamt</div>
                            <div class="stat-value" id="ra-stat-errors" style="color:var(--accent-red);">---</div>
                        </div>
                        <div class="stat-box" style="border-left-color:var(--accent-yellow);">
                            <div class="stat-label">Fehlerrate</div>
                            <div class="stat-value" id="ra-stat-errorrate" style="color:var(--accent-yellow);">---</div>
                        </div>
                    </div>
                    <div class="stats-grid" style="margin-bottom:24px;">
                        <div class="stat-box" style="border-left-color:var(--accent-purple);">
                            <div class="stat-label">WiFi RSSI</div>
                            <div class="stat-value" id="ra-stat-rssi" style="color:var(--accent-purple);">---</div>
                        </div>
                        <div class="stat-box" style="border-left-color:var(--accent-green);">
                            <div class="stat-label">Bridge Uptime</div>
                            <div class="stat-value" id="ra-stat-uptime" style="color:var(--accent-green);font-size:1.5rem;">---</div>
                        </div>
                        <div class="stat-box">
                            <div class="stat-label">MQTT Nachrichten</div>
                            <div class="stat-value" id="ra-stat-mqttcount">0</div>
                        </div>
                        <div class="stat-box" style="border-left-color:var(--accent-green);">
                            <div class="stat-label">Notaus</div>
                            <div class="stat-value" id="ra-stat-notaus" style="color:var(--accent-green);">AUS</div>
                        </div>
                    </div>
                    <!-- PPS Chart -->
                    <div class="card" style="margin-bottom:24px;">
                        <h3 style="margin-bottom:16px;display:flex;align-items:center;gap:8px;">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
                            Paketrate (Pakete/Sek)
                        </h3>
                        <div class="chart-container"><canvas id="ra-chart-pps"></canvas></div>
                    </div>
                    <!-- Sensor Heading Chart -->
                    <div class="card">
                        <h3 style="margin-bottom:16px;display:flex;align-items:center;gap:8px;">
                            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 6v6l4 2"/></svg>
                            Sensor Headings (Live)
                        </h3>
                        <div class="chart-container"><canvas id="ra-chart-heading"></canvas></div>
                    </div>
                </div>

                <!-- ====== SUB-VIEW: 3D-MODELL ====== -->
                <div id="ra-panel-3d" class="ra-panel" style="display:none;">
                    <div class="card" style="padding:0;overflow:hidden;">
                        <div id="ra-3d-container" style="width:100%;height:600px;position:relative;background:#0a0e1a;">
                            <div id="ra-3d-overlay" style="position:absolute;top:16px;left:16px;z-index:10;font-size:13px;color:var(--text-secondary);pointer-events:none;">
                                Mausrad = Zoom | Linksklick+Ziehen = Drehen | Rechtsklick+Ziehen = Verschieben
                            </div>
                            <div style="position:absolute;top:12px;right:12px;z-index:10;display:flex;gap:8px;">
                                <button id="ra-3d-toggle-mount" data-click="raToggleMount" class="btn btn-sm" style="background:var(--bg-secondary);border:1px solid var(--border-color);color:var(--text-primary);padding:6px 12px;border-radius:6px;font-size:12px;cursor:pointer;display:flex;align-items:center;gap:6px;">
                                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 3h18v18H3z"/><path d="M12 8v8"/><path d="M8 12h8"/></svg>
                                    Wandmontage
                                </button>
                                <button id="ra-3d-reset-cam" data-click="raResetCamera" class="btn btn-sm" style="background:var(--bg-secondary);border:1px solid var(--border-color);color:var(--text-primary);padding:6px 12px;border-radius:6px;font-size:12px;cursor:pointer;display:flex;align-items:center;gap:6px;">
                                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M1 4v6h6"/><path d="M3.51 15a9 9 0 1 0 2.13-9.36L1 10"/></svg>
                                    Kamera Reset
                                </button>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- ====== SUB-VIEW: DEBUG-KONSOLE ====== -->
                <div id="ra-panel-console" class="ra-panel" style="display:none;">
                    <div class="card">
                        <div class="terminal-header">
                            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="16" height="16"><polyline points="4 17 10 11 4 5"/><line x1="12" y1="19" x2="20" y2="19"/></svg>
                            MQTT Live-Stream (robotarm/#)
                        </div>
                        <div id="ra-console" class="terminal" style="height:500px;font-size:12px;"></div>
                        <div class="terminal-actions" style="display:flex;gap:8px;">
                            <button class="btn btn-danger" data-click="raClearConsole" style="flex:1;">
                                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="16" height="16"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/></svg>
                                Clear
                            </button>
                            <label class="switch" style="display:flex;align-items:center;gap:8px;padding:10px 16px;background:var(--bg-tertiary);border:1px solid var(--border-color);border-radius:8px;">
                                <input type="checkbox" id="ra-console-pause">
                                <span class="slider"></span>
                                <span style="font-size:13px;color:var(--text-secondary);white-space:nowrap;">Pause</span>
                            </label>
                        </div>
                    </div>
                </div>
            </div>
