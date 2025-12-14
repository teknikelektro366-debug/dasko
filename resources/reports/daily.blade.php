@extends('layouts.app')

@section('title', 'Daily Energy Report')
@section('page-title', 'Daily Energy Report - ' . $date->format('d F Y'))

@section('content')
<div class="p-6 space-y-6">
    <!-- Header with Download Options -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <div class="flex justify-between items-center">
            <div>
                <h2 class="text-2xl font-bold text-gray-900 dark:text-white mb-2">
                    <i class="fas fa-calendar-day mr-3 text-blue-500"></i>Daily Energy Report
                </h2>
                <p class="text-gray-600 dark:text-gray-400">{{ $summary['date'] }}</p>
            </div>
            <div class="flex space-x-3">
                <a href="{{ route('reports.daily', ['date' => $date->format('Y-m-d'), 'format' => 'download']) }}" 
                   class="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
                    <i class="fas fa-download"></i>
                    <span>Download PDF</span>
                </a>
                <a href="{{ route('reports.index') }}" 
                   class="bg-gray-600 hover:bg-gray-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
                    <i class="fas fa-arrow-left"></i>
                    <span>Back to Reports</span>
                </a>
            </div>
        </div>
    </div>

    <!-- Date Navigation -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-4">
        <div class="flex justify-center items-center space-x-4">
            <a href="{{ route('reports.daily', ['date' => $date->copy()->subDay()->format('Y-m-d')]) }}" 
               class="bg-gray-100 dark:bg-gray-700 hover:bg-gray-200 dark:hover:bg-gray-600 px-3 py-2 rounded-lg flex items-center space-x-2">
                <i class="fas fa-chevron-left"></i>
                <span>Previous Day</span>
            </a>
            
            <form method="GET" class="flex items-center space-x-2">
                <input type="date" name="date" value="{{ $date->format('Y-m-d') }}" 
                       class="px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-white">
                <button type="submit" class="bg-blue-600 hover:bg-blue-700 text-white px-3 py-2 rounded-lg">
                    <i class="fas fa-search"></i>
                </button>
            </form>
            
            <a href="{{ route('reports.daily', ['date' => $date->copy()->addDay()->format('Y-m-d')]) }}" 
               class="bg-gray-100 dark:bg-gray-700 hover:bg-gray-200 dark:hover:bg-gray-600 px-3 py-2 rounded-lg flex items-center space-x-2">
                <span>Next Day</span>
                <i class="fas fa-chevron-right"></i>
            </a>
        </div>
    </div>

    <!-- Summary Cards -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-blue-100 dark:bg-blue-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-bolt text-blue-600 dark:text-blue-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Energy</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $summary['total_energy_kwh'] }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">kWh</p>
                </div>
            </div>
        </div>

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-green-100 dark:bg-green-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-money-bill text-green-600 dark:text-green-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Cost</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ number_format($summary['total_cost_idr']) }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">IDR</p>
                </div>
            </div>
        </div>

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-purple-100 dark:bg-purple-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-list text-purple-600 dark:text-purple-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Sessions</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $summary['logs_count'] }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Operations</p>
                </div>
            </div>
        </div>

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-yellow-100 dark:bg-yellow-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-chart-line text-yellow-600 dark:text-yellow-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Peak Hour</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $summary['peak_hour'] }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Highest Usage</p>
                </div>
            </div>
        </div>
    </div>

    <!-- Hourly Breakdown Chart -->
    @if($summary['hourly_breakdown']->isNotEmpty())
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-chart-bar mr-2 text-indigo-500"></i>Hourly Energy Consumption
        </h3>
        <div class="h-64">
            <canvas id="hourlyChart"></canvas>
        </div>
    </div>
    @endif

    <!-- Detailed Energy Logs -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-list-alt mr-2 text-green-500"></i>Detailed Energy Logs
        </h3>
        
        @if($energyLogs->isNotEmpty())
        <div class="overflow-x-auto">
            <table class="min-w-full divide-y divide-gray-200 dark:divide-gray-700">
                <thead class="bg-gray-50 dark:bg-gray-700">
                    <tr>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Time</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Device</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Energy (Wh)</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Duration</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">People</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Cost (IDR)</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Efficiency</th>
                    </tr>
                </thead>
                <tbody class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700">
                    @foreach($energyLogs as $log)
                    <tr class="hover:bg-gray-50 dark:hover:bg-gray-700">
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $log->start_time->format('H:i:s') }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap">
                            <div class="text-sm font-medium text-gray-900 dark:text-white">{{ $log->device->name }}</div>
                            <div class="text-sm text-gray-500 dark:text-gray-400">{{ $log->device->type }}</div>
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($log->energy_consumed, 2) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $log->duration_minutes }}m
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $log->people_count ?? '-' }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($log->cost_estimate) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap">
                            <span class="px-2 inline-flex text-xs leading-5 font-semibold rounded-full 
                                {{ $log->efficiency_rating === 'Excellent' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 
                                   ($log->efficiency_rating === 'Good' ? 'bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200' : 
                                    ($log->efficiency_rating === 'Average' ? 'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200' : 
                                     'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200')) }}">
                                {{ $log->efficiency_rating }}
                            </span>
                        </td>
                    </tr>
                    @endforeach
                </tbody>
            </table>
        </div>
        @else
        <div class="text-center py-8">
            <i class="fas fa-inbox text-gray-400 text-4xl mb-4"></i>
            <p class="text-gray-500 dark:text-gray-400">No energy logs found for this date</p>
        </div>
        @endif
    </div>
</div>

@push('scripts')
<script>
@if($summary['hourly_breakdown']->isNotEmpty())
// Hourly Chart
const hourlyCtx = document.getElementById('hourlyChart').getContext('2d');
const hourlyChart = new Chart(hourlyCtx, {
    type: 'bar',
    data: {
        labels: {!! json_encode($summary['hourly_breakdown']->pluck('hour')->values()) !!},
        datasets: [{
            label: 'Energy Consumption (Wh)',
            data: {!! json_encode($summary['hourly_breakdown']->pluck('energy')->values()) !!},
            backgroundColor: 'rgba(79, 70, 229, 0.6)',
            borderColor: 'rgba(79, 70, 229, 1)',
            borderWidth: 1
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            y: {
                beginAtZero: true,
                title: {
                    display: true,
                    text: 'Energy (Wh)'
                }
            },
            x: {
                title: {
                    display: true,
                    text: 'Hour of Day'
                }
            }
        },
        plugins: {
            legend: {
                display: false
            },
            tooltip: {
                callbacks: {
                    label: function(context) {
                        return 'Energy: ' + context.parsed.y.toFixed(2) + ' Wh';
                    }
                }
            }
        }
    }
});
@endif
</script>
@endpush
@endsection