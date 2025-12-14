<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Energy Efficiency Report</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            color: #333;
        }
        .header {
            text-align: center;
            border-bottom: 2px solid #10b981;
            padding-bottom: 20px;
            margin-bottom: 30px;
        }
        .logo {
            font-size: 24px;
            font-weight: bold;
            color: #10b981;
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
            color: #10b981;
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
        .recommendation-box {
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            padding: 15px;
            margin-bottom: 15px;
        }
        .recommendation-excellent {
            background-color: #d1fae5;
            border-color: #10b981;
        }
        .recommendation-poor {
            background-color: #fee2e2;
            border-color: #ef4444;
        }
        .recommendation-title {
            font-weight: bold;
            margin-bottom: 10px;
        }
        .footer {
            margin-top: 40px;
            padding-top: 20px;
            border-top: 1px solid #e5e7eb;
            text-align: center;
            color: #6b7280;
            font-size: 12px;
        }
    </style>
</head>
<body>
    <!-- Header -->
    <div class="header">
        <div class="logo">🌱 Smart Energy Management</div>
        <div class="title">Energy Efficiency Report</div>
        <div class="subtitle">Lab Teknik Tegangan Tinggi</div>
        <div class="subtitle">{{ $dateFrom->format('d F Y') }} - {{ $dateTo->format('d F Y') }}</div>
    </div>

    <!-- Executive Summary -->
    <div class="section">
        <div class="section-title">Executive Summary</div>
        @php
            $totalDevices = $efficiencyData->count();
            $excellentDevices = $efficiencyData->where('efficiency_rating', 'Excellent')->count();
            $avgEfficiency = $efficiencyData->avg('efficiency_wh_per_person');
            $totalEnergyAnalyzed = $efficiencyData->sum('total_energy');
        @endphp
        
        <div class="summary-grid">
            <div class="summary-card">
                <h3>Devices Analyzed</h3>
                <div class="summary-value">{{ $totalDevices }}</div>
                <div class="summary-label">Total devices with efficiency data</div>
            </div>
            <div class="summary-card">
                <h3>Excellent Performers</h3>
                <div class="summary-value">{{ $excellentDevices }}</div>
                <div class="summary-label">{{ $totalDevices > 0 ? number_format(($excellentDevices / $totalDevices) * 100, 1) : 0 }}% of devices</div>
            </div>
            <div class="summary-card">
                <h3>Average Efficiency</h3>
                <div class="summary-value">{{ number_format($avgEfficiency, 1) }}</div>
                <div class="summary-label">Wh per person</div>
            </div>
            <div class="summary-card">
                <h3>Total Energy Analyzed</h3>
                <div class="summary-value">{{ number_format($totalEnergyAnalyzed / 1000, 2) }}</div>
                <div class="summary-label">kWh</div>
            </div>
        </div>
    </div>

    <!-- Efficiency Analysis -->
    @if($efficiencyData->isNotEmpty())
    <div class="section">
        <div class="section-title">Device Efficiency Analysis</div>
        <table>
            <thead>
                <tr>
                    <th>Device Name</th>
                    <th>Total Energy (kWh)</th>
                    <th>People Hours</th>
                    <th>Efficiency (Wh/person)</th>
                    <th>Rating</th>
                    <th>Sessions</th>
                    <th>Rank</th>
                </tr>
            </thead>
            <tbody>
                @foreach($efficiencyData->sortBy('efficiency_wh_per_person')->values() as $index => $data)
                <tr>
                    <td>{{ $data['device_name'] }}</td>
                    <td>{{ number_format($data['total_energy'] / 1000, 3) }}</td>
                    <td>{{ number_format($data['total_people_hours']) }}</td>
                    <td>{{ number_format($data['efficiency_wh_per_person'], 1) }}</td>
                    <td>
                        <span class="efficiency-badge efficiency-{{ strtolower($data['efficiency_rating']) }}">
                            {{ $data['efficiency_rating'] }}
                        </span>
                    </td>
                    <td>{{ $data['logs_count'] }}</td>
                    <td>{{ $index + 1 }}</td>
                </tr>
                @endforeach
            </tbody>
        </table>
    </div>

    <!-- Efficiency Distribution -->
    <div class="section">
        <div class="section-title">Efficiency Rating Distribution</div>
        <table>
            <thead>
                <tr>
                    <th>Rating</th>
                    <th>Device Count</th>
                    <th>Percentage</th>
                    <th>Total Energy (kWh)</th>
                    <th>Avg Efficiency (Wh/person)</th>
                </tr>
            </thead>
            <tbody>
                @php
                    $ratingStats = $efficiencyData->groupBy('efficiency_rating')->map(function ($devices) {
                        return [
                            'count' => $devices->count(),
                            'total_energy' => $devices->sum('total_energy'),
                            'avg_efficiency' => $devices->avg('efficiency_wh_per_person')
                        ];
                    });
                @endphp
                @foreach(['Excellent', 'Good', 'Average', 'Poor'] as $rating)
                    @if(isset($ratingStats[$rating]))
                    <tr>
                        <td>
                            <span class="efficiency-badge efficiency-{{ strtolower($rating) }}">
                                {{ $rating }}
                            </span>
                        </td>
                        <td>{{ $ratingStats[$rating]['count'] }}</td>
                        <td>{{ $totalDevices > 0 ? number_format(($ratingStats[$rating]['count'] / $totalDevices) * 100, 1) : 0 }}%</td>
                        <td>{{ number_format($ratingStats[$rating]['total_energy'] / 1000, 3) }}</td>
                        <td>{{ number_format($ratingStats[$rating]['avg_efficiency'], 1) }}</td>
                    </tr>
                    @endif
                @endforeach
            </tbody>
        </table>
    </div>

    <!-- Best Performers -->
    <div class="section">
        <div class="section-title">Performance Analysis</div>
        
        <div class="recommendation-box recommendation-excellent">
            <div class="recommendation-title" style="color: #065f46;">🏆 Top Performers (Excellent Rating)</div>
            @foreach($efficiencyData->where('efficiency_rating', 'Excellent')->sortBy('efficiency_wh_per_person')->take(5) as $data)
            <div style="margin-bottom: 8px;">
                <strong>{{ $data['device_name'] }}</strong>: {{ number_format($data['efficiency_wh_per_person'], 1) }} Wh/person
                <br><span style="font-size: 11px; color: #065f46;">Excellent efficiency - maintain current usage patterns</span>
            </div>
            @endforeach
        </div>

        <div class="recommendation-box recommendation-poor">
            <div class="recommendation-title" style="color: #991b1b;">⚠️ Improvement Opportunities</div>
            @foreach($efficiencyData->whereIn('efficiency_rating', ['Poor', 'Average'])->sortByDesc('efficiency_wh_per_person')->take(5) as $data)
            <div style="margin-bottom: 8px;">
                <strong>{{ $data['device_name'] }}</strong>: {{ number_format($data['efficiency_wh_per_person'], 1) }} Wh/person
                <br><span style="font-size: 11px; color: #991b1b;">
                    @if($data['efficiency_rating'] === 'Poor')
                        High energy per person - requires immediate optimization
                    @else
                        Moderate efficiency - consider usage improvements
                    @endif
                </span>
            </div>
            @endforeach
        </div>
    </div>

    <!-- Recommendations -->
    <div class="section">
        <div class="section-title">Efficiency Recommendations</div>
        
        <table>
            <tr>
                <td><strong>Overall Efficiency Status</strong></td>
                <td>
                    @if($excellentDevices / $totalDevices >= 0.7)
                        Excellent - Most devices are operating efficiently
                    @elseif($excellentDevices / $totalDevices >= 0.5)
                        Good - Majority of devices are efficient with room for improvement
                    @elseif($excellentDevices / $totalDevices >= 0.3)
                        Average - Significant optimization opportunities exist
                    @else
                        Poor - Immediate efficiency improvements needed
                    @endif
                </td>
            </tr>
            <tr>
                <td><strong>Priority Actions</strong></td>
                <td>
                    @php
                        $poorDevices = $efficiencyData->where('efficiency_rating', 'Poor')->count();
                        $averageDevices = $efficiencyData->where('efficiency_rating', 'Average')->count();
                    @endphp
                    @if($poorDevices > 0)
                        Focus on {{ $poorDevices }} device(s) with Poor rating for immediate improvements
                    @elseif($averageDevices > 0)
                        Optimize {{ $averageDevices }} device(s) with Average rating
                    @else
                        Maintain current excellent performance levels
                    @endif
                </td>
            </tr>
            <tr>
                <td><strong>Energy Saving Potential</strong></td>
                <td>
                    @php
                        $inefficientEnergy = $efficiencyData->whereIn('efficiency_rating', ['Poor', 'Average'])->sum('total_energy');
                        $savingsPotential = $inefficientEnergy * 0.2; // Assume 20% savings potential
                    @endphp
                    Potential savings: {{ number_format($savingsPotential / 1000, 2) }} kWh (20% improvement on inefficient devices)
                </td>
            </tr>
            <tr>
                <td><strong>Best Practice Device</strong></td>
                <td>
                    @php
                        $bestDevice = $efficiencyData->sortBy('efficiency_wh_per_person')->first();
                    @endphp
                    {{ $bestDevice ? $bestDevice['device_name'] . ' (' . number_format($bestDevice['efficiency_wh_per_person'], 1) . ' Wh/person)' : 'N/A' }}
                </td>
            </tr>
            <tr>
                <td><strong>Monitoring Frequency</strong></td>
                <td>
                    @if($excellentDevices / $totalDevices < 0.5)
                        Weekly monitoring recommended for poor performers
                    @else
                        Monthly monitoring sufficient for current performance levels
                    @endif
                </td>
            </tr>
        </table>

        <!-- Action Items -->
        <div style="margin-top: 20px; padding: 15px; border: 1px solid #3b82f6; border-radius: 8px; background-color: #eff6ff;">
            <div style="font-weight: bold; color: #1e40af; margin-bottom: 10px;">📋 Recommended Action Items:</div>
            <ul style="margin: 0; padding-left: 20px; color: #1e40af;">
                <li>Implement automated controls for devices with efficiency ratings below "Good"</li>
                <li>Schedule regular maintenance for devices with declining efficiency trends</li>
                <li>Provide energy awareness training for users of shared devices</li>
                <li>Consider upgrading devices with consistently poor efficiency ratings</li>
                <li>Monitor peak usage times and implement load balancing strategies</li>
                <li>Set efficiency targets and track progress monthly</li>
            </ul>
        </div>
    </div>

    <!-- Key Metrics Summary -->
    <div class="section">
        <div class="section-title">Key Performance Indicators</div>
        <table>
            <tr>
                <td><strong>Analysis Period</strong></td>
                <td>{{ $dateFrom->format('d F Y') }} to {{ $dateTo->format('d F Y') }} ({{ $dateFrom->diffInDays($dateTo) + 1 }} days)</td>
            </tr>
            <tr>
                <td><strong>Most Efficient Device</strong></td>
                <td>
                    @php
                        $mostEfficient = $efficiencyData->sortBy('efficiency_wh_per_person')->first();
                    @endphp
                    {{ $mostEfficient ? $mostEfficient['device_name'] . ' - ' . number_format($mostEfficient['efficiency_wh_per_person'], 1) . ' Wh/person' : 'N/A' }}
                </td>
            </tr>
            <tr>
                <td><strong>Least Efficient Device</strong></td>
                <td>
                    @php
                        $leastEfficient = $efficiencyData->sortByDesc('efficiency_wh_per_person')->first();
                    @endphp
                    {{ $leastEfficient ? $leastEfficient['device_name'] . ' - ' . number_format($leastEfficient['efficiency_wh_per_person'], 1) . ' Wh/person' : 'N/A' }}
                </td>
            </tr>
            <tr>
                <td><strong>Efficiency Range</strong></td>
                <td>
                    {{ number_format($efficiencyData->min('efficiency_wh_per_person'), 1) }} - {{ number_format($efficiencyData->max('efficiency_wh_per_person'), 1) }} Wh/person
                </td>
            </tr>
            <tr>
                <td><strong>Report Generated</strong></td>
                <td>{{ now()->format('d F Y, H:i:s') }} WIB</td>
            </tr>
        </table>
    </div>
    @endif

    <!-- Footer -->
    <div class="footer">
        <p><strong>Smart Energy Management System - Efficiency Analysis</strong></p>
        <p>Lab Teknik Tegangan Tinggi - Generated on {{ now()->format('d F Y, H:i:s') }} WIB</p>
        <p>This report contains confidential information. Please handle with care.</p>
    </div>
</body>
</html>