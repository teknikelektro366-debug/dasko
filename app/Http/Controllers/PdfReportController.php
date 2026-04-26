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

    /**
     * Generate Analytics Chart PDF Report
     * Route: /api/reports/pdf/analytics-chart?type=people
     */
    public function analyticsChartReport(Request $request)
    {
        try {
            $type = (string) $request->input('type', 'people');
            $configs = $this->analyticsChartConfigs();

            if (!array_key_exists($type, $configs)) {
                return response()->json([
                    'error' => 'Tipe grafik tidak valid',
                    'allowed_types' => array_keys($configs),
                ], 422);
            }

            $config = $configs[$type];
            $now = Carbon::now('Asia/Jakarta');
            $dateInput = (string) $request->input('date', $now->format('Y-m-d'));
            $reportDate = Carbon::createFromFormat('Y-m-d', $dateInput, 'Asia/Jakarta');
            $isToday = $reportDate->isSameDay($now);
            $lastHour = $isToday ? (int) $now->format('G') : 23;
            $startDate = $reportDate->copy()->startOfDay();
            $endDate = $reportDate->copy()->setTime($lastHour, 59, 59);

            $data = SensorData::whereBetween('created_at', [$startDate, $endDate])
                ->orderBy('created_at')
                ->get();

            $grouped = $data->groupBy(function ($item) {
                return $item->created_at->format('H:00');
            });

            $rows = collect(range(0, $lastHour))->map(function ($hour) use ($grouped, $config) {
                $time = str_pad((string) $hour, 2, '0', STR_PAD_LEFT) . ':00';
                $group = $grouped->get($time);
                $value = null;
                $records = 0;

                if ($group && $group->count() > 0) {
                    $records = $group->count();
                    $average = $group->avg($config['field']);
                    $value = $average === null ? null : round((float) $average, $config['decimals']);
                }

                return [
                    'time' => $time,
                    'value' => $value,
                    'records' => $records,
                ];
            });

            $numericValues = $rows
                ->pluck('value')
                ->filter(function ($value) {
                    return is_numeric($value);
                })
                ->values();

            $summary = [
                'total_records' => $data->count(),
                'filled_hours' => $numericValues->count(),
                'empty_hours' => $rows->count() - $numericValues->count(),
                'average' => $numericValues->count() > 0 ? round((float) $numericValues->avg(), $config['decimals']) : null,
                'min' => $numericValues->count() > 0 ? round((float) $numericValues->min(), $config['decimals']) : null,
                'max' => $numericValues->count() > 0 ? round((float) $numericValues->max(), $config['decimals']) : null,
            ];

            $chartImage = $this->buildAnalyticsLineChartImage($rows, $config);

            $pdfData = [
                'config' => $config,
                'rows' => $rows,
                'summary' => $summary,
                'chart_image' => $chartImage,
                'date' => $reportDate,
                'period' => '00:00 - ' . str_pad((string) $lastHour, 2, '0', STR_PAD_LEFT) . ':00 WIB',
            ];

            $filename = 'laporan_' . $config['slug'] . '_' . $reportDate->format('Y_m_d') . '.pdf';

            return $this->downloadPdfWithFallback('reports.pdf.analytics-chart', $pdfData, $filename);
        } catch (\Throwable $e) {
            Log::error('PDF Analytics Chart Error: ' . $e->getMessage(), [
                'trace' => $e->getTraceAsString(),
            ]);

            return response()->json(['error' => 'Gagal generate PDF grafik: ' . $e->getMessage()], 500);
        }
    }

    // --- Private Calculation Methods (Sudah Sinkron Nama Field) ---

    private function analyticsChartConfigs(): array
    {
        return [
            'people' => [
                'title' => 'Grafik Jumlah Orang',
                'label' => 'Jumlah Orang',
                'field' => 'people_count',
                'unit' => ' orang',
                'color' => '#2563EB',
                'slug' => 'jumlah_orang_hari_ini',
                'decimals' => 0,
            ],
            'ac_temperature' => [
                'title' => 'Grafik Suhu AC',
                'label' => 'Suhu AC',
                'field' => 'set_temperature',
                'unit' => '°C',
                'color' => '#0891B2',
                'slug' => 'suhu_ac_hari_ini',
                'decimals' => 0,
            ],
            'humidity' => [
                'title' => 'Grafik Kelembapan',
                'label' => 'Kelembapan',
                'field' => 'humidity',
                'unit' => '%',
                'color' => '#16A34A',
                'slug' => 'kelembapan_hari_ini',
                'decimals' => 1,
            ],
            'light' => [
                'title' => 'Grafik Cahaya',
                'label' => 'Cahaya',
                'field' => 'light_level',
                'unit' => ' lux',
                'color' => '#F59E0B',
                'slug' => 'cahaya_hari_ini',
                'decimals' => 0,
            ],
            'room_temperature' => [
                'title' => 'Grafik Suhu Ruangan',
                'label' => 'Suhu Ruangan',
                'field' => 'room_temperature',
                'unit' => '°C',
                'color' => '#DC2626',
                'slug' => 'suhu_ruangan_hari_ini',
                'decimals' => 1,
            ],
        ];
    }

    private function buildAnalyticsLineChartImage(Collection $rows, array $config): ?string
    {
        if (!extension_loaded('gd')) {
            return null;
        }

        $values = $rows->pluck('value')->filter(function ($value) {
            return is_numeric($value);
        });

        if ($values->count() === 0) {
            return null;
        }

        $width = 1100;
        $height = 430;
        $left = 92;
        $right = 34;
        $top = 34;
        $bottom = 78;
        $plotWidth = $width - $left - $right;
        $plotHeight = $height - $top - $bottom;

        $min = (float) $values->min();
        $max = (float) $values->max();

        if (in_array($config['field'], ['people_count', 'light_level', 'humidity'], true)) {
            $min = 0.0;
        }

        if ($min === $max) {
            $min = max(0, $min - 1);
            $max = $max + 1;
        }

        $range = max(1, $max - $min);
        $count = max(1, $rows->count() - 1);

        $image = imagecreatetruecolor($width, $height);
        imageantialias($image, true);

        $white = imagecolorallocate($image, 255, 255, 255);
        $axis = imagecolorallocate($image, 156, 163, 175);
        $grid = imagecolorallocate($image, 229, 231, 235);
        $text = imagecolorallocate($image, 75, 85, 99);
        $muted = imagecolorallocate($image, 156, 163, 175);
        $line = $this->allocateHexColor($image, $config['color']);

        imagefilledrectangle($image, 0, 0, $width, $height, $white);

        for ($i = 0; $i <= 4; $i++) {
            $tickValue = $min + (($range / 4) * $i);
            $y = (int) round($top + (($max - $tickValue) / $range * $plotHeight));

            imageline($image, $left, $y, $width - $right, $y, $grid);
            imagestring($image, 2, 8, $y - 7, $this->formatAnalyticsChartNumber($tickValue, $config) . $config['unit'], $text);
        }

        imageline($image, $left, $top, $left, $height - $bottom, $axis);
        imageline($image, $left, $height - $bottom, $width - $right, $height - $bottom, $axis);

        $points = [];
        foreach ($rows->values() as $index => $row) {
            $x = (int) round($left + (($plotWidth / $count) * $index));
            $value = $row['value'];

            if (!is_numeric($value)) {
                $points[] = ['x' => $x, 'y' => null, 'value' => null, 'time' => $row['time']];
                continue;
            }

            $y = (int) round($top + (($max - (float) $value) / $range * $plotHeight));
            $points[] = ['x' => $x, 'y' => $y, 'value' => (float) $value, 'time' => $row['time']];
        }

        $previous = null;
        foreach ($points as $point) {
            if ($point['y'] === null) {
                continue;
            }

            if ($previous !== null) {
                $this->drawThickLine($image, $previous['x'], $previous['y'], $point['x'], $point['y'], $line, 4);
            }

            $previous = $point;
        }

        foreach ($points as $point) {
            if ($point['y'] === null) {
                continue;
            }

            imagefilledellipse($image, $point['x'], $point['y'], 11, 11, $white);
            imageellipse($image, $point['x'], $point['y'], 11, 11, $line);
            imagefilledellipse($image, $point['x'], $point['y'], 6, 6, $line);
        }

        foreach ($points as $index => $point) {
            $label = $point['time'];
            $x = max($left - 10, min($width - $right - 28, $point['x'] - 14));
            imagestring($image, 2, $x, $height - 48, $label, $text);

            if ($point['value'] === null && ($index % 2 === 0)) {
                imagestring($image, 1, $point['x'] - 5, $height - 31, '-', $muted);
            }
        }

        imagestring($image, 3, $left, 10, $config['label'] . ' per jam', $text);
        imagestring($image, 2, $width - 230, 12, 'Sumber: tabel sensor_data', $muted);

        ob_start();
        imagepng($image);
        $png = ob_get_clean();
        imagedestroy($image);

        if ($png === false) {
            return null;
        }

        return 'data:image/png;base64,' . base64_encode($png);
    }

    private function allocateHexColor($image, string $hex, int $mixWithWhitePercent = 0): int
    {
        $hex = ltrim($hex, '#');
        $red = hexdec(substr($hex, 0, 2));
        $green = hexdec(substr($hex, 2, 2));
        $blue = hexdec(substr($hex, 4, 2));

        if ($mixWithWhitePercent > 0) {
            $ratio = min(100, max(0, $mixWithWhitePercent)) / 100;
            $red = (int) round($red + ((255 - $red) * $ratio));
            $green = (int) round($green + ((255 - $green) * $ratio));
            $blue = (int) round($blue + ((255 - $blue) * $ratio));
        }

        return imagecolorallocate($image, $red, $green, $blue);
    }

    private function drawThickLine($image, int $x1, int $y1, int $x2, int $y2, int $color, int $thickness): void
    {
        $half = max(1, (int) floor($thickness / 2));

        for ($offset = -$half; $offset <= $half; $offset++) {
            imageline($image, $x1, $y1 + $offset, $x2, $y2 + $offset, $color);
            imageline($image, $x1 + $offset, $y1, $x2 + $offset, $y2, $color);
        }
    }

    private function formatAnalyticsChartNumber(float $value, array $config): string
    {
        return number_format($value, $config['decimals'], '.', '');
    }

    private function buildAnalyticsSvgPath(Collection $rows, array $config): array
    {
        $width = 700;
        $height = 260;
        $paddingLeft = 52;
        $paddingRight = 18;
        $paddingTop = 18;
        $paddingBottom = 42;
        $plotWidth = $width - $paddingLeft - $paddingRight;
        $plotHeight = $height - $paddingTop - $paddingBottom;

        $values = $rows->pluck('value')->filter(function ($value) {
            return is_numeric($value);
        });

        $min = $values->count() > 0 ? (float) $values->min() : 0.0;
        $max = $values->count() > 0 ? (float) $values->max() : 1.0;

        if ($min === $max) {
            $min = max(0, $min - 1);
            $max = $max + 1;
        }

        if ($min > 0 && in_array($config['field'], ['people_count', 'light_level', 'humidity'], true)) {
            $min = 0;
        }

        $range = max(1, $max - $min);
        $count = max(1, $rows->count() - 1);
        $path = '';
        $started = false;
        $points = [];

        foreach ($rows->values() as $index => $row) {
            $x = $paddingLeft + (($plotWidth / $count) * $index);
            $value = $row['value'];

            if (!is_numeric($value)) {
                $started = false;
                $points[] = ['x' => $x, 'y' => null, 'value' => null, 'time' => $row['time']];
                continue;
            }

            $y = $paddingTop + (($max - (float) $value) / $range * $plotHeight);
            $path .= ($started ? ' L ' : ' M ') . round($x, 2) . ' ' . round($y, 2);
            $started = true;
            $points[] = ['x' => $x, 'y' => $y, 'value' => $value, 'time' => $row['time']];
        }

        $ticks = [];
        for ($i = 0; $i <= 4; $i++) {
            $value = $min + (($range / 4) * $i);
            $ticks[] = [
                'value' => round($value, $config['decimals']),
                'y' => $paddingTop + (($max - $value) / $range * $plotHeight),
            ];
        }

        return [
            'width' => $width,
            'height' => $height,
            'padding_left' => $paddingLeft,
            'padding_bottom' => $paddingBottom,
            'plot_width' => $plotWidth,
            'plot_height' => $plotHeight,
            'path' => trim($path),
            'points' => $points,
            'ticks' => array_reverse($ticks),
            'min' => $min,
            'max' => $max,
        ];
    }

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
