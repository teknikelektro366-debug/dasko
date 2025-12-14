<?php

namespace App\Services;

use Illuminate\Support\Facades\Http;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Cache;

class ArduinoService
{
    protected $baseUrl;
    protected $username;
    protected $password;
    protected $timeout;

    public function __construct()
    {
        $this->baseUrl = 'http://' . config('arduino.ip', '192.168.1.100');
        $this->username = config('arduino.username', 'admin');
        $this->password = config('arduino.password', 'elektro2024');
        $this->timeout = config('arduino.timeout', 5);
    }

    /**
     * Test connection to Arduino
     */
    public function testConnection()
    {
        try {
            $response = Http::timeout($this->timeout)
                ->get($this->baseUrl . '/api/status');

            if ($response->successful()) {
                Cache::put('arduino_connection_status', 'connected', 300);
                return [
                    'status' => 'connected',
                    'message' => 'Arduino connection successful',
                    'response_time' => $response->transferStats->getTransferTime()
                ];
            }

            Cache::put('arduino_connection_status', 'error', 300);
            return [
                'status' => 'error',
                'message' => 'Arduino responded with error: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Cache::put('arduino_connection_status', 'disconnected', 300);
            Log::error('Arduino connection failed: ' . $e->getMessage());
            
            return [
                'status' => 'disconnected',
                'message' => 'Failed to connect to Arduino: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Get connection status from cache
     */
    public function getConnectionStatus()
    {
        return Cache::get('arduino_connection_status', 'unknown');
    }

    /**
     * Send device control command to Arduino
     */
    public function sendCommand($device, $action)
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->post($this->baseUrl . '/api/control', [
                    'device' => $device,
                    'action' => $action
                ]);

            if ($response->successful()) {
                Log::info("Arduino command sent: {$device} -> {$action}");
                return [
                    'status' => 'success',
                    'message' => "Command sent: {$device} {$action}",
                    'response' => $response->json()
                ];
            }

            Log::error("Arduino command failed: {$device} -> {$action}, Status: " . $response->status());
            return [
                'status' => 'error',
                'message' => 'Command failed with status: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino command exception: {$device} -> {$action}, Error: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'Connection error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Send IR command to Arduino
     */
    public function sendIRCommand($device, $command)
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->post($this->baseUrl . '/api/ir', [
                    'device' => $device,
                    'command' => $command
                ]);

            if ($response->successful()) {
                Log::info("Arduino IR command sent: {$device} -> {$command}");
                return [
                    'status' => 'success',
                    'message' => "IR command sent: {$device} {$command}",
                    'response' => $response->json()
                ];
            }

            return [
                'status' => 'error',
                'message' => 'IR command failed with status: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino IR command exception: {$device} -> {$command}, Error: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'IR command error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Send temperature command to AC
     */
    public function sendTemperatureCommand($temperature)
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->post($this->baseUrl . '/api/ac-auto', [
                    'action' => 'set_temp',
                    'temperature' => $temperature
                ]);

            if ($response->successful()) {
                Log::info("Arduino temperature command sent: {$temperature}°C");
                return [
                    'status' => 'success',
                    'message' => "Temperature set to {$temperature}°C",
                    'response' => $response->json()
                ];
            }

            return [
                'status' => 'error',
                'message' => 'Temperature command failed with status: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino temperature command exception: {$temperature}°C, Error: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'Temperature command error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Get sensor data from Arduino
     */
    public function getSensorData()
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->get($this->baseUrl . '/api/sensors');

            if ($response->successful()) {
                $data = $response->json();
                
                // Cache the data for 30 seconds
                Cache::put('arduino_sensor_data', $data, 30);
                
                return [
                    'status' => 'success',
                    'data' => $data
                ];
            }

            return [
                'status' => 'error',
                'message' => 'Failed to get sensor data, status: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino sensor data exception: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'Sensor data error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Get AC automation status from Arduino
     */
    public function getACAutomationStatus()
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->get($this->baseUrl . '/api/ac-auto');

            if ($response->successful()) {
                return [
                    'status' => 'success',
                    'data' => $response->json()
                ];
            }

            return [
                'status' => 'error',
                'message' => 'Failed to get AC automation status: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino AC automation status exception: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'AC automation status error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Send working hours configuration to Arduino
     */
    public function updateWorkingHours($startHour, $endHour, $enabled)
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->post($this->baseUrl . '/api/working-hours', [
                    'action' => 'set_working_hours',
                    'start_hour' => $startHour,
                    'end_hour' => $endHour,
                    'enabled' => $enabled
                ]);

            if ($response->successful()) {
                Log::info("Arduino working hours updated: {$startHour}:00 - {$endHour}:00, Enabled: " . ($enabled ? 'Yes' : 'No'));
                return [
                    'status' => 'success',
                    'message' => 'Working hours updated successfully',
                    'response' => $response->json()
                ];
            }

            return [
                'status' => 'error',
                'message' => 'Failed to update working hours: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino working hours update exception: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'Working hours update error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Send energy saving configuration to Arduino
     */
    public function updateEnergySaving($enabled, $energySavingDelay, $shutdownDelay)
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->post($this->baseUrl . '/api/energy-saving', [
                    'action' => $enabled ? 'enable' : 'disable',
                    'energy_saving_delay' => $energySavingDelay,
                    'shutdown_delay' => $shutdownDelay
                ]);

            if ($response->successful()) {
                Log::info("Arduino energy saving updated: Enabled: " . ($enabled ? 'Yes' : 'No'));
                return [
                    'status' => 'success',
                    'message' => 'Energy saving settings updated successfully',
                    'response' => $response->json()
                ];
            }

            return [
                'status' => 'error',
                'message' => 'Failed to update energy saving settings: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino energy saving update exception: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'Energy saving update error: ' . $e->getMessage()
            ];
        }
    }

    /**
     * Perform emergency shutdown via Arduino
     */
    public function emergencyShutdown()
    {
        try {
            $response = Http::timeout($this->timeout)
                ->withBasicAuth($this->username, $this->password)
                ->post($this->baseUrl . '/api/energy-saving', [
                    'action' => 'emergency_shutdown'
                ]);

            if ($response->successful()) {
                Log::warning("Arduino emergency shutdown executed");
                return [
                    'status' => 'success',
                    'message' => 'Emergency shutdown executed successfully',
                    'response' => $response->json()
                ];
            }

            return [
                'status' => 'error',
                'message' => 'Failed to execute emergency shutdown: ' . $response->status()
            ];

        } catch (\Exception $e) {
            Log::error("Arduino emergency shutdown exception: " . $e->getMessage());
            return [
                'status' => 'error',
                'message' => 'Emergency shutdown error: ' . $e->getMessage()
            ];
        }
    }
}