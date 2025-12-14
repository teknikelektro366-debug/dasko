<?php

namespace App\Http\Controllers;

use App\Models\Device;
use App\Models\SensorReading;
use App\Models\EnergyLog;
use App\Models\ACAutomationRule;
use App\Services\ArduinoService;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Cache;
use Carbon\Carbon;

class DashboardController extends Controller
{
    protected $arduinoService;

    public function __construct(ArduinoService $arduinoService)
    {
        $this->arduinoService = $arduinoService;
    }

    public function index()
    {
        // Get latest sensor readings
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

        return view('dashboard.index', compact(
            'sensorData',
            'devices',
            'todayEnergy',
            'acRules',
            'workingHoursStatus',
            'energySavingStatus'
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
            $temperature = SensorReading::byType(SensorReading::TYPE_TEMPERATURE)
                ->latest()
                ->first();
                
            $humidity = SensorReading::byType(SensorReading::TYPE_HUMIDITY)
                ->latest()
                ->first();
                
            $light = SensorReading::byType(SensorReading::TYPE_LIGHT)
                ->latest()
                ->first();
                
            $peopleCount = SensorReading::byType(SensorReading::TYPE_PEOPLE_COUNT)
                ->latest()
                ->first();

            return [
                'temperature' => $temperature ? $temperature->value : 27.0,
                'humidity' => $humidity ? $humidity->value : 65.0,
                'light_intensity' => $light ? $light->value : 500,
                'people_count' => $peopleCount ? $peopleCount->value : 0,
                'last_updated' => $temperature ? $temperature->reading_time : now()
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
}