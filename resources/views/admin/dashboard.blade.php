@extends('layouts.app')

@section('title', 'Admin Dashboard')
@section('page-title', 'Admin Dashboard')

@section('content')
<div class="p-6 space-y-6">
    <!-- System Status Cards -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <!-- Total Users -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-blue-100 dark:bg-blue-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-users text-blue-600 dark:text-blue-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Users</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $stats['total_users'] }}</p>
                    <p class="text-sm text-green-600 dark:text-green-400">{{ $stats['active_users'] }} active</p>
                </div>
            </div>
        </div>

        <!-- Total Devices -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-green-100 dark:bg-green-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-microchip text-green-600 dark:text-green-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Devices</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $stats['total_devices'] }}</p>
                    <p class="text-sm text-green-600 dark:text-green-400">{{ $stats['active_devices'] }} active</p>
                </div>
            </div>
        </div>

        <!-- Energy Today -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-yellow-100 dark:bg-yellow-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-bolt text-yellow-600 dark:text-yellow-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Energy Today</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ number_format($stats['total_energy_today'], 1) }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">kWh</p>
                </div>
            </div>
        </div>

        <!-- Arduino Status -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-purple-100 dark:bg-purple-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-microchip text-purple-600 dark:text-purple-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Arduino Status</p>
                    <p class="text-lg font-semibold text-gray-900 dark:text-white">
                        @if($arduinoStatus === 'connected')
                            <span class="text-green-600 dark:text-green-400">Connected</span>
                        @elseif($arduinoStatus === 'disconnected')
                            <span class="text-red-600 dark:text-red-400">Disconnected</span>
                        @else
                            <span class="text-yellow-600 dark:text-yellow-400">Unknown</span>
                        @endif
                    </p>
                    <div class="status-indicator {{ $arduinoStatus === 'connected' ? 'status-online' : 'status-offline' }}"></div>
                </div>
            </div>
        </div>
    </div>

    <!-- System Health Overview -->
    <div class="grid grid-cols-1 lg:grid-cols-2 gap-6">
        <!-- System Health -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-4">
                <i class="fas fa-heartbeat mr-2 text-red-500"></i>System Health
            </h3>
            <div class="space-y-4">
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Database</span>
                    <span class="px-2 py-1 text-xs rounded-full {{ $systemHealth['database'] === 'healthy' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200' }}">
                        {{ ucfirst($systemHealth['database']) }}
                    </span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Storage</span>
                    <span class="px-2 py-1 text-xs rounded-full {{ $systemHealth['storage'] === 'healthy' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : ($systemHealth['storage'] === 'warning' ? 'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200') }}">
                        {{ ucfirst($systemHealth['storage']) }}
                    </span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Cache</span>
                    <span class="px-2 py-1 text-xs rounded-full {{ $systemHealth['cache'] === 'healthy' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200' }}">
                        {{ ucfirst($systemHealth['cache']) }}
                    </span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Arduino Connection</span>
                    <span class="px-2 py-1 text-xs rounded-full {{ $systemHealth['arduino'] === 'connected' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200' }}">
                        {{ ucfirst($systemHealth['arduino']) }}
                    </span>
                </div>
            </div>
        </div>

        <!-- Quick Actions -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-4">
                <i class="fas fa-bolt mr-2 text-yellow-500"></i>Quick Actions
            </h3>
            <div class="grid grid-cols-2 gap-4">
                <button onclick="testArduinoConnection()" class="flex flex-col items-center p-4 bg-blue-50 dark:bg-blue-900/20 rounded-lg hover:bg-blue-100 dark:hover:bg-blue-900/30 transition-colors">
                    <i class="fas fa-wifi text-blue-600 dark:text-blue-400 text-xl mb-2"></i>
                    <span class="text-sm font-medium text-blue-700 dark:text-blue-300">Test Arduino</span>
                </button>
                
                <button onclick="syncArduino()" class="flex flex-col items-center p-4 bg-green-50 dark:bg-green-900/20 rounded-lg hover:bg-green-100 dark:hover:bg-green-900/30 transition-colors">
                    <i class="fas fa-sync text-green-600 dark:text-green-400 text-xl mb-2"></i>
                    <span class="text-sm font-medium text-green-700 dark:text-green-300">Sync Devices</span>
                </button>
                
                <button onclick="backupDatabase()" class="flex flex-col items-center p-4 bg-purple-50 dark:bg-purple-900/20 rounded-lg hover:bg-purple-100 dark:hover:bg-purple-900/30 transition-colors">
                    <i class="fas fa-download text-purple-600 dark:text-purple-400 text-xl mb-2"></i>
                    <span class="text-sm font-medium text-purple-700 dark:text-purple-300">Backup DB</span>
                </button>
                
                <button onclick="emergencyShutdown()" class="flex flex-col items-center p-4 bg-red-50 dark:bg-red-900/20 rounded-lg hover:bg-red-100 dark:hover:bg-red-900/30 transition-colors">
                    <i class="fas fa-power-off text-red-600 dark:text-red-400 text-xl mb-2"></i>
                    <span class="text-sm font-medium text-red-700 dark:text-red-300">Emergency</span>
                </button>
            </div>
        </div>
    </div>

    <!-- Recent Activities -->
    <div class="grid grid-cols-1 lg:grid-cols-2 gap-6">
        <!-- Recent Energy Logs -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center justify-between mb-4">
                <h3 class="text-lg font-semibold text-gray-900 dark:text-white">
                    <i class="fas fa-chart-line mr-2 text-green-500"></i>Recent Energy Logs
                </h3>
                <a href="{{ route('admin.energy-logs.index') }}" class="text-sm text-indigo-600 dark:text-indigo-400 hover:text-indigo-500">View All</a>
            </div>
            <div class="space-y-3">
                @forelse($recentLogs as $log)
                    <div class="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                        <div class="flex items-center space-x-3">
                            <div class="w-8 h-8 bg-blue-100 dark:bg-blue-900 rounded-full flex items-center justify-center">
                                <i class="fas fa-plug text-blue-600 dark:text-blue-400 text-sm"></i>
                            </div>
                            <div>
                                <p class="text-sm font-medium text-gray-900 dark:text-white">{{ $log->device->name }}</p>
                                <p class="text-xs text-gray-500 dark:text-gray-400">{{ $log->start_time->diffForHumans() }}</p>
                            </div>
                        </div>
                        <div class="text-right">
                            <p class="text-sm font-semibold text-gray-900 dark:text-white">{{ number_format($log->energy_consumed, 2) }} Wh</p>
                            <p class="text-xs text-gray-500 dark:text-gray-400">{{ $log->duration_minutes }}m</p>
                        </div>
                    </div>
                @empty
                    <p class="text-gray-500 dark:text-gray-400 text-center py-4">No recent energy logs</p>
                @endforelse
            </div>
        </div>

        <!-- Recent Users -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center justify-between mb-4">
                <h3 class="text-lg font-semibold text-gray-900 dark:text-white">
                    <i class="fas fa-users mr-2 text-blue-500"></i>Recent User Activity
                </h3>
                <a href="{{ route('admin.users.index') }}" class="text-sm text-indigo-600 dark:text-indigo-400 hover:text-indigo-500">View All</a>
            </div>
            <div class="space-y-3">
                @forelse($recentUsers as $user)
                    <div class="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-700 rounded-lg">
                        <div class="flex items-center space-x-3">
                            <img class="w-8 h-8 rounded-full" src="{{ $user->avatar_url }}" alt="{{ $user->name }}">
                            <div>
                                <p class="text-sm font-medium text-gray-900 dark:text-white">{{ $user->name }}</p>
                                <p class="text-xs text-gray-500 dark:text-gray-400">{{ $user->role_display }}</p>
                            </div>
                        </div>
                        <div class="text-right">
                            <p class="text-xs text-gray-500 dark:text-gray-400">
                                @if($user->last_login_at)
                                    {{ $user->last_login_at->diffForHumans() }}
                                @else
                                    Never logged in
                                @endif
                            </p>
                            <span class="px-2 py-1 text-xs rounded-full {{ $user->is_active ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200' }}">
                                {{ $user->status_display }}
                            </span>
                        </div>
                    </div>
                @empty
                    <p class="text-gray-500 dark:text-gray-400 text-center py-4">No recent user activity</p>
                @endforelse
            </div>
        </div>
    </div>
</div>

<!-- Loading Modal -->
<div id="loadingModal" class="fixed inset-0 bg-black bg-opacity-50 hidden items-center justify-center z-50">
    <div class="bg-white dark:bg-gray-800 rounded-lg p-6 max-w-sm w-full mx-4">
        <div class="flex items-center space-x-3">
            <i class="fas fa-spinner fa-spin text-indigo-600 text-xl"></i>
            <span class="text-gray-900 dark:text-white font-medium" id="loadingText">Processing...</span>
        </div>
    </div>
</div>
@endsection

@push('scripts')
<script>
function showLoading(text = 'Processing...') {
    document.getElementById('loadingText').textContent = text;
    document.getElementById('loadingModal').classList.remove('hidden');
    document.getElementById('loadingModal').classList.add('flex');
}

function hideLoading() {
    document.getElementById('loadingModal').classList.add('hidden');
    document.getElementById('loadingModal').classList.remove('flex');
}

function showNotification(message, type = 'success') {
    const notification = document.createElement('div');
    notification.className = `fixed top-4 right-4 p-4 rounded-lg shadow-lg z-50 ${
        type === 'success' ? 'bg-green-500' : 
        type === 'error' ? 'bg-red-500' : 
        'bg-blue-500'
    } text-white`;
    notification.innerHTML = `
        <div class="flex items-center">
            <i class="fas ${type === 'success' ? 'fa-check' : type === 'error' ? 'fa-times' : 'fa-info'} mr-2"></i>
            ${message}
        </div>
    `;
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.remove();
    }, 5000);
}

async function testArduinoConnection() {
    showLoading('Testing Arduino connection...');
    
    try {
        const response = await fetch('{{ route("admin.arduino.test") }}', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
            }
        });
        
        const result = await response.json();
        
        if (result.status === 'success') {
            showNotification('Arduino connection successful!', 'success');
        } else {
            showNotification('Arduino connection failed: ' + result.message, 'error');
        }
    } catch (error) {
        showNotification('Error testing Arduino connection', 'error');
    } finally {
        hideLoading();
    }
}

async function syncArduino() {
    showLoading('Syncing Arduino devices...');
    
    try {
        const response = await fetch('{{ route("admin.arduino.sync") }}', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
            }
        });
        
        const result = await response.json();
        
        if (result.status === 'success') {
            showNotification('Arduino sync completed successfully!', 'success');
        } else {
            showNotification('Arduino sync failed: ' + result.message, 'error');
        }
    } catch (error) {
        showNotification('Error syncing Arduino', 'error');
    } finally {
        hideLoading();
    }
}

async function backupDatabase() {
    showLoading('Creating database backup...');
    
    try {
        const response = await fetch('{{ route("admin.database.backup") }}', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
            }
        });
        
        const result = await response.json();
        
        if (result.status === 'success') {
            showNotification('Database backup created successfully!', 'success');
        } else {
            showNotification('Database backup failed: ' + result.message, 'error');
        }
    } catch (error) {
        showNotification('Error creating database backup', 'error');
    } finally {
        hideLoading();
    }
}

async function emergencyShutdown() {
    if (!confirm('Are you sure you want to perform an emergency shutdown? This will turn off all devices immediately.')) {
        return;
    }
    
    showLoading('Performing emergency shutdown...');
    
    try {
        const response = await fetch('{{ route("admin.arduino.emergency") }}', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
            }
        });
        
        const result = await response.json();
        
        if (result.status === 'success') {
            showNotification('Emergency shutdown completed!', 'success');
        } else {
            showNotification('Emergency shutdown failed: ' + result.message, 'error');
        }
    } catch (error) {
        showNotification('Error performing emergency shutdown', 'error');
    } finally {
        hideLoading();
    }
}
</script>
@endpush