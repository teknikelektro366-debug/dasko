@extends('layouts.app')

@section('title', 'Dashboard')
@section('page-title', 'Smart Energy Dashboard')

@section('content')
<div class="p-6 space-y-6">
    <!-- Real-time Sensor Data -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <!-- Temperature -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-red-100 dark:bg-red-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-thermometer-half text-red-600 dark:text-red-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Temperature</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white" data-sensor="temperature">{{ $sensorData['temperature'] ?? 27.0 }}°C</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Outdoor</p>
                </div>
            </div>
        </div>

        <!-- Humidity -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-blue-100 dark:bg-blue-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-tint text-blue-600 dark:text-blue-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Humidity</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white" data-sensor="humidity">{{ $sensorData['humidity'] ?? 65.0 }}%</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Relative</p>
                </div>
            </div>
        </div>

        <!-- People Count -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-green-100 dark:bg-green-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-users text-green-600 dark:text-green-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">People Count</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white" data-sensor="people_count">{{ $sensorData['people_count'] ?? 0 }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">In Lab</p>
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
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ number_format($todayEnergy / 1000, 2) }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">kWh</p>
                </div>
            </div>
        </div>
    </div>

    <!-- System Status -->
    <div class="grid grid-cols-1 lg:grid-cols-2 gap-6">
        <!-- Working Hours Status -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-4">
                <i class="fas fa-clock mr-2 text-blue-500"></i>Working Hours Status
            </h3>
            <div class="space-y-4">
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Current Status</span>
                    <span class="px-3 py-1 text-sm rounded-full {{ $workingHoursStatus['is_working_hours'] ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200' }}">
                        {{ $workingHoursStatus['status'] }}
                    </span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Current Time</span>
                    <span class="text-sm font-medium text-gray-900 dark:text-white">{{ $workingHoursStatus['current_time'] }}</span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Working Hours</span>
                    <span class="text-sm font-medium text-gray-900 dark:text-white">{{ $workingHoursStatus['working_range'] }}</span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Day</span>
                    <span class="text-sm font-medium text-gray-900 dark:text-white">{{ $workingHoursStatus['current_day'] }}</span>
                </div>
            </div>
        </div>

        <!-- Energy Saving Status -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-4">
                <i class="fas fa-leaf mr-2 text-green-500"></i>Energy Saving Status
            </h3>
            <div class="space-y-4">
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Current Mode</span>
                    <span class="px-3 py-1 text-sm rounded-full {{ $energySavingStatus['current_state'] === 'Normal Operation' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : ($energySavingStatus['current_state'] === 'Energy Saving Mode' ? 'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200' : 'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200') }}">
                        {{ $energySavingStatus['current_state'] }}
                    </span>
                </div>
                <div class="flex items-center justify-between">
                    <span class="text-sm text-gray-600 dark:text-gray-400">Minutes Without People</span>
                    <span class="text-sm font-medium text-gray-900 dark:text-white">{{ $energySavingStatus['minutes_without_people'] }}</span>
                </div>
                @if($energySavingStatus['time_to_energy_saving'] > 0)
                    <div class="flex items-center justify-between">
                        <span class="text-sm text-gray-600 dark:text-gray-400">Time to Energy Saving</span>
                        <span class="text-sm font-medium text-gray-900 dark:text-white">{{ $energySavingStatus['time_to_energy_saving'] }}m</span>
                    </div>
                @endif
                @if($energySavingStatus['time_to_shutdown'] > 0)
                    <div class="flex items-center justify-between">
                        <span class="text-sm text-gray-600 dark:text-gray-400">Time to Shutdown</span>
                        <span class="text-sm font-medium text-gray-900 dark:text-white">{{ $energySavingStatus['time_to_shutdown'] }}m</span>
                    </div>
                @endif
            </div>
        </div>
    </div>

    <!-- Device Status -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-plug mr-2 text-purple-500"></i>Device Status
        </h3>
        <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-4">
            @foreach($devices as $device)
                <div class="bg-gray-50 dark:bg-gray-700 rounded-lg p-4">
                    <div class="flex items-center justify-between mb-2">
                        <h4 class="text-sm font-medium text-gray-900 dark:text-white">{{ $device->name }}</h4>
                        <div class="status-indicator {{ $device->is_active ? 'status-online' : 'status-offline' }}" data-device-status="{{ $device->type }}"></div>
                    </div>
                    <div class="space-y-1">
                        <div class="flex justify-between text-xs">
                            <span class="text-gray-500 dark:text-gray-400">Status</span>
                            <span class="font-medium {{ $device->is_active ? 'text-green-600 dark:text-green-400' : 'text-red-600 dark:text-red-400' }}">
                                {{ $device->is_active ? 'ON' : 'OFF' }}
                            </span>
                        </div>
                        <div class="flex justify-between text-xs">
                            <span class="text-gray-500 dark:text-gray-400">Power</span>
                            <span class="text-gray-900 dark:text-white">{{ $device->energy_consumption }}W</span>
                        </div>
                        <div class="flex justify-between text-xs">
                            <span class="text-gray-500 dark:text-gray-400">Today</span>
                            <span class="text-gray-900 dark:text-white">{{ number_format($device->total_energy_today / 1000, 2) }}kWh</span>
                        </div>
                    </div>
                    @if(auth()->user()->canManageDevices() && !$device->is_always_on)
                        <button onclick="toggleDevice('{{ $device->type }}')" class="mt-3 w-full text-xs bg-indigo-600 hover:bg-indigo-700 text-white py-1 px-2 rounded">
                            Toggle
                        </button>
                    @endif
                </div>
            @endforeach
        </div>
    </div>

    <!-- AC Automation Rules -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-snowflake mr-2 text-blue-500"></i>AC Automation Rules
        </h3>
        <div class="overflow-x-auto">
            <table class="min-w-full divide-y divide-gray-200 dark:divide-gray-700">
                <thead class="bg-gray-50 dark:bg-gray-700">
                    <tr>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">People Count</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Outdoor Temp</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">AC Units</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Target Temp</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Description</th>
                    </tr>
                </thead>
                <tbody class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700">
                    @foreach($acRules as $rule)
                        <tr class="hover:bg-gray-50 dark:hover:bg-gray-700 {{ $rule->matches($sensorData['people_count'] ?? 0, $sensorData['temperature'] ?? 27) ? 'bg-blue-50 dark:bg-blue-900/20 border-l-4 border-blue-500' : '' }}">
                            <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                                {{ $rule->min_people }}-{{ $rule->max_people }}
                            </td>
                            <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                                {{ $rule->min_outdoor_temp }}°C - {{ $rule->max_outdoor_temp }}°C
                            </td>
                            <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                                {{ $rule->ac_units_needed }}
                            </td>
                            <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                                {{ $rule->target_temperature }}°C
                            </td>
                            <td class="px-6 py-4 text-sm text-gray-900 dark:text-white">
                                {{ $rule->description }}
                            </td>
                        </tr>
                    @endforeach
                </tbody>
            </table>
        </div>
    </div>
</div>

@push('scripts')
<script>
async function toggleDevice(deviceType) {
    try {
        const response = await fetch('/api/devices/' + deviceType + '/control', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content,
                'Authorization': 'Bearer ' + '{{ auth()->user()->createToken("device-control")->plainTextToken ?? "" }}'
            },
            body: JSON.stringify({
                action: 'toggle'
            })
        });
        
        const result = await response.json();
        
        if (result.status === 'success') {
            // Update UI
            location.reload();
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        alert('Error controlling device');
    }
}

// Update real-time data every 5 seconds
setInterval(() => {
    fetch('/api/arduino/sensor-data')
        .then(response => response.json())
        .then(data => {
            // Update sensor readings
            document.querySelector('[data-sensor="temperature"]').textContent = data.temperature + '°C';
            document.querySelector('[data-sensor="humidity"]').textContent = data.humidity + '%';
            document.querySelector('[data-sensor="people_count"]').textContent = data.people_count;
            
            // Update device status indicators
            if (data.devices) {
                Object.keys(data.devices).forEach(deviceType => {
                    const indicator = document.querySelector(`[data-device-status="${deviceType}"]`);
                    if (indicator) {
                        indicator.className = data.devices[deviceType] ? 
                            'status-indicator status-online' : 
                            'status-indicator status-offline';
                    }
                });
            }
        })
        .catch(error => console.error('Error updating sensor data:', error));
}, 5000);
</script>
@endpush
@endsection