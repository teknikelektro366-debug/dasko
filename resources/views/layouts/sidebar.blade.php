<div class="h-full bg-white dark:bg-gray-800 shadow-lg sidebar-transition">
    <div class="flex flex-col h-full">
        <!-- Logo -->
        <div class="flex items-center justify-center h-16 px-4 bg-indigo-600 dark:bg-indigo-700">
            <div class="flex items-center space-x-2">
                <i class="fas fa-bolt text-white text-xl"></i>
                <span x-show="sidebarOpen" class="text-white font-bold text-lg">Smart Energy</span>
            </div>
        </div>

        <!-- Navigation -->
        <nav class="flex-1 px-2 py-4 space-y-1 overflow-y-auto">
            @if(auth()->user()->isAdmin())
                <!-- Admin Menu -->
                <div class="space-y-1">
                    <div x-show="sidebarOpen" class="px-3 py-2 text-xs font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                        Admin Panel
                    </div>
                    
                    <a href="{{ route('admin.dashboard') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.dashboard') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-tachometer-alt mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">Admin Dashboard</span>
                    </a>

                    <a href="{{ route('admin.users.index') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.users.*') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-users mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">User Management</span>
                    </a>

                    <a href="{{ route('admin.devices.index') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.devices.*') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-microchip mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">Device Management</span>
                    </a>

                    <a href="{{ route('admin.ac-rules.index') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.ac-rules.*') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-snowflake mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">AC Automation</span>
                    </a>

                    <a href="{{ route('admin.energy-logs.index') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.energy-logs.*') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-chart-line mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">Energy Logs</span>
                    </a>

                    <a href="{{ route('admin.arduino') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.arduino') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-microchip mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">Arduino Control</span>
                    </a>

                    <a href="{{ route('admin.database') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.database') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-database mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">Database</span>
                    </a>

                    <a href="{{ route('admin.settings') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.settings') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-cog mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">System Settings</span>
                    </a>

                    <a href="{{ route('admin.logs') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('admin.logs') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                        <i class="fas fa-file-alt mr-3 text-lg"></i>
                        <span x-show="sidebarOpen">System Logs</span>
                    </a>
                </div>

                <div x-show="sidebarOpen" class="border-t border-gray-200 dark:border-gray-700 my-4"></div>
            @endif

            <!-- Main Dashboard Menu -->
            <div class="space-y-1">
                <div x-show="sidebarOpen" class="px-3 py-2 text-xs font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                    Dashboard
                </div>
                
                <a href="{{ route('dashboard') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('dashboard') && !request()->routeIs('admin.*') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-home mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">Overview</span>
                </a>

                <a href="{{ route('dashboard.analytics') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('dashboard.analytics') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-chart-bar mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">Analytics</span>
                </a>

                <a href="{{ route('dashboard.devices') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('dashboard.devices') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-plug mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">Devices</span>
                </a>

                <a href="{{ route('dashboard.automation') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('dashboard.automation') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-robot mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">Automation</span>
                </a>

                <a href="{{ route('dashboard.history') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('dashboard.history') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-history mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">History</span>
                </a>

                <a href="{{ route('dashboard.settings') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('dashboard.settings') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-sliders-h mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">Settings</span>
                </a>

                <a href="{{ route('reports.index') }}" class="group flex items-center px-2 py-2 text-sm font-medium rounded-md {{ request()->routeIs('reports.*') ? 'bg-indigo-100 dark:bg-indigo-900 text-indigo-700 dark:text-indigo-300' : 'text-gray-600 dark:text-gray-300 hover:bg-gray-50 dark:hover:bg-gray-700' }}">
                    <i class="fas fa-file-alt mr-3 text-lg"></i>
                    <span x-show="sidebarOpen">Reports</span>
                </a>
            </div>
        </nav>

        <!-- User Info -->
        <div class="flex-shrink-0 p-4 border-t border-gray-200 dark:border-gray-700">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <img class="h-8 w-8 rounded-full" src="{{ auth()->user()->avatar_url }}" alt="{{ auth()->user()->name }}">
                </div>
                <div x-show="sidebarOpen" class="ml-3">
                    <p class="text-sm font-medium text-gray-700 dark:text-gray-300">{{ auth()->user()->name }}</p>
                    <p class="text-xs text-gray-500 dark:text-gray-400">{{ auth()->user()->role_display }}</p>
                </div>
            </div>
        </div>
    </div>
</div>