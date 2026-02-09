<?php

namespace App\Http\Controllers;

use App\Models\SensorData;
use Illuminate\Http\Request;
use Barryvdh\DomPDF\Facade\Pdf;
use Carbon\Carbon;

class PdfReportController extends Controller
{
    /**
     * Generate Daily PDF Report
     */
    public function dailyReport(Request $request)
    {
        try {
            $date = $request->input('date', now()->format('Y-m-d'));
            $startDate = Carbon::parse($date)->startOfDay();
            $endDate = Carbon::parse($date)->endOfDay();
            
            $data = SensorData::whereBetween('created_at', [$startDate, $endDate])
                             ->orderBy('created_at', 'desc')
                             ->get();
            
            $summary = $this->calculateDailySummary($data, $startDate);
            
            $pdf = PDF::loadView('reports.pdf.daily', [
                'data' => $data,
                'summary' => $summary,
                'date' => $startDate
            ]);
            
            $filename = 'laporan_harian_' . $startDate->format('Y_m_d') . '.pdf';
            
            // Use stream() instead of download() to open in browser
            return $pdf->stream($filename);
        } catch (\Exception $e) {
            \Log::error('PDF Generation Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate PDF: ' . $e->getMessage()], 500);
        }
    }
    
    /**
     * Generate Weekly PDF Report
     */
    public function weeklyReport(Request $request)
    {
        try {
            $weekStart = $request->input('week_start', now()->startOfWeek()->format('Y-m-d'));
            $startDate = Carbon::parse($weekStart)->startOfWeek();
            $endDate = $startDate->copy()->endOfWeek();
            
            $data = SensorData::whereBetween('created_at', [$startDate, $endDate])
                             ->orderBy('created_at', 'desc')
                             ->get();
            
            $summary = $this->calculateWeeklySummary($data, $startDate, $endDate);
            
            $pdf = PDF::loadView('reports.pdf.weekly', [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ]);
            
            $filename = 'laporan_mingguan_' . $startDate->format('Y_m_d') . '.pdf';
            return $pdf->stream($filename);
        } catch (\Exception $e) {
            \Log::error('PDF Generation Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate PDF: ' . $e->getMessage()], 500);
        }
    }
    
    /**
     * Generate Monthly PDF Report
     */
    public function monthlyReport(Request $request)
    {
        try {
            $month = $request->input('month', now()->format('Y-m'));
            $startDate = Carbon::parse($month)->startOfMonth();
            $endDate = Carbon::parse($month)->endOfMonth();
            
            $data = SensorData::whereBetween('created_at', [$startDate, $endDate])
                             ->orderBy('created_at', 'desc')
                             ->get();
            
            $summary = $this->calculateMonthlySummary($data, $startDate, $endDate);
            
            $pdf = PDF::loadView('reports.pdf.monthly', [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ]);
            
            $filename = 'laporan_bulanan_' . $startDate->format('Y_m') . '.pdf';
            return $pdf->stream($filename);
        } catch (\Exception $e) {
            \Log::error('PDF Generation Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate PDF: ' . $e->getMessage()], 500);
        }
    }
    
    /**
     * Generate Efficiency PDF Report
     */
    public function efficiencyReport(Request $request)
    {
        try {
            $dateFrom = $request->input('date_from', now()->subDays(7)->format('Y-m-d'));
            $dateTo = $request->input('date_to', now()->format('Y-m-d'));
            $startDate = Carbon::parse($dateFrom)->startOfDay();
            $endDate = Carbon::parse($dateTo)->endOfDay();
            
            $data = SensorData::whereBetween('created_at', [$startDate, $endDate])
                             ->orderBy('created_at', 'desc')
                             ->get();
            
            $summary = $this->calculateEfficiencySummary($data, $startDate, $endDate);
            
            $pdf = PDF::loadView('reports.pdf.efficiency', [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ]);
            
            $filename = 'laporan_efisiensi_' . $startDate->format('Y_m_d') . '.pdf';
            return $pdf->stream($filename);
        } catch (\Exception $e) {
            \Log::error('PDF Generation Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate PDF: ' . $e->getMessage()], 500);
        }
    }
    
    /**
     * Calculate Daily Summary
     */
    private function calculateDailySummary($data, $date)
    {
        return [
            'date' => $date->format('d F Y'),
            'total_records' => $data->count(),
            'avg_people' => round($data->avg('people_count') ?? 0, 1),
            'max_people' => $data->max('people_count') ?? 0,
            'avg_temperature' => round($data->avg('room_temperature') ?? 0, 1),
            'avg_humidity' => round($data->avg('humidity') ?? 0, 1),
            'avg_light' => round($data->avg('light_level') ?? 0, 0),
            'ac_on_count' => $data->where('ac_status', '!=', 'OFF')->count(),
            'lamp_on_count' => $data->where('lamp_status', 'ON')->count(),
        ];
    }
    
    /**
     * Calculate Weekly Summary
     */
    private function calculateWeeklySummary($data, $startDate, $endDate)
    {
        $dailyData = [];
        for ($date = $startDate->copy(); $date->lte($endDate); $date->addDay()) {
            $dayData = $data->filter(function($item) use ($date) {
                return $item->created_at->format('Y-m-d') === $date->format('Y-m-d');
            });
            
            $dailyData[] = [
                'date' => $date->format('d M'),
                'day' => $date->format('l'),
                'records' => $dayData->count(),
                'avg_people' => round($dayData->avg('people_count') ?? 0, 1),
                'avg_temp' => round($dayData->avg('room_temperature') ?? 0, 1),
            ];
        }
        
        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_records' => $data->count(),
            'avg_people' => round($data->avg('people_count') ?? 0, 1),
            'max_people' => $data->max('people_count') ?? 0,
            'avg_temperature' => round($data->avg('room_temperature') ?? 0, 1),
            'daily_data' => $dailyData,
        ];
    }
    
    /**
     * Calculate Monthly Summary
     */
    private function calculateMonthlySummary($data, $startDate, $endDate)
    {
        $weeklyData = [];
        $weekStart = $startDate->copy()->startOfWeek();
        $weekNum = 1;
        
        while ($weekStart->lte($endDate)) {
            $weekEnd = $weekStart->copy()->endOfWeek();
            if ($weekEnd->gt($endDate)) {
                $weekEnd = $endDate->copy();
            }
            
            $weekData = $data->filter(function($item) use ($weekStart, $weekEnd) {
                return $item->created_at->between($weekStart, $weekEnd);
            });
            
            $weeklyData[] = [
                'week' => 'Minggu ' . $weekNum,
                'period' => $weekStart->format('d M') . ' - ' . $weekEnd->format('d M'),
                'records' => $weekData->count(),
                'avg_people' => round($weekData->avg('people_count') ?? 0, 1),
                'avg_temp' => round($weekData->avg('room_temperature') ?? 0, 1),
            ];
            
            $weekStart->addWeek();
            $weekNum++;
        }
        
        return [
            'month' => $startDate->format('F Y'),
            'total_records' => $data->count(),
            'avg_people' => round($data->avg('people_count') ?? 0, 1),
            'max_people' => $data->max('people_count') ?? 0,
            'avg_temperature' => round($data->avg('room_temperature') ?? 0, 1),
            'weekly_data' => $weeklyData,
        ];
    }
    
    /**
     * Calculate Efficiency Summary
     */
    private function calculateEfficiencySummary($data, $startDate, $endDate)
    {
        $acOnData = $data->where('ac_status', '!=', 'OFF');
        $lampOnData = $data->where('lamp_status', 'ON');
        
        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_records' => $data->count(),
            'ac_usage_percentage' => $data->count() > 0 ? round(($acOnData->count() / $data->count()) * 100, 1) : 0,
            'lamp_usage_percentage' => $data->count() > 0 ? round(($lampOnData->count() / $data->count()) * 100, 1) : 0,
            'avg_people_when_ac_on' => round($acOnData->avg('people_count') ?? 0, 1),
            'avg_people_when_lamp_on' => round($lampOnData->avg('people_count') ?? 0, 1),
            'avg_temperature' => round($data->avg('room_temperature') ?? 0, 1),
            'avg_humidity' => round($data->avg('humidity') ?? 0, 1),
        ];
    }
    
    /**
     * Generate Custom PDF Report
     */
    public function customReport(Request $request)
    {
        try {
            $dateFrom = $request->input('date_from', now()->subDays(7)->format('Y-m-d'));
            $dateTo = $request->input('date_to', now()->format('Y-m-d'));
            $deviceType = $request->input('device_type', 'all');
            $format = $request->input('format', 'pdf');
            
            $startDate = Carbon::parse($dateFrom)->startOfDay();
            $endDate = Carbon::parse($dateTo)->endOfDay();
            
            // Query data
            $query = SensorData::whereBetween('created_at', [$startDate, $endDate]);
            
            // Filter by device type if specified
            if ($deviceType && $deviceType !== 'all') {
                $query->where('device_id', $deviceType);
            }
            
            $data = $query->orderBy('created_at', 'desc')->get();
            
            // Calculate summary
            $summary = [
                'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
                'total_records' => $data->count(),
                'avg_people' => round($data->avg('people_count') ?? 0, 1),
                'max_people' => $data->max('people_count') ?? 0,
                'avg_temperature' => round($data->avg('room_temperature') ?? 0, 1),
                'avg_humidity' => round($data->avg('humidity') ?? 0, 1),
                'avg_light' => round($data->avg('light_level') ?? 0, 0),
                'ac_on_count' => $data->where('ac_status', '!=', 'OFF')->count(),
                'lamp_on_count' => $data->where('lamp_status', 'ON')->count(),
                'device_type' => $deviceType === 'all' ? 'Semua Perangkat' : $deviceType,
            ];
            
            if ($format === 'pdf') {
                $pdf = PDF::loadView('reports.pdf.custom', [
                    'data' => $data,
                    'summary' => $summary,
                    'startDate' => $startDate,
                    'endDate' => $endDate
                ]);
                
                $filename = 'laporan_kustom_' . $startDate->format('Y_m_d') . '_' . $endDate->format('Y_m_d') . '.pdf';
                return $pdf->stream($filename);
            } else {
                // Return JSON for other formats
                return response()->json([
                    'data' => $data,
                    'summary' => $summary
                ]);
            }
        } catch (\Exception $e) {
            \Log::error('Custom Report Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate report: ' . $e->getMessage()], 500);
        }
    }
}
