<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\SensorData;
use Illuminate\Http\Request;
use Carbon\Carbon;

class SimpleReportController extends Controller
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
            return $this->generateSimplePDF('daily', $sensorData, $summary, $date);
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
            return $this->generateSimplePDF('weekly', $sensorData, $summary, $startDate, $endDate);
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
            return $this->generateSimplePDF('monthly', $sensorData, $summary, $startDate, $endDate);
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
        $avgTemperature = $sensorData->avg('temperature') ?? 0;
        $avgHumidity = $sensorData->avg('humidity') ?? 0;
        $avgLightIntensity = $sensorData->avg('light_intensity') ?? 0;
        $totalPeopleDetected = $sensorData->sum('people_count') ?? 0;
        $acOnTime = $sensorData->where('ac_status', 'ON')->count();

        return [
            'date' => $date->format('d F Y'),
            'total_records' => $totalRecords,
            'avg_temperature' => round($avgTemperature, 1),
            'avg_humidity' => round($avgHumidity, 1),
            'avg_light_intensity' => round($avgLightIntensity, 0),
            'total_people_detected' => $totalPeopleDetected,
            'ac_on_percentage' => $totalRecords > 0 ? round(($acOnTime / $totalRecords) * 100, 1) : 0,
        ];
    }

    private function calculateWeeklySummary($sensorData, $startDate, $endDate)
    {
        $totalRecords = $sensorData->count();
        $avgTemperature = $sensorData->avg('temperature') ?? 0;
        $avgHumidity = $sensorData->avg('humidity') ?? 0;
        $totalPeopleDetected = $sensorData->sum('people_count') ?? 0;

        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_records' => $totalRecords,
            'avg_temperature' => round($avgTemperature, 1),
            'avg_humidity' => round($avgHumidity, 1),
            'total_people_detected' => $totalPeopleDetected,
        ];
    }

    private function calculateMonthlySummary($sensorData, $startDate, $endDate)
    {
        $totalRecords = $sensorData->count();
        $avgTemperature = $sensorData->avg('temperature') ?? 0;
        $avgHumidity = $sensorData->avg('humidity') ?? 0;
        $totalPeopleDetected = $sensorData->sum('people_count') ?? 0;

        return [
            'month' => $startDate->format('F Y'),
            'total_records' => $totalRecords,
            'avg_temperature' => round($avgTemperature, 1),
            'avg_humidity' => round($avgHumidity, 1),
            'total_people_detected' => $totalPeopleDetected,
        ];
    }

    private function generateSimplePDF($type, $sensorData, $summary, ...$dates)
    {
        $title = '';
        $period = '';
        
        switch ($type) {
            case 'daily':
                $title = 'Laporan Harian Smart Energy';
                $period = $summary['date'];
                break;
            case 'weekly':
                $title = 'Laporan Mingguan Smart Energy';
                $period = $summary['period'];
                break;
            case 'monthly':
                $title = 'Laporan Bulanan Smart Energy';
                $period = $summary['month'];
                break;
        }

        $html = '<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>' . $title . '</title>
    <style>
        @media print {
            body { margin: 0; }
            .no-print { display: none; }
        }
        body { 
            font-family: Arial, sans-serif; 
            margin: 20px; 
            color: #333; 
            line-height: 1.6;
        }
        .header { 
            text-align: center; 
            margin-bottom: 30px; 
            border-bottom: 3px solid #007bff; 
            padding-bottom: 20px; 
        }
        .header h1 { 
            color: #007bff; 
            margin: 0; 
            font-size: 28px;
        }
        .header p { 
            margin: 5px 0; 
            color: #666; 
            font-size: 14px;
        }
        .summary { 
            background: #f8f9fa; 
            padding: 20px; 
            border-radius: 8px; 
            margin-bottom: 30px; 
            border-left: 5px solid #007bff;
        }
        .summary h2 { 
            color: #007bff; 
            margin-top: 0; 
            font-size: 20px;
        }
        .summary-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); 
            gap: 15px; 
            margin-top: 15px;
        }
        .summary-item { 
            background: white; 
            padding: 15px; 
            border-radius: 5px; 
            border-left: 4px solid #28a745; 
            text-align: center;
        }
        .summary-item h4 { 
            margin: 0 0 5px 0; 
            color: #333; 
            font-size: 14px;
        }
        .summary-item .value { 
            font-size: 24px; 
            font-weight: bold; 
            color: #007bff; 
        }
        .summary-item .unit { 
            font-size: 12px; 
            color: #666; 
        }
        .data-section {
            margin-top: 30px;
        }
        .data-section h3 {
            color: #007bff;
            border-bottom: 2px solid #e9ecef;
            padding-bottom: 10px;
        }
        table { 
            width: 100%; 
            border-collapse: collapse; 
            margin-top: 15px; 
            font-size: 12px;
        }
        th, td { 
            padding: 8px; 
            text-align: left; 
            border-bottom: 1px solid #ddd; 
        }
        th { 
            background: #007bff; 
            color: white; 
            font-weight: bold;
        }
        tr:nth-child(even) { 
            background: #f8f9fa; 
        }
        .footer { 
            margin-top: 40px; 
            text-align: center; 
            color: #666; 
            font-size: 11px; 
            border-top: 1px solid #ddd; 
            padding-top: 20px; 
        }
        .print-btn {
            background: #007bff;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin: 10px;
            font-size: 14px;
        }
        .print-btn:hover {
            background: #0056b3;
        }
    </style>
</head>
<body>
    <div class="no-print">
        <button class="print-btn" onclick="window.print()">🖨️ Print/Save as PDF</button>
        <button class="print-btn" onclick="window.close()">❌ Close</button>
    </div>
    
    <div class="header">
        <h1>' . $title . '</h1>
        <p><strong>Universitas Jambi - Fakultas Sains dan Teknologi</strong></p>
        <p>Ruang Dosen Prodi Teknik Elektro</p>
        <p><strong>Periode: ' . $period . '</strong></p>
        <p>Digenerate pada: ' . now()->setTimezone('Asia/Jakarta')->format('d F Y H:i:s') . ' WIB</p>
    </div>
    
    <div class="summary">
        <h2>📊 Ringkasan ' . ucfirst($type) . '</h2>
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
            </div>';

        if (isset($summary['avg_light_intensity'])) {
            $html .= '<div class="summary-item">
                <h4>Intensitas Cahaya</h4>
                <div class="value">' . $summary['avg_light_intensity'] . '</div>
                <div class="unit">lux</div>
            </div>';
        }

        if (isset($summary['total_people_detected'])) {
            $html .= '<div class="summary-item">
                <h4>Total Orang Terdeteksi</h4>
                <div class="value">' . $summary['total_people_detected'] . '</div>
                <div class="unit">orang</div>
            </div>';
        }

        if (isset($summary['ac_on_percentage'])) {
            $html .= '<div class="summary-item">
                <h4>AC Aktif</h4>
                <div class="value">' . $summary['ac_on_percentage'] . '</div>
                <div class="unit">% waktu</div>
            </div>';
        }

        $html .= '</div>
    </div>';

        // Add recent data table
        if ($sensorData->count() > 0) {
            $html .= '<div class="data-section">
                <h3>📋 Data Sensor Terbaru (10 Terakhir)</h3>
                <table>
                    <thead>
                        <tr>
                            <th>Waktu</th>
                            <th>Suhu (°C)</th>
                            <th>Kelembaban (%)</th>
                            <th>Cahaya (lux)</th>
                            <th>Orang</th>
                            <th>AC Status</th>
                        </tr>
                    </thead>
                    <tbody>';

            foreach ($sensorData->take(10) as $data) {
                $timestamp = Carbon::parse($data->created_at)->setTimezone('Asia/Jakarta')->format('d/m H:i');
                $html .= '<tr>
                    <td>' . $timestamp . '</td>
                    <td>' . ($data->temperature ?? '--') . '</td>
                    <td>' . ($data->humidity ?? '--') . '</td>
                    <td>' . ($data->light_intensity ?? '--') . '</td>
                    <td>' . ($data->people_count ?? 0) . '</td>
                    <td>' . ($data->ac_status ?? 'OFF') . '</td>
                </tr>';
            }

            $html .= '</tbody>
                </table>
            </div>';
        }

        $html .= '<div class="footer">
            <p><strong>Laporan ini digenerate secara otomatis oleh Smart Energy Management System</strong></p>
            <p>© 2025 Universitas Jambi - Sistem Monitoring Energi Cerdas</p>
            <p>Data diambil dari sensor ESP32 secara real-time</p>
        </div>
    </body>
    </html>';

        return response($html, 200, [
            'Content-Type' => 'text/html; charset=utf-8',
        ]);
    }
}