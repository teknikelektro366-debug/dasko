<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\Device;
use App\Models\SensorReading;
use App\Models\EnergyLog;
use App\Models\ACAutomationRule;
use App\Services\ArduinoService;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Cache;
use Illuminate\Support\Facades\Validator;
use Carbon\Carbon;

class ArduinoController extends Controller
{
    protected $arduinoService;

    public function __construct(ArduinoService $arduinoService)
    {
        $this->arduinoService = $arduinoService;
    }

    /**
     * Receive sensor data from Arduino
     */
    public function receiveSensorData(Request $request)
    {
        $validator = Validator::make($request->all(), [
            'temperature' => 'required|numeric|between:-50,100',
            'humidity' => 'required|numeric|between:0,100',
            'light_intensity' => 'required|integer|between:0,4095',
            'people_count' => 'required|integer|between:0,100',
            'proximity1' => 'boolean',
            'proximity2' => 'boolean',
            'timestamp' => 'nullable|date'
        ]);

        if ($validator->fails()) {
            return response()->json([
                'status' => 'error',
                'message' => 'Invalid sensor data',
                'errors' => $validator->errors()
            ], 400);
        }

        $timestamp = $request->timestamp ? Carbon::parse($request->timestamp) : now();

        // Store sensor readings
        $this->storeSensorReading(SensorReading::TYPE_TEMPERATURE, $request->temperature, '°C', $timestamp);
        $this->storeSensorReading(SensorReading::TYPE_HUMIDITY, $request->humidity, '%', $timestamp);
        $this->storeSensorReading(SensorReading::TYPE_LIGHT, $request->light_intensity, 'lux', $timestamp);
        $this->storeSensorReading(SensorReading::TYPE_PEOPLE_COUNT, $request->people_count, 'people', $timestamp);

        // Update cache for real-time display
        Cache::put('latest_sensor_data', [
            'temperature' => $request->temperature,
            'humidity' => $request->humidity,
            'light_intensity' => $request->light_intensity,
            'people_count' => $request->people_count,
            'last_updated' => $timestamp
        ], 300); // 5 minutes

        // Update last person detected time
        if ($request->people_count > 0) {
            Cache::put('last_person_detected', $timestamp, 3600); // 1 hour
        }

        return response()->json([
            'status' => 'success',
            'message' => 'Sensor data received',
            'timestamp' => $timestamp->toISOString()
        ]);
    }

    /**
     * Get current sensor data
     */
    public function getSensorData()
    {
        $sensorData = Cache::get('latest_sensor_data', [
            'temperature' => 27.0,
            'humidity' => 65.0,
            'light_intensity' => 500,
            'people_count' => 0,
            'last_updated' => now()
        ]);

        // Get device states
        $devices = Device::all()->mapWithKeys(function ($device) {
            return [$device->type => $device->is_active];
        });

        // Get AC automation status
        $acAutomation = $this->getACAutomationStatus($sensorData);

        // Get working hours status
        $workingHours = $this->getWorkingHoursStatus();

        // Get energy saving status
        $energySaving = $this->getEnergySavingStatus($sensorData);

        return response()->json([
            'temperature' => $sensorData['temperature'],
            'humidity' => $sensorData['humidity'],
            'light_intensity' => $sensorData['light_intensity'],
            'people_count' => $sensorData['people_count'],
            'devices' => $devices,
            'ac_automation' => $acAutomation,
            'working_hours' => $workingHours,
            'energy_saving' => $energySaving,
            'timestamp' => now()->toISOString()
        ]);
    }

    /**
     * Control device via Arduino
     */
    public function controlDevice(Request $request)
    {
        $validator = Validator::make($request->all(), [
            'device' => 'required|string|in:ac1,ac2,lamp1,lamp2,computer,tv,dispenser,kettle,coffee,fridge,router,panel',
            'action' => 'required|string|in:on,off,toggle'
        ]);

        if ($validator->fails()) {
            return response()->json([
                'status' => 'error',
                'message' => 'Invalid control data',
                'errors' => $validator->errors()
            ], 400);
        }

        $deviceName = $request->device;
        $action = $request->action;

        // Find device in database
        $device = Device::where('type', $deviceName)->first();
        
        if (!$device) {
            return response()->json([
                'status' => 'error',
                'message' => 'Device not found'
            ], 404);
        }

        // Determine new state
        $newState = $action === 'on' ? true : ($action === 'off' ? false : !$device->is_active);

        // Update device state
        $device->update(['is_active' => $newState]);

        // Log energy usage if turning off
        if (!$newState && $device->is_active) {
            $this->logEnergyUsage($device);
        }

        // Send command to Arduino
        $arduinoResponse = $this->arduinoService->sendCommand($deviceName, $newState ? 'on' : 'off');

        return response()->json([
            'status' => 'success',
            'message' => "Device {$deviceName} turned " . ($newState ? 'on' : 'off'),
            'device_state' => $newState,
            'arduino_response' => $arduinoResponse
        ]);
    }

    /**
     * Get AC automation rules and status
     */
    public function getACAutomation()
    {
        $rules = ACAutomationRule::active()->orderBy('min_people')->get();
        
        $sensorData = Cache::get('latest_sensor_data', [
            'people_count' => 0,
            'temperature' => 27.0
        ]);

        $activeRule = ACAutomationRule::forCondition(
            $sensorData['people_count'],
            $sensorData['temperature']
        )->first();

        return response()->json([
            'rules' => $rules->map(function ($rule) {
                return [
                    'id' => $rule->id,
                    'people_range' => $rule->min_people . '-' . $rule->max_people,
                    'temp_range' => $rule->min_outdoor_temp . '-' . $rule->max_outdoor_temp,
                    'ac_units' => $rule->ac_units_needed,
                    'target_temp' => $rule->target_temperature,
                    'description' => $rule->description,
                    'condition_text' => $rule->condition_text,
                    'action_text' => $rule->action_text
                ];
            }),
            'current_conditions' => [
                'people_count' => $sensorData['people_count'],
                'outdoor_temp' => $sensorData['temperature']
            ],
            'active_rule' => $activeRule ? [
                'id' => $activeRule->id,
                'description' => $activeRule->description,
                'ac_units' => $activeRule->ac_units_needed,
                'target_temp' => $activeRule->target_temperature
            ] : null
        ]);
    }

    /**
     * Update AC automation rule
     */
    public function updateACAutomation(Request $request)
    {
        $validator = Validator::make($request->all(), [
            'action' => 'required|string|in:toggle,force_check,set_temp,start_gradual,stop_gradual',
            'temperature' => 'nullable|integer|between:16,30'
        ]);

        if ($validator->fails()) {
            return response()->json([
                'status' => 'error',
                'message' => 'Invalid automation data',
                'errors' => $validator->errors()
            ], 400);
        }

        $action = $request->action;
        $response = ['status' => 'success'];

        switch ($action) {
            case 'toggle':
                $enabled = Cache::get('ac_automation_enabled', true);
                Cache::put('ac_automation_enabled', !$enabled, 3600);
                $response['message'] = 'AC automation ' . (!$enabled ? 'enabled' : 'disabled');
                $response['enabled'] = !$enabled;
                break;

            case 'force_check':
                $this->performACAutomationCheck();
                $response['message'] = 'AC automation check performed';
                break;

            case 'set_temp':
                if ($request->temperature) {
                    Cache::put('manual_ac_temp', $request->temperature, 3600);
                    $response['message'] = "AC temperature set to {$request->temperature}°C";
                    $response['temperature'] = $request->temperature;
                }
                break;

            case 'start_gradual':
                Cache::put('ac_gradual_mode', true, 3600);
                Cache::put('ac_gradual_start_time', now(), 3600);
                $response['message'] = 'AC gradual mode started';
                break;

            case 'stop_gradual':
                Cache::forget('ac_gradual_mode');
                Cache::forget('ac_gradual_start_time');
                $response['message'] = 'AC gradual mode stopped';
                break;
        }

        return response()->json($response);
    }

    /**
     * Get system status
     */
    public function getSystemStatus()
    {
        $sensorData = Cache::get('latest_sensor_data', []);
        $devices = Device::all();
        
        return response()->json([
            'status' => 'online',
            'uptime' => now()->diffInSeconds(Cache::get('system_start_time', now())),
            'sensor_data' => $sensorData,
            'devices' => $devices->mapWithKeys(function ($device) {
                return [$device->type => [
                    'name' => $device->name,
                    'active' => $device->is_active,
                    'power' => $device->power_consumption,
                    'energy_today' => $device->total_energy_today
                ]];
            }),
            'working_hours' => $this->getWorkingHoursStatus(),
            'energy_saving' => $this->getEnergySavingStatus($sensorData),
            'timestamp' => now()->toISOString()
        ]);
    }

    private function storeSensorReading($type, $value, $unit, $timestamp)
    {
        SensorReading::create([
            'sensor_type' => $type,
            'sensor_name' => $type . '_sensor',
            'value' => $value,
            'unit' => $unit,
            'location' => 'Lab Teknik Tegangan Tinggi',
            'reading_time' => $timestamp
        ]);
    }

    private function getACAutomationStatus($sensorData)
    {
        $enabled = Cache::get('ac_automation_enabled', true);
        $gradualMode = Cache::get('ac_gradual_mode', false);
        
        $activeRule = ACAutomationRule::forCondition(
            $sensorData['people_count'],
            $sensorData['temperature']
        )->first();

        $status = [
            'enabled' => $enabled,
            'gradual_mode' => [
                'active' => $gradualMode
            ],
            'current_rule' => null
        ];

        if ($gradualMode) {
            $startTime = Cache::get('ac_gradual_start_time', now());
            $elapsed = now()->diffInMinutes($startTime);
            $progress = min(100, ($elapsed / 15) * 100); // 15 minutes total

            $status['gradual_mode'] = [
                'active' => true,
                'progress_percent' => $progress,
                'elapsed_minutes' => $elapsed,
                'remaining_minutes' => max(0, 15 - $elapsed),
                'target_temp' => 28
            ];
        }

        if ($activeRule) {
            $status['current_rule'] = [
                'description' => $activeRule->description,
                'ac_units' => $activeRule->ac_units_needed,
                'target_temp' => $activeRule->target_temperature
            ];
        }

        return $status;
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
            'enabled' => $workingDaysEnabled,
            'start_hour' => $startHour,
            'end_hour' => $endHour,
            'is_working_hours' => $isWorkingDay && $isWorkingHours,
            'is_working_day' => $isWorkingDay,
            'current_time' => [
                'hour' => $now->hour,
                'minute' => $now->minute,
                'formatted' => $now->format('H:i'),
                'day_name' => $now->format('l')
            ],
            'status' => $isWorkingDay && $isWorkingHours ? 'Jam Kerja Aktif' : 
                       (!$isWorkingDay ? 'Hari Libur' : 'Di Luar Jam Kerja')
        ];
    }

    private function getEnergySavingStatus($sensorData)
    {
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
            'enabled' => true,
            'current_state' => $currentState,
            'minutes_without_people' => $minutesWithoutPeople,
            'time_to_energy_saving' => max(0, $energySavingDelay - $minutesWithoutPeople),
            'time_to_shutdown' => max(0, $panelShutdownDelay - $minutesWithoutPeople),
            'energy_saving_temp' => config('energy.energy_saving_temp', 28)
        ];
    }

    private function performACAutomationCheck()
    {
        $sensorData = Cache::get('latest_sensor_data', []);
        
        if (empty($sensorData)) {
            return;
        }

        $activeRule = ACAutomationRule::forCondition(
            $sensorData['people_count'],
            $sensorData['temperature']
        )->first();

        if ($activeRule) {
            // Apply the rule
            $ac1Device = Device::where('type', 'ac1')->first();
            $ac2Device = Device::where('type', 'ac2')->first();

            if ($ac1Device) {
                $needAC1 = $activeRule->ac_units_needed >= 1;
                $ac1Device->update(['is_active' => $needAC1]);
                $this->arduinoService->sendCommand('ac1', $needAC1 ? 'on' : 'off');
            }

            if ($ac2Device) {
                $needAC2 = $activeRule->ac_units_needed >= 2;
                $ac2Device->update(['is_active' => $needAC2]);
                $this->arduinoService->sendCommand('ac2', $needAC2 ? 'on' : 'off');
            }

            // Set temperature
            if ($activeRule->ac_units_needed > 0) {
                $this->arduinoService->sendTemperatureCommand($activeRule->target_temperature);
            }
        }
    }

    private function logEnergyUsage($device)
    {
        $startTime = Cache::get("device_{$device->id}_start_time");
        
        if ($startTime) {
            $duration = now()->diffInMinutes($startTime);
            $energyConsumed = ($device->power_consumption * $duration) / 60; // Wh

            EnergyLog::create([
                'device_id' => $device->id,
                'energy_consumed' => $energyConsumed,
                'duration_minutes' => $duration,
                'start_time' => $startTime,
                'end_time' => now(),
                'working_hours' => $this->isWorkingHours()
            ]);

            Cache::forget("device_{$device->id}_start_time");
        }
    }

    private function isWorkingHours()
    {
        $now = now();
        $startHour = config('energy.working_start_hour', 7);
        $endHour = config('energy.working_end_hour', 17);
        $workingDaysEnabled = config('energy.working_days_enabled', true);

        $isWorkingDay = !$workingDaysEnabled || $now->isWeekday();
        return $isWorkingDay && $now->hour >= $startHour && $now->hour < $endHour;
    }
}