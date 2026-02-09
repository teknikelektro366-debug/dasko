<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Laporan Kustom - {{ $summary['period'] }}</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            font-size: 12px;
            line-height: 1.6;
            color: #333;
        }
        .header {
            text-align: center;
            margin-bottom: 30px;
            border-bottom: 3px solid #3b82f6;
            padding-bottom: 20px;
        }
        .header h1 {
            color: #3b82f6;
            margin: 0;
            font-size: 24px;
        }
        .header p {
            margin: 5px 0;
            color: #666;
        }
        .summary {
            background: #F3F4F6;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
        }
        .summary-grid {
            display: table;
            width: 100%;
        }
        .summary-item {
            display: table-cell;
            padding: 10px;
            text-align: center;
            border-right: 1px solid #ddd;
        }
        .summary-item:last-child {
            border-right: none;
        }
        .summary-label {
            font-size: 10px;
            color: #666;
            text-transform: uppercase;
        }
        .summary-value {
            font-size: 18px;
            font-weight: bold;
            color: #3b82f6;
            margin-top: 5px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        th {
            background: #3b82f6;
            color: white;
            padding: 10px;
            text-align: left;
            font-size: 11px;
        }
        td {
            padding: 8px;
            border-bottom: 1px solid #E5E7EB;
            font-size: 10px;
        }
        tr:nth-child(even) {
            background: #F9FAFB;
        }
        .footer {
            margin-top: 30px;
            text-align: center;
            font-size: 10px;
            color: #666;
            border-top: 1px solid #ddd;
            padding-top: 10px;
        }
        .badge {
            padding: 3px 8px;
            border-radius: 4px;
            font-size: 9px;
            font-weight: bold;
        }
        .badge-success {
            background: #D1FAE5;
            color: #065F46;
        }
        .badge-danger {
            background: #FEE2E2;
            color: #991B1B;
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>LAPORAN KUSTOM</h1>
        <p>Smart Energy Monitoring System</p>
        <p>Ruang Dosen Prodi Teknik Elektro - UNJA</p>
        <p><strong>{{ $summary['period'] }}</strong></p>
        <p>{{ $summary['device_type'] }}</p>
    </div>

    <div class="summary">
        <h3 style="margin-top: 0;">Ringkasan Data</h3>
        <div class="summary-grid">
            <div class="summary-item">
                <div class="summary-label">Total Record</div>
                <div class="summary-value">{{ $summary['total_records'] }}</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Rata-rata Orang</div>
                <div class="summary-value">{{ $summary['avg_people'] }}</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Max Orang</div>
                <div class="summary-value">{{ $summary['max_people'] }}</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Rata-rata Suhu</div>
                <div class="summary-value">{{ $summary['avg_temperature'] }}°C</div>
            </div>
        </div>
        <div class="summary-grid" style="margin-top: 10px;">
            <div class="summary-item">
                <div class="summary-label">Rata-rata Kelembaban</div>
                <div class="summary-value">{{ $summary['avg_humidity'] }}%</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Rata-rata Cahaya</div>
                <div class="summary-value">{{ $summary['avg_light'] }} lux</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">AC ON</div>
                <div class="summary-value">{{ $summary['ac_on_count'] }}x</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Lampu ON</div>
                <div class="summary-value">{{ $summary['lamp_on_count'] }}x</div>
            </div>
        </div>
    </div>

    <h3>Detail Data Sensor</h3>
    <table>
        <thead>
            <tr>
                <th>Tanggal</th>
                <th>Waktu</th>
                <th>Orang</th>
                <th>Suhu</th>
                <th>Kelembaban</th>
                <th>Cahaya</th>
                <th>AC</th>
                <th>Lampu</th>
            </tr>
        </thead>
        <tbody>
            @forelse($data as $item)
            <tr>
                <td>{{ $item->created_at->format('d/m/Y') }}</td>
                <td>{{ $item->created_at->format('H:i:s') }}</td>
                <td>{{ $item->people_count }}</td>
                <td>{{ number_format($item->room_temperature, 1) }}°C</td>
                <td>{{ number_format($item->humidity, 1) }}%</td>
                <td>{{ $item->light_level }} lux</td>
                <td>
                    @if($item->ac_status != 'OFF')
                        <span class="badge badge-success">{{ $item->ac_status }}</span>
                    @else
                        <span class="badge badge-danger">OFF</span>
                    @endif
                </td>
                <td>
                    @if($item->lamp_status == 'ON')
                        <span class="badge badge-success">ON</span>
                    @else
                        <span class="badge badge-danger">OFF</span>
                    @endif
                </td>
            </tr>
            @empty
            <tr>
                <td colspan="8" style="text-align: center; padding: 20px;">Tidak ada data untuk periode yang dipilih</td>
            </tr>
            @endforelse
        </tbody>
    </table>

    <div class="footer">
        <p>Laporan ini dibuat secara otomatis oleh Smart Energy Monitoring System</p>
        <p>Dicetak pada: {{ now()->format('d F Y H:i:s') }} WIB</p>
    </div>
</body>
</html>
