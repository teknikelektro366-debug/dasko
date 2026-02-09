<?php

namespace App\Http\Controllers;

use App\Models\Device;
use App\Models\SensorReading;
use App\Models\SensorData;
use App\Models\EnergyLog;
use App\Models\ACAutomationRule;
use App\Services\ArduinoService;
use App\Services\EnergyCalculationService;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Cache;
use Carbon\Carbon;

class DashboardController extends Controller
{
    protected $arduinoService;
    protected $energyService;

    public function __construct(ArduinoService $arduinoService, EnergyCalculationService $energyService)
    {
        $this->arduinoService = $arduinoService;
        $this->energyService = $energyService;
        // Remove auth middleware for now
        // $this->middleware('auth');
    }

    public function index()
    {
        // Get latest sensor data from database
        $sensorData = $this->getLatestSensorData();
        
        // Get device status
        $devices = Device::with(['energyLogs' => function($query) {
            $query->today();
        }])->get();

        // Get today's energy consumption
        $todayEnergy = EnergyLog::today()->sum('energy_consumed');
        
        // Get AC automation rules
        $acRules = ACAutomationRule::active()->orderBy('min_people')->get();
        
        // Get working hours status
        $workingHoursStatus = $this->getWorkingHoursStatus();
        
        // Get energy saving status
        $energySavingStatus = $this->getEnergySavingStatus();

        // Get daily statistics
        $dailyStats = SensorData::getDailyStats();

        // Get total records count
        $totalRecords = SensorData::count();

        // Get recent data for frequency calculation
        $recentData = SensorData::where('created_at', '>=', now()->subMinutes(10))
                                ->orderBy('created_at', 'desc')
                                ->limit(5)
                                ->get();
        
        $updateFrequency = $this->calculateUpdateFrequency($recentData);

        // Get energy savings data
        $energySavings = $this->energyService->calculateEnergySavings(now()->format('Y-m-d'));

        return view('dashboard', compact(
            'sensorData',
            'devices',
            'todayEnergy',
            'acRules',
            'workingHoursStatus',
            'energySavingStatus',
            'dailyStats',
            'totalRecords',
            'updateFrequency',
            'energySavings'
        ));
    }

    public function analytics()
    {
        // Weekly energy consumption
        $weeklyData = EnergyLog::thisWeek()
            ->selectRaw('DATE(start_time) as date, SUM(energy_consumed) as total_energy')
            ->groupBy('date')
            ->orderBy('date')
            ->get();

        // Monthly energy consumption by device type
        $monthlyByDevice = EnergyLog::thisMonth()
            ->join('devices', 'energy_logs.device_id', '=', 'devices.id')
            ->selectRaw('devices.type, SUM(energy_consumed) as total_energy')
            ->groupBy('devices.type')
            ->get();

        // Efficiency trends
        $efficiencyData = EnergyLog::thisMonth()
            ->whereNotNull('people_count')
            ->where('people_count', '>', 0)
            ->selectRaw('DATE(start_time) as date, AVG(energy_consumed / people_count) as efficiency')
            ->groupBy('date')
            ->orderBy('date')
            ->get();

        return view('dashboard.analytics', compact(
            'weeklyData',
            'monthlyByDevice',
            'efficiencyData'
        ));
    }

    public function history()
    {
        $energyLogs = EnergyLog::with('device')
            ->latest('start_time')
            ->paginate(50);

        // Summary statistics
        $summary = [
            'today' => EnergyLog::today()->sum('energy_consumed'),
            'week' => EnergyLog::thisWeek()->sum('energy_consumed'),
            'month' => EnergyLog::thisMonth()->sum('energy_consumed'),
            'avg_daily' => EnergyLog::thisMonth()->avg('energy_consumed')
        ];

        return view('dashboard.history', compact('energyLogs', 'summary'));
    }

    public function devices()
    {
        $devices = Device::with(['energyLogs' => function($query) {
            $query->today();
        }])->get();

        // Get Arduino connection status
        $arduinoStatus = $this->arduinoService->getConnectionStatus();

        return view('dashboard.devices', compact('devices', 'arduinoStatus'));
    }

    public function automation()
    {
        $acRules = ACAutomationRule::orderBy('min_people')->get();
        
        // Get current conditions
        $currentConditions = $this->getCurrentConditions();
        
        // Get active rule
        $activeRule = null;
        if ($currentConditions) {
            $activeRule = ACAutomationRule::forCondition(
                $currentConditions['people_count'],
                $currentConditions['outdoor_temp']
            )->first();
        }

        return view('dashboard.automation', compact(
            'acRules',
            'currentConditions',
            'activeRule'
        ));
    }

    public function settings()
    {
        $config = [
            'arduino_ip' => config('arduino.ip'),
            'working_start_hour' => config('energy.working_start_hour'),
            'working_end_hour' => config('energy.working_end_hour'),
            'working_days_enabled' => config('energy.working_days_enabled'),
            'energy_saving_delay' => config('energy.energy_saving_delay'),
            'panel_shutdown_delay' => config('energy.panel_shutdown_delay'),
            'energy_saving_temp' => config('energy.energy_saving_temp')
        ];

        return view('dashboard.settings', compact('config'));
    }

    private function getLatestSensorData()
    {
        return Cache::remember('latest_sensor_data', 30, function () {
            $latestData = SensorData::latest()->first();
            
            if (!$latestData) {
                // Return default values if no data
                return [
                    'temperature' => 27.0,
                    'humidity' => 65.0,
                    'light_intensity' => 500,
                    'people_count' => 0,
                    'ac_status' => 'OFF',
                    'set_temperature' => null,
                    'proximity_in' => false,
                    'proximity_out' => false,
                    'wifi_rssi' => null,
                    'last_updated' => now(),
                    'device_id' => 'ESP32_Smart_Energy',
                    'location' => 'Ruang Dosen Prodi Teknik Elektro',
                    'status' => 'offline',
                    'data_age_minutes' => 0,
                    'connection_status' => 'offline'
                ];
            }

            $dataAgeMinutes = $latestData->created_at->diffInMinutes(now());
            $connectionStatus = $dataAgeMinutes <= 2 ? 'online' : 'offline';

            return [
                'id' => $latestData->id,
                'temperature' => $latestData->room_temperature ?? 27.0,
                'humidity' => $latestData->humidity ?? 65.0,
                'light_intensity' => $latestData->light_level ?? 500,
                'people_count' => $latestData->people_count ?? 0,
                'ac_status' => $latestData->ac_status ?? 'OFF',
                'set_temperature' => $latestData->set_temperature,
                'lamp_status' => $latestData->lamp_status ?? 'OFF',
                'proximity_in' => $latestData->proximity_in ?? false,
                'proximity_out' => $latestData->proximity_out ?? false,
                'wifi_rssi' => $latestData->wifi_rssi,
                'last_updated' => $latestData->created_at,
                'device_id' => $latestData->device_id,
                'location' => $latestData->location,
                'status' => $latestData->status ?? 'active',
                'data_age_minutes' => $dataAgeMinutes,
                'connection_status' => $connectionStatus,
                'formatted' => [
                    'temperature' => $latestData->formatted_temperature ?? $latestData->room_temperature . '°C',
                    'humidity' => $latestData->formatted_humidity ?? $latestData->humidity . '%',
                    'light' => $latestData->formatted_light ?? $latestData->light_level . ' lux',
                    'wifi' => $latestData->formatted_wifi ?? $latestData->wifi_rssi . ' dBm'
                ],
                'time_ago' => $latestData->time_ago ?? $latestData->created_at->diffForHumans(),
                'last_update_time' => $latestData->created_at->format('H:i:s'),
                'last_update_date' => $latestData->created_at->format('d/m/Y'),
                'wifi_quality' => $this->getWifiQuality($latestData->wifi_rssi)
            ];
        });
    }

    private function getWorkingHoursStatus()
    {
        $now = now();
        $startHour = config('energy.working_start_hour', 7);
        $endHour = config('energy.working_end_hour', 17);
        $workingDaysEnabled = config('energy.working_days_enabled', true);

        $isWorkingDay = !$workingDaysEnabled || $now->isWeekday();
        $isWorkingHours = $now->hour >= $startHour && $now->hour < $endHour;

        return [
            'is_working_hours' => $isWorkingDay && $isWorkingHours,
            'is_working_day' => $isWorkingDay,
            'current_time' => $now->format('H:i'),
            'current_day' => $now->format('l'),
            'working_range' => sprintf('%02d:00 - %02d:00', $startHour, $endHour),
            'status' => $isWorkingDay && $isWorkingHours ? 'Jam Kerja Aktif' : 
                       (!$isWorkingDay ? 'Hari Libur' : 'Di Luar Jam Kerja')
        ];
    }

    private function getEnergySavingStatus()
    {
        $sensorData = $this->getLatestSensorData();
        $lastPersonDetected = Cache::get('last_person_detected', now());
        
        $minutesWithoutPeople = $sensorData['people_count'] > 0 ? 0 : 
            now()->diffInMinutes($lastPersonDetected);

        $energySavingDelay = config('energy.energy_saving_delay', 15);
        $panelShutdownDelay = config('energy.panel_shutdown_delay', 30);

        $currentState = 'Normal Operation';
        if ($minutesWithoutPeople >= $panelShutdownDelay) {
            $currentState = 'Panel Shutdown';
        } elseif ($minutesWithoutPeople >= $energySavingDelay) {
            $currentState = 'Energy Saving Mode';
        }

        return [
            'current_state' => $currentState,
            'minutes_without_people' => $minutesWithoutPeople,
            'time_to_energy_saving' => max(0, $energySavingDelay - $minutesWithoutPeople),
            'time_to_shutdown' => max(0, $panelShutdownDelay - $minutesWithoutPeople),
            'energy_saving_temp' => config('energy.energy_saving_temp', 28)
        ];
    }

    private function getCurrentConditions()
    {
        $sensorData = $this->getLatestSensorData();
        
        return [
            'people_count' => $sensorData['people_count'],
            'outdoor_temp' => $sensorData['temperature'],
            'timestamp' => $sensorData['last_updated']
        ];
    }

    private function getWifiQuality($rssi)
    {
        if (!$rssi) return 'Unknown';
        if ($rssi >= -30) return 'Excellent';
        if ($rssi >= -50) return 'Good';
        if ($rssi >= -60) return 'Fair';
        if ($rssi >= -70) return 'Weak';
        return 'Very Weak';
    }

    private function calculateUpdateFrequency($recentData)
    {
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
}