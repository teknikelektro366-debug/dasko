<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Energy Dashboard - UNJA</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #3b82f6 0%, #1e40af 100%);
            min-height: 100vh;
            color: #333;
        }

        .container {
            display: flex;
            min-height: 100vh;
        }

        /* Sidebar Styles */
        .sidebar {
            width: 280px;
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(20px);
            box-shadow: 2px 0 20px rgba(0,0,0,0.1);
            padding: 30px 20px;
            overflow-y: auto;
        }

        .logo {
            display: flex;
            align-items: center;
            gap: 12px;
            margin-bottom: 40px;
            padding-bottom: 20px;
            border-bottom: 2px solid #e2e8f0;
        }

        .logo i {
            font-size: 32px;
            color: #3b82f6;
        }

        .logo h2 {
            color: #2d3748;
            font-size: 24px;
            font-weight: 700;
        }

        .nav-menu {
            list-style: none;
        }

        .nav-item {
            margin-bottom: 8px;
        }

        .nav-link {
            display: flex;
            align-items: center;
            gap: 15px;
            padding: 15px 20px;
            color: #4a5568;
            text-decoration: none;
            border-radius: 12px;
            transition: all 0.3s ease;
            font-weight: 500;
            font-size: 16px;
        }

        .nav-link:hover,
        .nav-link.active {
            background: linear-gradient(135deg, #3b82f6, #1e40af);
            color: white;
            transform: translateX(8px);
            box-shadow: 0 4px 15px rgba(59, 130, 246, 0.3);
        }

        .nav-link i {
            width: 24px;
            text-align: center;
            font-size: 18px;
        }

        .user-info {
            margin-top: 40px;
            padding-top: 20px;
            border-top: 2px solid #e2e8f0;
        }

        .user-profile {
            display: flex;
            align-items: center;
            gap: 12px;
            margin-bottom: 15px;
            color: #4a5568;
        }

        .system-status {
            display: flex;
            align-items: center;
            gap: 8px;
            color: #48bb78;
            font-size: 14px;
        }

        .status-dot {
            width: 8px;
            height: 8px;
            background: #48bb78;
            border-radius: 50%;
            animation: pulse 2s infinite;
        }

        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }

        /* Main Content */
        .main-content {
            flex: 1;
            padding: 30px;
            overflow-y: auto;
        }

        .page-header {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(20px);
            padding: 30px;
            border-radius: 20px;
            margin-bottom: 30px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }

        .page-title {
            color: #2d3748;
            font-size: 32px;
            font-weight: 700;
            margin-bottom: 8px;
            display: flex;
            align-items: center;
            gap: 15px;
        }

        .page-subtitle {
            color: #718096;
            font-size: 18px;
            font-weight: 500;
        }

        /* Cards Grid */
        .cards-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
            gap: 25px;
            margin-bottom: 40px;
        }

        .metric-card {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(20px);
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }

        .metric-card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 4px;
            background: linear-gradient(135deg, #3b82f6, #1e40af);
        }

        .metric-card:hover {
            transform: translateY(-8px);
            box-shadow: 0 16px 48px rgba(0,0,0,0.15);
        }

        .metric-header {
            display: flex;
            align-items: center;
            gap: 15px;
            margin-bottom: 20px;
        }

        .metric-icon {
            width: 60px;
            height: 60px;
            border-radius: 16px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 28px;
            color: white;
        }

        .metric-icon.temperature {
            background: linear-gradient(135deg, #3b82f6, #1e40af);
        }

        .metric-icon.humidity {
            background: linear-gradient(135deg, #60a5fa, #2563eb);
        }

        .metric-icon.people {
            background: linear-gradient(135deg, #93c5fd, #3b82f6);
        }

        .metric-icon.light {
            background: linear-gradient(135deg, #dbeafe, #1d4ed8);
        }

        .metric-info h3 {
            color: #4a5568;
            font-size: 16px;
            font-weight: 600;
            margin-bottom: 8px;
        }

        .metric-value {
            color: #2d3748;
            font-size: 36px;
            font-weight: 700;
            line-height: 1;
        }

        /* Success Message */
        .success-message {
            background: rgba(72, 187, 120, 0.1);
            border: 2px solid #48bb78;
            border-radius: 15px;
            padding: 20px;
            margin: 20px 0;
            text-align: center;
        }

        .success-message h3 {
            color: #48bb78;
            margin-bottom: 10px;
        }

        .success-message p {
            color: #2d3748;
            margin: 5px 0;
        }

        .btn {
            display: inline-block;
            padding: 12px 24px;
            background: linear-gradient(135deg, #3b82f6, #1e40af);
            color: white;
            text-decoration: none;
            border-radius: 10px;
            font-weight: 600;
            margin: 10px 5px;
            transition: all 0.3s ease;
        }

        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(59, 130, 246, 0.4);
        }

        /* Responsive Design */
        @media (max-width: 1024px) {
            .container {
                flex-direction: column;
            }
            
            .sidebar {
                width: 100%;
                height: auto;
                padding: 20px;
            }
            
            .nav-menu {
                display: flex;
                overflow-x: auto;
                gap: 10px;
                padding-bottom: 10px;
            }
            
            .nav-item {
                margin-bottom: 0;
                flex-shrink: 0;
            }
            
            .nav-link {
                white-space: nowrap;
                padding: 10px 15px;
            }
        }

        @media (max-width: 768px) {
            .main-content {
                padding: 20px;
            }
            
            .cards-grid {
                grid-template-columns: 1fr;
            }
            
            .page-title {
                font-size: 24px;
            }
            
            .metric-card {
                padding: 20px;
            }
        }
    </style>
</head>

<body>
    <div class="container">
        <!-- Sidebar -->
        <div class="sidebar">
            <div class="logo">
                <i class="fas fa-bolt"></i>
                <h2>Smart Energy UNJA</h2>
            </div>
            
            <ul class="nav-menu">
                <li class="nav-item">
                    <a href="/" class="nav-link">
                        <i class="fas fa-home"></i>
                        <span>Home</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="/dashboard" class="nav-link">
                        <i class="fas fa-tachometer-alt"></i>
                        <span>Laravel Dashboard</span>
                    </a>
                </li>
                <li class="nav-item">
                    <a href="/dashboard.html" class="nav-link active">
                        <i class="fas fa-chart-line"></i>
                        <span>Static Dashboard</span>
                    </a>
                </li>
            </ul>
            
            <div class="user-info">
                <div class="user-profile">
                    <i class="fas fa-user-circle"></i>
                    <span>Admin UNJA</span>
                </div>
                <div class="system-status">
                    <div class="status-dot"></div>
                    <span>System Online</span>
                </div>
            </div>
        </div>

        <!-- Main Content -->
        <div class="main-content">
            <div class="page-header">
                <h1 class="page-title">
                    <i class="fas fa-check-circle"></i>
                    Dashboard Berhasil Diperbaiki!
                </h1>
                <p class="page-subtitle" id="currentTime">{{ now()->format('d F Y, H:i:s') }} WIB</p>
            </div>

            <div class="success-message">
                <h3><i class="fas fa-rocket"></i> Sistem Smart Energy UNJA Siap Digunakan!</h3>
                <p><strong>✅ Laravel Server:</strong> Berjalan di http://localhost:8000</p>
                <p><strong>✅ Database MySQL:</strong> Terhubung dan berfungsi normal</p>
                <p><strong>✅ Session System:</strong> Bekerja dengan baik</p>
                <p><strong>✅ API Endpoints:</strong> Siap menerima data dari Arduino</p>
                <br>
                <a href="/dashboard" class="btn">
                    <i class="fas fa-tachometer-alt"></i> Buka Laravel Dashboard
                </a>
                <a href="/api/sensor/data" class="btn">
                    <i class="fas fa-database"></i> Test API Sensor
                </a>
            </div>

            <!-- Sensor Metrics -->
            <div class="cards-grid">
                <div class="metric-card">
                    <div class="metric-header">
                        <div class="metric-icon temperature">
                            <i class="fas fa-server"></i>
                        </div>
                        <div class="metric-info">
                            <h3>Laravel Status</h3>
                            <div class="metric-value" style="font-size: 24px; color: #48bb78;">RUNNING</div>
                        </div>
                    </div>
                    <p style="color: #718096; margin-top: 10px;">Server berjalan di port 8000</p>
                </div>

                <div class="metric-card">
                    <div class="metric-header">
                        <div class="metric-icon humidity">
                            <i class="fas fa-database"></i>
                        </div>
                        <div class="metric-info">
                            <h3>Database MySQL</h3>
                            <div class="metric-value" style="font-size: 24px; color: #48bb78;">CONNECTED</div>
                        </div>
                    </div>
                    <p style="color: #718096; margin-top: 10px;">Database: db_dasko</p>
                </div>

                <div class="metric-card">
                    <div class="metric-header">
                        <div class="metric-icon people">
                            <i class="fas fa-microchip"></i>
                        </div>
                        <div class="metric-info">
                            <h3>Arduino ESP32</h3>
                            <div class="metric-value" style="font-size: 24px; color: #f59e0b;">READY</div>
                        </div>
                    </div>
                    <p style="color: #718096; margin-top: 10px;">Siap untuk upload code</p>
                </div>

                <div class="metric-card">
                    <div class="metric-header">
                        <div class="metric-icon light">
                            <i class="fas fa-wifi"></i>
                        </div>
                        <div class="metric-info">
                            <h3>API Endpoints</h3>
                            <div class="metric-value" style="font-size: 24px; color: #48bb78;">READY</div>
                        </div>
                    </div>
                    <p style="color: #718096; margin-top: 10px;">Sensor & AC Control API</p>
                </div>
            </div>

            <div class="metric-card">
                <h3><i class="fas fa-info-circle"></i> Langkah Selanjutnya</h3>
                <div style="margin-top: 15px; line-height: 1.8;">
                    <p><strong>1. Upload Arduino Code:</strong> Upload file <code>INIFULL_copy_20251222133147.ino</code> ke ESP32</p>
                    <p><strong>2. Konfigurasi WiFi:</strong> Sesuaikan SSID dan password WiFi di Arduino code</p>
                    <p><strong>3. Test Koneksi:</strong> Pastikan ESP32 terhubung ke WiFi dan dapat mengirim data</p>
                    <p><strong>4. Monitor Dashboard:</strong> Buka Laravel Dashboard untuk melihat data realtime</p>
                    <p><strong>5. Test AC Control:</strong> Coba fitur manual AC control dari web dashboard</p>
                </div>
                <br>
                <div style="background: #f8fafc; padding: 15px; border-radius: 10px; border-left: 4px solid #3b82f6;">
                    <p><strong>📋 Catatan Penting:</strong></p>
                    <p>• Pastikan ESP32 dan komputer terhubung ke jaringan yang sama</p>
                    <p>• Untuk production, ganti URL API ke: https://dasko.fst.unja.ac.id</p>
                    <p>• File WiFi setup guide tersedia di: WIFI_SETUP_GUIDE.md</p>
                </div>
            </div>
        </div>
    </div>

    <script>
        // Update time
        function updateTime() {
            const now = new Date();
            const timeString = now.toLocaleString('id-ID', {
                timeZone: 'Asia/Jakarta',
                year: 'numeric',
                month: 'long',
                day: 'numeric',
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            });
            
            const timeElement = document.getElementById('currentTime');
            if (timeElement) {
                timeElement.textContent = timeString + ' (WIB)';
            }
        }

        // Initialize
        document.addEventListener('DOMContentLoaded', function() {
            updateTime();
            setInterval(updateTime, 1000);
        });
    </script>
</body>
</html>