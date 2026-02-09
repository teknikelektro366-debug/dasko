<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Laporan Efisiensi Energi</title>
    <style>
        body { font-family: Arial, sans-serif; font-size: 12px; line-height: 1.6; color: #333; }
        .header { text-align: center; margin-bottom: 30px; border-bottom: 3px solid #F59E0B; padding-bottom: 20px; }
        .header h1 { color: #F59E0B; margin: 0; font-size: 24px; }
        .header p { margin: 5px 0; color: #666; }
        .summary { background: #F3F4F6; padding: 15px; border-radius: 8px; margin-bottom: 20px; }
        .summary-grid { display: table; width: 100%; }
        .summary-item { display: table-cell; padding: 10px; text-align: center; border-right: 1px solid #ddd; }
        .summary-item:last-child { border-right: none; }
        .summary-label { font-size: 10px; color: #666; text-transform: uppercase; }
        .summary-value { font-size: 18px; font-weight: bold; color: #F59E0B; margin-top: 5px; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th { background: #F59E0B; color: white; padding: 10px; text-align: left; font-size: 11px; }
        td { padding: 8px; border-bottom: 1px solid #E5E7EB; font-size: 10px; }
        tr:nth-child(even) { background: #F9FAFB; }
        .footer { margin-top: 30px; text-align: center; font-size: 10px; color: #666; border-top: 1px solid #ddd; padding-top: 10px; }
        .chart-box { background: #F9FAFB; padding: 15px; border-radius: 8px; margin: 20px 0; }
    </style>
</head>
<body>
    <div class="header">
        <h1>LAPORAN EFISIENSI ENERGI</h1>
        <p>Smart Energy Monitoring System</p>
        <p>Ruang Dosen Prodi Teknik Elektro - UNJA</p>
        <p><strong>{{ $startDate->format('d M Y') }} - {{ $endDate->format('d M Y') }}</strong></p>
    </div>

    <div class="summary">
        <h3 style="margin-top: 0;">Analisis Efisiensi</h3>
        <div class="summary-grid">
            <div class="summary-item">
                <div class="summary-label">Total Record</div>
                <div class="summary-value">{{ $summary['total_records'] }}</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Penggunaan AC</div>
                <div class="summary-value">{{ $summary['ac_usage_percentage'] }}%</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Penggunaan Lampu</div>
                <div class="summary-value">{{ $summary['lamp_usage_percentage'] }}%</div>
            </div>
            <div class="summary-item">
                <div class="summary-label">Rata-rata Suhu</div>
                <div class="summary-value">{{ $summary['avg_temperature'] }}°C</div>
            </div>
        </div>
    </div>

    <div class="chart-box">
        <h3>Efisiensi Penggunaan Perangkat</h3>
        <table>
            <tr>
                <td><strong>Rata-rata orang saat AC ON:</strong></td>
                <td>{{ $summary['avg_people_when_ac_on'] }} orang</td>
            </tr>
            <tr>
                <td><strong>Rata-rata orang saat Lampu ON:</strong></td>
                <td>{{ $summary['avg_people_when_lamp_on'] }} orang</td>
            </tr>
            <tr>
                <td><strong>Rata-rata kelembaban:</strong></td>
                <td>{{ $summary['avg_humidity'] }}%</td>
            </tr>
        </table>
    </div>

    <h3>Rekomendasi Efisiensi</h3>
    <div style="background: #EFF6FF; padding: 15px; border-left: 4px solid #3B82F6; margin: 20px 0;">
        <p><strong>Berdasarkan analisis data:</strong></p>
        <ul style="margin: 10px 0; padding-left: 20px;">
            @if($summary['ac_usage_percentage'] > 70)
                <li>Penggunaan AC cukup tinggi ({{ $summary['ac_usage_percentage'] }}%). Pertimbangkan untuk mengoptimalkan suhu target AC.</li>
            @else
                <li>Penggunaan AC sudah efisien ({{ $summary['ac_usage_percentage'] }}%).</li>
            @endif
            
            @if($summary['lamp_usage_percentage'] > 80)
                <li>Penggunaan lampu sangat tinggi ({{ $summary['lamp_usage_percentage'] }}%). Manfaatkan cahaya alami lebih optimal.</li>
            @else
                <li>Penggunaan lampu sudah baik ({{ $summary['lamp_usage_percentage'] }}%).</li>
            @endif
            
            @if($summary['avg_people_when_ac_on'] < 3)
                <li>AC sering menyala dengan jumlah orang sedikit. Pertimbangkan untuk meningkatkan threshold aktivasi AC.</li>
            @endif
        </ul>
    </div>

    <div class="footer">
        <p>Laporan ini dibuat secara otomatis oleh Smart Energy Monitoring System</p>
        <p>Dicetak pada: {{ now()->format('d F Y H:i:s') }} WIB</p>
    </div>
</body>
</html>
