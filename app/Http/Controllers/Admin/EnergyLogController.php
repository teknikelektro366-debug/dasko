<?php

namespace App\Http\Controllers\Admin;

use App\Http\Controllers\Controller;
use App\Models\EnergyLog;
use App\Models\Device;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;
use Carbon\Carbon;

class EnergyLogController extends Controller
{
    public function index(Request $request)
    {
        $query = EnergyLog::with('device');

        // Apply filters
        if ($request->device_type) {
            $query->whereHas('device', function($q) use ($request) {
                $q->where('type', $request->device_type);
            });
        }

        if ($request->date_from) {
            $query->whereDate('start_time', '>=', $request->date_from);
        }

        if ($request->date_to) {
            $query->whereDate('start_time', '<=', $request->date_to);
        }

        if ($request->working_hours !== null) {
            $query->where('working_hours', $request->boolean('working_hours'));
        }

        if ($request->energy_saving !== null) {
            $query->where('energy_saving_mode', $request->boolean('energy_saving'));
        }

        // Sort by latest
        $query->latest('start_time');

        $logs = $query->paginate(25)->withQueryString();

        // Get summary statistics
        $stats = $this->getLogStatistics($request);

        // Get device types for filter
        $deviceTypes = Device::distinct('type')->pluck('type');

        return view('admin.energy-logs.index', compact('logs', 'stats', 'deviceTypes'));
    }

    public function show(EnergyLog $log)
    {
        $log->load('device');
        
        // Get related logs (same device, same day)
        $relatedLogs = EnergyLog::where('device_id', $log->device_id)
            ->whereDate('start_time', $log->start_time->toDateString())
            ->where('id', '!=', $log->id)
            ->latest('start_time')
            ->limit(10)
            ->get();

        return view('admin.energy-logs.show', compact('log', 'relatedLogs'));
    }

    public function destroy(EnergyLog $log)
    {
        $log->delete();

        return redirect()->route('admin.energy-logs.index')
            ->with('success', 'Energy log berhasil dihapus.');
    }

    public function cleanup(Request $request)
    {
        $request->validate([
            'cleanup_type' => 'required|in:old_data,test_data,all_data',
            'days_old' => 'nullable|integer|min:1|max:365'
        ]);

        try {
            $deletedCount = 0;

            switch ($request->cleanup_type) {
                case 'old_data':
                    $days = $request->days_old ?? 30;
                    $deletedCount = EnergyLog::where('created_at', '<', now()->subDays($days))->delete();
                    break;

                case 'test_data':
                    // Delete logs with unrealistic values (likely test data)
                    $deletedCount = EnergyLog::where('energy_consumed', '>', 10000) // > 10kWh
                        ->orWhere('duration_minutes', '>', 1440) // > 24 hours
                        ->orWhere('people_count', '>', 50) // > 50 people
                        ->delete();
                    break;

                case 'all_data':
                    $deletedCount = EnergyLog::count();
                    EnergyLog::truncate();
                    break;
            }

            return response()->json([
                'status' => 'success',
                'message' => "Berhasil menghapus {$deletedCount} energy logs.",
                'deleted_count' => $deletedCount
            ]);

        } catch (\Exception $e) {
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal membersihkan data: ' . $e->getMessage()
            ], 500);
        }
    }

    public function export(Request $request)
    {
        $query = EnergyLog::with('device');

        // Apply same filters as index
        if ($request->device_type) {
            $query->whereHas('device', function($q) use ($request) {
                $q->where('type', $request->device_type);
            });
        }

        if ($request->date_from) {
            $query->whereDate('start_time', '>=', $request->date_from);
        }

        if ($request->date_to) {
            $query->whereDate('start_time', '<=', $request->date_to);
        }

        $logs = $query->latest('start_time')->get();

        // Prepare CSV data
        $csvData = [];
        $csvData[] = [
            'ID', 'Device Name', 'Device Type', 'Energy Consumed (Wh)', 
            'Duration (minutes)', 'Start Time', 'End Time', 'People Count',
            'Outdoor Temperature', 'AC Target Temperature', 'Energy Saving Mode',
            'Working Hours', 'Cost Estimate (IDR)', 'Efficiency Rating'
        ];

        foreach ($logs as $log) {
            $csvData[] = [
                $log->id,
                $log->device->name,
                $log->device->type,
                $log->energy_consumed,
                $log->duration_minutes,
                $log->start_time->format('Y-m-d H:i:s'),
                $log->end_time ? $log->end_time->format('Y-m-d H:i:s') : '',
                $log->people_count,
                $log->outdoor_temperature,
                $log->ac_target_temperature,
                $log->energy_saving_mode ? 'Yes' : 'No',
                $log->working_hours ? 'Yes' : 'No',
                $log->cost_estimate,
                $log->efficiency_rating
            ];
        }

        $filename = 'energy_logs_' . now()->format('Y_m_d_H_i_s') . '.csv';

        $callback = function() use ($csvData) {
            $file = fopen('php://output', 'w');
            foreach ($csvData as $row) {
                fputcsv($file, $row);
            }
            fclose($file);
        };

        return response()->stream($callback, 200, [
            'Content-Type' => 'text/csv',
            'Content-Disposition' => 'attachment; filename="' . $filename . '"'
        ]);
    }

    private function getLogStatistics($request)
    {
        $query = EnergyLog::query();

        // Apply same filters
        if ($request->device_type) {
            $query->whereHas('device', function($q) use ($request) {
                $q->where('type', $request->device_type);
            });
        }

        if ($request->date_from) {
            $query->whereDate('start_time', '>=', $request->date_from);
        }

        if ($request->date_to) {
            $query->whereDate('start_time', '<=', $request->date_to);
        }

        if ($request->working_hours !== null) {
            $query->where('working_hours', $request->boolean('working_hours'));
        }

        if ($request->energy_saving !== null) {
            $query->where('energy_saving_mode', $request->boolean('energy_saving'));
        }

        return [
            'total_logs' => $query->count(),
            'total_energy' => round($query->sum('energy_consumed'), 2),
            'total_cost' => round($query->sum('cost_estimate'), 2),
            'avg_energy_per_log' => round($query->avg('energy_consumed'), 2),
            'total_runtime' => $query->sum('duration_minutes'),
            'energy_saving_logs' => $query->where('energy_saving_mode', true)->count(),
            'working_hours_logs' => $query->where('working_hours', true)->count(),
            'date_range' => [
                'from' => $query->min('start_time'),
                'to' => $query->max('start_time')
            ]
        ];
    }
}