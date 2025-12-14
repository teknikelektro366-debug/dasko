<!DOCTYPE html>
<html lang="{{ str_replace('_', '-', app()->getLocale()) }}">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="csrf-token" content="{{ csrf_token() }}">

    <title>@yield('title', 'Dashboard') - {{ config('app.name', 'Smart Energy Management') }}</title>

    <!-- Fonts -->
    <link rel="preconnect" href="https://fonts.bunny.net">
    <link href="https://fonts.bunny.net/css?family=figtree:400,500,600&display=swap" rel="stylesheet" />
    
    <!-- Font Awesome -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    
    <!-- Chart.js -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    
    <!-- Tailwind CSS -->
    <script src="https://cdn.tailwindcss.com"></script>
    
    <!-- Alpine.js -->
    <script defer src="https://unpkg.com/alpinejs@3.x.x/dist/cdn.min.js"></script>

    <style>
        .sidebar {
            width: 250px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            position: fixed;
            height: 100vh;
            left: 0;
            top: 0;
            transition: all 0.3s ease;
            z-index: 1000;
            overflow-y: auto;
        }
        
        .sidebar.collapsed {
            width: 70px;
        }
        
        .sidebar-toggle {
            padding: 15px;
            text-align: center;
            cursor: pointer;
            border-bottom: 1px solid rgba(255,255,255,0.1);
        }
        
        .sidebar-content h2 {
            padding: 20px;
            margin: 0;
            font-size: 1.2rem;
            border-bottom: 1px solid rgba(255,255,255,0.1);
        }
        
        .sidebar ul {
            list-style: none;
            padding: 0;
            margin: 0;
        }
        
        .sidebar ul li {
            border-bottom: 1px solid rgba(255,255,255,0.1);
        }
        
        .sidebar ul li a {
            display: flex;
            align-items: center;
            padding: 15px 20px;
            color: rgba(255,255,255,0.8);
            text-decoration: none;
            transition: all 0.3s ease;
        }
        
        .sidebar ul li a:hover,
        .sidebar ul li a.active {
            background: rgba(255,255,255,0.1);
            color: white;
        }
        
        .sidebar ul li a i {
            width: 20px;
            margin-right: 15px;
            text-align: center;
        }
        
        .sidebar-footer {
            position: absolute;
            bottom: 0;
            width: 100%;
            padding: 20px;
            border-top: 1px solid rgba(255,255,255,0.1);
        }
        
        .user-info, .system-status {
            display: flex;
            align-items: center;
            margin-bottom: 10px;
            font-size: 0.9rem;
        }
        
        .user-info i, .system-status i {
            margin-right: 10px;
        }
        
        .status-online {
            color: #10b981;
        }
        
        .main-content {
            margin-left: 250px;
            transition: all 0.3s ease;
            min-height: 100vh;
        }
        
        .main-content.expanded {
            margin-left: 70px;
        }
        
        .navbar {
            background: white;
            padding: 15px 30px;
            border-bottom: 1px solid #e5e7eb;
            font-weight: 600;
            color: #374151;
        }
        
        .subnav {
            background: #f9fafb;
            padding: 10px 30px;
            border-bottom: 1px solid #e5e7eb;
            color: #6b7280;
            font-size: 0.9rem;
        }
        
        .card-hover {
            transition: transform 0.2s ease-in-out, box-shadow 0.2s ease-in-out;
        }
        
        .card-hover:hover {
            transform: translateY(-2px);
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.1);
        }
        
        .status-indicator {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            display: inline-block;
        }
        
        .status-online {
            background-color: #10b981;
            box-shadow: 0 0 0 2px rgba(16, 185, 129, 0.3);
        }
        
        .status-offline {
            background-color: #ef4444;
            box-shadow: 0 0 0 2px rgba(239, 68, 68, 0.3);
        }
        
        .sidebar-text {
            transition: opacity 0.3s ease;
        }
        
        .sidebar.collapsed .sidebar-text {
            opacity: 0;
            width: 0;
            overflow: hidden;
        }
        
        .sidebar.collapsed ul li a {
            justify-content: center;
            padding: 15px 10px;
        }
        
        .sidebar.collapsed ul li a i {
            margin-right: 0;
        }
        
        .sidebar.collapsed .sidebar-footer {
            text-align: center;
        }
        
        .sidebar.collapsed .sidebar-footer .sidebar-text {
            display: none;
        }
        
        /* Section visibility */
        .content-section {
            display: none;
        }
        
        .content-section.active {
            display: block;
        }
        
        /* Custom styles for cards and components */
        .sensor-grid, .status-grid, .device-grid {
            display: grid;
            gap: 20px;
            margin-top: 20px;
        }
        
        .sensor-grid {
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
        }
        
        .status-grid {
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
        }
        
        .device-grid {
            grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
        }
        
        .card {
            background: white;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            border: 1px solid #e5e7eb;
        }
        
        .sensor-card .card-icon {
            width: 50px;
            height: 50px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-bottom: 15px;
            font-size: 1.5rem;
        }
        
        .sensor-card .title {
            font-weight: 600;
            color: #374151;
            margin-bottom: 10px;
        }
        
        .sensor-card .value {
            font-size: 2rem;
            font-weight: bold;
            color: #1f2937;
        }
        
        .toggle-switch {
            position: relative;
            width: 50px;
            height: 24px;
            background: #ccc;
            border-radius: 12px;
            cursor: pointer;
            transition: background 0.3s;
        }
        
        .toggle-switch.active {
            background: #10b981;
        }
        
        .toggle-slider {
            position: absolute;
            top: 2px;
            left: 2px;
            width: 20px;
            height: 20px;
            background: white;
            border-radius: 50%;
            transition: transform 0.3s;
        }
        
        .toggle-switch.active .toggle-slider {
            transform: translateX(26px);
        }
    </style>
</head>
<body class="bg-gray-100">
    <!-- Sidebar -->
    <div class="sidebar" id="sidebar">
        <!-- Toggle Button -->
        <div class="sidebar-toggle" onclick="toggleSidebar()">
            <i class="fas fa-bars" id="toggleIcon"></i>
        </div>
        
        <div class="sidebar-content">
            <h2><i class="fas fa-home"></i> <span class="sidebar-text">Smart Energy</span></h2>
            <ul>
                <li><a href="#" onclick="showSection('monitoring', this)" class="active" title="Monitoring"><i class="fas fa-desktop"></i> <span class="sidebar-text">Monitoring</span></a></li>
                <li><a href="#" onclick="showSection('analytics', this)" title="Analisa"><i class="fas fa-chart-line"></i> <span class="sidebar-text">Analisa</span></a></li>
                <li><a href="#" onclick="showSection('history', this)" title="History"><i class="fas fa-history"></i> <span class="sidebar-text">History</span></a></li>
                <li><a href="#" onclick="showSection('reports', this)" title="Laporan"><i class="fas fa-file-alt"></i> <span class="sidebar-text">Laporan</span></a></li>
                <li><a href="#" onclick="showSection('devices', this)" title="Perangkat"><i class="fas fa-microchip"></i> <span class="sidebar-text">Perangkat</span></a></li>
                <li><a href="#" onclick="showSection('automation', this)" title="Otomasi"><i class="fas fa-robot"></i> <span class="sidebar-text">Otomasi</span></a></li>
                <li><a href="#" onclick="showSection('alerts', this)" title="Notifikasi"><i class="fas fa-bell"></i> <span class="sidebar-text">Notifikasi</span></a></li>
                <li><a href="#" onclick="showSection('settings', this)" title="Pengaturan"><i class="fas fa-cog"></i> <span class="sidebar-text">Pengaturan</span></a></li>
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

    <!-- Main Content -->
    <div class="main-content" id="mainContent">
        @yield('content')
    </div>

    @stack('scripts')
    
    <script>
        function toggleSidebar() {
            const sidebar = document.getElementById('sidebar');
            const mainContent = document.getElementById('mainContent');
            const toggleIcon = document.getElementById('toggleIcon');
            
            sidebar.classList.toggle('collapsed');
            mainContent.classList.toggle('expanded');
            
            if (sidebar.classList.contains('collapsed')) {
                toggleIcon.classList.remove('fa-bars');
                toggleIcon.classList.add('fa-chevron-right');
            } else {
                toggleIcon.classList.remove('fa-chevron-right');
                toggleIcon.classList.add('fa-bars');
            }
        }
        
        function showSection(sectionName, element) {
            // Hide all sections
            const sections = document.querySelectorAll('.content-section');
            sections.forEach(section => {
                section.classList.remove('active');
            });
            
            // Show selected section
            const targetSection = document.getElementById(sectionName);
            if (targetSection) {
                targetSection.classList.add('active');
            }
            
            // Update active menu item
            const menuItems = document.querySelectorAll('.sidebar ul li a');
            menuItems.forEach(item => {
                item.classList.remove('active');
            });
            element.classList.add('active');
        }
        
        // Update real-time clock
        function updateClock() {
            const now = new Date();
            const timeString = now.toLocaleTimeString('id-ID', {
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            });
            const dayString = now.toLocaleDateString('id-ID', {
                weekday: 'long',
                year: 'numeric',
                month: 'long',
                day: 'numeric'
            });
            
            const clockElement = document.getElementById('waktuRealtime');
            if (clockElement) {
                clockElement.innerHTML = `<i class="fas fa-clock"></i> ${timeString} - ${dayString}`;
            }
        }
        
        // Update clock every second
        setInterval(updateClock, 1000);
        updateClock(); // Initial call
    </script>
</body>
</html>