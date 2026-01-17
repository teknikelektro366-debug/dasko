<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\SensorData;
use Illuminate\Http\Request;
use Carbon\Carbon;
use Barryvdh\DomPDF\Facade\Pdf;

class ReportController extends Controller
{
    public function dailyReport(Request $request)
    {
        $date = $request->date ? Carbon::parse($request->date) : Carbon::today();
        
        // Get sensor data for the day
        $sensorData = SensorData::whereDate('created_at', $date)
            ->orderBy('created_at', 'desc')
            ->get();

        $summary = $this->calculateDailySummary($sensorData, $date);

        if ($request->format === 'pdf') {
            return $this->generateDailyPDF($sensorData, $summary, $date);
        }

        return response()->json([
            'success' => true,
            'data' => $sensorData,
            'summary' => $summary
        ]);
    }

    public function weeklyReport(Request $request)
    {
        $startDate = $request->week_start ? Carbon::parse($request->week_start) : Carbon::now()->startOfWeek();
        $endDate = $startDate->copy()->endOfWeek();
        
        $sensorData = SensorData::whereBetween('created_at', [$startDate, $endDate])
            ->orderBy('created_at', 'desc')
            ->get();

        $summary = $this->calculateWeeklySummary($sensorData, $startDate, $endDate);

        if ($request->format === 'pdf') {
            return $this->generateWeeklyPDF($sensorData, $summary, $startDate, $endDate);
        }

        return response()->json([
            'success' => true,
            'data' => $sensorData,
            'summary' => $summary
        ]);
    }

    public function monthlyReport(Request $request)
    {
        $month = $request->month ? Carbon::parse($request->month) : Carbon::now()->startOfMonth();
        $startDate = $month->copy()->startOfMonth();
        $endDate = $month->copy()->endOfMonth();
        
        $sensorData = SensorData::whereBetween('created_at', [$startDate, $endDate])
            ->orderBy('created_at', 'desc')
            ->get();

        $summary = $this->calculateMonthlySummary($sensorData, $startDate, $endDate);

        if ($request->format === 'pdf') {
            return $this->generateMonthlyPDF($sensorData, $summary, $startDate, $endDate);
        }

        return response()->json([
            'success' => true,
            'data' => $sensorData,
            'summary' => $summary
        ]);
    }

    public function efficiencyReport(Request $request)
    {
        $dateFrom = $request->date_from ? Carbon::parse($request->date_from) : Carbon::now()->subMonth();
        $dateTo = $request->date_to ? Carbon::parse($request->date_to) : Carbon::now();
        
        $sensorData = SensorData::whereBetween('created_at', [$dateFrom, $dateTo])
            ->orderBy('created_at', 'desc')
            ->get();

        $summary = $this->calculateEfficiencySummary($sensorData, $dateFrom, $dateTo);

        if ($request->format === 'pdf') {
            return $this->generateEfficiencyPDF($sensorData, $summary, $dateFrom, $dateTo);
        }

        return response()->json([
            'success' => true,
            'data' => $sensorData,
            'summary' => $summary
        ]);
    }

    public function customReport(Request $request)
    {
        $dateFrom = Carbon::parse($request->date_from);
        $dateTo = Carbon::parse($request->date_to);
        $deviceType = $request->device_type;
        
        $query = SensorData::whereBetween('created_at', [$dateFrom, $dateTo]);
        
        if ($deviceType) {
            $query->where('device_id', 'like', '%' . $deviceType . '%');
        }
        
        $sensorData = $query->orderBy('created_at', 'desc')->get();
        $summary = $this->calculateCustomSummary($sensorData, $dateFrom, $dateTo, $deviceType);

        if ($request->format === 'pdf') {
            return $this->generateCustomPDF($sensorData, $summary, $dateFrom, $dateTo, $deviceType);
        }

        return response()->json([
            'success' => true,
            'data' => $sensorData,
            'summary' => $summary
        ]);
    }

    private function calculateDailySummary($sensorData, $date)
    {
        $totalRecords = $sensorData->count();
        $avgTemperature = $sensorData->avg('temperature');
        $avgHumidity = $sensorData->avg('humidity');
        $avgLightIntensity = $sensorData->avg('light_intensity');
        $totalPeopleDetected = $sensorData->sum('people_count');
        $acOnTime = $sensorData->where('ac_status', 'ON')->count();
        $acOffTime = $sensorData->where('ac_status', 'OFF')->count();

        // Hourly breakdown
        $hourlyData = $sensorData->groupBy(function ($item) {
            return Carbon::parse($item->created_at)->format('H');
        })->map(function ($items, $hour) {
            return [
                'hour' => $hour . ':00',
                'count' => $items->count(),
                'avg_temp' => round($items->avg('temperature'), 1),
                'avg_humidity' => round($items->avg('humidity'), 1),
                'people_count' => $items->sum('people_count'),
                'ac_on_count' => $items->where('ac_status', 'ON')->count()
            ];
        });

        return [
            'date' => $date->format('d F Y'),
            'total_records' => $totalRecords,
            'avg_temperature' => round($avgTemperature, 1),
            'avg_humidity' => round($avgHumidity, 1),
            'avg_light_intensity' => round($avgLightIntensity, 0),
            'total_people_detected' => $totalPeopleDetected,
            'ac_on_percentage' => $totalRecords > 0 ? round(($acOnTime / $totalRecords) * 100, 1) : 0,
            'hourly_data' => $hourlyData,
            'peak_hour' => $hourlyData->sortByDesc('people_count')->keys()->first() ?? 'N/A'
        ];
    }

    private function calculateWeeklySummary($sensorData, $startDate, $endDate)
    {
        $totalRecords = $sensorData->count();
        $avgTemperature = $sensorData->avg('temperature');
        $avgHumidity = $sensorData->avg('humidity');
        $totalPeopleDetected = $sensorData->sum('people_count');

        // Daily breakdown
        $dailyData = collect();
        for ($date = $startDate->copy(); $date->lte($endDate); $date->addDay()) {
            $dayData = $sensorData->filter(function ($item) use ($date) {
                return Carbon::parse($item->created_at)->format('Y-m-d') === $date->format('Y-m-d');
            });

            $dailyData->put($date->format('Y-m-d'), [
                'day_name' => $date->format('l'),
                'date' => $date->format('d M'),
                'count' => $dayData->count(),
                'avg_temp' => round($dayData->avg('temperature'), 1),
                'people_count' => $dayData->sum('people_count'),
                'ac_on_count' => $dayData->where('ac_status', 'ON')->count()
            ]);
        }

        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_records' => $totalRecords,
            'avg_temperature' => round($avgTemperature, 1),
            'avg_humidity' => round($avgHumidity, 1),
            'total_people_detected' => $totalPeopleDetected,
            'daily_data' => $dailyData,
            'peak_day' => $dailyData->sortByDesc('people_count')->keys()->first() ?? 'N/A'
        ];
    }

    private function calculateMonthlySummary($sensorData, $startDate, $endDate)
    {
        $totalRecords = $sensorData->count();
        $avgTemperature = $sensorData->avg('temperature');
        $avgHumidity = $sensorData->avg('humidity');
        $totalPeopleDetected = $sensorData->sum('people_count');

        // Weekly breakdown
        $weeklyData = collect();
        $weekStart = $startDate->copy()->startOfWeek();
        $weekNumber = 1;

        while ($weekStart->lte($endDate)) {
            $weekEnd = $weekStart->copy()->endOfWeek();
            if ($weekEnd->gt($endDate)) {
                $weekEnd = $endDate->copy();
            }

            $weekData = $sensorData->filter(function ($item) use ($weekStart, $weekEnd) {
                $itemDate = Carbon::parse($item->created_at);
                return $itemDate->between($weekStart, $weekEnd);
            });

            $weeklyData->put("week_{$weekNumber}", [
                'period' => $weekStart->format('d M') . ' - ' . $weekEnd->format('d M'),
                'count' => $weekData->count(),
                'avg_temp' => round($weekData->avg('temperature'), 1),
                'people_count' => $weekData->sum('people_count')
            ]);

            $weekStart->addWeek();
            $weekNumber++;
        }

        return [
            'month' => $startDate->format('F Y'),
            'total_records' => $totalRecords,
            'avg_temperature' => round($avgTemperature, 1),
            'avg_humidity' => round($avgHumidity, 1),
            'total_people_detected' => $totalPeopleDetected,
            'weekly_data' => $weeklyData
        ];
    }

    private function calculateEfficiencySummary($sensorData, $dateFrom, $dateTo)
    {
        $totalRecords = $sensorData->count();
        $acOnRecords = $sensorData->where('ac_status', 'ON');
        $acOffRecords = $sensorData->where('ac_status', 'OFF');
        
        $efficiencyScore = 0;
        if ($totalRecords > 0) {
            $peopleWithAcOn = $acOnRecords->where('people_count', '>', 0)->count();
            $peopleWithAcOff = $acOffRecords->where('people_count', 0)->count();
            $efficiencyScore = (($peopleWithAcOn + $peopleWithAcOff) / $totalRecords) * 100;
        }

        return [
            'period' => $dateFrom->format('d M Y') . ' - ' . $dateTo->format('d M Y'),
            'total_records' => $totalRecords,
            'ac_on_records' => $acOnRecords->count(),
            'ac_off_records' => $acOffRecords->count(),
            'efficiency_score' => round($efficiencyScore, 1),
            'avg_people_when_ac_on' => round($acOnRecords->avg('people_count'), 1),
            'avg_temp_when_ac_on' => round($acOnRecords->avg('temperature'), 1),
            'energy_saving_opportunities' => $acOnRecords->where('people_count', 0)->count()
        ];
    }

    private function calculateCustomSummary($sensorData, $dateFrom, $dateTo, $deviceType)
    {
        return [
            'period' => $dateFrom->format('d M Y') . ' - ' . $dateTo->format('d M Y'),
            'device_filter' => $deviceType ?? 'Semua Perangkat',
            'total_records' => $sensorData->count(),
            'avg_temperature' => round($sensorData->avg('temperature'), 1),
            'avg_humidity' => round($sensorData->avg('humidity'), 1),
            'total_people_detected' => $sensorData->sum('people_count'),
            'ac_usage_percentage' => $sensorData->count() > 0 ? 
                round(($sensorData->where('ac_status', 'ON')->count() / $sensorData->count()) * 100, 1) : 0
        ];
    }

    private function generateDailyPDF($sensorData, $summary, $date)
    {
        $html = $this->generateReportHTML('daily', [
            'sensorData' => $sensorData,
            'summary' => $summary,
            'date' => $date
        ]);

        $filename = 'laporan_harian_' . $date->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html; charset=utf-8')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateWeeklyPDF($sensorData, $summary, $startDate, $endDate)
    {
        $html = $this->generateReportHTML('weekly', [
            'sensorData' => $sensorData,
            'summary' => $summary,
            'startDate' => $startDate,
            'endDate' => $endDate
        ]);

        $filename = 'laporan_mingguan_' . $startDate->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html; charset=utf-8')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateMonthlyPDF($sensorData, $summary, $startDate, $endDate)
    {
        $html = $this->generateReportHTML('monthly', [
            'sensorData' => $sensorData,
            'summary' => $summary,
            'startDate' => $startDate,
            'endDate' => $endDate
        ]);

        $filename = 'laporan_bulanan_' . $startDate->format('Y_m') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html; charset=utf-8')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateEfficiencyPDF($sensorData, $summary, $dateFrom, $dateTo)
    {
        $html = $this->generateReportHTML('efficiency', [
            'sensorData' => $sensorData,
            'summary' => $summary,
            'dateFrom' => $dateFrom,
            'dateTo' => $dateTo
        ]);

        $filename = 'laporan_efisiensi_' . $dateFrom->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html; charset=utf-8')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateCustomPDF($sensorData, $summary, $dateFrom, $dateTo, $deviceType)
    {
        $html = $this->generateReportHTML('custom', [
            'sensorData' => $sensorData,
            'summary' => $summary,
            'dateFrom' => $dateFrom,
            'dateTo' => $dateTo,
            'deviceType' => $deviceType
        ]);

        $filename = 'laporan_kustom_' . $dateFrom->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html; charset=utf-8')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateReportHTML($type, $data)
    {
        $title = '';
        $content = '';

        switch ($type) {
            case 'daily':
                $title = 'Laporan Harian Smart Energy - ' . $data['date']->format('d F Y');
                $content = $this->generateDailyContent($data);
                break;
            case 'weekly':
                $title = 'Laporan Mingguan Smart Energy - ' . $data['startDate']->format('d M') . ' s/d ' . $data['endDate']->format('d M Y');
                $content = $this->generateWeeklyContent($data);
                break;
            case 'monthly':
                $title = 'Laporan Bulanan Smart Energy - ' . $data['startDate']->format('F Y');
                $content = $this->generateMonthlyContent($data);
                break;
            case 'efficiency':
                $title = 'Laporan Efisiensi Energi - ' . $data['dateFrom']->format('d M') . ' s/d ' . $data['dateTo']->format('d M Y');
                $content = $this->generateEfficiencyContent($data);
                break;
            case 'custom':
                $title = 'Laporan Kustom Smart Energy - ' . $data['dateFrom']->format('d M') . ' s/d ' . $data['dateTo']->format('d M Y');
                $content = $this->generateCustomContent($data);
                break;
        }

        return '<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>' . $title . '</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; color: #333; }
        .header { text-align: center; margin-bottom: 30px; border-bottom: 2px solid #007bff; padding-bottom: 20px; }
        .header h1 { color: #007bff; margin: 0; }
        .header p { margin: 5px 0; color: #666; }
        .summary { background: #f8f9fa; padding: 20px; border-radius: 8px; margin-bottom: 30px; }
        .summary h2 { color: #007bff; margin-top: 0; }
        .summary-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }
        .summary-item { background: white; padding: 15px; border-radius: 5px; border-left: 4px solid #007bff; }
        .summary-item h4 { margin: 0 0 5px 0; color: #333; }
        .summary-item .value { font-size: 24px; font-weight: bold; color: #007bff; }
        .summary-item .unit { font-size: 14px; color: #666; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }
        th { background: #007bff; color: white; }
        tr:nth-child(even) { background: #f8f9fa; }
        .footer { margin-top: 40px; text-align: center; color: #666; font-size: 12px; border-top: 1px solid #ddd; padding-top: 20px; }
        .status-on { background: #28a745; color: white; padding: 2px 8px; border-radius: 12px; font-size: 11px; }
        .status-off { background: #6c757d; color: white; padding: 2px 8px; border-radius: 12px; font-size: 11px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>' . $title . '</h1>
        <p>Universitas Jambi - Fakultas Sains dan Teknologi</p>
        <p>Ruang Dosen Prodi Teknik Elektro</p>
        <p>Digenerate pada: ' . now()->setTimezone('Asia/Jakarta')->format('d F Y H:i:s') . ' WIB</p>
    </div>
    
    ' . $content . '
    
    <div class="footer">
        <p>Laporan ini digenerate secara otomatis oleh Smart Energy Management System</p>
        <p>© 2025 Universitas Jambi - Sistem Monitoring Energi Cerdas</p>
    </div>
</body>
</html>';
    }

    private function generateDailyContent($data)
    {
        $summary = $data['summary'];
        $sensorData = $data['sensorData'];

        $content = '<div class="summary">
            <h2>Ringkasan Harian</h2>
            <div class="summary-grid">
                <div class="summary-item">
                    <h4>Total Data</h4>
                    <div class="value">' . number_format($summary['total_records']) . '</div>
                    <div class="unit">records</div>
                </div>
                <div class="summary-item">
                    <h4>Suhu Rata-rata</h4>
                    <div class="value">' . $summary['avg_temperature'] . '</div>
                    <div class="unit">°C</div>
                </div>
                <div class="summary-item">
                    <h4>Kelembaban Rata-rata</h4>
                    <div class="value">' . $summary['avg_humidity'] . '</div>
                    <div class="unit">%</div>
                </div>
                <div class="summary-item">
                    <h4>Total Orang Terdeteksi</h4>
                    <div class="value">' . $summary['total_people_detected'] . '</div>
                    <div class="unit">orang</div>
                </div>
                <div class="summary-item">
                    <h4>AC Aktif</h4>
                    <div class="value">' . $summary['ac_on_percentage'] . '</div>
                    <div class="unit">% waktu</div>
                </div>
                <div class="summary-item">
                    <h4>Jam Puncak</h4>
                    <div class="value">' . $summary['peak_hour'] . '</div>
                    <div class="unit">aktivitas tertinggi</div>
                </div>
            </div>
        </div>';

        if ($summary['hourly_data']->isNotEmpty()) {
            $content .= '<h2>Data Per Jam</h2>
            <table>
                <thead>
                    <tr>
                        <th>Jam</th>
                        <th>Jumlah Data</th>
                        <th>Suhu Rata-rata (°C)</th>
                        <th>Kelembaban (%)</th>
                        <th>Orang Terdeteksi</th>
                        <th>AC Aktif</th>
                    </tr>
                </thead>
                <tbody>';

            foreach ($summary['hourly_data'] as $hourData) {
                $content .= '<tr>
                    <td>' . $hourData['hour'] . '</td>
                    <td>' . $hourData['count'] . '</td>
                    <td>' . $hourData['avg_temp'] . '</td>
                    <td>' . $hourData['avg_humidity'] . '</td>
                    <td>' . $hourData['people_count'] . '</td>
                    <td>' . $hourData['ac_on_count'] . '</td>
                </tr>';
            }

            $content .= '</tbody></table>';
        }

        return $content;
    }

    private function generateWeeklyContent($data)
    {
        $summary = $data['summary'];
        
        $content = '<div class="summary">
            <h2>Ringkasan Mingguan</h2>
            <div class="summary-grid">
                <div class="summary-item">
                    <h4>Total Data</h4>
                    <div class="value">' . number_format($summary['total_records']) . '</div>
                    <div class="unit">records</div>
                </div>
                <div class="summary-item">
                    <h4>Suhu Rata-rata</h4>
                    <div class="value">' . $summary['avg_temperature'] . '</div>
                    <div class="unit">°C</div>
                </div>
                <div class="summary-item">
                    <h4>Kelembaban Rata-rata</h4>
                    <div class="value">' . $summary['avg_humidity'] . '</div>
                    <div class="unit">%</div>
                </div>
                <div class="summary-item">
                    <h4>Total Orang Terdeteksi</h4>
                    <div class="value">' . $summary['total_people_detected'] . '</div>
                    <div class="unit">orang</div>
                </div>
            </div>
        </div>';

        if ($summary['daily_data']->isNotEmpty()) {
            $content .= '<h2>Data Per Hari</h2>
            <table>
                <thead>
                    <tr>
                        <th>Hari</th>
                        <th>Tanggal</th>
                        <th>Jumlah Data</th>
                        <th>Suhu Rata-rata (°C)</th>
                        <th>Orang Terdeteksi</th>
                        <th>AC Aktif</th>
                    </tr>
                </thead>
                <tbody>';

            foreach ($summary['daily_data'] as $dayData) {
                $content .= '<tr>
                    <td>' . $dayData['day_name'] . '</td>
                    <td>' . $dayData['date'] . '</td>
                    <td>' . $dayData['count'] . '</td>
                    <td>' . $dayData['avg_temp'] . '</td>
                    <td>' . $dayData['people_count'] . '</td>
                    <td>' . $dayData['ac_on_count'] . '</td>
                </tr>';
            }

            $content .= '</tbody></table>';
        }

        return $content;
    }

    private function generateMonthlyContent($data)
    {
        $summary = $data['summary'];
        
        return '<div class="summary">
            <h2>Ringkasan Bulanan</h2>
            <div class="summary-grid">
                <div class="summary-item">
                    <h4>Total Data</h4>
                    <div class="value">' . number_format($summary['total_records']) . '</div>
                    <div class="unit">records</div>
                </div>
                <div class="summary-item">
                    <h4>Suhu Rata-rata</h4>
                    <div class="value">' . $summary['avg_temperature'] . '</div>
                    <div class="unit">°C</div>
                </div>
                <div class="summary-item">
                    <h4>Kelembaban Rata-rata</h4>
                    <div class="value">' . $summary['avg_humidity'] . '</div>
                    <div class="unit">%</div>
                </div>
                <div class="summary-item">
                    <h4>Total Orang Terdeteksi</h4>
                    <div class="value">' . $summary['total_people_detected'] . '</div>
                    <div class="unit">orang</div>
                </div>
            </div>
        </div>';
    }

    private function generateEfficiencyContent($data)
    {
        $summary = $data['summary'];
        
        return '<div class="summary">
            <h2>Analisis Efisiensi Energi</h2>
            <div class="summary-grid">
                <div class="summary-item">
                    <h4>Skor Efisiensi</h4>
                    <div class="value">' . $summary['efficiency_score'] . '</div>
                    <div class="unit">%</div>
                </div>
                <div class="summary-item">
                    <h4>AC Aktif</h4>
                    <div class="value">' . $summary['ac_on_records'] . '</div>
                    <div class="unit">records</div>
                </div>
                <div class="summary-item">
                    <h4>Rata-rata Orang saat AC ON</h4>
                    <div class="value">' . $summary['avg_people_when_ac_on'] . '</div>
                    <div class="unit">orang</div>
                </div>
                <div class="summary-item">
                    <h4>Peluang Hemat Energi</h4>
                    <div class="value">' . $summary['energy_saving_opportunities'] . '</div>
                    <div class="unit">kejadian</div>
                </div>
            </div>
        </div>';
    }

    private function generateCustomContent($data)
    {
        $summary = $data['summary'];
        
        return '<div class="summary">
            <h2>Laporan Kustom</h2>
            <p><strong>Filter Perangkat:</strong> ' . $summary['device_filter'] . '</p>
            <div class="summary-grid">
                <div class="summary-item">
                    <h4>Total Data</h4>
                    <div class="value">' . number_format($summary['total_records']) . '</div>
                    <div class="unit">records</div>
                </div>
                <div class="summary-item">
                    <h4>Suhu Rata-rata</h4>
                    <div class="value">' . $summary['avg_temperature'] . '</div>
                    <div class="unit">°C</div>
                </div>
                <div class="summary-item">
                    <h4>Kelembaban Rata-rata</h4>
                    <div class="value">' . $summary['avg_humidity'] . '</div>
                    <div class="unit">%</div>
                </div>
                <div class="summary-item">
                    <h4>Penggunaan AC</h4>
                    <div class="value">' . $summary['ac_usage_percentage'] . '</div>
                    <div class="unit">%</div>
                </div>
            </div>
        </div>';
    }
}