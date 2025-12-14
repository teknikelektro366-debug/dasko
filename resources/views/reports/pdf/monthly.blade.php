<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Monthly Energy Report - {{ $summary['month'] }}</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            color: #333;
        }
        .header {
            text-align: center;
            border-bottom: 2px solid #8b5cf6;
            padding-bottom: 20px;
            margin-bottom: 30px;
        }
        .logo {
            font-size: 24px;
            font-weight: bold;
            color: #8b5cf6;
            margin-bottom: 10px;
        }
        .title {
            font-size: 20px;
            font-weight: bold;
            margin-bottom: 5px;
        }
        .subtitle {
            color: #666;
            font-size: 14px;
        }
        .summary-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 20px;
            margin-bottom: 30px;
        }
        .summary-card {
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            padding: 15px;
            background-color: #f9fafb;
        }
        .summary-card h3 {
            margin: 0 0 10px 0;
            color: #8b5cf6;
            font-size: 16px;
        }
        .summary-value {
            font-size: 24px;
            font-weight: bold;
            color: #1f2937;
        }
        .summary-label {
            color: #6b7280;
            font-size: 12px;
        }
        .section {
            margin-bottom: 30px;
        }
        .section-title {
            font-size: 18px;
            font-weight: bold;
            color: #1f2937;
            margin-bottom: 15px;
            border-bottom: 1px solid #e5e7eb;
            padding-bottom: 5px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 20px;
        }
        th, td {
            border: 1px solid #e5e7eb;
            padding: 8px;
            text-align: left;
            font-size: 12px;
        }
        th {
            background-color: #f3f4f6;
            font-weight: bold;
            color: #374151;
        }
        tr:nth-child(even) {
            background-color: #f9fafb;
        }
        .device-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-bottom: 20px;
        }
        .device-card {
            border: 1px solid #e5e7eb;
            border-radius: 6px;
            padding: 10px;
            text-align: center;
        }
        .device-card h4 {
            margin: 0 0 5px 0;
            font-size: 14px;
            color: #8b5cf6;
        }
        .device-energy {
            font-size: 16px;
            font-weight: bold;
            color: #1f2937;
        }
        .device-cost {
            font-size: 11px;
            color: #6b7280;
        }
        .footer {
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #e5e7eb;
            text-align: center;
            color: #6b7280;
            font-size: 12px;
        }
        .efficiency-badge {
            display: inline-block;
            padding: 2px 6px;
            border-radius: 4px;
            font-size: 10px;
            font-weight: bold;
        }
        .efficiency-excellent { background-color: #d1fae5; color: #065f46; }
        .efficiency-good { background-color: #dbeafe; color: #1e40af; }
        .efficiency-average { background-color: #fef3c7; color: #92400e; }
        .efficiency-poor { background-color: #fee2e2; color: #991b1b; }
        .comparison {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 20px;
        }
        .comparison-card {
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            padding: 15px;
        }
        .positive { color: #dc2626; }
        .negative { color: #16a34a; }
    </style>
</head>
<body>
    <!-- Header -->
    <div class="header">
        <div class="logo">⚡ Smart Energy Management</div>
        <div class="title">Monthly Energy Report</div>
        <div class="subtitle">Lab Teknik Tegangan Tinggi</div>
        <div class="subtitle">{{ $summary['month'] }}</div>
    </div>

    <!-- Summary Section -->
    <div class="section">
        <div class="section-title">Monthly Summary</div>
        <div class="summary-grid">
            <div class="summary-card">
                <h3>Total Energy Consumption</h3>
                <div class="summary-value">{{ $summary['total_energy_kwh'] }}</div>
                <div class="summary-label">kWh</div>
            </div>
            <div class="summary-card">
                <h3>Total Cost</h3>
                <div class="summary-value">Rp {{ number_format($summary['total_cost_idr']) }}</div>
                <div class="summary-label">Indonesian Rupiah</div>
            </div>
            <div class="summary-card">
                <h3>Average Daily Energy</h3>
                <div class="summary-value">{{ number_format($summary['avg_daily_energy'], 1) }}</div>
                <div class="summary-label">Wh per day</div>
            </div>
            <div class="summary-card">
                <h3>Total Sessions</h3>
                <div class="summary-value">{{ $summary['logs_count'] }}</div>
                <div class="summary-label">Device Operations</div>
            </div>
        </div>
    </div>

    <!-- Weekly Breakdown -->
    @if($summary['weekly_breakdown']->isNotEmpty())
    <div class="section">
        <div class="section-title">Weekly Energy Breakdown</div>
        <table>
            <thead>
                <tr>
                    <th>Week</th>
                    <th>Period</th>
                    <th>Energy (kWh)</th>
                    <th>Cost (IDR)</th>
                    <th>Sessions</th>
                    <th>Percentage</th>
                </tr>
            </thead>
            <tbody>
                @php
                    $totalMonthEnergy = $summary['weekly_breakdown']->sum('energy');
                @endphp
                @foreach($summary['weekly_breakdown'] as $weekKey => $data)
                <tr>
                    <td>Week {{ $loop->iteration }}</td>
                    <td>{{ $data['period'] }}</td>
                    <td>{{ number_format($data['energy'] / 1000, 3) }}</td>
                    <td>Rp {{ number_format($data['cost']) }}</td>
                    <td>{{ $data['count'] }}</td>
                    <td>{{ $totalMonthEnergy > 0 ? number_format(($data['energy'] / $totalMonthEnergy) * 100, 1) : 0 }}%</td>
                </tr>
                @endforeach
            </tbody>
        </table>
    </div>
    @endif

    <!-- Device Performance Analysis -->
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
    <div class="section">
        <div class="section-title">Device Performance Analysis</div>
        <div class="device-grid">
            @foreach($deviceStats as $deviceType => $stats)
            <div class="device-card">
                <h4>{{ $stats['device_name'] }}</h4>
                <div class="device-energy">{{ number_format($stats['total_energy'] / 1000, 2) }} kWh</div>
                <div class="device-cost">Rp {{ number_format($stats['total_cost']) }}</div>
                <div class="device-cost">{{ $stats['sessions'] }} sessions</div>
                <div class="device-cost">{{ number_format($stats['total_runtime'] / 60, 1) }} hours</div>
            </div>
            @endforeach
        </div>
        
        <table>
            <thead>
                <tr>
                    <th>Device</th>
                    <th>Energy (kWh)</th>
                    <th>Cost (IDR)</th>
                    <th>Sessions</th>
                    <th>Runtime (Hours)</th>
                    <th>Avg per Session (Wh)</th>
                    <th>% of Total</th>
                </tr>
            </thead>
            <tbody>
                @php
                    $totalDeviceEnergy = $deviceStats->sum('total_energy');
                @endphp
                @foreach($deviceStats as $deviceType => $stats)
                <tr>
                    <td>{{ $stats['device_name'] }}</td>
                    <td>{{ number_format($stats['total_energy'] / 1000, 3) }}</td>
                    <td>Rp {{ number_format($stats['total_cost']) }}</td>
                    <td>{{ $stats['sessions'] }}</td>
                    <td>{{ number_format($stats['total_runtime'] / 60, 1) }}</td>
                    <td>{{ number_format($stats['avg_energy'], 1) }}</td>
                    <td>{{ $totalDeviceEnergy > 0 ? number_format(($stats['total_energy'] / $totalDeviceEnergy) * 100, 1) : 0 }}%</td>
                </tr>
                @endforeach
            </tbody>
        </table>
    </div>
    @endif

    <!-- Monthly Comparison -->
    <div class="section">
        <div class="section-title">Monthly Comparison & Analysis</div>
        <div class="comparison">
            <div class="comparison-card">
                <h4 style="margin: 0 0 10px 0; color: #8b5cf6;">Key Performance Indicators</h4>
                <table style="margin: 0;">
                    <tr>
                        <td><strong>Average Energy per Session</strong></td>
                        <td>{{ $summary['logs_count'] > 0 ? number_format($summary['total_energy_kwh'] * 1000 / $summary['logs_count'], 1) : 0 }} Wh</td>
                    </tr>
                    <tr>
                        <td><strong>Cost per kWh</strong></td>
                        <td>Rp {{ $summary['total_energy_kwh'] > 0 ? number_format($summary['total_cost_idr'] / $summary['total_energy_kwh']) : 0 }}</td>
                    </tr>
                    <tr>
                        <td><strong>Energy Efficiency Rating</strong></td>
                        <td>
                            @php
                                $excellentCount = $energyLogs->where('efficiency_rating', 'Excellent')->count();
                                $totalWithRating = $energyLogs->whereNotNull('efficiency_rating')->count();
                                $efficiencyPercentage = $totalWithRating > 0 ? ($excellentCount / $totalWithRating) * 100 : 0;
                            @endphp
                            {{ number_format($efficiencyPercentage, 1) }}% Excellent
                        </td>
                    </tr>
                    <tr>
                        <td><strong>Working Hours Usage</strong></td>
                        <td>{{ $summary['logs_count'] > 0 ? number_format(($energyLogs->where('working_hours', true)->count() / $summary['logs_count']) * 100, 1) : 0 }}%</td>
                    </tr>
                </table>
            </div>
            
            <div class="comparison-card">
                <h4 style="margin: 0 0 10px 0; color: #8b5cf6;">Previous Month Comparison</h4>
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
                <table style="margin: 0;">
                    <tr>
                        <td><strong>Previous Month Energy</strong></td>
                        <td>{{ number_format($previousEnergy, 3) }} kWh</td>
                    </tr>
                    <tr>
                        <td><strong>Energy Change</strong></td>
                        <td class="{{ $energyChange >= 0 ? 'positive' : 'negative' }}">
                            {{ $energyChange >= 0 ? '+' : '' }}{{ number_format($energyChange, 1) }}%
                        </td>
                    </tr>
                    <tr>
                        <td><strong>Previous Month Cost</strong></td>
                        <td>Rp {{ number_format($previousCost) }}</td>
                    </tr>
                    <tr>
                        <td><strong>Cost Change</strong></td>
                        <td class="{{ $costChange >= 0 ? 'positive' : 'negative' }}">
                            {{ $costChange >= 0 ? '+' : '' }}{{ number_format($costChange, 1) }}%
                        </td>
                    </tr>
                </table>
            </div>
        </div>
    </div>

    <!-- Energy Efficiency Analysis -->
    <div class="section">
        <div class="section-title">Energy Efficiency Analysis</div>
        <table>
            <thead>
                <tr>
                    <th>Efficiency Rating</th>
                    <th>Sessions Count</th>
                    <th>Percentage</th>
                    <th>Total Energy (kWh)</th>
                    <th>Average per Session (Wh)</th>
                </tr>
            </thead>
            <tbody>
                @php
                    $efficiencyStats = $energyLogs->groupBy('efficiency_rating')->map(function ($logs) {
                        return [
                            'count' => $logs->count(),
                            'total_energy' => $logs->sum('energy_consumed'),
                            'avg_energy' => $logs->avg('energy_consumed')
                        ];
                    });
                @endphp
                @foreach(['Excellent', 'Good', 'Average', 'Poor'] as $rating)
                    @if(isset($efficiencyStats[$rating]))
                    <tr>
                        <td>
                            <span class="efficiency-badge efficiency-{{ strtolower($rating) }}">
                                {{ $rating }}
                            </span>
                        </td>
                        <td>{{ $efficiencyStats[$rating]['count'] }}</td>
                        <td>{{ $summary['logs_count'] > 0 ? number_format(($efficiencyStats[$rating]['count'] / $summary['logs_count']) * 100, 1) : 0 }}%</td>
                        <td>{{ number_format($efficiencyStats[$rating]['total_energy'] / 1000, 3) }}</td>
                        <td>{{ number_format($efficiencyStats[$rating]['avg_energy'], 1) }}</td>
                    </tr>
                    @endif
                @endforeach
            </tbody>
        </table>
    </div>

    <!-- Key Insights & Recommendations -->
    <div class="section">
        <div class="section-title">Key Insights & Recommendations</div>
        <table>
            <tr>
                <td><strong>Peak Energy Week</strong></td>
                <td>
                    @php
                        $peakWeek = $summary['weekly_breakdown']->sortByDesc('energy')->first();
                    @endphp
                    {{ $peakWeek ? $peakWeek['period'] : 'N/A' }}
                </td>
            </tr>
            <tr>
                <td><strong>Most Efficient Device</strong></td>
                <td>
                    @php
                        $mostEfficient = $deviceStats->sortBy('avg_energy')->first();
                    @endphp
                    {{ $mostEfficient ? $mostEfficient['device_name'] : 'N/A' }}
                </td>
            </tr>
            <tr>
                <td><strong>Highest Energy Consumer</strong></td>
                <td>
                    @php
                        $highestConsumer = $deviceStats->sortByDesc('total_energy')->first();
                    @endphp
                    {{ $highestConsumer ? $highestConsumer['device_name'] : 'N/A' }}
                </td>
            </tr>
            <tr>
                <td><strong>Energy Saving Opportunities</strong></td>
                <td>
                    @if($energyChange > 10)
                        Consider implementing stricter energy saving policies
                    @elseif($energyChange > 0)
                        Monitor energy usage patterns for optimization
                    @else
                        Good energy management - maintain current practices
                    @endif
                </td>
            </tr>
            <tr>
                <td><strong>Report Generated</strong></td>
                <td>{{ now()->format('d F Y, H:i:s') }} WIB</td>
            </tr>
        </table>
    </div>

    <!-- Footer -->
    <div class="footer">
        <p><strong>Smart Energy Management System</strong></p>
        <p>Lab Teknik Tegangan Tinggi - Generated on {{ now()->format('d F Y, H:i:s') }} WIB</p>
        <p>This report contains confidential information. Please handle with care.</p>
    </div>
</body>
</html>