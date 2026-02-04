<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\SensorData;
use Illuminate\Http\Request;
use Illuminate\Http\JsonResponse;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Validator;

class SensorDataController extends Controller
{
    /**
     * Get all sensor data (for testing and API access)
     */
    public function index(Request $request): JsonResponse
    {
        try {
            $perPage = $request->input('per_page', 20);
            $perPage = min(max($perPage, 5), 100); // Limit 5-100

            $data = SensorData::latest()
                             ->paginate($perPage);

            return response()->json([
                'success' => true,
                'message' => 'Sensor data retrieved successfully',
                'data' => $data->items(),
                'pagination' => [
                    'current_page' => $data->currentPage(),
                    'last_page' => $data->lastPage(),
                    'per_page' => $data->perPage(),
                    'total' => $data->total(),
                    'from' => $data->firstItem(),
                    'to' => $data->lastItem()
                ],
                'meta' => [
                    'endpoint' => '/api/sensor/data',
                    'method' => 'GET',
                    'description' => 'Get paginated sensor data',
                    'parameters' => [
                        'per_page' => 'Number of items per page (5-100, default: 20)'
                    ]
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting sensor data index:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Error retrieving sensor data',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Terima data dari Arduino ESP32
     */
    public function store(Request $request): JsonResponse
    {
        try {
            // Log request untuk debugging
            Log::info('Arduino data received:', $request->all());

            // Validasi data dari Arduino
            $validator = Validator::make($request->all(), [
                'device_id' => 'string|max:255',
                'location' => 'string|max:255',
                'people_count' => 'required|integer|min:0|max:100',
                'ac_status' => 'required|string|max:50',
                'set_temperature' => 'nullable|integer|max:30',
                'room_temperature' => 'nullable|numeric|min:-10|max:60',
                'humidity' => 'nullable|numeric|min:0|max:100',
                'light_level' => 'nullable|integer|min:0|max:1000',
                'proximity_in' => 'nullable|boolean',
                'proximity_out' => 'nullable|boolean',
                'wifi_rssi' => 'nullable|integer|min:-100|max:0',
                'status' => 'nullable|string|max:50',
                'timestamp' => 'nullable|integer'
            ]);

            if ($validator->fails()) {
                Log::warning('Arduino data validation failed:', $validator->errors()->toArray());
                return response()->json([
                    'success' => false,
                    'message' => 'Validation failed',
                    'errors' => $validator->errors()
                ], 422);
            }

            $deviceId = $request->input('device_id', 'ESP32_Smart_Energy');
            $location = $request->input('location', 'Lab Teknik Tegangan Tinggi');

            // Update or Create data berdasarkan device_id dan location
            $sensorData = SensorData::updateOrCreate(
                [
                    'device_id' => $deviceId,
                    'location' => $location
                ],
                [
                    'people_count' => $request->input('people_count'),
                    'ac_status' => $request->input('ac_status'),
                    'set_temperature' => $request->input('set_temperature'),
                    'room_temperature' => $request->input('room_temperature'),
                    'humidity' => $request->input('humidity'),
                    'light_level' => $request->input('light_level', 0),
                    'proximity_in' => $request->input('proximity_in', false),
                    'proximity_out' => $request->input('proximity_out', false),
                    'wifi_rssi' => $request->input('wifi_rssi'),
                    'status' => $request->input('status', 'active'),
                    'device_timestamp' => $request->input('timestamp'),
                    'updated_at' => now()
                ]
            );

            // Determine if this was an update or create
            $wasRecentlyCreated = $sensorData->wasRecentlyCreated;
            $action = $wasRecentlyCreated ? 'created' : 'updated';

            // Log berhasil
            Log::info("Sensor data {$action} successfully:", [
                'id' => $sensorData->id,
                'device_id' => $sensorData->device_id,
                'action' => $action
            ]);

            return response()->json([
                'success' => true,
                'message' => "Data berhasil {$action}",
                'action' => $action,
                'data' => [
                    'id' => $sensorData->id,
                    'device_id' => $sensorData->device_id,
                    'location' => $sensorData->location,
                    'people_count' => $sensorData->people_count,
                    'ac_status' => $sensorData->ac_status,
                    'set_temperature' => $sensorData->set_temperature,
                    'room_temperature' => $sensorData->room_temperature,
                    'humidity' => $sensorData->humidity,
                    'light_level' => $sensorData->light_level,
                    'timestamp' => $sensorData->updated_at->toISOString(),
                    'was_recently_created' => $wasRecentlyCreated
                ]
            ], $wasRecentlyCreated ? 201 : 200);

        } catch (\Exception $e) {
            Log::error('Error saving sensor data:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString(),
                'request_data' => $request->all()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Terjadi kesalahan saat menyimpan data',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Get latest sensor data (alias for realtime method)
     */
    public function latest(): JsonResponse
    {
        return $this->realtime();
    }

    /**
     * Get realtime data untuk dashboard
     */
    public function realtime(): JsonResponse
    {
        try {
            $latestData = SensorData::latest()->first();

            if (!$latestData) {
                return response()->json([
                    'success' => false,
                    'message' => 'Tidak ada data sensor',
                    'data' => null
                ], 404);
            }

            // Check if data is recent (within last 2 minutes)
            $isRecent = $latestData->created_at->diffInMinutes(now()) <= 2;

            return response()->json([
                'success' => true,
                'data' => [
                    'id' => $latestData->id,
                    'device_id' => $latestData->device_id,
                    'location' => $latestData->location,
                    'people_count' => $latestData->people_count,
                    'ac_status' => $latestData->ac_status,
                    'set_temperature' => $latestData->set_temperature,
                    'room_temperature' => $latestData->room_temperature,
                    'humidity' => $latestData->humidity,
                    'light_level' => $latestData->light_level,
                    'proximity_in' => $latestData->proximity_in,
                    'proximity_out' => $latestData->proximity_out,
                    'wifi_rssi' => $latestData->wifi_rssi,
                    'status' => $latestData->status,
                    'created_at' => $latestData->created_at->toISOString(),
                    'time_ago' => $latestData->time_ago,
                    'is_recent' => $isRecent,
                    'formatted' => [
                        'temperature' => $latestData->formatted_temperature,
                        'humidity' => $latestData->formatted_humidity,
                        'light' => $latestData->formatted_light,
                        'wifi' => $latestData->formatted_wifi
                    ],
                    'realtime_info' => [
                        'last_update' => $latestData->created_at->format('H:i:s'),
                        'update_frequency' => $this->calculateUpdateFrequency(),
                        'data_age_seconds' => $latestData->created_at->diffInSeconds(now()),
                        'connection_quality' => $isRecent ? 'good' : 'poor'
                    ]
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting realtime sensor data:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Terjadi kesalahan saat mengambil data realtime',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Calculate update frequency from recent data
     */
    private function calculateUpdateFrequency()
    {
        $recentData = SensorData::where('created_at', '>=', now()->subMinutes(5))
                                ->orderBy('created_at', 'desc')
                                ->limit(10)
                                ->get();

        if ($recentData->count() < 2) {
            return null;
        }

        $intervals = [];
        for ($i = 0; $i < $recentData->count() - 1; $i++) {
            $interval = $recentData[$i]->created_at->diffInSeconds($recentData[$i + 1]->created_at);
            $intervals[] = $interval;
        }

        return round(array_sum($intervals) / count($intervals), 1);
    }

    /**
     * Get data untuk chart (24 jam terakhir)
     */
    public function chartData(Request $request): JsonResponse
    {
        try {
            $hours = $request->input('hours', 24);
            $hours = min(max($hours, 1), 168); // Batasi 1-168 jam (1 minggu)

            $data = SensorData::where('created_at', '>=', now()->subHours($hours))
                             ->orderBy('created_at')
                             ->get();

            // Group data per jam untuk chart
            $chartData = $data->groupBy(function($item) {
                return $item->created_at->format('H:00');
            })->map(function($group) {
                return [
                    'time' => $group->first()->created_at->format('H:00'),
                    'people_count' => round($group->avg('people_count'), 1),
                    'temperature' => round($group->avg('room_temperature'), 1),
                    'humidity' => round($group->avg('humidity'), 1),
                    'light_level' => round($group->avg('light_level'), 0),
                    'ac_on' => $group->where('ac_status', '!=', 'OFF')->count() > 0
                ];
            })->values();

            return response()->json([
                'success' => true,
                'data' => $chartData,
                'period' => $hours . ' hours',
                'total_records' => $data->count()
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting chart data:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Terjadi kesalahan saat mengambil data chart',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Get statistik harian
     */
    public function dailyStats(): JsonResponse
    {
        try {
            $stats = SensorData::getDailyStats();

            return response()->json([
                'success' => true,
                'data' => $stats
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting daily stats:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Terjadi kesalahan saat mengambil statistik',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Get data history dengan pagination
     */
    public function history(Request $request): JsonResponse
    {
        try {
            $perPage = $request->input('per_page', 50);
            $perPage = min(max($perPage, 10), 100); // Batasi 10-100

            // Debug: Log request untuk troubleshooting
            Log::info('History API called:', [
                'per_page' => $perPage,
                'request_params' => $request->all()
            ]);

            $query = SensorData::latest();
            
            // Add device filter if provided
            if ($request->has('device_id') && $request->device_id) {
                $query->where('device_id', $request->device_id);
            }
            
            $data = $query->paginate($perPage);
            
            // Debug: Log hasil query
            Log::info('History query result:', [
                'total_records' => $data->total(),
                'current_page' => $data->currentPage(),
                'per_page' => $data->perPage(),
                'data_count' => $data->items() ? count($data->items()) : 0
            ]);

            return response()->json([
                'success' => true,
                'data' => $data->items(),
                'pagination' => [
                    'current_page' => $data->currentPage(),
                    'last_page' => $data->lastPage(),
                    'per_page' => $data->perPage(),
                    'total' => $data->total(),
                    'from' => $data->firstItem(),
                    'to' => $data->lastItem()
                ],
                'debug_info' => [
                    'query_executed' => true,
                    'total_in_db' => SensorData::count(),
                    'latest_record' => SensorData::latest()->first() ? SensorData::latest()->first()->created_at : null
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting history data:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Terjadi kesalahan saat mengambil history',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error',
                'debug_info' => [
                    'total_records_in_db' => SensorData::count(),
                    'error_occurred' => true
                ]
            ], 500);
        }
    }

    /**
     * Cleanup data lama
     */
    public function cleanup(Request $request): JsonResponse
    {
        try {
            $days = $request->input('days', 30);
            $days = min(max($days, 7), 365); // Batasi 7-365 hari

            $deletedCount = SensorData::cleanup($days);

            Log::info('Sensor data cleanup completed:', ['deleted_count' => $deletedCount, 'days' => $days]);

            return response()->json([
                'success' => true,
                'message' => "Berhasil menghapus {$deletedCount} data lama (lebih dari {$days} hari)",
                'deleted_count' => $deletedCount
            ]);

        } catch (\Exception $e) {
            Log::error('Error during cleanup:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Terjadi kesalahan saat cleanup data',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }
}