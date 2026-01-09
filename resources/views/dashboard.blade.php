<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="csrf-token" content="{{ csrf_token() }}">
    <meta http-equiv="refresh" content="30">
    <title>Smart Energy Dashboard - Server Side</title>
    <link rel="stylesheet" href="{{ asset('css/style.css') }}">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        /* Realtime indicator styles */
        .realtime-indicator {
            display: inline-flex;
            align-items: center;
            gap: 5px;
            font-size: 12px;
            color: #28a745;
            margin-left: 10px;
        }
        .realtime-dot {
            width: 8px;
            height: 8px;
            background: #28a745;
            border-radius: 50%;
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.5; }
            100% { opacity: 1; }
        }
        .data-timestamp {
            font-size: 11px;
            color: #666;
            margin-top: 5px;
        }
        .connection-status {
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }
        .connection-status.online {
            background: #d4edda;
            color: #155724;
        }
        .connection-status.offline {
            background: #f8d7da;
            color: #721c24;
        }
        .sensor-card.updating {
            border: 2px solid #28a745;
            box-shadow: 0 0 10px rgba(40, 167, 69, 0.3);
        }

        /* AC Control Styles */
        .ac-control-container {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-top: 10px;
        }

        .control-mode-selector {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
        }

        .mode-btn {
            padding: 10px 20px;
            border: 2px solid #ddd;
            background: white;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: bold;
        }

        .mode-btn.active {
            background: #007bff;
            color: white;
            border-color: #007bff;
        }

        .mode-btn:hover {
            background: #e9ecef;
        }

        .mode-btn.active:hover {
            background: #0056b3;
        }

        .manual-controls {
            background: white;
            border-radius: 8px;
            padding: 20px;
            border: 1px solid #ddd;
        }

        .ac-units-control {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-bottom: 20px;
        }

        .ac-unit-control {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 15px;
            background: #f8f9fa;
        }

        .ac-unit-control h4 {
            margin: 0 0 15px 0;
            color: #333;
        }

        .unit-controls {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }

        .power-control, .temp-control {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .power-control label, .temp-control label {
            font-weight: bold;
            min-width: 60px;
        }

        .toggle-switch {
            position: relative;
            width: 50px;
            height: 25px;
            background: #ccc;
            border-radius: 25px;
            cursor: pointer;
            transition: background 0.3s;
        }

        .toggle-switch.active {
            background: #28a745;
        }

        .toggle-slider {
            position: absolute;
            top: 2px;
            left: 2px;
            width: 21px;
            height: 21px;
            background: white;
            border-radius: 50%;
            transition: transform 0.3s;
        }

        .toggle-switch.active .toggle-slider {
            transform: translateX(25px);
        }

        .temp-buttons {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .temp-buttons button {
            width: 30px;
            height: 30px;
            border: 1px solid #ddd;
            background: white;
            border-radius: 5px;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .temp-buttons button:hover {
            background: #e9ecef;
        }

        .temp-buttons span {
            font-weight: bold;
            min-width: 30px;
            text-align: center;
        }

        .control-actions {
            border-top: 1px solid #ddd;
            padding-top: 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 15px;
        }

        .duration-control {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .duration-control select {
            padding: 8px 12px;
            border: 1px solid #ddd;
            border-radius: 5px;
        }

        .action-buttons {
            display: flex;
            gap: 10px;
        }

        .apply-btn, .emergency-btn {
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-weight: bold;
            transition: all 0.3s;
        }

        .apply-btn {
            background: #28a745;
            color: white;
        }

        .apply-btn:hover {
            background: #218838;
        }

        .emergency-btn {
            background: #dc3545;
            color: white;
        }

        .emergency-btn:hover {
            background: #c82333;
        }

        .auto-mode-info {
            background: white;
            border-radius: 8px;
            padding: 20px;
            border: 1px solid #ddd;
        }

        .info-card {
            display: flex;
            gap: 15px;
            align-items: flex-start;
        }

        .info-card i {
            font-size: 24px;
            color: #17a2b8;
            margin-top: 5px;
        }

        .info-card h4 {
            margin: 0 0 10px 0;
            color: #333;
        }

        .info-card p {
            margin: 0 0 10px 0;
            color: #666;
        }

        .info-card ul {
            margin: 0;
            padding-left: 20px;
            color: #666;
        }

        .current-control-status {
            margin-top: 20px;
            padding: 15px;
            background: white;
            border-radius: 8px;
            border: 1px solid #ddd;
        }

        .current-control-status h4 {
            margin: 0 0 10px 0;
            color: #333;
        }

        .status-display {
            padding: 10px;
            background: #f8f9fa;
            border-radius: 5px;
            font-family: monospace;
            color: #495057;
        }

        @media (max-width: 768px) {
            .ac-units-control {
                grid-template-columns: 1fr;
            }
            
            .control-actions {
                flex-direction: column;
                align-items: stretch;
            }
            
            .action-buttons {
                justify-content: center;
            }
        }

        /* History Section Styles - Consistent with existing template */
        .history-tabs {
            display: flex;
            gap: 5px;
            margin-bottom: 20px;
            border-bottom: 2px solid #e9ecef;
            overflow-x: auto;
            background: #f8f9fa;
            padding: 10px;
            border-radius: 8px;
        }

        .tab-button {
            padding: 12px 20px;
            border: none;
            background: transparent;
            cursor: pointer;
            border-bottom: 3px solid transparent;
            transition: all 0.3s;
            white-space: nowrap;
            font-weight: 500;
            border-radius: 5px;
        }

        .tab-button:hover {
            background: #e9ecef;
            color: #007bff;
        }

        .tab-button.active {
            color: #007bff;
            border-bottom-color: #007bff;
            background: white;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }

        .history-table-container {
            background: white;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            overflow: hidden;
            margin-top: 20px;
        }

        .table-responsive {
            overflow-x: auto;
        }

        .history-table {
            width: 100%;
            border-collapse: collapse;
            font-size: 14px;
        }

        .history-table th {
            background: #f8f9fa;
            padding: 12px 8px;
            text-align: left;
            font-weight: 600;
            color: #333;
            border-bottom: 2px solid #e9ecef;
            white-space: nowrap;
        }

        .history-table td {
            padding: 10px 8px;
            border-bottom: 1px solid #e9ecef;
            vertical-align: middle;
        }

        .history-table tr:hover {
            background: #f8f9fa;
        }

        .loading-row,
        .no-data,
        .error-row {
            text-align: center;
            padding: 30px;
            color: #666;
            font-style: italic;
        }

        .error-row {
            color: #dc3545;
        }

        .device-badge {
            background: #e9ecef;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
            color: #495057;
        }

        .people-count {
            background: #007bff;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-weight: bold;
            font-size: 12px;
        }

        .ac-status.status-on {
            background: #28a745;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .ac-status.status-off {
            background: #6c757d;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .mode-badge {
            background: #17a2b8;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .change-indicator.increase {
            color: #28a745;
            font-weight: bold;
        }

        .change-indicator.decrease {
            color: #dc3545;
            font-weight: bold;
        }

        .change-indicator.same {
            color: #6c757d;
        }

        .ac-response {
            background: #ffc107;
            color: #212529;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .condition-badge {
            background: #17a2b8;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .event-type-badge {
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
            color: white;
        }

        .event-type-badge.sensor_update {
            background: #007bff;
        }

        .event-type-badge.ac_control {
            background: #28a745;
        }

        .event-type-badge.connection {
            background: #17a2b8;
        }

        .event-type-badge.system_error {
            background: #dc3545;
        }

        .event-type-badge.manual_control {
            background: #ffc107;
            color: #212529;
        }

        .status-badge.status-success {
            background: #28a745;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .status-badge.status-error {
            background: #dc3545;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .status-badge.status-info {
            background: #17a2b8;
            color: white;
            padding: 2px 8px;
            border-radius: 12px;
            font-size: 11px;
            font-weight: bold;
        }

        .table-pagination {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 15px 20px;
            background: #f8f9fa;
            border-top: 1px solid #e9ecef;
        }

        .form-control {
            width: 100%;
            padding: 8px 12px;
            border: 1px solid #ddd;
            border-radius: 5px;
            font-size: 14px;
            background: white;
        }

        .btn-refresh,
        .btn-export,
        .btn-search {
            padding: 8px 15px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            transition: all 0.3s;
            width: 100%;
            margin-top: 5px;
        }

        .btn-refresh {
            background: #17a2b8;
            color: white;
        }

        .btn-refresh:hover {
            background: #138496;
        }

        .btn-export {
            background: #28a745;
            color: white;
        }

        .btn-export:hover {
            background: #218838;
        }

        .btn-search {
            background: #007bff;
            color: white;
        }

        .btn-search:hover {
            background: #0056b3;
        }

        .chart-tab-btn {
            width: 100%;
            padding: 15px;
            border: none;
            background: transparent;
            border-radius: 8px;
            cursor: pointer;
            transition: all 0.3s;
            font-size: 14px;
            font-weight: 500;
        }

        .chart-tab-btn:hover {
            background: #f8f9fa;
            color: #007bff;
        }

        .chart-tab-btn.active {
            background: #007bff;
            color: white;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }

        .chart-container {
            position: relative;
            height: 300px;
            margin-top: 20px;
            background: white;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }

        @media (max-width: 768px) {
            .history-tabs {
                flex-direction: column;
            }
            
            .tab-button {
                text-align: center;
            }
        }
    </style>
</head>

<body>

    <!-- ===== SIDEBAR ===== -->
    <div class="sidebar" id="sidebar">
        <!-- Toggle Button -->
        <div class="sidebar-toggle" onclick="toggleSidebar()">
            <i class="fas fa-bars" id="toggleIcon"></i>
        </div>
        
        <div class="sidebar-content">
            <h2><i class="fas fa-home"></i> <span class="sidebar-text">Smart Energy</span></h2>
            <ul>
                <li><a href="#" onclick="showSection('Kontrol', this)" class="active" title="Monitoring"><i class="fas fa-desktop"></i> <span class="sidebar-text">Monitoring</span></a></li>
                <li><a href="#" onclick="showSection('analytics', this)" title="Analisa"><i class="fas fa-chart-line"></i> <span class="sidebar-text">Analisa</span></a></li>
                <li><a href="#" onclick="showSection('history', this)" title="History"><i class="fas fa-history"></i> <span class="sidebar-text">History</span></a></li>
                <li><a href="#" onclick="showSection('reports', this)" title="Laporan"><i class="fas fa-file-alt"></i> <span class="sidebar-text">Laporan</span></a></li>
               
            </ul>
            
            <div class="sidebar-footer">
                <div class="user-info">
                    <i class="fas fa-user-circle"></i>
                    <span class="sidebar-text">Admin User</span>
                </div>
                <div class="system-status">
                    <i class="fas fa-circle status-online"></i>
                    <span class="sidebar-text">System Online</span>
                </div>
            </div>
        </div>
    </div>

    <!-- ===== MAIN CONTENT ===== -->
    <div class="main-content">

        <!-- ===== Monitoring Section ===== -->
        <section id="Kontrol">
            <div class="navbar">
                <i class="fas fa-building"></i> {{ $sensorData['location'] ?? 'Ruang Dosen Prodi Teknik Elektro' }}
                @if($sensorData['connection_status'] === 'online')
                    <span class="realtime-indicator">
                        <div class="realtime-dot"></div>
                        ONLINE
                    </span>
                @endif
            </div>
            <div class="subnav" id="waktuRealtime">
                <i class="fas fa-clock"></i> {{ now()->format('l, d F Y - H:i:s') }} WIB
            </div>

            <!-- Connection Status -->
            <div class="sensor-section">
                <div class="sensor-grid" style="margin-bottom: 20px;">
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-wifi"></i>
                        </div>
                        <div class="title">Status Koneksi ESP32</div>
                        <div class="value">
                            <span class="connection-status {{ $sensorData['connection_status'] }}">
                                {{ $sensorData['status'] === 'active' ? 'Online' : 'Offline' }}
                            </span>
                        </div>
                        <div class="data-timestamp">
                            @if($sensorData['connection_status'] === 'online')
                                Terakhir update: {{ $sensorData['last_update_time'] }}
                            @else
                                Data terakhir: {{ $sensorData['time_ago'] ?? '' }}
                            @endif
                        </div>
                    </div>
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-microchip"></i>
                        </div>
                        <div class="title">Device Info</div>
                        <div class="value">{{ $sensorData['device_id'] ?? 'ESP32_Smart_Energy' }}</div>
                        <div class="data-timestamp">{{ $sensorData['location'] ?? 'Lab Teknik Tegangan Tinggi' }}</div>
                    </div>
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-signal"></i>
                        </div>
                        <div class="title">WiFi Signal</div>
                        <div class="value">
                            @if($sensorData['wifi_rssi'])
                                {{ $sensorData['wifi_rssi'] }} dBm
                            @else
                                -- dBm
                            @endif
                        </div>
                        <div class="data-timestamp">{{ $sensorData['wifi_quality'] ?? '--' }}</div>
                    </div>
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-database"></i>
                        </div>
                        <div class="title">Total Data Records</div>
                        <div class="value">{{ number_format($totalRecords) }}</div>
                        <div class="data-timestamp">Data tersimpan di database</div>
                    </div>
                </div>
            </div>

            <!-- Sensor Cards -->
            <div class="sensor-section">
                <h3><i class="fas fa-chart-line"></i> Data Sensor dari ESP32</h3>
                <div class="sensor-grid">
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-thermometer-half"></i>
                        </div>
                        <div class="title">Suhu Ruangan</div>
                        <div class="value">
                            @if($sensorData['temperature'])
                                {{ number_format($sensorData['temperature'], 1) }}°C
                            @else
                                --°C
                            @endif
                        </div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-tint"></i>
                        </div>
                        <div class="title">Kelembaban Ruangan</div>
                        <div class="value">
                            @if($sensorData['humidity'])
                                {{ number_format($sensorData['humidity'], 1) }}%
                            @else
                                --%
                            @endif
                        </div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-users"></i>
                        </div>
                        <div class="title">Jumlah Orang di Ruangan</div>
                        <div class="value">{{ $sensorData['people_count'] ?? 0 }}</div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-sun"></i>
                        </div>
                        <div class="title">Intensitas Cahaya</div>
                        <div class="value">
                            @if($sensorData['light_intensity'])
                                {{ number_format($sensorData['light_intensity']) }} lux
                            @else
                                -- lux
                            @endif
                        </div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>
                </div>
            </div>

            <!-- AC Status from ESP32 -->
            <div class="sensor-section">
                <h3><i class="fas fa-snowflake"></i> Status AC dari ESP32</h3>
                <div class="sensor-grid">
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-snowflake"></i>
                        </div>
                        <div class="title">Status AC</div>
                        <div class="value">{{ $sensorData['ac_status'] ?? 'OFF' }}</div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-temperature-low"></i>
                        </div>
                        <div class="title">Set Temperature AC</div>
                        <div class="value">
                            @if($sensorData['set_temperature'])
                                {{ $sensorData['set_temperature'] }}°C
                            @else
                                --°C
                            @endif
                        </div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-walking"></i>
                        </div>
                        <div class="title">Sensor Deteksi Orang</div>
                        <div class="value">
                            IN: {{ $sensorData['proximity_in'] ? 'AKTIF' : 'OFF' }} | 
                            OUT: {{ $sensorData['proximity_out'] ? 'AKTIF' : 'OFF' }}
                        </div>
                        <div class="data-timestamp">{{ $sensorData['last_update_time'] ?? 'Belum ada data' }}</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-clock"></i>
                        </div>
                        <div class="title">Update Frequency</div>
                        <div class="value">
                            @if($updateFrequency)
                                {{ $updateFrequency }} detik
                            @else
                                -- detik
                            @endif
                        </div>
                        <div class="data-timestamp">Interval update data</div>
                    </div>
                </div>
            </div>

            <!-- Manual AC Control Section -->
            <div class="sensor-section">
                <h3><i class="fas fa-hand-paper"></i> Kontrol AC Manual</h3>
                <div class="ac-control-container">
                    <div class="control-mode-selector">
                        <button id="autoModeBtn" class="mode-btn active" onclick="setControlMode('auto')">
                            <i class="fas fa-robot"></i> Mode Otomatis
                        </button>
                        <button id="manualModeBtn" class="mode-btn" onclick="setControlMode('manual')">
                            <i class="fas fa-hand-paper"></i> Mode Manual
                        </button>
                    </div>

                    <div id="manualControls" class="manual-controls" style="display: none;">
                        <div class="ac-units-control">
                            <div class="ac-unit-control">
                                <h4><i class="fas fa-snowflake"></i> AC Unit 1</h4>
                                <div class="unit-controls">
                                    <div class="power-control">
                                        <label>Status:</label>
                                        <div class="toggle-switch" id="ac1Toggle" onclick="toggleAC(1)">
                                            <span class="toggle-slider"></span>
                                        </div>
                                        <span id="ac1Status">OFF</span>
                                    </div>
                                    <div class="temp-control">
                                        <label>Suhu:</label>
                                        <div class="temp-buttons">
                                            <button onclick="changeTemp(1, -1)"><i class="fas fa-minus"></i></button>
                                            <span id="ac1Temp">25</span>°C
                                            <button onclick="changeTemp(1, 1)"><i class="fas fa-plus"></i></button>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            <div class="ac-unit-control">
                                <h4><i class="fas fa-snowflake"></i> AC Unit 2</h4>
                                <div class="unit-controls">
                                    <div class="power-control">
                                        <label>Status:</label>
                                        <div class="toggle-switch" id="ac2Toggle" onclick="toggleAC(2)">
                                            <span class="toggle-slider"></span>
                                        </div>
                                        <span id="ac2Status">OFF</span>
                                    </div>
                                    <div class="temp-control">
                                        <label>Suhu:</label>
                                        <div class="temp-buttons">
                                            <button onclick="changeTemp(2, -1)"><i class="fas fa-minus"></i></button>
                                            <span id="ac2Temp">25</span>°C
                                            <button onclick="changeTemp(2, 1)"><i class="fas fa-plus"></i></button>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>

                        <div class="control-actions">
                            <div class="duration-control">
                                <label>Durasi Kontrol Manual:</label>
                                <select id="controlDuration">
                                    <option value="">Permanen</option>
                                    <option value="15">15 Menit</option>
                                    <option value="30">30 Menit</option>
                                    <option value="60">1 Jam</option>
                                    <option value="120">2 Jam</option>
                                    <option value="240">4 Jam</option>
                                </select>
                            </div>
                            
                            <div class="action-buttons">
                                <button class="apply-btn" onclick="applyACControl()">
                                    <i class="fas fa-check"></i> Terapkan Pengaturan
                                </button>
                                <button class="emergency-btn" onclick="emergencyStopAC()">
                                    <i class="fas fa-stop"></i> Emergency Stop
                                </button>
                            </div>
                        </div>
                    </div>

                    <div id="autoModeInfo" class="auto-mode-info">
                        <div class="info-card">
                            <i class="fas fa-info-circle"></i>
                            <div>
                                <h4>Mode Otomatis Aktif</h4>
                                <p>Arduino mengontrol AC berdasarkan jumlah orang di ruangan</p>
                                <ul>
                                    <li>0 orang: AC OFF</li>
                                    <li>1-5 orang: 1 AC ON (25°C)</li>
                                    <li>6-10 orang: 1 AC ON (22°C)</li>
                                    <li>11-15 orang: 2 AC ON (22°C)</li>
                                    <li>16+ orang: 2 AC ON (20°C)</li>
                                </ul>
                            </div>
                        </div>
                    </div>

                    <div class="current-control-status">
                        <h4><i class="fas fa-info"></i> Status Kontrol Saat Ini</h4>
                        <div id="controlStatus" class="status-display">
                            Mode: Otomatis | Tidak ada kontrol manual aktif
                        </div>
                    </div>
                </div>
            </div>

            <!-- Data Summary -->
            <div class="sensor-section">
                <h3><i class="fas fa-info-circle"></i> Ringkasan Data</h3>
                <div class="sensor-grid">
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-calendar-day"></i>
                        </div>
                        <div class="title">Data Hari Ini</div>
                        <div class="value">
                            @if(isset($dailyStats['today_count']))
                                {{ number_format($dailyStats['today_count']) }}
                            @else
                                0
                            @endif
                        </div>
                        <div class="data-timestamp">Record data sensor</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-chart-line"></i>
                        </div>
                        <div class="title">Rata-rata Suhu Hari Ini</div>
                        <div class="value">
                            @if(isset($dailyStats['avg_temperature']))
                                {{ number_format($dailyStats['avg_temperature'], 1) }}°C
                            @else
                                --°C
                            @endif
                        </div>
                        <div class="data-timestamp">Suhu rata-rata</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-users"></i>
                        </div>
                        <div class="title">Max Orang Hari Ini</div>
                        <div class="value">
                            @if(isset($dailyStats['max_people']))
                                {{ $dailyStats['max_people'] }}
                            @else
                                0
                            @endif
                        </div>
                        <div class="data-timestamp">Jumlah maksimum</div>
                    </div>

                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-clock"></i>
                        </div>
                        <div class="title">Data Age</div>
                        <div class="value">
                            @if($sensorData['data_age_minutes'] <= 2)
                                <span style="color: #28a745;">Fresh</span>
                            @elseif($sensorData['data_age_minutes'] <= 10)
                                <span style="color: #ffc107;">{{ $sensorData['data_age_minutes'] }} min</span>
                            @else
                                <span style="color: #dc3545;">{{ $sensorData['data_age_minutes'] }} min</span>
                            @endif
                        </div>
                        <div class="data-timestamp">{{ $sensorData['time_ago'] ?? '' }}</div>
                    </div>
                </div>
            </div>

            <!-- Status Cards -->
            <div class="status-section">
                <h3><i class="fas fa-info-circle"></i> Status Sistem</h3>
                <div class="status-grid">
                    <div class="card status-card">
                        <div class="card-icon">
                            <i class="fas fa-clock"></i>
                        </div>
                        <div class="title">Status Jam Kerja</div>
                        <div id="workingHoursStatus" class="status-off">Di Luar Jam Kerja</div>
                        <div class="work-hours-info">
                            <div id="workingHoursRange">07:00 - 17:00 WIB</div>
                            <div id="workingDayInfo">Senin - Jumat</div>
                            <div id="currentTimeDisplay">--:-- (---)</div>
                        </div>
                    </div>

                    <div class="card status-card">
                        <div class="card-icon">
                            <i class="fas fa-leaf"></i>
                        </div>
                        <div class="title">Energi yang Di Hemat</div>
                        <div class="value" id="energySave">0%</div>
                    </div>

                    <div class="card status-card">
                        <div class="card-icon">
                            <i class="fas fa-chart-pie"></i>
                        </div>
                        <div class="title">Pemakaian Hari Ini</div>
                        <div class="daily-usage">
                            <div class="usage-item">
                                <span>Total Perangkat:</span>
                                <span id="totalDevicesOn">0</span>
                            </div>
                            <div class="usage-item">
                                <span>Total Energi:</span>
                                <span id="dailyTotalEnergy">0 Wh</span>
                            </div>
                        </div>
                    </div>

                    <div class="card status-card">
                        <div class="card-icon">
                            <i class="fas fa-shield-alt"></i>
                        </div>
                        <div class="title">Auto Shutdown</div>
                        <div id="autoShutdownStatus" class="status-on">AKTIF</div>
                        <div class="shutdown-info">
                            <div class="usage-item">
                                <span>Waktu Tanpa Orang:</span>
                                <span id="noPersonTime">0 menit</span>
                            </div>
                            <div class="usage-item">
                                <span>Status:</span>
                                <span id="shutdownCountdown">Normal</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Device Control Section -->
            <div class="device-section">
                <h3><i class="fas fa-microchip"></i> Kontrol Perangkat Listrik</h3>
                <div class="device-grid">
                    
                    <!-- AC Panasonic -->
                    <div class="card device-card ac-card">
                        <div class="card-icon">
                            <i class="fas fa-snowflake"></i>
                        </div>
                        <div class="title">AC Panasonic CS-PN12WKJ (2 Unit)</div>
                        <div class="device-info">
                            <span>Daya: 1050W per unit | Total Konsumsi: 10.8 kWh</span>
                        </div>
                        
                        <!-- Status kedua AC -->
                        <div class="ac-units-status">
                            <div class="ac-unit">
                                <span>AC Unit 1:</span>
                                <div id="ac1Status" class="mini-status status-off">MATI</div>
                                <button onclick="toggleACUnit(1)" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                            <div class="ac-unit">
                                <span>AC Unit 2:</span>
                                <div id="ac2Status" class="mini-status status-off">MATI</div>
                                <button onclick="toggleACUnit(2)" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>
                        
                        <!-- Status keseluruhan -->
                        <div id="acStatus" class="status-off">SEMUA MATI</div>
                        
                        <!-- Kontrol keseluruhan -->
                        <button onclick="toggleAllAC()"><i class="fas fa-power-off"></i> KONTROL SEMUA AC</button>
                        
                        <div class="device-controls">
                            <div class="title" style="margin-top:10px;">Suhu Setting</div>
                            <div class="value" id="acTempValue">24 °C</div>
                            <div class="temp-controls">
                                <button onclick="tempUp()" class="temp-btn"><i class="fas fa-plus"></i></button>
                                <button onclick="tempDown()" class="temp-btn"><i class="fas fa-minus"></i></button>
                            </div>
                            <div class="gradual-controls" style="margin-top: 10px;">
                                <button onclick="startACGradualMode()" class="btn-small" style="font-size: 11px;">
                                    <i class="fas fa-thermometer-half"></i> Mode Gradual
                                </button>
                            </div>
                        </div>
                    </div>

                    <!-- Lampu TL -->
                    <div class="card device-card lamp-card">
                        <div class="card-icon">
                            <i class="fas fa-lightbulb"></i>
                        </div>
                        <div class="title">Lampu TL (12 Unit - 2 Jalur)</div>
                        <div class="device-info">
                            <span>Daya: 22W per unit | Total Konsumsi: 2.112 kWh</span>
                        </div>
                        
                        <!-- Status kedua jalur lampu -->
                        <div class="lamp-circuits-status">
                            <div class="lamp-circuit">
                                <span>Jalur 1 (6 Unit):</span>
                                <div id="lamp1Status" class="mini-status status-off">MATI</div>
                                <button onclick="toggleLampCircuit(1)" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                            <div class="lamp-circuit">
                                <span>Jalur 2 (6 Unit):</span>
                                <div id="lamp2Status" class="mini-status status-off">MATI</div>
                                <button onclick="toggleLampCircuit(2)" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>
                        
                        <!-- Status keseluruhan -->
                        <div id="lampStatus" class="status-off">SEMUA MATI</div>
                        
                        <!-- Kontrol keseluruhan -->
                        <button onclick="toggleAllLamps()"><i class="fas fa-power-off"></i> KONTROL SEMUA LAMPU</button>
                    </div>

                    <!-- Panel Listrik -->
                    <div class="card device-card">
                        <div class="card-icon">
                            <i class="fas fa-bolt"></i>
                        </div>
                        <div class="title">Panel Listrik (1 Unit)</div>
                        <div class="device-info">
                            <span>Daya: 300W | Konsumsi: 0.3 kWh</span>
                        </div>
                        <div id="panelStatus" class="status-on">MENYALA</div>
                        <button onclick="toggleDevice('panel')"><i class="fas fa-power-off"></i> ON / OFF</button>
                    </div>

                </div>
            </div>
        </section>

        <!-- ===== Analisa Section (dari Analytics) ===== -->
        <section id="analytics" style="display:none;">
            <header>
                <h1><i class="fas fa-chart-bar"></i> Analisa</h1>
            </header>
            <div class="chart-container">
                <canvas id="myChart"></canvas>
            </div>
        </section>

        <!-- ===== History Section ===== -->
        <section id="history" style="display:none;">
            <div class="navbar">
                <i class="fas fa-history"></i> Riwayat Data Sensor & Aktivitas Sistem
                <span class="realtime-indicator">
                    <div class="realtime-dot"></div>
                    HISTORY
                </span>
            </div>
            <div class="subnav">
                <i class="fas fa-database"></i> Riwayat lengkap data sensor ESP32, aktivitas AC, dan perubahan sistem Smart Energy UNJA
            </div>

           

            <!-- Summary Cards -->
            <div class="sensor-section">
                <h3><i class="fas fa-chart-bar"></i> Ringkasan Data</h3>
                <div class="sensor-grid">
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-database"></i>
                        </div>
                        <div class="title">Total Records</div>
                        <div class="value" id="totalRecords">{{ number_format($totalRecords) }}</div>
                        <div class="data-timestamp">Data tersimpan</div>
                    </div>
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-clock"></i>
                        </div>
                        <div class="title">Data Terakhir</div>
                        <div class="value" id="lastUpdate">{{ $sensorData['last_update_time'] ?? '--' }}</div>
                        <div class="data-timestamp">{{ $sensorData['time_ago'] ?? 'Belum ada data' }}</div>
                    </div>
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-wifi"></i>
                        </div>
                        <div class="title">Status Koneksi</div>
                        <div class="value">
                            <span class="connection-status {{ $sensorData['connection_status'] ?? 'offline' }}">
                                {{ $sensorData['status'] === 'active' ? 'Online' : 'Offline' }}
                            </span>
                        </div>
                        <div class="data-timestamp">ESP32 Connection</div>
                    </div>
                    <div class="card sensor-card">
                        <div class="card-icon">
                            <i class="fas fa-chart-line"></i>
                        </div>
                        <div class="title">Update Frequency</div>
                        <div class="value" id="updateFreq">
                            @if($updateFrequency)
                                {{ $updateFrequency }}s
                            @else
                                --s
                            @endif
                        </div>
                        <div class="data-timestamp">Interval rata-rata</div>
                    </div>
                </div>
            </div>

            <!-- Tab Navigation -->
            <div class="sensor-section">
                <h3><i class="fas fa-table"></i> Data History</h3>
                <div class="history-tabs">
                    <button class="tab-button active" onclick="showHistoryTab('sensor-data')">
                        <i class="fas fa-chart-line"></i> Data Sensor
                    </button>
                    <button class="tab-button" onclick="showHistoryTab('ac-activity')">
                        <i class="fas fa-snowflake"></i> Aktivitas AC
                    </button>
                    <button class="tab-button" onclick="showHistoryTab('people-count')">
                        <i class="fas fa-users"></i> Jumlah Orang
                    </button>
                    <button class="tab-button" onclick="showHistoryTab('environment')">
                        <i class="fas fa-thermometer-half"></i> Lingkungan
                    </button>
                    <button class="tab-button" onclick="showHistoryTab('system-events')">
                        <i class="fas fa-cog"></i> System Events
                    </button>
                </div>

                <!-- Data Sensor Tab -->
                <div id="sensor-data-table" class="history-table-container">
                    <div class="table-responsive">
                        <table class="history-table">
                            <thead>
                                <tr>
                                    <th><i class="fas fa-hashtag"></i> ID</th>
                                    <th><i class="fas fa-clock"></i> Timestamp</th>
                                    <th><i class="fas fa-microchip"></i> Device</th>
                                    <th><i class="fas fa-users"></i> Orang</th>
                                    <th><i class="fas fa-thermometer-half"></i> Suhu</th>
                                    <th><i class="fas fa-tint"></i> Kelembaban</th>
                                    <th><i class="fas fa-sun"></i> Cahaya</th>
                                    <th><i class="fas fa-snowflake"></i> AC Status</th>
                                    <th><i class="fas fa-temperature-low"></i> Set Temp</th>
                                    <th><i class="fas fa-walking"></i> Proximity</th>
                                    <th><i class="fas fa-signal"></i> WiFi</th>
                                </tr>
                            </thead>
                            <tbody id="sensor-data-body">
                                <tr>
                                    <td colspan="11" class="loading-row">
                                        <i class="fas fa-spinner fa-spin"></i> Loading data sensor...
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    <div class="table-pagination">
                        <button onclick="previousPage('sensor')" id="sensorPrevBtn" class="btn-small" disabled>
                            <i class="fas fa-chevron-left"></i> Previous
                        </button>
                        <span id="sensorPageInfo">Page 1 of 1</span>
                        <button onclick="nextPage('sensor')" id="sensorNextBtn" class="btn-small" disabled>
                            Next <i class="fas fa-chevron-right"></i>
                        </button>
                    </div>
                </div>

                <!-- AC Activity Tab -->
                <div id="ac-activity-table" class="history-table-container" style="display:none;">
                    <div class="sensor-grid" style="margin-bottom: 20px;">
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-snowflake"></i>
                            </div>
                            <div class="title">Total AC Events</div>
                            <div class="value" id="totalACEvents">--</div>
                            <div class="data-timestamp">Perubahan status AC</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-clock"></i>
                            </div>
                            <div class="title">AC ON Duration</div>
                            <div class="value" id="acOnDuration">--</div>
                            <div class="data-timestamp">Total waktu AC menyala</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-temperature-low"></i>
                            </div>
                            <div class="title">Avg Temperature</div>
                            <div class="value" id="avgACTemp">--°C</div>
                            <div class="data-timestamp">Suhu rata-rata setting</div>`
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-robot"></i>
                            </div>
                            <div class="title">Control Mode</div>
                            <div class="value" id="controlMode">Auto</div>
                            <div class="data-timestamp">Mode kontrol aktif</div>
                        </div>
                    </div>
                    <div class="table-responsive">
                        <table class="history-table">
                            <thead>
                                <tr>
                                    <th><i class="fas fa-clock"></i> Waktu</th>
                                    <th><i class="fas fa-snowflake"></i> AC Status</th>
                                    <th><i class="fas fa-temperature-low"></i> Set Temperature</th>
                                    <th><i class="fas fa-users"></i> Jumlah Orang</th>
                                    <th><i class="fas fa-thermometer-half"></i> Suhu Ruang</th>
                                    <th><i class="fas fa-robot"></i> Mode</th>
                                    <th><i class="fas fa-info-circle"></i> Keterangan</th>
                                </tr>
                            </thead>
                            <tbody id="ac-activity-body">
                                <tr>
                                    <td colspan="7" class="loading-row">
                                        <i class="fas fa-spinner fa-spin"></i> Loading aktivitas AC...
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>

                <!-- People Count Tab -->
                <div id="people-count-table" class="history-table-container" style="display:none;">
                    <div class="sensor-grid" style="margin-bottom: 20px;">
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-users"></i>
                            </div>
                            <div class="title">Max Hari Ini</div>
                            <div class="value" id="maxPeopleToday">{{ $dailyStats['max_people'] ?? 0 }}</div>
                            <div class="data-timestamp">Jumlah maksimum</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-chart-line"></i>
                            </div>
                            <div class="title">Rata-rata</div>
                            <div class="value" id="avgPeopleToday">{{ isset($dailyStats['avg_people']) ? number_format($dailyStats['avg_people'], 1) : '0.0' }}</div>
                            <div class="data-timestamp">Rata-rata harian</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-clock"></i>
                            </div>
                            <div class="title">Waktu Puncak</div>
                            <div class="value" id="peakTimeToday">{{ $dailyStats['peak_time'] ?? '--:--' }}</div>
                            <div class="data-timestamp">Jam tersibuk</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-walking"></i>
                            </div>
                            <div class="title">Total Movement</div>
                            <div class="value" id="totalMovement">--</div>
                            <div class="data-timestamp">Deteksi masuk/keluar</div>
                        </div>
                    </div>
                    <div class="table-responsive">
                        <table class="history-table">
                            <thead>
                                <tr>
                                    <th><i class="fas fa-clock"></i> Waktu</th>
                                    <th><i class="fas fa-users"></i> Jumlah Orang</th>
                                    <th><i class="fas fa-arrow-up"></i> Perubahan</th>
                                    <th><i class="fas fa-walking"></i> Sensor MASUK</th>
                                    <th><i class="fas fa-walking"></i> Sensor KELUAR</th>
                                    <th><i class="fas fa-snowflake"></i> Respon AC</th>
                                    <th><i class="fas fa-info-circle"></i> Keterangan</th>
                                </tr>
                            </thead>
                            <tbody id="people-count-body">
                                <tr>
                                    <td colspan="7" class="loading-row">
                                        <i class="fas fa-spinner fa-spin"></i> Loading data jumlah orang...
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>

                <!-- Environment Tab -->
                <div id="environment-table" class="history-table-container" style="display:none;">
                    <div class="sensor-grid" style="margin-bottom: 20px;">
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-thermometer-half"></i>
                            </div>
                            <div class="title">Suhu Rata-rata</div>
                            <div class="value" id="avgTempToday">{{ isset($dailyStats['avg_temperature']) ? number_format($dailyStats['avg_temperature'], 1) : '--' }}°C</div>
                            <div class="data-timestamp">Hari ini</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-tint"></i>
                            </div>
                            <div class="title">Kelembaban Rata-rata</div>
                            <div class="value" id="avgHumidityToday">{{ isset($dailyStats['avg_humidity']) ? number_format($dailyStats['avg_humidity'], 1) : '--' }}%</div>
                            <div class="data-timestamp">Hari ini</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-sun"></i>
                            </div>
                            <div class="title">Cahaya Rata-rata</div>
                            <div class="value" id="avgLightToday">{{ isset($dailyStats['avg_light']) ? number_format($dailyStats['avg_light']) : '--' }} lux</div>
                            <div class="data-timestamp">Hari ini</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-leaf"></i>
                            </div>
                            <div class="title">Kondisi Ruangan</div>
                            <div class="value" id="roomCondition">Normal</div>
                            <div class="data-timestamp">Status lingkungan</div>
                        </div>
                    </div>
                    <div class="table-responsive">
                        <table class="history-table">
                            <thead>
                                <tr>
                                    <th><i class="fas fa-clock"></i> Waktu</th>
                                    <th><i class="fas fa-thermometer-half"></i> Suhu (°C)</th>
                                    <th><i class="fas fa-tint"></i> Kelembaban (%)</th>
                                    <th><i class="fas fa-sun"></i> Cahaya (lux)</th>
                                    <th><i class="fas fa-users"></i> Orang</th>
                                    <th><i class="fas fa-snowflake"></i> AC Status</th>
                                    <th><i class="fas fa-leaf"></i> Kondisi</th>
                                </tr>
                            </thead>
                            <tbody id="environment-body">
                                <tr>
                                    <td colspan="7" class="loading-row">
                                        <i class="fas fa-spinner fa-spin"></i> Loading data lingkungan...
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>

                <!-- System Events Tab -->
                <div id="system-events-table" class="history-table-container" style="display:none;">
                    <div class="sensor-grid" style="margin-bottom: 20px;">
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-cog"></i>
                            </div>
                            <div class="title">Total Events</div>
                            <div class="value" id="totalEvents">--</div>
                            <div class="data-timestamp">System events</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-check-circle"></i>
                            </div>
                            <div class="title">Success Events</div>
                            <div class="value" id="successEvents">--</div>
                            <div class="data-timestamp">Berhasil</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-exclamation-triangle"></i>
                            </div>
                            <div class="title">Error Events</div>
                            <div class="value" id="errorEvents">--</div>
                            <div class="data-timestamp">Error/Warning</div>
                        </div>
                        <div class="card sensor-card">
                            <div class="card-icon">
                                <i class="fas fa-filter"></i>
                            </div>
                            <div class="title">Filter Events</div>
                            <select id="eventTypeFilter" onchange="loadSystemEvents()" class="form-control">
                                <option value="">Semua Events</option>
                                <option value="connection">Connection</option>
                                <option value="ac_control">AC Control</option>
                                <option value="sensor_update">Sensor Update</option>
                                <option value="system_error">System Error</option>
                                <option value="manual_control">Manual Control</option>
                            </select>
                        </div>
                    </div>
                    <div class="table-responsive">
                        <table class="history-table">
                            <thead>
                                <tr>
                                    <th><i class="fas fa-clock"></i> Timestamp</th>
                                    <th><i class="fas fa-tag"></i> Event Type</th>
                                    <th><i class="fas fa-microchip"></i> Device</th>
                                    <th><i class="fas fa-info-circle"></i> Description</th>
                                    <th><i class="fas fa-database"></i> Data</th>
                                    <th><i class="fas fa-exclamation-triangle"></i> Status</th>
                                </tr>
                            </thead>
                            <tbody id="system-events-body">
                                <tr>
                                    <td colspan="6" class="loading-row">
                                        <i class="fas fa-spinner fa-spin"></i> Loading system events...
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>

           
        </section>

        <!-- ===== Reports Section ===== -->
        <section id="reports" style="display:none;">
            <header>
                <h1><i class="fas fa-file-chart-line"></i> Laporan Energi</h1>
            </header>
            
            <!-- Quick Reports -->
            <div class="reports-container">
                <div class="report-card">
                    <div class="report-icon">
                        <i class="fas fa-calendar-day"></i>
                    </div>
                    <h3>Laporan Harian</h3>
                    <p>Konsumsi energi hari ini</p>
                    <div class="report-actions">
                        <button onclick="viewDailyReport()"><i class="fas fa-eye"></i> Lihat</button>
                        <button onclick="downloadDailyReport()"><i class="fas fa-download"></i> PDF</button>
                    </div>
                </div>
                
                <div class="report-card">
                    <div class="report-icon">
                        <i class="fas fa-calendar-week"></i>
                    </div>
                    <h3>Laporan Mingguan</h3>
                    <p>Ringkasan pemakaian energi 7 hari terakhir</p>
                    <div class="report-actions">
                        <button onclick="viewWeeklyReport()"><i class="fas fa-eye"></i> Lihat</button>
                        <button onclick="downloadWeeklyReport()"><i class="fas fa-download"></i> PDF</button>
                    </div>
                </div>
                
                <div class="report-card">
                    <div class="report-icon">
                        <i class="fas fa-calendar-alt"></i>
                    </div>
                    <h3>Laporan Bulanan</h3>
                    <p>Analisis komprehensif pemakaian bulanan</p>
                    <div class="report-actions">
                        <button onclick="viewMonthlyReport()"><i class="fas fa-eye"></i> Lihat</button>
                        <button onclick="downloadMonthlyReport()"><i class="fas fa-download"></i> PDF</button>
                    </div>
                </div>
                
                <div class="report-card">
                    <div class="report-icon">
                        <i class="fas fa-leaf"></i>
                    </div>
                    <h3>Laporan Efisiensi</h3>
                    <p>Evaluasi tingkat efisiensi energi</p>
                    <div class="report-actions">
                        <button onclick="viewEfficiencyReport()"><i class="fas fa-eye"></i> Lihat</button>
                        <button onclick="downloadEfficiencyReport()"><i class="fas fa-download"></i> PDF</button>
                    </div>
                </div>
            </div>

            <!-- Custom Report Generator -->
            <div class="custom-report-section">
                <h2><i class="fas fa-cogs"></i> Generator Laporan Kustom</h2>
                <div class="custom-report-form">
                    <div class="form-row">
                        <div class="form-group">
                            <label>Tanggal Mulai:</label>
                            <input type="date" id="reportDateFrom">
                        </div>
                        <div class="form-group">
                            <label>Tanggal Akhir:</label>
                            <input type="date" id="reportDateTo">
                        </div>
                        <div class="form-group">
                            <label>Jenis Perangkat:</label>
                            <select id="reportDeviceType">
                                <option value="">Semua Perangkat</option>
                                <option value="ac1">AC Unit 1</option>
                                <option value="ac2">AC Unit 2</option>
                                <option value="lamp1">Lampu Circuit 1</option>
                                <option value="lamp2">Lampu Circuit 2</option>
                                <option value="computer">Komputer</option>
                                <option value="tv">Smart TV</option>
                                <option value="dispenser">Dispenser</option>
                                <option value="kettle">Teko Listrik</option>
                                <option value="coffee">Mesin Kopi</option>
                            </select>
                        </div>
                        <div class="form-group">
                            <label>Format:</label>
                            <select id="reportFormat">
                                <option value="pdf">PDF Report</option>
                                <option value="excel">Excel Spreadsheet</option>
                                <option value="csv">CSV Data</option>
                            </select>
                        </div>
                    </div>
                    <div class="form-actions">
                        <button onclick="generateCustomReport()">
                            <i class="fas fa-download"></i> Generate & Download
                        </button>
                    </div>
                </div>
            </div>

            <!-- Quick Download Links -->
            <div class="quick-downloads">
                <h2><i class="fas fa-bolt"></i> Download Cepat</h2>
                <div class="download-links">
                    <a href="#" onclick="downloadTodayReport()" class="download-link">
                        <i class="fas fa-file-pdf"></i>
                        <div>
                            <h4>Laporan Hari Ini</h4>
                            <p>Download laporan energi hari ini</p>
                        </div>
                    </a>
                    <a href="#" onclick="downloadWeekReport()" class="download-link">
                        <i class="fas fa-file-excel"></i>
                        <div>
                            <h4>Laporan Minggu Ini</h4>
                            <p>Download ringkasan minggu ini</p>
                        </div>
                    </a>
                    <a href="#" onclick="downloadMonthReport()" class="download-link">
                        <i class="fas fa-file-csv"></i>
                        <div>
                            <h4>Laporan Bulan Ini</h4>
                            <p>Download analisis bulanan</p>
                        </div>
                    </a>
                </div>
            </div>
        </section>

        <!-- ===== Devices Section ===== -->
        <section id="devices" style="display:none;">
            <header>
                <h1><i class="fas fa-microchip"></i> Manajemen Perangkat</h1>
            </header>
            <div class="devices-grid">
                <div class="device-card">
                    <div class="device-status online"></div>
                    <i class="fas fa-lightbulb device-icon"></i>
                    <h3>Lampu TL (12 Unit - 2 Jalur)</h3>
                    <p>Status: <span class="status-online">Online</span></p>
                    <p>Jalur 1: 6 unit | Jalur 2: 6 unit</p>
                    <div class="device-controls">
                        <button class="btn-small" onclick="toggleAllLamps()"><i class="fas fa-power-off"></i></button>
                        <button class="btn-small"><i class="fas fa-cog"></i></button>
                    </div>
                </div>
                <div class="device-card">
                    <div class="device-status online"></div>
                    <i class="fas fa-snowflake device-icon"></i>
                    <h3>AC Panasonic (2 Unit)</h3>
                    <p>Status: <span class="status-online">Online</span></p>
                    <p>Konsumsi: 1650W per unit</p>
                    <div class="device-controls">
                        <button class="btn-small"><i class="fas fa-power-off"></i></button>
                        <button class="btn-small"><i class="fas fa-cog"></i></button>
                    </div>
                </div>
                <div class="device-card">
                    <div class="device-status online"></div>
                    <i class="fas fa-bolt device-icon"></i>
                    <h3>Panel Listrik</h3>
                    <p>Status: <span class="status-online">Online</span></p>
                    <p>Konsumsi: 300W</p>
                    <div class="device-controls">
                        <button class="btn-small" disabled><i class="fas fa-lock"></i></button>
                        <button class="btn-small"><i class="fas fa-cog"></i></button>
                    </div>
                </div>
                
                <!-- Pemakaian Listrik Lainnya -->
                <div class="device-card other-devices-card-sidebar">
                    <div class="device-status partial"></div>
                    <i class="fas fa-plug device-icon"></i>
                    <h3>Pemakaian Listrik Lainnya</h3>
                    <p>Total Daya: 2795W | Total: 7 Perangkat</p>
                    
                    <!-- Detail perangkat lainnya -->
                    <div class="other-devices-detail-sidebar">
                        <div class="device-item-sidebar">
                            <span><i class="fas fa-desktop"></i> Komputer (2 Unit)</span>
                            <div class="device-toggle-sidebar">
                                <div id="computerStatusSidebar" class="mini-status status-off">OFF</div>
                                <button onclick="toggleDevice('computer')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>

                        <div class="device-item-sidebar">
                            <span><i class="fas fa-tint"></i> Dispenser Sanken</span>
                            <div class="device-toggle-sidebar">
                                <div id="dispenserStatusSidebar" class="mini-status status-off">OFF</div>
                                <button onclick="toggleDevice('dispenser')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>

                        <div class="device-item-sidebar">
                            <span><i class="fas fa-tv"></i> Smart TV Horizon</span>
                            <div class="device-toggle-sidebar">
                                <div id="tvStatusSidebar" class="mini-status status-off">OFF</div>
                                <button onclick="toggleDevice('tv')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>

                        <div class="device-item-sidebar">
                            <span><i class="fas fa-archive"></i> Kulkas Sharp</span>
                            <div class="device-toggle-sidebar">
                                <div id="fridgeStatusSidebar" class="mini-status status-on">ON</div>
                                <button onclick="toggleDevice('fridge')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>

                        <div class="device-item-sidebar">
                            <span><i class="fas fa-wifi"></i> Router Wi-Fi</span>
                            <div class="device-toggle-sidebar">
                                <div id="routerStatusSidebar" class="mini-status status-on">ON</div>
                                <button onclick="toggleDevice('router')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>

                        <div class="device-item-sidebar">
                            <span><i class="fas fa-coffee"></i> Teko Listrik</span>
                            <div class="device-toggle-sidebar">
                                <div id="kettleStatusSidebar" class="mini-status status-off">OFF</div>
                                <button onclick="toggleDevice('kettle')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>

                        <div class="device-item-sidebar">
                            <span><i class="fas fa-mug-hot"></i> Mesin Kopi</span>
                            <div class="device-toggle-sidebar">
                                <div id="coffeeStatusSidebar" class="mini-status status-off">OFF</div>
                                <button onclick="toggleDevice('coffee')" class="mini-btn"><i class="fas fa-power-off"></i></button>
                            </div>
                        </div>
                    </div>
                    
                    <div class="device-controls">
                        <button class="btn-small" onclick="toggleAllOtherDevices()"><i class="fas fa-power-off"></i></button>
                        <button class="btn-small"><i class="fas fa-cog"></i></button>
                    </div>
                </div>
            </div>
        </section>

        <!-- ===== Automation Section ===== -->
        <section id="automation" style="display:none;">
            <header>
                <h1><i class="fas fa-robot"></i> Sistem Otomasi</h1>
            </header>
            <div class="automation-container">
                <div class="automation-rule">
                    <div class="rule-header">
                        <i class="fas fa-clock"></i>
                        <h3>Jadwal Otomatis</h3>
                        <div class="toggle-switch active" onclick="toggleScheduledShutdown()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <p>Listrik di ruangan mati otomatis di luar jam 07:00 - 17:00</p>
                </div>
                <div class="automation-rule">
                    <div class="rule-header">
                        <i class="fas fa-calendar-week"></i>
                        <h3>Hari Kerja</h3>
                        <div class="toggle-switch active" onclick="toggleWorkingDays()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <p>Sistem aktif hanya pada hari kerja (Senin - Jumat)</p>
                </div>

                <div class="automation-rule">
                    <div class="rule-header">
                        <i class="fas fa-lightbulb"></i>
                        <h3>Kontrol Lampu Otomatis</h3>
                        <div class="toggle-switch active" onclick="toggleAutoLampControl()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <p>Lampu menyala otomatis saat intensitas cahaya rendah/gelap atau hujan</p>
                </div>

                <div class="automation-rule">
                    <div class="rule-header">
                        <i class="fas fa-users"></i>
                        <h3>Deteksi Kehadiran & AC Otomatis</h3>
                        <div class="toggle-switch active" onclick="togglePresenceDetection()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <p>AC menyala otomatis saat ada orang di ruangan</p>
                </div>

                <div class="automation-rule">
                    <div class="rule-header">
                        <i class="fas fa-power-off"></i>
                        <h3>Auto Shutdown</h3>
                        <div class="toggle-switch active" id="autoShutdownToggle" onclick="toggleAutoShutdown()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <p>Panel listrik dimatikan otomatis setelah jam kerja atau tidak ada orang selama 15 menit</p>
                    <div class="automation-controls">
                        <button onclick="emergencyOverride()" class="emergency-btn">
                            <i class="fas fa-exclamation-triangle"></i> Emergency Override
                        </button>
                        <button onclick="manualShutdownAll()" class="shutdown-btn">
                            <i class="fas fa-power-off"></i> Manual Shutdown
                        </button>
                    </div>
                </div>

                <div class="automation-rule">
                    <div class="rule-header">
                        <i class="fas fa-snowflake"></i>
                        <h3>Estimasi AC Berdasarkan Kondisi</h3>
                    </div>
                    <p>Sistem AC otomatis berdasarkan jumlah orang dan suhu luar ruangan</p>
                    <div class="rules-table">
                        <table class="ac-rules-table">
                            <thead>
                                <tr>
                                    <th>Jumlah Orang</th>
                                    <th>Suhu Luar Ruangan (°C)</th>
                                    <th>AC Aktif</th>
                                    <th>Target Suhu Dalam (°C)</th>
                                    <th>Keterangan</th>
                                </tr>
                            </thead>
                            <tbody>
                                <tr>
                                    <td>0</td>
                                    <td>--</td>
                                    <td>Mode Gradual</td>
                                    <td>28°C (15 menit)</td>
                                    <td>Tidak ada orang → naikkan suhu bertahap ke suhu ruang, lalu mati</td>
                                </tr>
                                <tr>
                                    <td>1 - 4</td>
                                    <td>--</td>
                                    <td>AC Mati</td>
                                    <td>--</td>
                                    <td>Di bawah threshold → AC tidak perlu menyala</td>
                                </tr>
                                <tr>
                                    <td>5 - 10</td>
                                    <td>30.0 - 32.0</td>
                                    <td>1 AC</td>
                                    <td>25°C</td>
                                    <td>Beban ringan → hanya satu AC</td>
                                </tr>
                                <tr>
                                    <td>5 - 10</td>
                                    <td>32.1 - 35.0</td>
                                    <td>1 AC</td>
                                    <td>26°C</td>
                                    <td>Suhu luar tinggi → setpoint lebih longgar</td>
                                </tr>
                                <tr>
                                    <td>10 - 13</td>
                                    <td>30.0 - 32.0</td>
                                    <td>2 AC</td>
                                    <td>24°C</td>
                                    <td>Beban meningkat → aktifkan kedua AC</td>
                                </tr>
                                <tr>
                                    <td>10 - 20</td>
                                    <td>32.1 - 35.0</td>
                                    <td>2 AC</td>
                                    <td>25°C</td>
                                    <td>Kondisi luar panas → pendinginan lebih ringan</td>
                                </tr>
                                <tr>
                                    <td>21+</td>
                                    <td>--</td>
                                    <td>2 AC</td>
                                    <td>23°C</td>
                                    <td>Beban maksimal → pendinginan agresif</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>
        </section>

        <!-- ===== Alerts Section ===== -->
        <section id="alerts" style="display:none;">
            <header>
                <h1><i class="fas fa-bell"></i> Notifikasi & Peringatan</h1>
            </header>
            <div class="alerts-container">
                <div class="alert-item warning">
                    <i class="fas fa-exclamation-triangle"></i>
                    <div class="alert-content">
                        <h4>Konsumsi Energi Tinggi</h4>
                        <p>Pemakaian listrik hari ini 15% lebih tinggi dari rata-rata</p>
                        <span class="alert-time">2 jam yang lalu</span>
                    </div>
                </div>
                <div class="alert-item info">
                    <i class="fas fa-info-circle"></i>
                    <div class="alert-content">
                        <h4>Sistem Update</h4>
                        <p>Firmware sensor berhasil diperbarui ke versi 2.1.0</p>
                        <span class="alert-time">5 jam yang lalu</span>
                    </div>
                </div>
                <div class="alert-item success">
                    <i class="fas fa-check-circle"></i>
                    <div class="alert-content">
                        <h4>Target Penghematan Tercapai</h4>
                        <p>Berhasil menghemat 12% energi minggu ini</p>
                        <span class="alert-time">1 hari yang lalu</span>
                    </div>
                </div>
            </div>
        </section>

        <!-- ===== Settings Section ===== -->
        <section id="settings" style="display:none;">
            <header>
                <h1><i class="fas fa-cog"></i> Pengaturan Sistem</h1>
            </header>
            <div class="settings-container">
                <div class="settings-group">
                    <h3><i class="fas fa-user"></i> Profil Pengguna</h3>
                    <div class="setting-item">
                        <label>Nama Pengguna</label>
                        <input type="text" value="Admin User" />
                    </div>
                    <div class="setting-item">
                        <label>Email</label>
                        <input type="email" value="admin@elektro.ac.id" />
                    </div>
                </div>
                <div class="settings-group">
                    <h3><i class="fas fa-bell"></i> Notifikasi</h3>
                    <div class="setting-item">
                        <label>Email Notifikasi</label>
                        <div class="toggle-switch active">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <div class="setting-item">
                        <label>Push Notification</label>
                        <div class="toggle-switch">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                </div>
                <div class="settings-group">
                    <h3><i class="fas fa-palette"></i> Tampilan</h3>
                    <div class="setting-item">
                        <label>Mode Gelap</label>
                        <div class="toggle-switch active" id="darkModeToggle" onclick="toggleDarkMode()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                </div>
                
                <div class="settings-group">
                    <h3><i class="fas fa-microchip"></i> Koneksi Arduino</h3>
                    <div class="setting-item">
                        <label>Gunakan Data Arduino</label>
                        <div class="toggle-switch" id="arduinoToggle" onclick="toggleArduinoConnection()">
                            <span class="toggle-slider"></span>
                        </div>
                    </div>
                    <div class="setting-item">
                        <label>IP Address Arduino</label>
                        <input type="text" id="arduinoIP" value="192.168.1.100" onchange="updateArduinoIP()" />
                    </div>
                    <div class="setting-item">
                        <label>Status Koneksi</label>
                        <span id="connectionStatus" class="connection-status disconnected">Terputus</span>
                    </div>
                </div>
            </div>
        </section>

    </div>

    <script>
        // ===== BASIC UI FUNCTIONS =====
        function toggleSidebar() {
            const sidebar = document.getElementById('sidebar');
            const toggleIcon = document.getElementById('toggleIcon');
            
            sidebar.classList.toggle('collapsed');
            
            if (sidebar.classList.contains('collapsed')) {
                toggleIcon.classList.remove('fa-bars');
                toggleIcon.classList.add('fa-chevron-right');
            } else {
                toggleIcon.classList.remove('fa-chevron-right');
                toggleIcon.classList.add('fa-bars');
            }
        }

        function showSection(sectionId, element) {
            // Hide all sections
            const sections = document.querySelectorAll('section');
            sections.forEach(section => {
                section.style.display = 'none';
            });
            
            // Show selected section
            document.getElementById(sectionId).style.display = 'block';
            
            // Update active menu item
            const menuItems = document.querySelectorAll('.sidebar a');
            menuItems.forEach(item => {
                item.classList.remove('active');
            });
            element.classList.add('active');
        }

        // ===== AUTO REFRESH PAGE =====
        function autoRefreshPage() {
            // Auto refresh setiap 30 detik untuk update data
            setTimeout(function() {
                window.location.reload();
            }, 30000);
        }

        // ===== HISTORY FUNCTIONS =====
        let historyChart = null;
        let currentHistoryPage = 1;
        let historyData = {};

        function initializeHistory() {
            // Initialize history data loading
            loadSensorData();
            
            // Set up period filter change handler
            const periodSelect = document.getElementById('historyPeriod');
            if (periodSelect) {
                periodSelect.addEventListener('change', function() {
                    const customRange = document.getElementById('customDateRange');
                    if (customRange) {
                        if (this.value === 'custom') {
                            customRange.style.display = 'flex';
                            // Set default dates
                            const today = new Date();
                            const weekAgo = new Date(today.getTime() - 7 * 24 * 60 * 60 * 1000);
                            const dateToInput = document.getElementById('dateTo');
                            const dateFromInput = document.getElementById('dateFrom');
                            if (dateToInput) dateToInput.value = today.toISOString().split('T')[0];
                            if (dateFromInput) dateFromInput.value = weekAgo.toISOString().split('T')[0];
                        } else {
                            customRange.style.display = 'none';
                        }
                    }
                });
            }
        }

        function showHistoryTab(tabName) {
            // Hide all tab contents
            const tabContents = document.querySelectorAll('.history-table-container');
            tabContents.forEach(content => {
                content.style.display = 'none';
            });
            
            // Remove active class from all tab buttons
            const tabButtons = document.querySelectorAll('.tab-button');
            tabButtons.forEach(button => {
                button.classList.remove('active');
            });
            
            // Show selected tab content
            const selectedTab = document.getElementById(tabName + '-table');
            if (selectedTab) {
                selectedTab.style.display = 'block';
            }
            
            // Add active class to clicked button
            if (event && event.target) {
                event.target.classList.add('active');
            }
            
            // Load data for the selected tab
            switch(tabName) {
                case 'sensor-data':
                    loadSensorData();
                    break;
                case 'ac-activity':
                    loadACActivity();
                    break;
                case 'people-count':
                    loadPeopleCountData();
                    break;
                case 'environment':
                    loadEnvironmentData();
                    break;
                case 'system-events':
                    loadSystemEvents();
                    break;
            }
        }

        function loadHistoryData() {
            // Load data based on current active tab
            const activeTab = document.querySelector('.tab-button.active');
            if (activeTab) {
                const tabText = activeTab.textContent.trim();
                if (tabText.includes('Data Sensor')) {
                    loadSensorData();
                } else if (tabText.includes('Aktivitas AC')) {
                    loadACActivity();
                } else if (tabText.includes('Jumlah Orang')) {
                    loadPeopleCountData();
                } else if (tabText.includes('Lingkungan')) {
                    loadEnvironmentData();
                } else if (tabText.includes('System Events')) {
                    loadSystemEvents();
                }
            }
        }

        function loadSensorData() {
            const tbody = document.getElementById('sensor-data-body');
            if (!tbody) return;
            
            const limitSelect = document.getElementById('sensorDataLimit');
            const periodSelect = document.getElementById('historyPeriod');
            const deviceFilterSelect = document.getElementById('deviceFilter');
            
            const limit = limitSelect ? limitSelect.value : '100';
            const period = periodSelect ? periodSelect.value : 'week';
            const deviceFilter = deviceFilterSelect ? deviceFilterSelect.value : '';
            
            tbody.innerHTML = '<tr><td colspan="11" class="loading-row"><i class="fas fa-spinner fa-spin"></i> Loading data sensor...</td></tr>';
            
            let url = `/api/sensor/history?per_page=${limit}`;
            if (deviceFilter) {
                url += `&device_id=${deviceFilter}`;
            }
            
            fetch(url)
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data.length > 0) {
                        let html = '';
                        data.data.forEach((item, index) => {
                            const timestamp = new Date(item.created_at).toLocaleString('id-ID');
                            const proximityStatus = `IN: ${item.proximity_in ? 'ON' : 'OFF'} | OUT: ${item.proximity_out ? 'ON' : 'OFF'}`;
                            
                            html += `
                                <tr>
                                    <td>${item.id}</td>
                                    <td>${timestamp}</td>
                                    <td><span class="device-badge">${item.device_id || 'ESP32'}</span></td>
                                    <td><span class="people-count">${item.people_count || 0}</span></td>
                                    <td>${item.room_temperature ? item.room_temperature.toFixed(1) : '--'}</td>
                                    <td>${item.humidity ? item.humidity.toFixed(1) : '--'}</td>
                                    <td>${item.light_level || '--'}</td>
                                    <td><span class="ac-status ${item.ac_status === 'OFF' ? 'status-off' : 'status-on'}">${item.ac_status || 'OFF'}</span></td>
                                    <td>${item.set_temperature || '--'}°C</td>
                                    <td><small>${proximityStatus}</small></td>
                                    <td>${item.wifi_rssi || '--'}</td>
                                </tr>
                            `;
                        });
                        tbody.innerHTML = html;
                        
                        // Update pagination info
                        const pageInfo = document.getElementById('sensorPageInfo');
                        if (data.pagination && pageInfo) {
                            pageInfo.textContent = `Page ${data.pagination.current_page} of ${data.pagination.last_page}`;
                        }
                        
                        // Update summary stats
                        updateSummaryStats(data.data);
                    } else {
                        tbody.innerHTML = '<tr><td colspan="11" class="no-data">Tidak ada data sensor ditemukan</td></tr>';
                    }
                })
                .catch(error => {
                    console.error('Error loading sensor data:', error);
                    tbody.innerHTML = '<tr><td colspan="11" class="error-row">Error loading data. Please try again.</td></tr>';
                });
        }

        function loadACActivity() {
            const tbody = document.getElementById('ac-activity-body');
            if (!tbody) return;
            
            tbody.innerHTML = '<tr><td colspan="7" class="loading-row"><i class="fas fa-spinner fa-spin"></i> Loading aktivitas AC...</td></tr>';
            
            // Load AC control history from API
            fetch('/api/ac/history')
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data && data.data.length > 0) {
                        let html = '';
                        let acEvents = 0;
                        let totalOnTime = 0;
                        let tempSum = 0;
                        let tempCount = 0;
                        
                        data.data.forEach((item, index) => {
                            const timestamp = new Date(item.created_at).toLocaleString('id-ID');
                            const mode = item.control_mode === 'manual' ? 'Manual Control' : 'Auto Control';
                            
                            let keterangan = '';
                            if (item.control_mode === 'manual') {
                                keterangan = `Manual control by ${item.created_by || 'user'}`;
                            } else {
                                keterangan = 'Automatic control based on people count';
                            }
                            
                            const ac1Status = item.ac1_status ? 'ON' : 'OFF';
                            const ac2Status = item.ac2_status ? 'ON' : 'OFF';
                            const acStatus = (item.ac1_status && item.ac2_status) ? '2 AC ON' : 
                                           (item.ac1_status || item.ac2_status) ? '1 AC ON' : 'AC OFF';
                            
                            html += `
                                <tr>
                                    <td>${timestamp}</td>
                                    <td><span class="ac-status ${acStatus === 'AC OFF' ? 'status-off' : 'status-on'}">${acStatus}</span></td>
                                    <td>${item.ac1_temperature || item.ac2_temperature || '--'}°C</td>
                                    <td>--</td>
                                    <td>--°C</td>
                                    <td><span class="mode-badge">${mode}</span></td>
                                    <td><small>${keterangan}</small></td>
                                </tr>
                            `;
                            
                            acEvents++;
                            if (item.ac1_temperature) {
                                tempSum += item.ac1_temperature;
                                tempCount++;
                            }
                            if (item.ac2_temperature && item.ac2_temperature !== item.ac1_temperature) {
                                tempSum += item.ac2_temperature;
                                tempCount++;
                            }
                        });
                        
                        tbody.innerHTML = html;
                        
                        // Update AC activity stats
                        document.getElementById('totalACEvents').textContent = acEvents;
                        document.getElementById('avgACTemp').textContent = tempCount > 0 ? (tempSum / tempCount).toFixed(1) + '°C' : '--°C';
                        document.getElementById('controlMode').textContent = 'Auto/Manual';
                        
                    } else {
                        // Fallback to sensor data for AC activity
                        loadACActivityFromSensorData();
                    }
                })
                .catch(error => {
                    console.error('Error loading AC history:', error);
                    // Fallback to sensor data
                    loadACActivityFromSensorData();
                });
        }

        function loadACActivityFromSensorData() {
            const tbody = document.getElementById('ac-activity-body');
            
            fetch('/api/sensor/history?per_page=100')
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data.length > 0) {
                        let html = '';
                        let lastACStatus = '';
                        let acEvents = 0;
                        let tempSum = 0;
                        let tempCount = 0;
                        
                        data.data.forEach((item, index) => {
                            // Only show records where AC status changed
                            if (item.ac_status !== lastACStatus) {
                                const timestamp = new Date(item.created_at).toLocaleString('id-ID');
                                const mode = 'Auto (People Detection)';
                                let keterangan = '';
                                
                                if (item.ac_status === 'OFF') {
                                    keterangan = 'AC dimatikan karena tidak ada orang';
                                } else if (item.people_count <= 5) {
                                    keterangan = '1 AC aktif untuk 1-5 orang';
                                } else if (item.people_count <= 10) {
                                    keterangan = '1 AC aktif untuk 6-10 orang';
                                } else {
                                    keterangan = '2 AC aktif untuk 11+ orang';
                                }
                                
                                html += `
                                    <tr>
                                        <td>${timestamp}</td>
                                        <td><span class="ac-status ${item.ac_status === 'OFF' ? 'status-off' : 'status-on'}">${item.ac_status}</span></td>
                                        <td>${item.set_temperature || '--'}°C</td>
                                        <td>${item.people_count || 0}</td>
                                        <td>${item.room_temperature ? item.room_temperature.toFixed(1) : '--'}°C</td>
                                        <td><span class="mode-badge">${mode}</span></td>
                                        <td><small>${keterangan}</small></td>
                                    </tr>
                                `;
                                lastACStatus = item.ac_status;
                                acEvents++;
                                
                                if (item.set_temperature) {
                                    tempSum += item.set_temperature;
                                    tempCount++;
                                }
                            }
                        });
                        
                        if (html) {
                            tbody.innerHTML = html;
                        } else {
                            tbody.innerHTML = '<tr><td colspan="7" class="no-data">Tidak ada perubahan aktivitas AC ditemukan</td></tr>';
                        }
                        
                        // Update AC activity stats
                        document.getElementById('totalACEvents').textContent = acEvents;
                        document.getElementById('avgACTemp').textContent = tempCount > 0 ? (tempSum / tempCount).toFixed(1) + '°C' : '--°C';
                        document.getElementById('controlMode').textContent = 'Auto';
                        
                    } else {
                        tbody.innerHTML = '<tr><td colspan="7" class="no-data">Tidak ada data aktivitas AC ditemukan</td></tr>';
                    }
                })
                .catch(error => {
                    console.error('Error loading AC activity:', error);
                    tbody.innerHTML = '<tr><td colspan="7" class="error-row">Error loading data. Please try again.</td></tr>';
                });
        }

        function loadPeopleCountData() {
            const tbody = document.getElementById('people-count-body');
            if (!tbody) return;
            
            tbody.innerHTML = '<tr><td colspan="7" class="loading-row"><i class="fas fa-spinner fa-spin"></i> Loading data jumlah orang...</td></tr>';
            
            fetch('/api/sensor/history?per_page=100')
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data.length > 0) {
                        let html = '';
                        let lastPeopleCount = -1;
                        let totalMovement = 0;
                        
                        data.data.forEach((item, index) => {
                            // Only show records where people count changed
                            if (item.people_count !== lastPeopleCount) {
                                const timestamp = new Date(item.created_at).toLocaleString('id-ID');
                                const change = lastPeopleCount === -1 ? 0 : item.people_count - lastPeopleCount;
                                const changeIcon = change > 0 ? '↑' : change < 0 ? '↓' : '=';
                                const changeClass = change > 0 ? 'increase' : change < 0 ? 'decrease' : 'same';
                                
                                let acResponse = 'Tidak ada perubahan';
                                if (item.people_count === 0) {
                                    acResponse = 'AC OFF';
                                } else if (item.people_count <= 5) {
                                    acResponse = '1 AC ON (25°C)';
                                } else if (item.people_count <= 10) {
                                    acResponse = '1 AC ON (22°C)';
                                } else {
                                    acResponse = '2 AC ON (20°C)';
                                }
                                
                                let keterangan = '';
                                if (change > 0) {
                                    keterangan = `${change} orang masuk ruangan`;
                                    totalMovement += change;
                                } else if (change < 0) {
                                    keterangan = `${Math.abs(change)} orang keluar ruangan`;
                                    totalMovement += Math.abs(change);
                                } else if (lastPeopleCount === -1) {
                                    keterangan = 'Data awal sistem';
                                }
                                
                                html += `
                                    <tr>
                                        <td>${timestamp}</td>
                                        <td><span class="people-count">${item.people_count}</span></td>
                                        <td><span class="change-indicator ${changeClass}">${changeIcon} ${Math.abs(change)}</span></td>
                                        <td>${item.proximity_in ? 'AKTIF' : 'OFF'}</td>
                                        <td>${item.proximity_out ? 'AKTIF' : 'OFF'}</td>
                                        <td><span class="ac-response">${acResponse}</span></td>
                                        <td><small>${keterangan}</small></td>
                                    </tr>
                                `;
                                lastPeopleCount = item.people_count;
                            }
                        });
                        
                        if (html) {
                            tbody.innerHTML = html;
                        } else {
                            tbody.innerHTML = '<tr><td colspan="7" class="no-data">Tidak ada perubahan jumlah orang ditemukan</td></tr>';
                        }
                        
                        // Update total movement
                        document.getElementById('totalMovement').textContent = totalMovement;
                        
                    } else {
                        tbody.innerHTML = '<tr><td colspan="7" class="no-data">Tidak ada data jumlah orang ditemukan</td></tr>';
                    }
                })
                .catch(error => {
                    console.error('Error loading people count data:', error);
                    tbody.innerHTML = '<tr><td colspan="7" class="error-row">Error loading data. Please try again.</td></tr>';
                });
        }

        function loadEnvironmentData() {
            const tbody = document.getElementById('environment-body');
            if (!tbody) return;
            
            tbody.innerHTML = '<tr><td colspan="7" class="loading-row"><i class="fas fa-spinner fa-spin"></i> Loading data lingkungan...</td></tr>';
            
            fetch('/api/sensor/history?per_page=50')
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data.length > 0) {
                        let html = '';
                        let tempSum = 0, humSum = 0, lightSum = 0, count = 0;
                        
                        data.data.forEach((item, index) => {
                            const timestamp = new Date(item.created_at).toLocaleString('id-ID');
                            
                            // Determine condition based on environment data
                            let kondisi = 'Normal';
                            if (item.room_temperature > 28) {
                                kondisi = 'Panas';
                            } else if (item.room_temperature < 20) {
                                kondisi = 'Dingin';
                            }
                            
                            if (item.humidity > 70) {
                                kondisi += ', Lembab';
                            } else if (item.humidity < 40) {
                                kondisi += ', Kering';
                            }
                            
                            if (item.light_level < 200) {
                                kondisi += ', Gelap';
                            } else if (item.light_level > 800) {
                                kondisi += ', Terang';
                            }
                            
                            html += `
                                <tr>
                                    <td>${timestamp}</td>
                                    <td>${item.room_temperature ? item.room_temperature.toFixed(1) : '--'}</td>
                                    <td>${item.humidity ? item.humidity.toFixed(1) : '--'}</td>
                                    <td>${item.light_level || '--'}</td>
                                    <td>${item.people_count || 0}</td>
                                    <td><span class="ac-status ${item.ac_status === 'OFF' ? 'status-off' : 'status-on'}">${item.ac_status || 'OFF'}</span></td>
                                    <td><span class="condition-badge">${kondisi}</span></td>
                                </tr>
                            `;
                            
                            // Calculate averages
                            if (item.room_temperature) {
                                tempSum += item.room_temperature;
                                count++;
                            }
                            if (item.humidity) humSum += item.humidity;
                            if (item.light_level) lightSum += item.light_level;
                        });
                        
                        tbody.innerHTML = html;
                        
                        // Update room condition
                        if (count > 0) {
                            const avgTemp = tempSum / count;
                            const avgHum = humSum / count;
                            let roomCondition = 'Normal';
                            
                            if (avgTemp > 28) roomCondition = 'Panas';
                            else if (avgTemp < 20) roomCondition = 'Dingin';
                            
                            if (avgHum > 70) roomCondition += ', Lembab';
                            else if (avgHum < 40) roomCondition += ', Kering';
                            
                            document.getElementById('roomCondition').textContent = roomCondition;
                        }
                        
                    } else {
                        tbody.innerHTML = '<tr><td colspan="7" class="no-data">Tidak ada data lingkungan ditemukan</td></tr>';
                    }
                })
                .catch(error => {
                    console.error('Error loading environment data:', error);
                    tbody.innerHTML = '<tr><td colspan="7" class="error-row">Error loading data. Please try again.</td></tr>';
                });
        }

        function loadSystemEvents() {
            const tbody = document.getElementById('system-events-body');
            if (!tbody) return;
            
            const eventFilterSelect = document.getElementById('eventTypeFilter');
            const eventFilter = eventFilterSelect ? eventFilterSelect.value : '';
            tbody.innerHTML = '<tr><td colspan="6" class="loading-row"><i class="fas fa-spinner fa-spin"></i> Loading system events...</td></tr>';
            
            // Generate system events based on sensor data and AC control history
            Promise.all([
                fetch('/api/sensor/history?per_page=30').then(r => r.json()),
                fetch('/api/ac/history').then(r => r.json()).catch(() => ({success: false}))
            ])
            .then(([sensorData, acData]) => {
                let html = '';
                let events = [];
                let totalEvents = 0;
                let successEvents = 0;
                let errorEvents = 0;
                
                // Generate events from sensor data
                if (sensorData.success && sensorData.data) {
                    sensorData.data.forEach((item, index) => {
                        const timestamp = new Date(item.created_at);
                        
                        // Connection events
                        events.push({
                            timestamp: timestamp,
                            type: 'sensor_update',
                            device: item.device_id || 'ESP32',
                            description: 'Sensor data received',
                            data: `People: ${item.people_count}, Temp: ${item.room_temperature}°C`,
                            status: 'success'
                        });
                        
                        // AC control events
                        if (item.ac_status !== 'OFF') {
                            events.push({
                                timestamp: timestamp,
                                type: 'ac_control',
                                device: item.device_id || 'ESP32',
                                description: `AC activated: ${item.ac_status}`,
                                data: `Set temp: ${item.set_temperature}°C, People: ${item.people_count}`,
                                status: 'info'
                            });
                        }
                    });
                }
                
                // Generate events from AC control history
                if (acData.success && acData.data) {
                    acData.data.forEach((item, index) => {
                        const timestamp = new Date(item.created_at);
                        
                        events.push({
                            timestamp: timestamp,
                            type: 'manual_control',
                            device: 'Dashboard',
                            description: `Manual AC control activated`,
                            data: `AC1: ${item.ac1_status ? 'ON' : 'OFF'}, AC2: ${item.ac2_status ? 'ON' : 'OFF'}`,
                            status: 'info'
                        });
                    });
                }
                
                // Sort events by timestamp (newest first)
                events.sort((a, b) => b.timestamp - a.timestamp);
                
                // Filter events if needed
                if (eventFilter) {
                    events = events.filter(event => event.type === eventFilter);
                }
                
                // Generate HTML
                events.slice(0, 50).forEach(event => {
                    const timestampStr = event.timestamp.toLocaleString('id-ID');
                    const statusClass = event.status === 'success' ? 'status-success' : 
                                      event.status === 'error' ? 'status-error' : 'status-info';
                    
                    html += `
                        <tr>
                            <td>${timestampStr}</td>
                            <td><span class="event-type-badge ${event.type}">${event.type.replace('_', ' ').toUpperCase()}</span></td>
                            <td><span class="device-badge">${event.device}</span></td>
                            <td>${event.description}</td>
                            <td><small>${event.data}</small></td>
                            <td><span class="status-badge ${statusClass}">${event.status.toUpperCase()}</span></td>
                        </tr>
                    `;
                    
                    totalEvents++;
                    if (event.status === 'success') successEvents++;
                    if (event.status === 'error') errorEvents++;
                });
                
                if (html) {
                    tbody.innerHTML = html;
                } else {
                    tbody.innerHTML = '<tr><td colspan="6" class="no-data">Tidak ada system events ditemukan</td></tr>';
                }
                
                // Update event stats
                document.getElementById('totalEvents').textContent = totalEvents;
                document.getElementById('successEvents').textContent = successEvents;
                document.getElementById('errorEvents').textContent = errorEvents;
                
            })
            .catch(error => {
                console.error('Error loading system events:', error);
                tbody.innerHTML = '<tr><td colspan="6" class="error-row">Error loading data. Please try again.</td></tr>';
            });
        }

        function updateSummaryStats(data) {
            if (!data || data.length === 0) return;
            
            // Update total records
            const totalRecordsEl = document.getElementById('totalRecords');
            if (totalRecordsEl) {
                totalRecordsEl.textContent = data.length.toLocaleString();
            }
            
            // Update last update time
            const lastUpdateEl = document.getElementById('lastUpdate');
            if (lastUpdateEl && data[0]) {
                const lastUpdate = new Date(data[0].created_at);
                lastUpdateEl.textContent = lastUpdate.toLocaleString('id-ID');
            }
            
            // Calculate update frequency
            if (data.length > 1) {
                const firstTime = new Date(data[0].created_at);
                const lastTime = new Date(data[data.length - 1].created_at);
                const diffMinutes = (firstTime - lastTime) / (1000 * 60);
                const avgInterval = diffMinutes / data.length;
                
                const updateFreqEl = document.getElementById('updateFreq');
                if (updateFreqEl) {
                    updateFreqEl.textContent = Math.round(avgInterval * 60) + 's';
                }
            }
        }

        function refreshHistoryData() {
            loadHistoryData();
        }

        function exportHistoryData() {
            const periodSelect = document.getElementById('historyPeriod');
            const deviceFilterSelect = document.getElementById('deviceFilter');
            
            const period = periodSelect ? periodSelect.value : 'week';
            const deviceFilter = deviceFilterSelect ? deviceFilterSelect.value : '';
            
            let url = '/api/sensor/history?format=csv';
            if (deviceFilter) {
                url += `&device_id=${deviceFilter}`;
            }
            
            // Create download link
            const link = document.createElement('a');
            link.href = url;
            link.download = `sensor_history_${period}_${new Date().toISOString().split('T')[0]}.csv`;
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }

        function previousPage(type) {
            if (currentHistoryPage > 1) {
                currentHistoryPage--;
                loadHistoryData();
            }
        }

        function nextPage(type) {
            currentHistoryPage++;
            loadHistoryData();
        }

        function showChart(chartType) {
            // Remove active class from all chart tab buttons
            document.querySelectorAll('.chart-tab-btn').forEach(btn => {
                btn.classList.remove('active');
            });
            
            // Add active class to clicked button
            if (event && event.target) {
                event.target.classList.add('active');
            }
            
            // Load chart data
            loadChartData(chartType);
        }

        function loadChartData(chartType) {
            fetch('/api/dashboard/chart/24')
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data.length > 0) {
                        const canvas = document.getElementById('historyChart');
                        if (!canvas) return;
                        
                        const ctx = canvas.getContext('2d');
                        
                        // Destroy existing chart
                        if (historyChart) {
                            historyChart.destroy();
                        }
                        
                        const labels = data.data.map(item => item.time);
                        let chartData, label, color;
                        
                        switch(chartType) {
                            case 'temperature':
                                chartData = data.data.map(item => item.temperature);
                                label = 'Suhu (°C)';
                                color = 'rgb(255, 99, 132)';
                                break;
                            case 'humidity':
                                chartData = data.data.map(item => item.humidity);
                                label = 'Kelembaban (%)';
                                color = 'rgb(54, 162, 235)';
                                break;
                            case 'people':
                                chartData = data.data.map(item => item.people_count);
                                label = 'Jumlah Orang';
                                color = 'rgb(75, 192, 192)';
                                break;
                            case 'light':
                                chartData = data.data.map(item => item.light_level);
                                label = 'Intensitas Cahaya (lux)';
                                color = 'rgb(255, 205, 86)';
                                break;
                        }
                        
                        historyChart = new Chart(ctx, {
                            type: 'line',
                            data: {
                                labels: labels,
                                datasets: [{
                                    label: label,
                                    data: chartData,
                                    borderColor: color,
                                    backgroundColor: color + '20',
                                    tension: 0.1,
                                    fill: true
                                }]
                            },
                            options: {
                                responsive: true,
                                maintainAspectRatio: false,
                                plugins: {
                                    title: {
                                        display: true,
                                        text: `Grafik ${label} - 24 Jam Terakhir`
                                    }
                                },
                                scales: {
                                    y: {
                                        beginAtZero: true
                                    }
                                }
                            }
                        });
                    } else {
                        // Show message if no data available
                        const canvas = document.getElementById('historyChart');
                        if (canvas) {
                            const ctx = canvas.getContext('2d');
                            ctx.clearRect(0, 0, canvas.width, canvas.height);
                            ctx.font = '16px Arial';
                            ctx.fillStyle = '#666';
                            ctx.textAlign = 'center';
                            ctx.fillText('Tidak ada data untuk ditampilkan', canvas.width / 2, canvas.height / 2);
                        }
                    }
                })
                .catch(error => {
                    console.error('Error loading chart data:', error);
                    const canvas = document.getElementById('historyChart');
                    if (canvas) {
                        const ctx = canvas.getContext('2d');
                        ctx.clearRect(0, 0, canvas.width, canvas.height);
                        ctx.font = '16px Arial';
                        ctx.fillStyle = '#dc3545';
                        ctx.textAlign = 'center';
                        ctx.fillText('Error loading chart data', canvas.width / 2, canvas.height / 2);
                    }
                });
        }
        // ===== BASIC UI FUNCTIONS =====
        function toggleSidebar() {
            const sidebar = document.getElementById('sidebar');
            const toggleIcon = document.getElementById('toggleIcon');
            
            sidebar.classList.toggle('collapsed');
            
            if (sidebar.classList.contains('collapsed')) {
                toggleIcon.classList.remove('fa-bars');
                toggleIcon.classList.add('fa-chevron-right');
            } else {
                toggleIcon.classList.remove('fa-chevron-right');
                toggleIcon.classList.add('fa-bars');
            }
        }

        function showSection(sectionId, element) {
            // Hide all sections
            const sections = document.querySelectorAll('section');
            sections.forEach(section => {
                section.style.display = 'none';
            });
            
            // Show selected section
            document.getElementById(sectionId).style.display = 'block';
            
            // Update active menu item
            const menuItems = document.querySelectorAll('.sidebar a');
            menuItems.forEach(item => {
                item.classList.remove('active');
            });
            element.classList.add('active');
        }

        // ===== AUTO REFRESH PAGE =====
        function autoRefreshPage() {
            // Auto refresh setiap 30 detik untuk update data
            setTimeout(function() {
                window.location.reload();
            }, 30000);
        }

        // ===== INITIALIZE =====
        document.addEventListener('DOMContentLoaded', function() {
            console.log('Smart Energy Dashboard - Server-side Mode');
            
            // Start auto refresh
            autoRefreshPage();
            
            // Initialize AC control
            initializeACControl();
            
            // Add manual refresh button
            const navbar = document.querySelector('.navbar');
            if (navbar) {
                const refreshBtn = document.createElement('button');
                refreshBtn.innerHTML = '<i class="fas fa-sync-alt"></i> Refresh';
                refreshBtn.onclick = () => window.location.reload();
                refreshBtn.style.cssText = 'margin-left: auto; padding: 5px 10px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer;';
                navbar.appendChild(refreshBtn);
            }
        });

        // ===== AC CONTROL FUNCTIONS =====
        let currentACState = {
            mode: 'auto',
            ac1_status: false,
            ac2_status: false,
            ac1_temperature: 25,
            ac2_temperature: 25
        };

        function initializeACControl() {
            // Load current AC control status
            loadACControlStatus();
        }

        function loadACControlStatus() {
            fetch('/api/ac/control')
                .then(response => response.json())
                .then(data => {
                    if (data.success && data.data.control_mode !== 'auto') {
                        currentACState = {
                            mode: data.data.control_mode,
                            ac1_status: data.data.ac1_status,
                            ac2_status: data.data.ac2_status,
                            ac1_temperature: data.data.ac1_temperature,
                            ac2_temperature: data.data.ac2_temperature
                        };
                        updateACControlUI();
                        updateControlStatus(data.data);
                    }
                })
                .catch(error => {
                    console.log('No active AC control found, using auto mode');
                });
        }

        function setControlMode(mode) {
            const autoBtn = document.getElementById('autoModeBtn');
            const manualBtn = document.getElementById('manualModeBtn');
            const manualControls = document.getElementById('manualControls');
            const autoModeInfo = document.getElementById('autoModeInfo');

            if (mode === 'auto') {
                autoBtn.classList.add('active');
                manualBtn.classList.remove('active');
                manualControls.style.display = 'none';
                autoModeInfo.style.display = 'block';
                currentACState.mode = 'auto';
                
                // Return to auto mode via API
                returnToAutoMode();
            } else {
                autoBtn.classList.remove('active');
                manualBtn.classList.add('active');
                manualControls.style.display = 'block';
                autoModeInfo.style.display = 'none';
                currentACState.mode = 'manual';
            }
        }

        function toggleAC(unit) {
            const toggle = document.getElementById(`ac${unit}Toggle`);
            const status = document.getElementById(`ac${unit}Status`);
            
            if (unit === 1) {
                currentACState.ac1_status = !currentACState.ac1_status;
                toggle.classList.toggle('active', currentACState.ac1_status);
                status.textContent = currentACState.ac1_status ? 'ON' : 'OFF';
            } else {
                currentACState.ac2_status = !currentACState.ac2_status;
                toggle.classList.toggle('active', currentACState.ac2_status);
                status.textContent = currentACState.ac2_status ? 'ON' : 'OFF';
            }
        }

        function changeTemp(unit, delta) {
            const tempSpan = document.getElementById(`ac${unit}Temp`);
            let currentTemp = parseInt(tempSpan.textContent);
            let newTemp = currentTemp + delta;
            
            // Limit temperature range
            newTemp = Math.max(16, Math.min(30, newTemp));
            
            if (unit === 1) {
                currentACState.ac1_temperature = newTemp;
            } else {
                currentACState.ac2_temperature = newTemp;
            }
            
            tempSpan.textContent = newTemp;
        }

        function updateACControlUI() {
            // Update toggles
            document.getElementById('ac1Toggle').classList.toggle('active', currentACState.ac1_status);
            document.getElementById('ac2Toggle').classList.toggle('active', currentACState.ac2_status);
            
            // Update status text
            document.getElementById('ac1Status').textContent = currentACState.ac1_status ? 'ON' : 'OFF';
            document.getElementById('ac2Status').textContent = currentACState.ac2_status ? 'ON' : 'OFF';
            
            // Update temperatures
            document.getElementById('ac1Temp').textContent = currentACState.ac1_temperature;
            document.getElementById('ac2Temp').textContent = currentACState.ac2_temperature;
        }

        function applyACControl() {
            const duration = document.getElementById('controlDuration').value;
            
            const controlData = {
                device_id: 'ESP32_Smart_Energy_ChangeDetection',
                location: 'Lab Teknik Tegangan Tinggi',
                ac1_status: currentACState.ac1_status,
                ac2_status: currentACState.ac2_status,
                ac1_temperature: currentACState.ac1_temperature,
                ac2_temperature: currentACState.ac2_temperature,
                control_mode: 'manual',
                duration_minutes: duration ? parseInt(duration) : null,
                created_by: 'dashboard_user'
            };

            fetch('/api/ac/control', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content')
                },
                body: JSON.stringify(controlData)
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    alert('✓ Pengaturan AC berhasil diterapkan!');
                    updateControlStatus(data.data);
                } else {
                    alert('✗ Gagal menerapkan pengaturan: ' + data.message);
                }
            })
            .catch(error => {
                console.error('Error:', error);
                alert('✗ Terjadi kesalahan saat menerapkan pengaturan');
            });
        }

        function emergencyStopAC() {
            if (confirm('Apakah Anda yakin ingin mematikan semua AC secara darurat?')) {
                fetch('/api/ac/emergency-stop', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                        'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content')
                    },
                    body: JSON.stringify({
                        device_id: 'ESP32_Smart_Energy_ChangeDetection',
                        location: 'Lab Teknik Tegangan Tinggi'
                    })
                })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        alert('✓ Emergency stop berhasil! Semua AC dimatikan.');
                        // Reset UI
                        currentACState.ac1_status = false;
                        currentACState.ac2_status = false;
                        updateACControlUI();
                        updateControlStatus(data.data);
                    } else {
                        alert('✗ Gagal melakukan emergency stop: ' + data.message);
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    alert('✗ Terjadi kesalahan saat emergency stop');
                });
            }
        }

        function returnToAutoMode() {
            fetch('/api/ac/auto-mode', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content')
                },
                body: JSON.stringify({
                    device_id: 'ESP32_Smart_Energy_ChangeDetection',
                    location: 'Lab Teknik Tegangan Tinggi'
                })
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    updateControlStatus({
                        control_mode: 'auto',
                        manual_override: false,
                        message: 'Arduino mengontrol AC berdasarkan jumlah orang'
                    });
                }
            })
            .catch(error => {
                console.error('Error returning to auto mode:', error);
            });
        }

        function updateControlStatus(controlData) {
            const statusDisplay = document.getElementById('controlStatus');
            let statusText = '';
            
            if (controlData.control_mode === 'auto') {
                statusText = 'Mode: Otomatis | Arduino mengontrol AC berdasarkan jumlah orang';
            } else {
                const ac1Text = controlData.ac1_status ? `AC1: ON (${controlData.ac1_temperature}°C)` : 'AC1: OFF';
                const ac2Text = controlData.ac2_status ? `AC2: ON (${controlData.ac2_temperature}°C)` : 'AC2: OFF';
                statusText = `Mode: Manual | ${ac1Text}, ${ac2Text}`;
                
                if (controlData.expires_at) {
                    const expiryDate = new Date(controlData.expires_at);
                    statusText += ` | Berakhir: ${expiryDate.toLocaleString('id-ID')}`;
                }
            }
            
            statusDisplay.textContent = statusText;
        }

        // ===== INITIALIZE =====
        document.addEventListener('DOMContentLoaded', function() {
            console.log('Smart Energy Dashboard - Server-side Mode');
            
            // Start auto refresh
            autoRefreshPage();
            
            // Initialize AC control
            initializeACControl();
            
            // Initialize History section
            initializeHistory();
            
            // Add manual refresh button
            const navbar = document.querySelector('.navbar');
            if (navbar) {
                const refreshBtn = document.createElement('button');
                refreshBtn.innerHTML = '<i class="fas fa-sync-alt"></i> Refresh';
                refreshBtn.onclick = () => window.location.reload();
                refreshBtn.style.cssText = 'margin-left: auto; padding: 5px 10px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer;';
                navbar.appendChild(refreshBtn);
            }
        });
    </script>
    <script src="{{ asset('ac-control.js') }}"></script>
    <script src="{{ asset('script.js') }}"></script>
</body>
</html>