<?php

namespace App\Http\Controllers;

use App\Models\SensorData;
use Dompdf\Dompdf;
use Illuminate\Http\Request;
use Barryvdh\DomPDF\Facade\Pdf;
use Carbon\Carbon;
use Illuminate\Support\Collection;
use Illuminate\Support\Facades\Log;

class PdfReportController extends Controller
{
    private const PDF_MAX_DETAIL_ROWS = 300;

    /**
     * Generate Daily PDF Report
     * Route: /api/reports/pdf/daily
     */
    public function dailyReport(Request $request)
    {
        try {
            $dateInput = (string) $request->input('date', now('Asia/Jakarta')->format('Y-m-d'));
            $date = Carbon::createFromFormat('Y-m-d', $dateInput, 'Asia/Jakarta');
            if ($date === false) {
                return response()->json(['error' => 'Format date tidak valid, gunakan Y-m-d'], 422);
            }

            $startDate = $date->copy()->startOfDay();
            $endDate = $date->copy()->endOfDay();

            $baseQuery = SensorData::whereBetween('created_at', [$startDate, $endDate]);

            $totalRecords = (clone $baseQuery)->count();
            
            $aggregate = (clone $baseQuery)->selectRaw("
                COUNT(*) as total_records,
                AVG(people_count) as avg_people,
                MAX(people_count) as max_people,
                AVG(room_temperature) as avg_temperature,
                AVG(humidity) as avg_humidity,
                AVG(light_level) as avg_light,
                SUM(CASE WHEN UPPER(TRIM(COALESCE(ac_status, ''))) = 'ON' THEN 1 ELSE 0 END) as ac_on_count,
                SUM(CASE WHEN UPPER(TRIM(COALESCE(lamp_status, ''))) = 'ON' THEN 1 ELSE 0 END) as lamp_on_count
            ")->first();

            $summary = [
                'date' => $date->format('d F Y'),
                'total_records' => (int) ($aggregate->total_records ?? $totalRecords),
                'avg_people' => (int) round((float) ($aggregate->avg_people ?? 0)),
                'max_people' => (int) ($aggregate->max_people ?? 0),
                'avg_temperature' => round((float) ($aggregate->avg_temperature ?? 0), 1),
                'avg_humidity' => round((float) ($aggregate->avg_humidity ?? 0), 1),
                'avg_light' => round((float) ($aggregate->avg_light ?? 0), 0),
                'ac_on_count' => (int) ($aggregate->ac_on_count ?? 0),
                'lamp_on_count' => (int) ($aggregate->lamp_on_count ?? 0),
            ];

            $totalParts = max(1, (int) ceil($totalRecords / self::PDF_MAX_DETAIL_ROWS));
            $requestedPart = max(1, (int) $request->input('part', 1));

            if ($request->boolean('meta')) {
                return response()->json([
                    'success' => true,
                    'total_records' => $totalRecords,
                    'per_file' => self::PDF_MAX_DETAIL_ROWS,
                    'total_parts' => $totalParts,
                    'date' => $date->format('Y-m-d'),
                ]);
            }

            if ($requestedPart > $totalParts) {
                return response()->json([
                    'error' => 'Parameter part melebihi total bagian laporan',
                    'total_parts' => $totalParts,
                ], 422);
            }

            $offset = ($requestedPart - 1) * self::PDF_MAX_DETAIL_ROWS;
            $data = (clone $baseQuery)
                ->select([
                    'id',
                    'created_at',
                    'people_count',
                    'room_temperature',
                    'humidity',
                    'light_level',
                    'ac_status',
                    'lamp_status',
                ])
                ->orderBy('created_at', 'desc')
                ->offset($offset)
                ->limit(self::PDF_MAX_DETAIL_ROWS)
                ->get();

            $pdfData = [
                'data' => $data,
                'summary' => $summary,
                'date' => $date,
                'is_truncated' => false,
                'displayed_records' => $data->count(),
                'part_number' => $requestedPart,
                'total_parts' => $totalParts,
            ];

            $baseFilename = 'laporan_harian_' . $date->format('Y_m_d');
            $filename = $totalParts > 1
                ? $baseFilename . '_part_' . $requestedPart . '_of_' . $totalParts . '.pdf'
                : $baseFilename . '.pdf';

            return $this->downloadPdfWithFallback('reports.pdf.daily', $pdfData, $filename);
            
        } catch (\Throwable $e) {
            Log::error('PDF Daily Error: ' . $e->getMessage(), [
                'trace' => $e->getTraceAsString()
            ]);
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
            
            $pdfData = [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ];
            
            $filename = 'laporan_mingguan_' . $startDate->format('Y_m_d') . '.pdf';
            return $this->downloadPdfWithFallback('reports.pdf.weekly', $pdfData, $filename);
        } catch (\Throwable $e) {
            Log::error('PDF Weekly Error: ' . $e->getMessage(), [
                'trace' => $e->getTraceAsString()
            ]);
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
            
            $pdfData = [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ];
            
            $filename = 'laporan_bulanan_' . $startDate->format('Y_m') . '.pdf';
            return $this->downloadPdfWithFallback('reports.pdf.monthly', $pdfData, $filename);
        } catch (\Throwable $e) {
            Log::error('PDF Monthly Error: ' . $e->getMessage(), [
                'trace' => $e->getTraceAsString()
            ]);
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
            
            $pdfData = [
                'data' => $data,
                'summary' => $summary,
                'startDate' => $startDate,
                'endDate' => $endDate
            ];
            
            $filename = 'laporan_efisiensi_' . $startDate->format('Y_m_d') . '.pdf';
            return $this->downloadPdfWithFallback('reports.pdf.efficiency', $pdfData, $filename);
        } catch (\Throwable $e) {
            Log::error('PDF Efficiency Error: ' . $e->getMessage(), [
                'trace' => $e->getTraceAsString()
            ]);
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
            $dateFrom = (string) $request->input('date_from', now('Asia/Jakarta')->subDays(7)->format('Y-m-d'));
            $dateTo = (string) $request->input('date_to', now('Asia/Jakarta')->format('Y-m-d'));
            $deviceType = $request->input('device_type', 'all');
            $format = $request->input('format', 'pdf');
            
            $startDate = Carbon::createFromFormat('Y-m-d', $dateFrom, 'Asia/Jakarta');
            $endDate = Carbon::createFromFormat('Y-m-d', $dateTo, 'Asia/Jakarta');

            if ($startDate === false || $endDate === false) {
                return response()->json(['error' => 'Format date_from/date_to tidak valid, gunakan Y-m-d'], 422);
            }

            $startDate = $startDate->startOfDay();
            $endDate = $endDate->endOfDay();

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
            
            $totalRecords = (clone $query)->count();
            $data = (clone $query)
                ->select([
                    'id',
                    'device_id',
                    'created_at',
                    'people_count',
                    'room_temperature',
                    'humidity',
                    'light_level',
                    'ac_status',
                    'lamp_status',
                ])
                ->orderBy('created_at', 'desc')
                ->when($format === 'pdf', function ($q) {
                    $q->limit(self::PDF_MAX_DETAIL_ROWS);
                })
                ->get();
            
            $aggregate = (clone $query)->selectRaw("
                COUNT(*) as total_records,
                AVG(people_count) as avg_people,
                MAX(people_count) as max_people,
                AVG(room_temperature) as avg_temperature,
                AVG(humidity) as avg_humidity,
                AVG(light_level) as avg_light,
                SUM(CASE WHEN UPPER(TRIM(COALESCE(ac_status, ''))) = 'ON' THEN 1 ELSE 0 END) as ac_on_count,
                SUM(CASE WHEN UPPER(TRIM(COALESCE(lamp_status, ''))) = 'ON' THEN 1 ELSE 0 END) as lamp_on_count
            ")->first();

            $summary = [
                'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
                'total_records' => (int) ($aggregate->total_records ?? $totalRecords),
                'avg_people' => (int) round((float) ($aggregate->avg_people ?? 0)),
                'max_people' => (int) ($aggregate->max_people ?? 0),
                'avg_temperature' => round((float) ($aggregate->avg_temperature ?? 0), 1),
                'avg_humidity' => round((float) ($aggregate->avg_humidity ?? 0), 1),
                'avg_light' => round((float) ($aggregate->avg_light ?? 0), 0),
                'ac_on_count' => (int) ($aggregate->ac_on_count ?? 0),
                'lamp_on_count' => (int) ($aggregate->lamp_on_count ?? 0),
                'device_type' => $deviceType === 'all' ? 'Semua Perangkat' : $deviceType,
            ];
            
            if ($format === 'pdf') {
                $totalParts = max(1, (int) ceil($totalRecords / self::PDF_MAX_DETAIL_ROWS));
                $requestedPart = max(1, (int) $request->input('part', 1));

                if ($request->boolean('meta')) {
                    return response()->json([
                        'success' => true,
                        'total_records' => $totalRecords,
                        'per_file' => self::PDF_MAX_DETAIL_ROWS,
                        'total_parts' => $totalParts,
                        'date_from' => $startDate->format('Y-m-d'),
                        'date_to' => $endDate->format('Y-m-d'),
                    ]);
                }

                if ($requestedPart > $totalParts) {
                    return response()->json([
                        'error' => 'Parameter part melebihi total bagian laporan',
                        'total_parts' => $totalParts,
                    ], 422);
                }

                $offset = ($requestedPart - 1) * self::PDF_MAX_DETAIL_ROWS;
                $data = (clone $query)
                    ->select([
                        'id',
                        'device_id',
                        'created_at',
                        'people_count',
                        'room_temperature',
                        'humidity',
                        'light_level',
                        'ac_status',
                        'lamp_status',
                    ])
                    ->orderBy('created_at', 'desc')
                    ->offset($offset)
                    ->limit(self::PDF_MAX_DETAIL_ROWS)
                    ->get();

                $pdfData = [
                    'data' => $data,
                    'summary' => $summary,
                    'startDate' => $startDate,
                    'endDate' => $endDate,
                    'is_truncated' => false,
                    'displayed_records' => $data->count(),
                    'part_number' => $requestedPart,
                    'total_parts' => $totalParts,
                ];
                
                $baseFilename = 'laporan_kustom_' . $startDate->format('Y_m_d');
                $filename = $totalParts > 1
                    ? $baseFilename . '_part_' . $requestedPart . '_of_' . $totalParts . '.pdf'
                    : $baseFilename . '.pdf';

                return $this->downloadPdfWithFallback('reports.pdf.custom', $pdfData, $filename);
            }

            return response()->json(['data' => $data, 'summary' => $summary]);

        } catch (\Throwable $e) {
            Log::error('PDF Custom Error: ' . $e->getMessage(), [
                'trace' => $e->getTraceAsString()
            ]);
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

    private function downloadPdfWithFallback(string $view, array $data, string $filename)
    {
        $pdfBinary = $this->renderPdfBinary($view, $data);

        return response($pdfBinary, 200, [
            'Content-Type' => 'application/pdf',
            'Content-Disposition' => 'attachment; filename="' . $filename . '"',
        ]);
    }

    private function renderPdfBinary(string $view, array $data): string
    {
        try {
            $html = view($view, $data)->render();

            // Guard untuk shared hosting: hindari render HTML terlalu besar.
            if (strlen($html) > 2_500_000) {
                throw new \RuntimeException('Ukuran dokumen PDF terlalu besar untuk diproses server.');
            }

            if (!class_exists(Dompdf::class)) {
                return Pdf::loadHTML($html)->output();
            }

            $dompdf = new Dompdf([
                'isRemoteEnabled' => false,
                'isHtml5ParserEnabled' => true,
                'defaultFont' => 'Arial',
            ]);
            $dompdf->setPaper('A4', 'portrait');
            $dompdf->loadHtml($html, 'UTF-8');
            $dompdf->render();

            return $dompdf->output();
        } catch (\Throwable $e) {
            Log::warning('Native Dompdf gagal, coba facade: ' . $e->getMessage());
            return Pdf::loadView($view, $data)->output();
        }
    }

}
