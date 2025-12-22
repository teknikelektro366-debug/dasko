<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="csrf-token" content="{{ csrf_token() }}">
    <title>Smart Energy Dashboard - Realtime</title>
    <link rel="stylesheet" href="{{ asset('css/style.css') }}">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        .realtime-indicator {
            display: inline-block;
            width: 10px;
            height: 10px;
            background: #28a745;
            border-radius: 50%;
            margin-right: 8px;
            animation: blink 1s infinite;
        }
        
        @keyframes blink {
            0%, 50% { opacity: 1; }
            51%, 100% { opacity: 0.3; }
        }
        
        .data-timestamp {
            font-size: 0.8em;
            color: #666;
            margin-top: 5px;
        }
        
        .connection-status {
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 10px 15px;
            border-radius: 20px;
            color: white;
            font-size: 0.9em;
            z-index: 1000;
            transition: all 0.3s ease;
        }
        
        .connection-status.connected {
            background: #28a745;
        }
        
        .connection-status.disconnected {
            background: #dc3545;
        }
        
        .sensor-card.updating {
            border: 2px solid #007bff;
            box-shadow: 0 0 10px rgba(0, 123, 255, 0.3);
        }
        
        .chart-container {
            background: white;
            border-radius: 10px;
            padding: 20px;
            margin: 20px 0;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
    </style>
</head>

<body>
    <!-- Connection Status Indicator -->
    <div id="connectionStatus" class="connection-status disconnected">
        <i class="fas fa-wifi"></i> Connecting...
    </div>

    <!-- ===== SIDEBAR ===== -->
    <div class="sidebar" id="sidebar">
        <div class="sidebar-toggle" onclick="toggleSidebar()">
            <i class="fas fa-bars" id="toggleIcon"></i>
        </div>
        
        <div class="sidebar-content">
            <h2><i class="fas fa-home"></i> <span class="sidebar-text">Smart Energy</span></h2>
            <ul>
                <li><a href="#" onclick="showSection('monitoring', this)" class="active" title="Monitoring"><i class="fas fa-desktop"></i> <span class="sidebar-text">Monitoring</span></a></li>
                <li><a href="#" onclick="showSection('analytics', this)" title="Analisa"><i class="fas fa-chart-line"></i> <span class="sidebar-text">Analisa</span></a></li>
                <li><a href="#" onclick="showSection('history', this)" title="History"><i class="fas fa-history"></i> <span class="sidebar-text">History</span></a></li>
                <li><a href="#" onclick="showSection('devices', this)" title="Perangkat"><i class="fas fa-microchip"></i> <span class="sidebar-text">Perangkat</span></a></li>
                <li><a href="#" onclick="showSection('settings', this)" title="Pengaturan"><i class="fas fa-cog"></i> <span class="sidebar-text">Pengaturan</span></a></li>
            </ul>
            
            <div class="sidebar-footer">
                <div class="user-info">
                    <i class="fas fa-user-circle"></i>
                    <span class="sidebar-text">Admin User</span>
                </div>
                <div class="system-status">
                    <span class="realtime-indicator"></span>
                    <span class="sidebar-text">Realtime Active</span>
                </div>
            </div>
        </div>
    </div>

    <!-- ===== MAIN CONTENT ===== -->
    <div class="main-content">

        <!-- ===== Monitoring Section ===== -->
        <section id="monitoring">
            <div class="navbar">
                <i class="fas fa-building"></i> {{ $sensorData['location'] ?? 'Lab Teknik Tegangan Tinggi' }}
                <span class="realtime-indicator"></span>
                <span>Live Data</span>
            </div>
            <div class="subnav" id="waktuRealtime">
                <i class="fas fa-clock"></i> <span id="currentTime">{{ now()->format('d/m/Y H:i:s') }}</span>
                <span id="lastUpdate" class="data-timestamp">Last update: {{ $sensorData['time_ago'] ?? 'Never' }}</span>
            </div>

            <!-- Sensor Cards -->
            <div class="sensor-section">
                <h3><i class="fas fa-chart-line"></i> Data Sensor Real-time</h3>
                <div class="sensor-grid">
                    <div class="card sensor-card" id="tempCard">
                        <div class="card-icon">
                            <i class="fas fa-thermometer-half"></i>
                        </div>
                        <div class="title">Suhu Ruangan</div>
                        <div class="value" id="suhuValue">{{ $sensorData['formatted']['temperature'] ?? '27.0°C' }}</div>
                        <div class="data-timestamp" id="tempTimestamp">{{ $sensorData['time_ago'] ?? 'No data' }}</div>
                    </div>

                    <div class="card sensor-card" id="humidityCard">
                        <div class="card-icon">
                            <i class="fas fa-tint"></i>
                        </div>
                        <div class="title">Kelembaban</div>
                        <div class="value" id="humidityValue">{{ $sensorData['formatted']['humidity'] ?? '65.0%' }}</div>
                        <div class="data-timestamp" id="humidityTimestamp">{{ $sensorData['time_ago'] ?? 'No data' }}</div>
                    </div>

                    <div class="card sensor-card" id="peopleCard">
                        <div class="card-icon">
                            <i class="fas fa-users"></i>
                        </div>
                        <div class="title">Jumlah Orang</div>
                        <div class="value" id="jumlahOrang">{{ $sensorData['people_count'] ?? 0 }}</div>
                        <div class="data-timestamp" id="peopleTimestamp">{{ $sensorData['time_ago'] ?? 'No data' }}</div>
                    </div>

                    <div class="card sensor-card" id="lightCard">
                        <div class="card-icon">
                            <i class="fas fa-sun"></i>
                        </div>
                        <div class="title">Intensitas Cahaya</div>
                        <div class="value" id="ldrValue">{{ $sensorData['formatted']['light'] ?? '500 lux' }}</div>
                        <div class="data-timestamp" id="lightTimestamp">{{ $sensorData['time_ago'] ?? 'No data' }}</div>
                    </div>
                </div>
            </div>

            <!-- AC Status Section -->
            <div class="s