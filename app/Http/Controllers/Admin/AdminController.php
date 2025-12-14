<?php

namespace App\Http\Controllers\Admin;

use App\Http\Controllers\Controller;
use App\Models\User;
use App\Models\Device;
use App\Models\SensorReading;
use App\Models\EnergyLog;
use App\Models\ACAutomationRule;
use App\Services\ArduinoService;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Artisan;
use Illuminate\Support\Facades\Cache;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Storage;
use Carbon\Carbon;

class AdminController extends Controller
{
    protected $arduinoService;

    public function __construct(ArduinoService $arduinoService)
    {
        $this->arduinoService = $arduinoService;
    }

    public function index()
    {
        // System overview statistics
        $stats = [
            'total_users' => User::count(),
            'active_users' => User::active()->count(),
            'total_devices' => Device::count(),
            'active_devices' => Device::active()->count(),
            'total_energy_today' => EnergyLog::today()->sum('energy_consumed'),
            'total_energy_month' => EnergyLog::thisMonth()->sum('energy_consumed'),
            'ac_rules_count' => ACAutomationRule::active()->count(),
            'sensor_readings_today' => SensorReading::today()->count()
        ];

        // Recent activities
        $recentLogs = EnergyLog::with('device')
            ->latest()
            ->limit(10)
            ->get();

        $recentUsers = User::latest('last_login_at')
            ->limit(5)
            ->get();

        // System health
        $systemHealth = $this->getSystemHealth();

        // Arduino connection status
        $arduinoStatus = $this->arduinoService->getConnectionStatus();

        return view('admin.dashboard', compact(
            'stats',
            'recentLogs',
            'recentUsers',
            'systemHealth',
            'arduinoStatus'
        ));
    }

    public function overview()
    {
        // Detailed system overview
        $overview = [
            'database' => $this->getDatabaseStats(),
            'storage' => $this->getStorageStats(),
            'cache' => $this->getCacheStats(),
            'logs' => $this->getLogStats(),
            'arduino' => $this->getArduinoStats()
        ];

        return view('admin.overview', compact('overview'));
    }

    public function settings()
    {
        $settings = [
            'app_name' => config('app.name'),
            'app_env' => config('app.env'),
            'app_debug' => config('app.debug'),
            'arduino_ip' => config('arduino.ip'),
            'arduino_username' => config('arduino.username'),
            'working_start_hour' => config('energy.working_start_hour'),
            'working_end_hour' => config('energy.working_end_hour'),
            'working_days_enabled' => config('energy.working_days_enabled'),
            'energy_saving_delay' => config('energy.energy_saving_delay'),
            'panel_shutdown_delay' => config('energy.panel_shutdown_delay'),
            'energy_saving_temp' => config('energy.energy_saving_temp'),
            'electricity_rate' => config('energy.electricity_rate')
        ];

        return view('admin.settings', compact('settings'));
    }

    public function updateSettings(Request $request)
    {
        $request->validate([
            'arduino_ip' => 'required|ip',
            'arduino_username' => 'required|string|max:50',
            'arduino_password' => 'nullable|string|max:50',
            'working_start_hour' => 'required|integer|between:0,23',
            'working_end_hour' => 'required|integer|between:0,23',
            'working_days_enabled' => 'boolean',
            'energy_saving_delay' => 'required|integer|between:1,60',
            'panel_shutdown_delay' => 'required|integer|between:1,120',
            'energy_saving_temp' => 'required|integer|between:20,35',
            'electricity_rate' => 'required|numeric|min:0'
        ]);

        // Update environment variables (in production, this would update .env file)
        $envUpdates = [
            'ARDUINO_IP' => $request->arduino_ip,
            'ARDUINO_USERNAME' => $request->arduino_username,
            'WORKING_START_HOUR' => $request->working_start_hour,
            'WORKING_END_HOUR' => $request->working_end_hour,
            'WORKING_DAYS_ENABLED' => $request->boolean('working_days_enabled'),
            'ENERGY_SAVING_DELAY' => $request->energy_saving_delay,
            'PANEL_SHUTDOWN_DELAY' => $request->panel_shutdown_delay,
            'ENERGY_SAVING_TEMP' => $request->energy_saving_temp,
            'ELECTRICITY_RATE' => $request->electricity_rate
        ];

        if ($request->arduino_password) {
            $envUpdates['ARDUINO_PASSWORD'] = $request->arduino_password;
        }

        // Update cache
        foreach ($envUpdates as $key => $value) {
            Cache::put("config.{$key}", $value, 3600);
        }

        // Send updated settings to Arduino
        if ($request->arduino_password) {
            $this->arduinoService->updateWorkingHours(
                $request->working_start_hour,
                $request->working_end_hour,
                $request->boolean('working_days_enabled')
            );

            $this->arduinoService->updateEnergySaving(
                true,
                $request->energy_saving_delay,
                $request->panel_shutdown_delay
            );
        }

        return redirect()->back()->with('success', 'Pengaturan berhasil diperbarui.');
    }

    public function arduino()
    {
        $connectionStatus = $this->arduinoService->testConnection();
        $sensorData = Cache::get('arduino_sensor_data', []);
        $acAutomationStatus = $this->arduinoService->getACAutomationStatus();

        return view('admin.arduino', compact(
            'connectionStatus',
            'sensorData',
            'acAutomationStatus'
        ));
    }

    public function testArduinoConnection()
    {
        $result = $this->arduinoService->testConnection();
        
        return response()->json($result);
    }

    public function syncArduino()
    {
        try {
            // Sync device states
            $devices = Device::all();
            $syncResults = [];

            foreach ($devices as $device) {
                if ($device->type !== 'panel') {
                    $result = $this->arduinoService->sendCommand(
                        $device->type,
                        $device->is_active ? 'on' : 'off'
                    );
                    $syncResults[$device->type] = $result;
                }
            }

            // Sync AC automation rules
            $acRules = ACAutomationRule::active()->get();
            
            return response()->json([
                'status' => 'success',
                'message' => 'Arduino berhasil disinkronisasi',
                'sync_results' => $syncResults,
                'rules_synced' => $acRules->count()
            ]);

        } catch (\Exception $e) {
            Log::error('Arduino sync failed: ' . $e->getMessage());
            
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal sinkronisasi Arduino: ' . $e->getMessage()
            ], 500);
        }
    }

    public function emergencyShutdown()
    {
        try {
            $result = $this->arduinoService->emergencyShutdown();
            
            // Update all devices to inactive
            Device::where('type', '!=', 'fridge')
                  ->where('type', '!=', 'router')
                  ->update(['is_active' => false]);

            Log::warning('Emergency shutdown executed by admin: ' . auth()->user()->name);

            return response()->json([
                'status' => 'success',
                'message' => 'Emergency shutdown berhasil dieksekusi',
                'arduino_response' => $result
            ]);

        } catch (\Exception $e) {
            Log::error('Emergency shutdown failed: ' . $e->getMessage());
            
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal melakukan emergency shutdown: ' . $e->getMessage()
            ], 500);
        }
    }

    public function database()
    {
        $stats = $this->getDatabaseStats();
        
        return view('admin.database', compact('stats'));
    }

    public function backupDatabase()
    {
        try {
            $filename = 'smart_energy_backup_' . now()->format('Y_m_d_H_i_s') . '.sql';
            
            // Run database backup command
            Artisan::call('backup:run', ['--only-db' => true]);
            
            return response()->json([
                'status' => 'success',
                'message' => 'Database backup berhasil dibuat',
                'filename' => $filename
            ]);

        } catch (\Exception $e) {
            Log::error('Database backup failed: ' . $e->getMessage());
            
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal membuat backup database: ' . $e->getMessage()
            ], 500);
        }
    }

    public function cleanupDatabase()
    {
        try {
            $deletedLogs = EnergyLog::where('created_at', '<', now()->subDays(30))->delete();
            $deletedReadings = SensorReading::where('created_at', '<', now()->subDays(7))->delete();
            
            return response()->json([
                'status' => 'success',
                'message' => 'Database cleanup berhasil',
                'deleted_logs' => $deletedLogs,
                'deleted_readings' => $deletedReadings
            ]);

        } catch (\Exception $e) {
            Log::error('Database cleanup failed: ' . $e->getMessage());
            
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal membersihkan database: ' . $e->getMessage()
            ], 500);
        }
    }

    public function resetDatabase()
    {
        try {
            // Clear all data except users and devices
            EnergyLog::truncate();
            SensorReading::truncate();
            
            // Reset device states
            Device::update(['is_active' => false]);
            
            // Clear cache
            Cache::flush();
            
            return response()->json([
                'status' => 'success',
                'message' => 'Database berhasil direset'
            ]);

        } catch (\Exception $e) {
            Log::error('Database reset failed: ' . $e->getMessage());
            
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal mereset database: ' . $e->getMessage()
            ], 500);
        }
    }

    public function logs()
    {
        $logFiles = $this->getLogFiles();
        
        return view('admin.logs', compact('logFiles'));
    }

    public function clearLogs()
    {
        try {
            $logPath = storage_path('logs');
            $files = glob($logPath . '/*.log');
            
            foreach ($files as $file) {
                if (is_file($file)) {
                    unlink($file);
                }
            }
            
            return response()->json([
                'status' => 'success',
                'message' => 'Log files berhasil dihapus',
                'cleared_files' => count($files)
            ]);

        } catch (\Exception $e) {
            return response()->json([
                'status' => 'error',
                'message' => 'Gagal menghapus log files: ' . $e->getMessage()
            ], 500);
        }
    }

    private function getSystemHealth()
    {
        return [
            'database' => $this->checkDatabaseHealth(),
            'storage' => $this->checkStorageHealth(),
            'cache' => $this->checkCacheHealth(),
            'arduino' => $this->arduinoService->getConnectionStatus()
        ];
    }

    private function getDatabaseStats()
    {
        return [
            'users_count' => User::count(),
            'devices_count' => Device::count(),
            'energy_logs_count' => EnergyLog::count(),
            'sensor_readings_count' => SensorReading::count(),
            'ac_rules_count' => ACAutomationRule::count(),
            'database_size' => $this->getDatabaseSize()
        ];
    }

    private function getStorageStats()
    {
        $totalSize = 0;
        $files = Storage::allFiles();
        
        foreach ($files as $file) {
            $totalSize += Storage::size($file);
        }

        return [
            'total_files' => count($files),
            'total_size' => $totalSize,
            'available_space' => disk_free_space(storage_path())
        ];
    }

    private function getCacheStats()
    {
        return [
            'driver' => config('cache.default'),
            'cached_items' => Cache::getStore()->getPrefix() ? 'Available' : 'Not Available'
        ];
    }

    private function getLogStats()
    {
        $logPath = storage_path('logs');
        $files = glob($logPath . '/*.log');
        $totalSize = 0;

        foreach ($files as $file) {
            $totalSize += filesize($file);
        }

        return [
            'log_files' => count($files),
            'total_size' => $totalSize,
            'latest_log' => count($files) > 0 ? basename(end($files)) : 'None'
        ];
    }

    private function getArduinoStats()
    {
        $status = $this->arduinoService->getConnectionStatus();
        $lastSensorUpdate = Cache::get('latest_sensor_data.last_updated', 'Never');

        return [
            'connection_status' => $status,
            'last_sensor_update' => $lastSensorUpdate,
            'ip_address' => config('arduino.ip'),
            'timeout' => config('arduino.timeout')
        ];
    }

    private function checkDatabaseHealth()
    {
        try {
            DB::connection()->getPdo();
            return 'healthy';
        } catch (\Exception $e) {
            return 'error';
        }
    }

    private function checkStorageHealth()
    {
        $freeSpace = disk_free_space(storage_path());
        $totalSpace = disk_total_space(storage_path());
        $usagePercent = (($totalSpace - $freeSpace) / $totalSpace) * 100;

        if ($usagePercent > 90) return 'critical';
        if ($usagePercent > 75) return 'warning';
        return 'healthy';
    }

    private function checkCacheHealth()
    {
        try {
            Cache::put('health_check', 'ok', 1);
            $result = Cache::get('health_check');
            return $result === 'ok' ? 'healthy' : 'error';
        } catch (\Exception $e) {
            return 'error';
        }
    }

    private function getDatabaseSize()
    {
        try {
            $result = DB::select("SELECT 
                ROUND(SUM(data_length + index_length) / 1024 / 1024, 2) AS 'size_mb'
                FROM information_schema.tables 
                WHERE table_schema = ?", [config('database.connections.mysql.database')]);
            
            return $result[0]->size_mb ?? 0;
        } catch (\Exception $e) {
            return 0;
        }
    }

    private function getLogFiles()
    {
        $logPath = storage_path('logs');
        $files = glob($logPath . '/*.log');
        $logFiles = [];

        foreach ($files as $file) {
            $logFiles[] = [
                'name' => basename($file),
                'size' => filesize($file),
                'modified' => filemtime($file),
                'path' => $file
            ];
        }

        return collect($logFiles)->sortByDesc('modified');
    }
}