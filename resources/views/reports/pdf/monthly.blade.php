<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Laporan Bulanan</title>
    <style>
        body { font-family: Arial, sans-serif; font-size: 12px; line-height: 1.6; color: #333; }
        .header { text-align: center; margin-bottom: 30px; border-bottom: 3px solid #8B5CF6; padding-bottom: 20px; }
        .header h1 { color: #8B5CF6; margin: 0; font-size: 24px; }
        .header p { margin: 5px 0; color: #666; }
        .summary { background: #F3F4F6; padding: 15px; border-radius: 8px; margin-bottom: 20px; }
        .summary-grid { display: table; width: 100%; }
        .summary-item { display: table-cell; padding: 10px; text-align: center; border-right: 1px solid #ddd; }
        .summary-item:last-child { border-right: none; }
        .summary-label { font-size: 10px; color: #666; text-transform: uppercase; }
        .summary-value { font-size: 18px; font-weight: bold; color: #8B5CF6; margin-top: 5px; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th { background: #8B5CF6; color: white; padding: 10px; text-align: left; font-size: 11px; }
        td { padding: 8px; border-bottom: 1px solid #E5E7EB; font-size: 10px; }
        tr:nth-child(even) { background: #F9FAFB; }
        .footer { margin-top: 30px; text-align: center; font-size: 10px; color: #666; border-top: 1px solid #ddd; padding-top: 10px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>LAPORAN BULANAN</h1>
        <p>Smart Energy Monitoring System</p>
        <p>Ruang Dosen Prodi Teknik Elektro - UNJA</p>
        <p><strong>{{ $summary['month'] }}</strong></p>
    </div>

    <div class="summary">
        <h3 style="margin-top: 0;">Ringkasan Bulanan</h3>
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
    </div>

    <h3>Ringkasan Per Minggu</h3>
    <table>
        <thead>
            <tr>
                <th>Minggu</th>
                <th>Periode</th>
                <th>Total Record</th>
                <th>Rata-rata Orang</th>
                <th>Rata-rata Suhu</th>
            </tr>
        </thead>
        <tbody>
            @foreach($summary['weekly_data'] as $week)
            <tr>
                <td>{{ $week['week'] }}</td>
                <td>{{ $week['period'] }}</td>
                <td>{{ $week['records'] }}</td>
                <td>{{ $week['avg_people'] }}</td>
                <td>{{ $week['avg_temp'] }}°C</td>
            </tr>
            @endforeach
        </tbody>
    </table>

    <div class="footer">
        <p>Laporan ini dibuat secara otomatis oleh Smart Energy Monitoring System</p>
        <p>Dicetak pada: {{ now()->format('d F Y H:i:s') }} WIB</p>
    </div>
</body>
</html>
