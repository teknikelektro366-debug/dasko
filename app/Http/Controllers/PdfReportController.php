<?php

namespace App\Http\Controllers;

use App\Models\SensorData;
use Illuminate\Http\Request;
use Barryvdh\DomPDF\Facade\Pdf;
use Carbon\Carbon;
use Illuminate\Support\Collection;
use Illuminate\Support\Facades\Log;

class PdfReportController extends Controller
{
    /**
     * Generate Daily PDF Report
     * Route: /api/reports/pdf/daily
     */
    public function dailyReport(Request $request)
    {
        try {
            $dateInput = $request->input('date', now()->format('Y-m-d'));
            $date = Carbon::parse($dateInput);
            
            // Menggunakan whereDate untuk presisi data harian
            $data = SensorData::whereDate('created_at', $date)
                               ->orderBy('created_at', 'desc')
                               ->get();
            
            $summary = $this->calculateDailySummary($data, $date);
            
            $pdf = Pdf::loadView('reports.pdf.daily', [
                'data' => $data,
                'summary' => $summary,
                'date' => $date
            ]);
            
            $filename = 'laporan_harian_' . $date->format('Y_m_d') . '.pdf';
            return $pdf->download($filename);
            
        } catch (\Exception $e) {
            Log::error('PDF Daily Error: ' . $e->getMessage());
            return response()->json(['error' => 'Gagal generate PDF Harian: ' . $e->getMessage()], 500);
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
            
            $pdf = Pdf::loadView('reports.pdf.weekly', [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ]);
            
            $filename = 'laporan_mingguan_' . $startDate->format('Y_m_d') . '.pdf';
            return $pdf->download($filename);
        } catch (\Exception $e) {
            Log::error('PDF Weekly Error: ' . $e->getMessage());
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
            
            $pdf = Pdf::loadView('reports.pdf.monthly', [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ]);
            
            $filename = 'laporan_bulanan_' . $startDate->format('Y_m') . '.pdf';
            return $pdf->download($filename);
        } catch (\Exception $e) {
            Log::error('PDF Monthly Error: ' . $e->getMessage());
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
            
            $pdf = Pdf::loadView('reports.pdf.efficiency', [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ]);
            
            $filename = 'laporan_efisiensi_' . $startDate->format('Y_m_d') . '.pdf';
            return $pdf->download($filename);
        } catch (\Exception $e) {
            Log::error('PDF Efficiency Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate PDF: ' . $e->getMessage()], 500);
        }
    }
    
    /**
     * Generate Custom PDF Report
     * Route: /api/reports/custom
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

            if ($endDate->lt($startDate)) {
                return response()->json(['error' => 'Parameter date_to tidak boleh lebih kecil dari date_from'], 422);
            }

            if (!in_array($format, ['pdf', 'json'], true)) {
                return response()->json(['error' => 'Format harus pdf atau json'], 422);
            }
            
            $query = SensorData::whereBetween('created_at', [$startDate, $endDate]);
            
            if ($deviceType && $deviceType !== 'all') {
                $query->where('device_id', $deviceType);
            }
            
            $data = $query->orderBy('created_at', 'desc')->get();
            
            $summary = [
                'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
                'total_records' => $data->count(),
                'avg_people' => $this->calculateAveragePeople($data),
                'max_people' => (int) ($data->max('people_count') ?? 0),
                'avg_temperature' => $this->calculateAverageFloat($data, 'room_temperature', 1),
                'avg_humidity' => $this->calculateAverageFloat($data, 'humidity', 1),
                'avg_light' => $this->calculateAverageFloat($data, 'light_level', 0),
                'ac_on_count' => $this->countActiveAc($data),
                'lamp_on_count' => $data->where('lamp_status', 'ON')->count(),
                'device_type' => $deviceType === 'all' ? 'Semua Perangkat' : $deviceType,
            ];
            
            if ($format === 'pdf') {
                $pdf = Pdf::loadView('reports.pdf.custom', [
                    'data' => $data,
                    'summary' => $summary,
                    'startDate' => $startDate,
                    'endDate' => $endDate
                ]);
                
                $filename = 'laporan_kustom_' . $startDate->format('Y_m_d') . '.pdf';
                return $pdf->download($filename);
            }

            return response()->json(['data' => $data, 'summary' => $summary]);

        } catch (\Exception $e) {
            Log::error('PDF Custom Error: ' . $e->getMessage());
            return response()->json(['error' => 'Failed to generate Custom report: ' . $e->getMessage()], 500);
        }
    }

    // --- Private Calculation Methods (Sudah Sinkron Nama Field) ---

    private function calculateDailySummary($data, $date)
    {
        return [
            'date' => $date->format('d F Y'),
            'total_records' => $data->count(),
            'avg_people' => $this->calculateAveragePeople($data),
            'max_people' => (int) ($data->max('people_count') ?? 0),
            'avg_temperature' => $this->calculateAverageFloat($data, 'room_temperature', 1),
            'avg_humidity' => $this->calculateAverageFloat($data, 'humidity', 1),
            'avg_light' => $this->calculateAverageFloat($data, 'light_level', 0),
            'ac_on_count' => $this->countActiveAc($data),
            'lamp_on_count' => $data->where('lamp_status', 'ON')->count(),
        ];
    }
    
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
                'avg_people' => $this->calculateAveragePeople($dayData),
                'avg_temp' => $this->calculateAverageFloat($dayData, 'room_temperature', 1),
            ];
        }
        
        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_records' => $data->count(),
            'avg_people' => $this->calculateAveragePeople($data),
            'max_people' => (int) ($data->max('people_count') ?? 0),
            'avg_temperature' => $this->calculateAverageFloat($data, 'room_temperature', 1),
            'daily_data' => $dailyData,
        ];
    }
    
    private function calculateMonthlySummary($data, $startDate, $endDate)
    {
        $weeklyData = [];
        $weekStart = $startDate->copy()->startOfWeek();
        $weekNum = 1;
        
        while ($weekStart->lte($endDate)) {
            $weekEnd = $weekStart->copy()->endOfWeek();
            if ($weekEnd->gt($endDate)) $weekEnd = $endDate->copy();
            
            $weekData = $data->filter(function($item) use ($weekStart, $weekEnd) {
                return $item->created_at->between($weekStart, $weekEnd);
            });
            
            $weeklyData[] = [
                'week' => 'Minggu ' . $weekNum,
                'period' => $weekStart->format('d M') . ' - ' . $weekEnd->format('d M'),
                'records' => $weekData->count(),
                'avg_people' => $this->calculateAveragePeople($weekData),
                'avg_temp' => $this->calculateAverageFloat($weekData, 'room_temperature', 1),
            ];
            
            $weekStart->addWeek();
            $weekNum++;
        }
        
        return [
            'month' => $startDate->format('F Y'),
            'total_records' => $data->count(),
            'avg_people' => $this->calculateAveragePeople($data),
            'max_people' => (int) ($data->max('people_count') ?? 0),
            'avg_temperature' => $this->calculateAverageFloat($data, 'room_temperature', 1),
            'weekly_data' => $weeklyData,
        ];
    }
    
    private function calculateEfficiencySummary($data, $startDate, $endDate)
    {
        $acOnData = $data->filter(function ($item) {
            return $this->isStatusOn($item->ac_status ?? null);
        });
        $lampOnData = $data->filter(function ($item) {
            return $this->isStatusOn($item->lamp_status ?? null);
        });
        $acValidStatusCount = $this->countValidStatus($data, 'ac_status');
        $lampValidStatusCount = $this->countValidStatus($data, 'lamp_status');
        
        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_records' => $data->count(),
            'ac_usage_percentage' => $acValidStatusCount > 0 ? round(($acOnData->count() / $acValidStatusCount) * 100, 1) : 0,
            'lamp_usage_percentage' => $lampValidStatusCount > 0 ? round(($lampOnData->count() / $lampValidStatusCount) * 100, 1) : 0,
            'avg_people_when_ac_on' => $this->calculateAveragePeople($acOnData),
            'avg_people_when_lamp_on' => $this->calculateAveragePeople($lampOnData),
            'avg_temperature' => $this->calculateAverageFloat($data, 'room_temperature', 1),
            'avg_humidity' => $this->calculateAverageFloat($data, 'humidity', 1),
        ];
    }

    private function calculateAveragePeople(Collection $data): int
    {
        $avgPeople = $data
            ->pluck('people_count')
            ->filter(function ($value) {
                return is_numeric($value);
            })
            ->avg();

        return $avgPeople === null ? 0 : (int) round($avgPeople);
    }

    private function calculateAverageFloat(Collection $data, string $field, int $precision = 1): float
    {
        $average = $data
            ->pluck($field)
            ->filter(function ($value) {
                return is_numeric($value);
            })
            ->avg();

        if ($average === null) {
            return 0.0;
        }

        return round((float) $average, $precision);
    }

    private function countActiveAc(Collection $data): int
    {
        return $data->filter(function ($item) {
            return $this->isStatusOn($item->ac_status ?? null);
        })->count();
    }

    private function countValidStatus(Collection $data, string $field): int
    {
        return $data->pluck($field)
            ->filter(function ($value) {
                return is_string($value) && trim($value) !== '';
            })
            ->count();
    }

    private function isStatusOn($status): bool
    {
        if (!is_string($status)) {
            return false;
        }

        $status = strtoupper(trim($status));

        return $status === 'ON';
    }
}
