<?php

namespace App\Http\Controllers;

use App\Models\Device;
use App\Models\SensorReading;
use App\Models\EnergyLog;
use App\Models\ACAutomationRule;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;
use Carbon\Carbon;

class ReportController extends Controller
{
    public function index()
    {
        return view('reports.index');
    }

    public function energyReport(Request $request)
    {
        $request->validate([
            'date_from' => 'required|date',
            'date_to' => 'required|date|after_or_equal:date_from',
            'device_type' => 'nullable|string',
            'format' => 'required|in:pdf,excel,csv'
        ]);

        $dateFrom = Carbon::parse($request->date_from)->startOfDay();
        $dateTo = Carbon::parse($request->date_to)->endOfDay();
        
        // Get energy data
        $query = EnergyLog::with('device')
            ->whereBetween('start_time', [$dateFrom, $dateTo]);

        if ($request->device_type) {
            $query->whereHas('device', function($q) use ($request) {
                $q->where('type', $request->device_type);
            });
        }

        $energyLogs = $query->orderBy('start_time', 'desc')->get();

        // Calculate summary statistics
        $summary = $this->calculateEnergySummary($energyLogs, $dateFrom, $dateTo);

        // Generate report based on format
        switch ($request->format) {
            case 'pdf':
                return $this->generatePDFReport($energyLogs, $summary, $dateFrom, $dateTo, $request->device_type);
            case 'excel':
                return $this->generateExcelReport($energyLogs, $summary, $dateFrom, $dateTo, $request->device_type);
            case 'csv':
                return $this->generateCSVReport($energyLogs, $summary, $dateFrom, $dateTo, $request->device_type);
        }
    }

    public function dailyReport(Request $request)
    {
        $date = $request->date ? Carbon::parse($request->date) : Carbon::today();
        
        $energyLogs = EnergyLog::with('device')
            ->whereDate('start_time', $date)
            ->orderBy('start_time', 'desc')
            ->get();

        $summary = $this->calculateDailySummary($energyLogs, $date);

        if ($request->format === 'download') {
            return $this->generateDailyPDFReport($energyLogs, $summary, $date);
        }

        return view('reports.daily', compact('energyLogs', 'summary', 'date'));
    }

    public function weeklyReport(Request $request)
    {
        $startDate = $request->week_start ? Carbon::parse($request->week_start) : Carbon::now()->startOfWeek();
        $endDate = $startDate->copy()->endOfWeek();
        
        $energyLogs = EnergyLog::with('device')
            ->whereBetween('start_time', [$startDate, $endDate])
            ->get();

        $summary = $this->calculateWeeklySummary($energyLogs, $startDate, $endDate);

        if ($request->format === 'download') {
            return $this->generateWeeklyPDFReport($energyLogs, $summary, $startDate, $endDate);
        }

        return view('reports.weekly', compact('energyLogs', 'summary', 'startDate', 'endDate'));
    }

    public function monthlyReport(Request $request)
    {
        $month = $request->month ? Carbon::parse($request->month) : Carbon::now()->startOfMonth();
        $startDate = $month->copy()->startOfMonth();
        $endDate = $month->copy()->endOfMonth();
        
        $energyLogs = EnergyLog::with('device')
            ->whereBetween('start_time', [$startDate, $endDate])
            ->get();

        $summary = $this->calculateMonthlySummary($energyLogs, $startDate, $endDate);

        if ($request->format === 'download') {
            return $this->generateMonthlyPDFReport($energyLogs, $summary, $startDate, $endDate);
        }

        return view('reports.monthly', compact('energyLogs', 'summary', 'startDate', 'endDate'));
    }

    public function deviceReport(Request $request)
    {
        $request->validate([
            'device_id' => 'required|exists:devices,id',
            'date_from' => 'required|date',
            'date_to' => 'required|date|after_or_equal:date_from'
        ]);

        $device = Device::findOrFail($request->device_id);
        $dateFrom = Carbon::parse($request->date_from)->startOfDay();
        $dateTo = Carbon::parse($request->date_to)->endOfDay();

        $energyLogs = EnergyLog::where('device_id', $device->id)
            ->whereBetween('start_time', [$dateFrom, $dateTo])
            ->orderBy('start_time', 'desc')
            ->get();

        $summary = $this->calculateDeviceSummary($energyLogs, $device, $dateFrom, $dateTo);

        if ($request->format === 'download') {
            return $this->generateDevicePDFReport($energyLogs, $summary, $device, $dateFrom, $dateTo);
        }

        return view('reports.device', compact('energyLogs', 'summary', 'device', 'dateFrom', 'dateTo'));
    }

    public function efficiencyReport(Request $request)
    {
        $request->validate([
            'date_from' => 'required|date',
            'date_to' => 'required|date|after_or_equal:date_from'
        ]);

        $dateFrom = Carbon::parse($request->date_from)->startOfDay();
        $dateTo = Carbon::parse($request->date_to)->endOfDay();

        // Get efficiency data
        $efficiencyData = EnergyLog::with('device')
            ->whereBetween('start_time', [$dateFrom, $dateTo])
            ->whereNotNull('people_count')
            ->where('people_count', '>', 0)
            ->get()
            ->groupBy('device.type')
            ->map(function ($logs) {
                $totalEnergy = $logs->sum('energy_consumed');
                $totalPeople = $logs->sum('people_count');
                $avgEfficiency = $totalPeople > 0 ? $totalEnergy / $totalPeople : 0;
                
                return [
                    'device_name' => $logs->first()->device->name,
                    'total_energy' => $totalEnergy,
                    'total_people_hours' => $totalPeople,
                    'efficiency_wh_per_person' => $avgEfficiency,
                    'efficiency_rating' => $this->getEfficiencyRating($avgEfficiency),
                    'logs_count' => $logs->count()
                ];
            });

        if ($request->format === 'download') {
            return $this->generateEfficiencyPDFReport($efficiencyData, $dateFrom, $dateTo);
        }

        return view('reports.efficiency', compact('efficiencyData', 'dateFrom', 'dateTo'));
    }

    private function calculateEnergySummary($energyLogs, $dateFrom, $dateTo)
    {
        $totalEnergy = $energyLogs->sum('energy_consumed');
        $totalCost = $energyLogs->sum('cost_estimate');
        $totalDuration = $energyLogs->sum('duration_minutes');
        $avgEnergyPerLog = $energyLogs->count() > 0 ? $totalEnergy / $energyLogs->count() : 0;

        // Group by device type
        $byDeviceType = $energyLogs->groupBy('device.type')->map(function ($logs) {
            return [
                'device_name' => $logs->first()->device->name,
                'total_energy' => $logs->sum('energy_consumed'),
                'total_cost' => $logs->sum('cost_estimate'),
                'logs_count' => $logs->count(),
                'avg_energy' => $logs->avg('energy_consumed')
            ];
        });

        // Daily breakdown
        $dailyBreakdown = $energyLogs->groupBy(function ($log) {
            return $log->start_time->format('Y-m-d');
        })->map(function ($logs) {
            return [
                'total_energy' => $logs->sum('energy_consumed'),
                'total_cost' => $logs->sum('cost_estimate'),
                'logs_count' => $logs->count()
            ];
        });

        return [
            'period' => $dateFrom->format('d M Y') . ' - ' . $dateTo->format('d M Y'),
            'total_energy_kwh' => round($totalEnergy / 1000, 3),
            'total_cost_idr' => round($totalCost, 0),
            'total_duration_hours' => round($totalDuration / 60, 1),
            'avg_energy_per_log' => round($avgEnergyPerLog, 2),
            'logs_count' => $energyLogs->count(),
            'by_device_type' => $byDeviceType,
            'daily_breakdown' => $dailyBreakdown,
            'energy_saving_logs' => $energyLogs->where('energy_saving_mode', true)->count(),
            'working_hours_logs' => $energyLogs->where('working_hours', true)->count()
        ];
    }

    private function calculateDailySummary($energyLogs, $date)
    {
        $totalEnergy = $energyLogs->sum('energy_consumed');
        $totalCost = $energyLogs->sum('cost_estimate');

        // Hourly breakdown
        $hourlyBreakdown = $energyLogs->groupBy(function ($log) {
            return $log->start_time->format('H');
        })->map(function ($logs, $hour) {
            return [
                'hour' => $hour . ':00',
                'energy' => $logs->sum('energy_consumed'),
                'cost' => $logs->sum('cost_estimate'),
                'count' => $logs->count()
            ];
        })->sortKeys();

        return [
            'date' => $date->format('d F Y'),
            'total_energy_kwh' => round($totalEnergy / 1000, 3),
            'total_cost_idr' => round($totalCost, 0),
            'logs_count' => $energyLogs->count(),
            'hourly_breakdown' => $hourlyBreakdown,
            'peak_hour' => $hourlyBreakdown->sortByDesc('energy')->keys()->first() ?? 'N/A'
        ];
    }

    private function calculateWeeklySummary($energyLogs, $startDate, $endDate)
    {
        $totalEnergy = $energyLogs->sum('energy_consumed');
        $totalCost = $energyLogs->sum('cost_estimate');

        // Daily breakdown for the week
        $dailyBreakdown = collect();
        for ($date = $startDate->copy(); $date->lte($endDate); $date->addDay()) {
            $dayLogs = $energyLogs->filter(function ($log) use ($date) {
                return $log->start_time->format('Y-m-d') === $date->format('Y-m-d');
            });

            $dailyBreakdown->put($date->format('Y-m-d'), [
                'day_name' => $date->format('l'),
                'date' => $date->format('d M'),
                'energy' => $dayLogs->sum('energy_consumed'),
                'cost' => $dayLogs->sum('cost_estimate'),
                'count' => $dayLogs->count()
            ]);
        }

        return [
            'period' => $startDate->format('d M Y') . ' - ' . $endDate->format('d M Y'),
            'total_energy_kwh' => round($totalEnergy / 1000, 3),
            'total_cost_idr' => round($totalCost, 0),
            'logs_count' => $energyLogs->count(),
            'daily_breakdown' => $dailyBreakdown,
            'avg_daily_energy' => round($totalEnergy / 7, 2),
            'peak_day' => $dailyBreakdown->sortByDesc('energy')->keys()->first() ?? 'N/A'
        ];
    }

    private function calculateMonthlySummary($energyLogs, $startDate, $endDate)
    {
        $totalEnergy = $energyLogs->sum('energy_consumed');
        $totalCost = $energyLogs->sum('cost_estimate');

        // Weekly breakdown
        $weeklyBreakdown = collect();
        $weekStart = $startDate->copy()->startOfWeek();
        $weekNumber = 1;

        while ($weekStart->lte($endDate)) {
            $weekEnd = $weekStart->copy()->endOfWeek();
            if ($weekEnd->gt($endDate)) {
                $weekEnd = $endDate->copy();
            }

            $weekLogs = $energyLogs->filter(function ($log) use ($weekStart, $weekEnd) {
                return $log->start_time->between($weekStart, $weekEnd);
            });

            $weeklyBreakdown->put("week_{$weekNumber}", [
                'period' => $weekStart->format('d M') . ' - ' . $weekEnd->format('d M'),
                'energy' => $weekLogs->sum('energy_consumed'),
                'cost' => $weekLogs->sum('cost_estimate'),
                'count' => $weekLogs->count()
            ]);

            $weekStart->addWeek();
            $weekNumber++;
        }

        return [
            'month' => $startDate->format('F Y'),
            'total_energy_kwh' => round($totalEnergy / 1000, 3),
            'total_cost_idr' => round($totalCost, 0),
            'logs_count' => $energyLogs->count(),
            'weekly_breakdown' => $weeklyBreakdown,
            'avg_daily_energy' => round($totalEnergy / $startDate->daysInMonth, 2)
        ];
    }

    private function calculateDeviceSummary($energyLogs, $device, $dateFrom, $dateTo)
    {
        $totalEnergy = $energyLogs->sum('energy_consumed');
        $totalCost = $energyLogs->sum('cost_estimate');
        $totalDuration = $energyLogs->sum('duration_minutes');

        return [
            'device_name' => $device->name,
            'device_type' => $device->type,
            'period' => $dateFrom->format('d M Y') . ' - ' . $dateTo->format('d M Y'),
            'total_energy_kwh' => round($totalEnergy / 1000, 3),
            'total_cost_idr' => round($totalCost, 0),
            'total_runtime_hours' => round($totalDuration / 60, 1),
            'logs_count' => $energyLogs->count(),
            'avg_energy_per_session' => $energyLogs->count() > 0 ? round($totalEnergy / $energyLogs->count(), 2) : 0,
            'power_rating' => $device->power_consumption . 'W',
            'efficiency_rating' => $this->calculateDeviceEfficiency($energyLogs)
        ];
    }

    private function getEfficiencyRating($energyPerPerson)
    {
        if ($energyPerPerson < 50) return 'Excellent';
        if ($energyPerPerson < 100) return 'Good';
        if ($energyPerPerson < 150) return 'Average';
        return 'Poor';
    }

    private function calculateDeviceEfficiency($energyLogs)
    {
        $logsWithPeople = $energyLogs->where('people_count', '>', 0);
        if ($logsWithPeople->isEmpty()) return 'N/A';

        $totalEnergy = $logsWithPeople->sum('energy_consumed');
        $totalPeople = $logsWithPeople->sum('people_count');
        $energyPerPerson = $totalPeople > 0 ? $totalEnergy / $totalPeople : 0;

        return $this->getEfficiencyRating($energyPerPerson);
    }

    private function generateCSVReport($energyLogs, $summary, $dateFrom, $dateTo, $deviceType = null)
    {
        $filename = 'energy_report_' . $dateFrom->format('Y_m_d') . '_to_' . $dateTo->format('Y_m_d') . '.csv';
        
        $headers = [
            'Content-Type' => 'text/csv',
            'Content-Disposition' => 'attachment; filename="' . $filename . '"'
        ];

        $callback = function() use ($energyLogs, $summary) {
            $file = fopen('php://output', 'w');
            
            // Write summary header
            fputcsv($file, ['ENERGY CONSUMPTION REPORT']);
            fputcsv($file, ['Period', $summary['period']]);
            fputcsv($file, ['Total Energy (kWh)', $summary['total_energy_kwh']]);
            fputcsv($file, ['Total Cost (IDR)', number_format($summary['total_cost_idr'])]);
            fputcsv($file, ['Total Logs', $summary['logs_count']]);
            fputcsv($file, []);
            
            // Write detailed data header
            fputcsv($file, [
                'Date', 'Time', 'Device Name', 'Device Type', 'Energy (Wh)', 
                'Duration (min)', 'People Count', 'Outdoor Temp', 'AC Target Temp',
                'Energy Saving', 'Working Hours', 'Cost (IDR)', 'Efficiency'
            ]);

            // Write data rows
            foreach ($energyLogs as $log) {
                fputcsv($file, [
                    $log->start_time->format('Y-m-d'),
                    $log->start_time->format('H:i:s'),
                    $log->device->name,
                    $log->device->type,
                    number_format($log->energy_consumed, 2),
                    $log->duration_minutes,
                    $log->people_count ?? 'N/A',
                    $log->outdoor_temperature ?? 'N/A',
                    $log->ac_target_temperature ?? 'N/A',
                    $log->energy_saving_mode ? 'Yes' : 'No',
                    $log->working_hours ? 'Yes' : 'No',
                    number_format($log->cost_estimate, 0),
                    $log->efficiency_rating
                ]);
            }

            fclose($file);
        };

        return response()->stream($callback, 200, $headers);
    }

    private function generateExcelReport($energyLogs, $summary, $dateFrom, $dateTo, $deviceType = null)
    {
        // For now, return CSV format (can be enhanced with PhpSpreadsheet later)
        return $this->generateCSVReport($energyLogs, $summary, $dateFrom, $dateTo, $deviceType);
    }

    private function generatePDFReport($energyLogs, $summary, $dateFrom, $dateTo, $deviceType = null)
    {
        $html = view('reports.pdf.energy', compact('energyLogs', 'summary', 'dateFrom', 'dateTo', 'deviceType'))->render();
        
        $filename = 'energy_report_' . $dateFrom->format('Y_m_d') . '_to_' . $dateTo->format('Y_m_d') . '.pdf';
        
        // Simple HTML to PDF conversion (can be enhanced with DomPDF or similar)
        return response($html)
            ->header('Content-Type', 'text/html')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateDailyPDFReport($energyLogs, $summary, $date)
    {
        $html = view('reports.pdf.daily', compact('energyLogs', 'summary', 'date'))->render();
        $filename = 'daily_report_' . $date->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateWeeklyPDFReport($energyLogs, $summary, $startDate, $endDate)
    {
        $html = view('reports.pdf.weekly', compact('energyLogs', 'summary', 'startDate', 'endDate'))->render();
        $filename = 'weekly_report_' . $startDate->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateMonthlyPDFReport($energyLogs, $summary, $startDate, $endDate)
    {
        $html = view('reports.pdf.monthly', compact('energyLogs', 'summary', 'startDate', 'endDate'))->render();
        $filename = 'monthly_report_' . $startDate->format('Y_m') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateDevicePDFReport($energyLogs, $summary, $device, $dateFrom, $dateTo)
    {
        $html = view('reports.pdf.device', compact('energyLogs', 'summary', 'device', 'dateFrom', 'dateTo'))->render();
        $filename = 'device_report_' . $device->type . '_' . $dateFrom->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function generateEfficiencyPDFReport($efficiencyData, $dateFrom, $dateTo)
    {
        $html = view('reports.pdf.efficiency', compact('efficiencyData', 'dateFrom', 'dateTo'))->render();
        $filename = 'efficiency_report_' . $dateFrom->format('Y_m_d') . '.pdf';
        
        return response($html)
            ->header('Content-Type', 'text/html')
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }
}