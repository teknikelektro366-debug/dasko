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
    
    <!-- Custom CSS -->
    <link rel="stylesheet" href="{{ asset('css/style.css') }}">
    
    <!-- Alpine.js -->
    <script defer src="https://unpkg.com/alpinejs@3.x.x/dist/cdn.min.js"></script>


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
                <li><a href="#" onclick="showSection('Kontrol', this)" class="active" title="Monitoring"><i class="fas fa-desktop"></i> <span class="sidebar-text">Monitoring</span></a></li>
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