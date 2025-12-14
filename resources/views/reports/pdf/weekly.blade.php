<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Weekly Energy Report - {{ $summary['period'] }}</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            color: #333;
        }
        .header {
            text-align: center;
            border-bottom: 2px solid #22c55e;
            padding-bottom: 20px;
            margin-bottom: 30px;
        }
        .logo {
            font-size: 24px;
            font-weight: bold;
            color: #22c55e;
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
            color: #22c55e;
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
    </style>
</head>
<body>
    <!-- Header -->
    <div class="header">
        <div class="logo">⚡ Smart Energy Management</div>
        <div class="title">Weekly Energy Report</div>
        <div class="subtitle">Lab Teknik Tegangan Tinggi</div>
        <div class="subtitle">{{ $summary['period'] }}</div>
    </div>

    <!-- Summary Section -->
    <div class="section">
        <div class="section-title">Weekly Summary</div>
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

    <!-- Daily Breakdown -->
    @if($summary['daily_breakdown']->isNotEmpty())
    <div class="section">
        <div class="section-title">Daily Energy Breakdown</div>
        <table>
            <thead>
                <tr>
                    <th>Day</th>
                    <th>Date</th>
                    <th>Energy (kWh)</th>
                    <th>Cost (IDR)</th>
                    <th>Sessions</th>
                    <th>Percentage</th>
                </tr>
            </thead>
            <tbody>
                @php
                    $totalWeekEnergy = $summary['daily_breakdown']->sum('energy');
                @endphp
                @foreach($summary['daily_breakdown'] as $date => $data)
                <tr>
                    <td>{{ $data['day_name'] }}</td>
                    <td>{{ $data['date'] }}</td>
                    <td>{{ number_format($data['energy'] / 1000, 3) }}</td>
                    <td>Rp {{ number_format($data['cost']) }}</td>
                    <td>{{ $data['count'] }}</td>
                    <td>{{ $totalWeekEnergy > 0 ? number_format(($data['energy'] / $totalWeekEnergy) * 100, 1) : 0 }}%</td>
                </tr>
                @endforeach
            </tbody>
        </table>
    </div>
    @endif

    <!-- Device Performance -->
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
        <table>
            <thead>
                <tr>
                    <th>Device</th>
                    <th>Energy (kWh)</th>
                    <th>Cost (IDR)</th>
                    <th>Sessions</th>
                    <th>Runtime (Hours)</th>
                    <th>Avg per Session (Wh)</th>
                </tr>
            </thead>
            <tbody>
                @foreach($deviceStats as $deviceType => $stats)
                <tr>
                    <td>{{ $stats['device_name'] }}</td>
                    <td>{{ number_format($stats['total_energy'] / 1000, 3) }}</td>
                    <td>Rp {{ number_format($stats['total_cost']) }}</td>
                    <td>{{ $stats['sessions'] }}</td>
                    <td>{{ number_format($stats['total_runtime'] / 60, 1) }}</td>
                    <td>{{ number_format($stats['avg_energy'], 1) }}</td>
                </tr>
                @endforeach
            </tbody>
        </table>
    </div>
    @endif

    <!-- Detailed Energy Logs (Latest 30) -->
    <div class="section">
        <div class="section-title">Recent Energy Logs (Latest 30 entries)</div>
        <table>
            <thead>
                <tr>
                    <th>Date & Time</th>
                    <th>Device</th>
                    <th>Energy (Wh)</th>
                    <th>Duration</th>
                    <th>People</th>
                    <th>Cost (IDR)</th>
                    <th>Efficiency</th>
                </tr>
            </thead>
            <tbody>
                @foreach($energyLogs->take(30) as $log)
                <tr>
                    <td>{{ $log->start_time->format('d/m H:i') }}</td>
                    <td>{{ $log->device->name }}</td>
                    <td>{{ number_format($log->energy_consumed, 1) }}</td>
                    <td>{{ $log->duration_minutes }}m</td>
                    <td>{{ $log->people_count ?? '-' }}</td>
                    <td>{{ number_format($log->cost_estimate) }}</td>
                    <td>
                        <span class="efficiency-badge efficiency-{{ strtolower($log->efficiency_rating) }}">
                            {{ $log->efficiency_rating }}
                        </span>
                    </td>
                </tr>
                @endforeach
            </tbody>
        </table>
        
        @if($energyLogs->count() > 30)
        <p style="font-style: italic; color: #6b7280; font-size: 12px;">
            * Showing latest 30 entries out of {{ $energyLogs->count() }} total entries
        </p>
        @endif
    </div>

    <!-- Key Insights -->
    <div class="section">
        <div class="section-title">Key Insights</div>
        <table>
            <tr>
                <td><strong>Peak Day</strong></td>
                <td>
                    @if($summary['peak_day'])
                        {{ \Carbon\Carbon::parse($summary['peak_day'])->format('l, d F Y') }}
                    @else
                        No peak day identified
                    @endif
                </td>
            </tr>
            <tr>
                <td><strong>Average Energy per Session</strong></td>
                <td>{{ $summary['logs_count'] > 0 ? number_format($summary['total_energy_kwh'] * 1000 / $summary['logs_count'], 1) : 0 }} Wh</td>
            </tr>
            <tr>
                <td><strong>Energy Efficiency</strong></td>
                <td>
                    @php
                        $excellentCount = $energyLogs->where('efficiency_rating', 'Excellent')->count();
                        $totalWithRating = $energyLogs->whereNotNull('efficiency_rating')->count();
                        $efficiencyPercentage = $totalWithRating > 0 ? ($excellentCount / $totalWithRating) * 100 : 0;
                    @endphp
                    {{ number_format($efficiencyPercentage, 1) }}% of sessions rated as Excellent
                </td>
            </tr>
            <tr>
                <td><strong>Cost per kWh</strong></td>
                <td>Rp {{ $summary['total_energy_kwh'] > 0 ? number_format($summary['total_cost_idr'] / $summary['total_energy_kwh']) : 0 }}</td>
            </tr>
            <tr>
                <td><strong>Working Hours Usage</strong></td>
                <td>{{ $summary['logs_count'] > 0 ? number_format(($energyLogs->where('working_hours', true)->count() / $summary['logs_count']) * 100, 1) : 0 }}% of sessions during working hours</td>
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