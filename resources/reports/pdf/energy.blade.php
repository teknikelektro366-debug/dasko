<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Energy Consumption Report</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            color: #333;
        }
        .header {
            text-align: center;
            border-bottom: 2px solid #4f46e5;
            padding-bottom: 20px;
            margin-bottom: 30px;
        }
        .logo {
            font-size: 24px;
            font-weight: bold;
            color: #4f46e5;
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
            color: #4f46e5;
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
        .device-summary {
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
            color: #4f46e5;
        }
        .device-energy {
            font-size: 18px;
            font-weight: bold;
            color: #1f2937;
        }
        .device-cost {
            font-size: 12px;
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
    </style>
</head>
<body>
    <!-- Header -->
    <div class="header">
        <div class="logo">⚡ Smart Energy Management</div>
        <div class="title">Energy Consumption Report</div>
        <div class="subtitle">Lab Teknik Tegangan Tinggi</div>
        <div class="subtitle">{{ $summary['period'] }}</div>
    </div>

    <!-- Summary Section -->
    <div class="section">
        <div class="section-title">Executive Summary</div>
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
                <h3>Total Runtime</h3>
                <div class="summary-value">{{ $summary['total_duration_hours'] }}</div>
                <div class="summary-label">Hours</div>
            </div>
            <div class="summary-card">
                <h3>Total Sessions</h3>
                <div class="summary-value">{{ $summary['logs_count'] }}</div>
                <div class="summary-label">Device Operations</div>
            </div>
        </div>
    </div>

    <!-- Device Summary -->
    @if($summary['by_device_type']->isNotEmpty())
    <div class="section">
        <div class="section-title">Energy Consumption by Device Type</div>
        <div class="device-summary">
            @foreach($summary['by_device_type'] as $deviceType => $data)
            <div class="device-card">
                <h4>{{ $data['device_name'] }}</h4>
                <div class="device-energy">{{ number_format($data['total_energy'] / 1000, 2) }} kWh</div>
                <div class="device-cost">Rp {{ number_format($data['total_cost']) }}</div>
                <div class="device-cost">{{ $data['logs_count'] }} sessions</div>
            </div>
            @endforeach
        </div>
    </div>
    @endif

    <!-- Key Statistics -->
    <div class="section">
        <div class="section-title">Key Statistics</div>
        <table>
            <tr>
                <td><strong>Average Energy per Session</strong></td>
                <td>{{ $summary['avg_energy_per_log'] }} Wh</td>
            </tr>
            <tr>
                <td><strong>Energy Saving Mode Sessions</strong></td>
                <td>{{ $summary['energy_saving_logs'] }} ({{ $summary['logs_count'] > 0 ? round(($summary['energy_saving_logs'] / $summary['logs_count']) * 100, 1) : 0 }}%)</td>
            </tr>
            <tr>
                <td><strong>Working Hours Sessions</strong></td>
                <td>{{ $summary['working_hours_logs'] }} ({{ $summary['logs_count'] > 0 ? round(($summary['working_hours_logs'] / $summary['logs_count']) * 100, 1) : 0 }}%)</td>
            </tr>
            <tr>
                <td><strong>Report Generated</strong></td>
                <td>{{ now()->format('d F Y, H:i:s') }} WIB</td>
            </tr>
        </table>
    </div>

    <!-- Detailed Energy Logs -->
    <div class="section">
        <div class="section-title">Detailed Energy Logs (Latest 50 entries)</div>
        <table>
            <thead>
                <tr>
                    <th>Date & Time</th>
                    <th>Device</th>
                    <th>Energy (Wh)</th>
                    <th>Duration</th>
                    <th>People</th>
                    <th>Temp</th>
                    <th>Cost (IDR)</th>
                    <th>Efficiency</th>
                </tr>
            </thead>
            <tbody>
                @foreach($energyLogs->take(50) as $log)
                <tr>
                    <td>{{ $log->start_time->format('d/m/Y H:i') }}</td>
                    <td>{{ $log->device->name }}</td>
                    <td>{{ number_format($log->energy_consumed, 1) }}</td>
                    <td>{{ $log->duration_minutes }}m</td>
                    <td>{{ $log->people_count ?? '-' }}</td>
                    <td>{{ $log->outdoor_temperature ?? '-' }}°C</td>
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
        
        @if($energyLogs->count() > 50)
        <p style="font-style: italic; color: #6b7280; font-size: 12px;">
            * Showing latest 50 entries out of {{ $energyLogs->count() }} total entries
        </p>
        @endif
    </div>

    <!-- Daily Breakdown -->
    @if($summary['daily_breakdown']->isNotEmpty())
    <div class="section">
        <div class="section-title">Daily Breakdown</div>
        <table>
            <thead>
                <tr>
                    <th>Date</th>
                    <th>Energy (kWh)</th>
                    <th>Cost (IDR)</th>
                    <th>Sessions</th>
                </tr>
            </thead>
            <tbody>
                @foreach($summary['daily_breakdown'] as $date => $data)
                <tr>
                    <td>{{ \Carbon\Carbon::parse($date)->format('d F Y') }}</td>
                    <td>{{ number_format($data['total_energy'] / 1000, 3) }}</td>
                    <td>Rp {{ number_format($data['total_cost']) }}</td>
                    <td>{{ $data['logs_count'] }}</td>
                </tr>
                @endforeach
            </tbody>
        </table>
    </div>
    @endif

    <!-- Footer -->
    <div class="footer">
        <p><strong>Smart Energy Management System</strong></p>
        <p>Lab Teknik Tegangan Tinggi - Generated on {{ now()->format('d F Y, H:i:s') }} WIB</p>
        <p>This report contains confidential information. Please handle with care.</p>
    </div>
</body>
</html>