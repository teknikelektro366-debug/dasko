@extends('layouts.app')

@section('title', 'Energy Efficiency Report')
@section('page-title', 'Energy Efficiency Report')

@section('content')
<div class="p-6 space-y-6">
    <!-- Header with Download Options -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <div class="flex justify-between items-center">
            <div>
                <h2 class="text-2xl font-bold text-gray-900 dark:text-white mb-2">
                    <i class="fas fa-leaf mr-3 text-green-500"></i>Energy Efficiency Report
                </h2>
                <p class="text-gray-600 dark:text-gray-400">{{ $dateFrom->format('d M Y') }} - {{ $dateTo->format('d M Y') }}</p>
            </div>
            <div class="flex space-x-3">
                <form method="POST" action="{{ route('reports.efficiency') }}" class="inline">
                    @csrf
                    <input type="hidden" name="date_from" value="{{ $dateFrom->format('Y-m-d') }}">
                    <input type="hidden" name="date_to" value="{{ $dateTo->format('Y-m-d') }}">
                    <input type="hidden" name="format" value="download">
                    <button type="submit" class="bg-green-600 hover:bg-green-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
                        <i class="fas fa-download"></i>
                        <span>Download PDF</span>
                    </button>
                </form>
                <a href="{{ route('reports.index') }}" 
                   class="bg-gray-600 hover:bg-gray-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
                    <i class="fas fa-arrow-left"></i>
                    <span>Back to Reports</span>
                </a>
            </div>
        </div>
    </div>

    <!-- Efficiency Overview -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        @php
            $totalDevices = $efficiencyData->count();
            $excellentDevices = $efficiencyData->where('efficiency_rating', 'Excellent')->count();
            $avgEfficiency = $efficiencyData->avg('efficiency_wh_per_person');
            $totalEnergyAnalyzed = $efficiencyData->sum('total_energy');
        @endphp

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-green-100 dark:bg-green-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-star text-green-600 dark:text-green-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Excellent Devices</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $excellentDevices }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">of {{ $totalDevices }} devices</p>
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
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Avg Efficiency</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ number_format($avgEfficiency, 1) }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Wh per person</p>
                </div>
            </div>
        </div>

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-purple-100 dark:bg-purple-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-bolt text-purple-600 dark:text-purple-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Total Energy</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ number_format($totalEnergyAnalyzed / 1000, 2) }}</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">kWh analyzed</p>
                </div>
            </div>
        </div>

        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-yellow-100 dark:bg-yellow-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-percentage text-yellow-600 dark:text-yellow-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Efficiency Rate</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">{{ $totalDevices > 0 ? number_format(($excellentDevices / $totalDevices) * 100, 1) : 0 }}%</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Excellent rating</p>
                </div>
            </div>
        </div>
    </div>

    <!-- Efficiency Chart -->
    @if($efficiencyData->isNotEmpty())
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-chart-bar mr-2 text-indigo-500"></i>Device Efficiency Comparison
        </h3>
        <div class="h-64">
            <canvas id="efficiencyChart"></canvas>
        </div>
    </div>
    @endif

    <!-- Efficiency Details Table -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-table mr-2 text-green-500"></i>Device Efficiency Analysis
        </h3>
        
        @if($efficiencyData->isNotEmpty())
        <div class="overflow-x-auto">
            <table class="min-w-full divide-y divide-gray-200 dark:divide-gray-700">
                <thead class="bg-gray-50 dark:bg-gray-700">
                    <tr>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Device</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Total Energy (kWh)</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">People Hours</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Efficiency (Wh/person)</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Rating</th>
                        <th class="px-6 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider">Sessions</th>
                    </tr>
                </thead>
                <tbody class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700">
                    @foreach($efficiencyData->sortBy('efficiency_wh_per_person') as $deviceType => $data)
                    <tr class="hover:bg-gray-50 dark:hover:bg-gray-700">
                        <td class="px-6 py-4 whitespace-nowrap">
                            <div class="text-sm font-medium text-gray-900 dark:text-white">{{ $data['device_name'] }}</div>
                            <div class="text-sm text-gray-500 dark:text-gray-400">{{ $deviceType }}</div>
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($data['total_energy'] / 1000, 3) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($data['total_people_hours']) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ number_format($data['efficiency_wh_per_person'], 1) }}
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap">
                            <span class="px-2 inline-flex text-xs leading-5 font-semibold rounded-full 
                                {{ $data['efficiency_rating'] === 'Excellent' ? 'bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200' : 
                                   ($data['efficiency_rating'] === 'Good' ? 'bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200' : 
                                    ($data['efficiency_rating'] === 'Average' ? 'bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200' : 
                                     'bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200')) }}">
                                {{ $data['efficiency_rating'] }}
                            </span>
                        </td>
                        <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-900 dark:text-white">
                            {{ $data['logs_count'] }}
                        </td>
                    </tr>
                    @endforeach
                </tbody>
            </table>
        </div>
        @else
        <div class="text-center py-8">
            <i class="fas fa-chart-line text-gray-400 text-4xl mb-4"></i>
            <p class="text-gray-500 dark:text-gray-400">No efficiency data available for the selected period</p>
            <p class="text-sm text-gray-400 dark:text-gray-500 mt-2">Efficiency analysis requires energy logs with people count data</p>
        </div>
        @endif
    </div>

    <!-- Efficiency Recommendations -->
    @if($efficiencyData->isNotEmpty())
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-lightbulb mr-2 text-yellow-500"></i>Efficiency Recommendations
        </h3>
        
        <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
            <!-- Best Performers -->
            <div>
                <h4 class="font-medium text-gray-900 dark:text-white mb-4 text-green-600 dark:text-green-400">
                    <i class="fas fa-trophy mr-2"></i>Best Performers
                </h4>
                <div class="space-y-3">
                    @foreach($efficiencyData->where('efficiency_rating', 'Excellent')->take(3) as $deviceType => $data)
                    <div class="bg-green-50 dark:bg-green-900/20 border border-green-200 dark:border-green-800 rounded-lg p-3">
                        <div class="flex justify-between items-center">
                            <span class="font-medium text-green-800 dark:text-green-200">{{ $data['device_name'] }}</span>
                            <span class="text-sm text-green-600 dark:text-green-400">{{ number_format($data['efficiency_wh_per_person'], 1) }} Wh/person</span>
                        </div>
                        <p class="text-sm text-green-700 dark:text-green-300 mt-1">
                            Excellent efficiency - maintain current usage patterns
                        </p>
                    </div>
                    @endforeach
                </div>
            </div>

            <!-- Improvement Opportunities -->
            <div>
                <h4 class="font-medium text-gray-900 dark:text-white mb-4 text-red-600 dark:text-red-400">
                    <i class="fas fa-exclamation-triangle mr-2"></i>Improvement Opportunities
                </h4>
                <div class="space-y-3">
                    @foreach($efficiencyData->whereIn('efficiency_rating', ['Poor', 'Average'])->sortByDesc('efficiency_wh_per_person')->take(3) as $deviceType => $data)
                    <div class="bg-red-50 dark:bg-red-900/20 border border-red-200 dark:border-red-800 rounded-lg p-3">
                        <div class="flex justify-between items-center">
                            <span class="font-medium text-red-800 dark:text-red-200">{{ $data['device_name'] }}</span>
                            <span class="text-sm text-red-600 dark:text-red-400">{{ number_format($data['efficiency_wh_per_person'], 1) }} Wh/person</span>
                        </div>
                        <p class="text-sm text-red-700 dark:text-red-300 mt-1">
                            @if($data['efficiency_rating'] === 'Poor')
                                High energy per person - consider usage optimization
                            @else
                                Moderate efficiency - room for improvement
                            @endif
                        </p>
                    </div>
                    @endforeach
                </div>
            </div>
        </div>

        <!-- General Recommendations -->
        <div class="mt-6 bg-blue-50 dark:bg-blue-900/20 border border-blue-200 dark:border-blue-800 rounded-lg p-4">
            <h5 class="font-medium text-blue-800 dark:text-blue-200 mb-2">
                <i class="fas fa-info-circle mr-2"></i>General Recommendations
            </h5>
            <ul class="text-sm text-blue-700 dark:text-blue-300 space-y-1">
                <li>• Monitor devices with efficiency ratings below "Good" for optimization opportunities</li>
                <li>• Implement automated controls for devices with high energy per person ratios</li>
                <li>• Consider scheduling non-essential devices during off-peak hours</li>
                <li>• Regular maintenance can improve device efficiency over time</li>
                <li>• Train users on energy-conscious practices for shared devices</li>
            </ul>
        </div>
    </div>
    @endif
</div>

@push('scripts')
<script>
@if($efficiencyData->isNotEmpty())
// Efficiency Chart
const efficiencyCtx = document.getElementById('efficiencyChart').getContext('2d');
const efficiencyChart = new Chart(efficiencyCtx, {
    type: 'bar',
    data: {
        labels: {!! json_encode($efficiencyData->pluck('device_name')->values()) !!},
        datasets: [{
            label: 'Energy Efficiency (Wh per person)',
            data: {!! json_encode($efficiencyData->pluck('efficiency_wh_per_person')->values()) !!},
            backgroundColor: function(context) {
                const value = context.parsed.y;
                if (value < 50) return 'rgba(34, 197, 94, 0.6)'; // Excellent - Green
                if (value < 100) return 'rgba(59, 130, 246, 0.6)'; // Good - Blue
                if (value < 150) return 'rgba(245, 158, 11, 0.6)'; // Average - Yellow
                return 'rgba(239, 68, 68, 0.6)'; // Poor - Red
            },
            borderColor: function(context) {
                const value = context.parsed.y;
                if (value < 50) return 'rgba(34, 197, 94, 1)';
                if (value < 100) return 'rgba(59, 130, 246, 1)';
                if (value < 150) return 'rgba(245, 158, 11, 1)';
                return 'rgba(239, 68, 68, 1)';
            },
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
                    text: 'Energy per Person (Wh)'
                },
                ticks: {
                    callback: function(value) {
                        return value + ' Wh';
                    }
                }
            },
            x: {
                title: {
                    display: true,
                    text: 'Devices'
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
                        const value = context.parsed.y;
                        let rating = 'Poor';
                        if (value < 50) rating = 'Excellent';
                        else if (value < 100) rating = 'Good';
                        else if (value < 150) rating = 'Average';
                        
                        return [
                            'Efficiency: ' + value.toFixed(1) + ' Wh/person',
                            'Rating: ' + rating
                        ];
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