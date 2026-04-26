<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>{{ $config['title'] }}</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            font-size: 12px;
            line-height: 1.5;
            color: #111827;
        }
        .header {
            text-align: center;
            margin-bottom: 24px;
            border-bottom: 3px solid {{ $config['color'] }};
            padding-bottom: 16px;
        }
        .header h1 {
            color: {{ $config['color'] }};
            margin: 0;
            font-size: 22px;
        }
        .header p {
            margin: 4px 0;
            color: #4B5563;
        }
        .summary {
            background: #F3F4F6;
            padding: 14px;
            border-radius: 8px;
            margin-bottom: 18px;
        }
        .summary-grid {
            display: table;
            width: 100%;
        }
        .summary-item {
            display: table-cell;
            padding: 8px;
            text-align: center;
            border-right: 1px solid #D1D5DB;
        }
        .summary-item:last-child {
            border-right: none;
        }
        .summary-label {
            font-size: 10px;
            color: #6B7280;
            text-transform: uppercase;
        }
        .summary-value {
            font-size: 17px;
            font-weight: bold;
            color: {{ $config['color'] }};
            margin-top: 4px;
        }
        .chart-card {
            border: 1px solid #E5E7EB;
            border-radius: 8px;
            padding: 12px;
            margin-bottom: 18px;
        }
        .chart-title {
            margin: 0 0 10px 0;
            color: #111827;
            font-size: 15px;
        }
        .chart-note {
            margin: 0 0 12px 0;
            color: #6B7280;
            font-size: 10px;
        }
        .line-chart-image {
            width: 100%;
            height: auto;
            display: block;
            border: 1px solid #E5E7EB;
        }
        .chart-placeholder {
            padding: 32px 12px;
            text-align: center;
            color: #6B7280;
            background: #F9FAFB;
            border: 1px dashed #D1D5DB;
            border-radius: 6px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 14px;
        }
        th {
            background: {{ $config['color'] }};
            color: white;
            padding: 8px;
            text-align: left;
            font-size: 11px;
        }
        td {
            padding: 7px 8px;
            border-bottom: 1px solid #E5E7EB;
            font-size: 10px;
        }
        tr:nth-child(even) {
            background: #F9FAFB;
        }
        .empty {
            color: #9CA3AF;
            font-style: italic;
        }
        .footer {
            margin-top: 24px;
            text-align: center;
            font-size: 10px;
            color: #6B7280;
            border-top: 1px solid #E5E7EB;
            padding-top: 10px;
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>{{ strtoupper($config['title']) }}</h1>
        <p>Smart Energy Monitoring System</p>
        <p>Ruang Dosen Prodi Teknik Elektro - UNJA</p>
        <p><strong>{{ $date->format('d F Y') }}</strong> | {{ $period }}</p>
    </div>

    <div class="summary">
        <div class="summary-grid">
            <div class="summary-item">
                <div class="summary-label">Total Record</div>
                <div class="summary-value">{{ $summary['total_records'] }}</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Jam Ada Data</div>
                <div class="summary-value">{{ $summary['filled_hours'] }}</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Rata-rata</div>
                <div class="summary-value">
                    @if($summary['average'] !== null)
                        {{ number_format((float) $summary['average'], $config['decimals']) }}{{ $config['unit'] }}
                    @else
                        -
                    @endif
                </div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Maksimum</div>
                <div class="summary-value">
                    @if($summary['max'] !== null)
                        {{ number_format((float) $summary['max'], $config['decimals']) }}{{ $config['unit'] }}
                    @else
                        -
                    @endif
                </div>
            </div>
        </div>
    </div>

    <div class="chart-card">
        <h3 class="chart-title">{{ $config['label'] }} per Jam</h3>
        <p class="chart-note">Grafik menampilkan nilai rata-rata per jam dari tabel sensor_data.</p>

        @if(!empty($chart_image))
            <img class="line-chart-image" src="{{ $chart_image }}" alt="{{ $config['label'] }} per jam">
        @else
            <div class="chart-placeholder">Tidak ada data grafik untuk hari ini.</div>
        @endif
    </div>

    <h3>Data Per Jam</h3>
    <table>
        <thead>
            <tr>
                <th>Waktu</th>
                <th>{{ $config['label'] }}</th>
                <th>Jumlah Log</th>
            </tr>
        </thead>
        <tbody>
            @foreach($rows as $row)
            <tr>
                <td>{{ $row['time'] }}</td>
                <td>
                    @if($row['value'] !== null)
                        {{ number_format((float) $row['value'], $config['decimals']) }}{{ $config['unit'] }}
                    @else
                        <span class="empty">Tidak ada data</span>
                    @endif
                </td>
                <td>{{ $row['records'] }}</td>
            </tr>
            @endforeach
        </tbody>
    </table>

    <div class="footer">
        <p>Laporan ini dibuat otomatis dari tabel sensor_data.</p>
        <p>Dicetak pada: {{ now('Asia/Jakarta')->format('d F Y H:i:s') }} WIB</p>
    </div>
</body>
</html>
