@extends('layouts.app')

@section('title', 'Monthly Energy Report')
@section('page-title', 'Monthly Energy Report')

@section('content')
<div class="p-6 space-y-6">
    <!-- Header with Download Options -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <div class="flex justify-between items-center">
            <div>
                <h2 class="text-2xl font-bold text-gray-900 dark:text-white mb-2">
                    <i class="fas fa-calendar-alt mr-3 text-purple-500"></i>Monthly Energy Report
                </h2>
                <p class="text-gray-600 dark:text-gray-400">{{ $summary['month'] }}</p>
            </div>
            <div class="flex space-x-3">
                <a href="{{ route('reports.monthly', ['month' => $startDate->format('Y-m'), 'format' => 'download']) }}" 
                   class="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
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

    <!-- Month Navigation -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-4">
        <div class="flex justify-center items-center space-x-4">
            <a href="{{ route('reports.monthly', ['month' => $startDate->copy()->subMonth()->format('Y-m')]) }}" 
               class="bg-gray-100 dark:bg-gray-700 hover:bg-gray-200 dark:hover:bg-gray-600 px-3 py-2 rounded-lg flex items-center space-x-2">
                <i class="fas fa-chevron-left"></i>
                <span>Previous Month</span>
            </a>
            
            <form method="GET" class="flex items-center space-x-2">
                <input type="month" name="month" value="{{ $startDate->format('Y-m') }}" 
                       class="px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-purple-500 dark:bg-gray-700 dark:text-white">
                <button type="submit" class="bg-purple-600 hover:bg-purple-700 text-white px-3 py-2 rounded-lg">
                    <i class="fas fa-search"></i>
                </button>
            </form>
            
            <a href="{{ route('reports.monthly', ['month' => $startDate->copy()->addMonth()->format('Y-m')]) }}" 
               class="bg-gray-100 dark:bg-gray-700 hover:bg-gray-200 dark:hover:bg-gray-600 px-3 py-2 rounded-lg flex items-center space-x-2">
                <span>Next Month</span>
                <i class="fas fa-chevron-right"></i>
            </a>
        </div>
    </div>

    <!-- Summary Cards -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-purple-100 dark:bg-purple-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-bolt text-purple-600 dark:text-purple-400 text-xl"></i>
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
                    <div class="w-12 h-12 bg-blue-100 dark:bg-blue-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-chart-line text-blue-600 dark:text-blue-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Avg Daily</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ number_format($summary['avg_daily_energy'], 1) }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Wh/day</p>
                </div>
            </div>
        </div>

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-yellow-100 dark:bg-yellow-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-list text-yellow-600 dark:text-yellow-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Sessions</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $summary['logs_count'] }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Operations</p>
                </div>
            </div>
        </div>
    </div>

    <!-- Weekly Breakdown Chart -->
    @if($summary['weekly_breakdown']->isNotEmpty())
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-chart-area mr-2 text-indigo-500"></i>Weekly Energy Consumption Trend
        </h3>
        <div class="h-64">
            <canvas id="weeklyChart"></canvas>
        </div>
    </div>
    @endif

    <!-- Weekly Breakdown Table -->
    @if($summary['weekly_breakdown']->isNotEmpty())
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-table mr-2 text-purple-500"></i>Weekly Breakdown
        </h3>
        <div class="overflow-x-auto">
            <table class="min-w-full divide-y divide-gray-200 dark:divide-gray-700">
                <thead class="bg-gray-50 dark:bg-gray-700">
                    <tr>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Week</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Period</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Energy (kWh)</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Cost (IDR)</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Sessions</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Percentage</th>
                    </tr>
                </thead>
                <tbody class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700">
                    @php
                        $totalMonthEnergy = $summary['weekly_breakdown']->sum('energy');
                    @endphp
                    @foreach($summary['weekly_breakdown'] as $weekKey => $data)
                    <tr class="hover:bg-gray-50 dark:hover:bg-gray-700">
                        <td class="px-6 py-4 whitespace-nowrap text-sm font-medium text-gray-900 dark:text-white">
                            Week {{ $loop->iteration }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $data['period'] }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($data['energy'] / 1000, 3) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($data['cost']) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $data['count'] }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $totalMonthEnergy > 0 ? number_format(($data['energy'] / $totalMonthEnergy) * 100, 1) : 0 }}%
                        </td>
                    </tr>
                    @endforeach
                </tbody>
            </table>
        </div>
    </div>
    @endif

    <!-- Device Performance Analysis -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-microchip mr-2 text-green-500"></i>Device Performance Analysis
        </h3>
        
        @php
            $deviceStats = $energyLogs->groupBy('device.type')->map(function ($logs) {
                return [
                    'device_name' => $logs->first()->device->name,
                    'total_energy' => $logs->sum('energy_consumed'),
                    'total_cost' => $logs->sum('cost_estimate'),
                    'sessions' => $logs->count(),
                    'avg_energy' => $logs->avg('energy_consumed'),
                    'total_runtime' => $logs->sum('duration_minutes')
                ];
            });
        @endphp
        
        @if($deviceStats->isNotEmpty())
        <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4 mb-6">
            @foreach($deviceStats as $deviceType => $stats)
            <div class="bg-gray-50 dark:bg-gray-700 rounded-lg p-4">
                <h4 class="font-medium text-gray-900 dark:text-white mb-2">{{ $stats['device_name'] }}</h4>
                <div class="space-y-1 text-sm">
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Energy:</span>
                        <span class="font-medium text-gray-900 dark:text-white">{{ number_format($stats['total_energy'] / 1000, 2) }} kWh</span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Cost:</span>
                        <span class="font-medium text-gray-900 dark:text-white">Rp {{ number_format($stats['total_cost']) }}</span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Sessions:</span>
                        <span class="font-medium text-gray-900 dark:text-white">{{ $stats['sessions'] }}</span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Runtime:</span>
                        <span class="font-medium text-gray-900 dark:text-white">{{ number_format($stats['total_runtime'] / 60, 1) }}h</span>
                    </div>
                </div>
            </div>
            @endforeach
        </div>
        @endif
    </div>

    <!-- Monthly Statistics -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-chart-pie mr-2 text-blue-500"></i>Monthly Statistics
        </h3>
        
        <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
            <div>
                <h4 class="font-medium text-gray-900 dark:text-white mb-4">Key Metrics</h4>
                <div class="space-y-3">
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Average Energy per Session:</span>
                        <span class="font-medium text-gray-900 dark:text-white">
                            {{ $summary['logs_count'] > 0 ? number_format($summary['total_energy_kwh'] * 1000 / $summary['logs_count'], 1) : 0 }} Wh
                        </span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Cost per kWh:</span>
                        <span class="font-medium text-gray-900 dark:text-white">
                            Rp {{ $summary['total_energy_kwh'] > 0 ? number_format($summary['total_cost_idr'] / $summary['total_energy_kwh']) : 0 }}
                        </span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Energy Efficiency:</span>
                        <span class="font-medium text-gray-900 dark:text-white">
                            @php
                                $excellentCount = $energyLogs->where('efficiency_rating', 'Excellent')->count();
                                $totalWithRating = $energyLogs->whereNotNull('efficiency_rating')->count();
                                $efficiencyPercentage = $totalWithRating > 0 ? ($excellentCount / $totalWithRating) * 100 : 0;
                            @endphp
                            {{ number_format($efficiencyPercentage, 1) }}% Excellent
                        </span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Working Hours Usage:</span>
                        <span class="font-medium text-gray-900 dark:text-white">
                            {{ $summary['logs_count'] > 0 ? number_format(($energyLogs->where('working_hours', true)->count() / $summary['logs_count']) * 100, 1) : 0 }}%
                        </span>
                    </div>
                </div>
            </div>
            
            <div>
                <h4 class="font-medium text-gray-900 dark:text-white mb-4">Comparison</h4>
                <div class="space-y-3">
                    @php
                        $previousMonth = $startDate->copy()->subMonth();
                        $previousMonthLogs = \App\Models\EnergyLog::whereBetween('start_time', [
                            $previousMonth->startOfMonth(), 
                            $previousMonth->endOfMonth()
                        ])->get();
                        $previousEnergy = $previousMonthLogs->sum('energy_consumed') / 1000;
                        $previousCost = $previousMonthLogs->sum('cost_estimate');
                        
                        $energyChange = $previousEnergy > 0 ? (($summary['total_energy_kwh'] - $previousEnergy) / $previousEnergy) * 100 : 0;
                        $costChange = $previousCost > 0 ? (($summary['total_cost_idr'] - $previousCost) / $previousCost) * 100 : 0;
                    @endphp
                    
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">vs Previous Month (Energy):</span>
                        <span class="font-medium {{ $energyChange >= 0 ? 'text-red-600 dark:text-red-400' : 'text-green-600 dark:text-green-400' }}">
                            {{ $energyChange >= 0 ? '+' : '' }}{{ number_format($energyChange, 1) }}%
                        </span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">vs Previous Month (Cost):</span>
                        <span class="font-medium {{ $costChange >= 0 ? 'text-red-600 dark:text-red-400' : 'text-green-600 dark:text-green-400' }}">
                            {{ $costChange >= 0 ? '+' : '' }}{{ number_format($costChange, 1) }}%
                        </span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Days in Month:</span>
                        <span class="font-medium text-gray-900 dark:text-white">{{ $startDate->daysInMonth }}</span>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-gray-600 dark:text-gray-400">Report Generated:</span>
                        <span class="font-medium text-gray-900 dark:text-white">{{ now()->format('d/m/Y H:i') }}</span>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

@push('scripts')
<script>
@if($summary['weekly_breakdown']->isNotEmpty())
// Weekly Chart
const weeklyCtx = document.getElementById('weeklyChart').getContext('2d');
const weeklyChart = new Chart(weeklyCtx, {
    type: 'bar',
    data: {
        labels: {!! json_encode($summary['weekly_breakdown']->keys()->map(function($key, $index) { return 'Week ' . ($index + 1); })->values()) !!},
        datasets: [{
            label: 'Energy Consumption (Wh)',
            data: {!! json_encode($summary['weekly_breakdown']->pluck('energy')->values()) !!},
            backgroundColor: 'rgba(147, 51, 234, 0.6)',
            borderColor: 'rgba(147, 51, 234, 1)',
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
                    text: 'Week of Month'
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