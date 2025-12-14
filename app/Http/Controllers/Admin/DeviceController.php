<?php

namespace App\Http\Controllers\Admin;

use App\Http\Controllers\Controller;
use App\Models\Device;
use App\Services\ArduinoService;
use Illuminate\Http\Request;

class DeviceController extends Controller
{
    protected $arduinoService;

    public function __construct(ArduinoService $arduinoService)
    {
        $this->arduinoService = $arduinoService;
    }

    public function index()
    {
        $devices = Device::with(['energyLogs' => function($query) {
            $query->today();
        }])->get();

        $totalPowerConsumption = $devices->sum('energy_consumption');
        $activeDevices = $devices->where('is_active', true)->count();

        return view('admin.devices.index', compact('devices', 'totalPowerConsumption', 'activeDevices'));
    }

    public function create()
    {
        return view('admin.devices.create');
    }

    public function store(Request $request)
    {
        $request->validate([
            'name' => 'required|string|max:255',
            'type' => 'required|string|max:50',
            'power_consumption' => 'required|numeric|min:0',
            'units' => 'required|integer|min:1',
            'location' => 'nullable|string|max:255',
            'description' => 'nullable|string',
            'is_always_on' => 'boolean',
            'gpio_pin' => 'nullable|integer|between:0,39',
            'relay_pin' => 'nullable|integer|between:0,39'
        ]);

        Device::create([
            'name' => $request->name,
            'type' => $request->type,
            'power_consumption' => $request->power_consumption,
            'units' => $request->units,
            'location' => $request->location ?? 'Lab Teknik Tegangan Tinggi',
            'description' => $request->description,
            'is_active' => false,
            'is_always_on' => $request->boolean('is_always_on'),
            'gpio_pin' => $request->gpio_pin,
            'relay_pin' => $request->relay_pin
        ]);

        return redirect()->route('admin.devices.index')
            ->with('success', 'Device berhasil ditambahkan.');
    }

    public function show(Device $device)
    {
        $device->load(['energyLogs' => function($query) {
            $query->latest()->limit(20);
        }, 'sensorReadings' => function($query) {
            $query->latest()->limit(10);
        }]);

        // Calculate statistics
        $stats = [
            'energy_today' => $device->energyLogs()->today()->sum('energy_consumed'),
            'energy_week' => $device->energyLogs()->thisWeek()->sum('energy_consumed'),
            'energy_month' => $device->energyLogs()->thisMonth()->sum('energy_consumed'),
            'avg_daily_usage' => $device->energyLogs()->thisMonth()->avg('energy_consumed'),
            'total_runtime_today' => $device->energyLogs()->today()->sum('duration_minutes'),
            'efficiency_rating' => $this->calculateEfficiencyRating($device)
        ];

        return view('admin.devices.show', compact('device', 'stats'));
    }

    public function edit(Device $device)
    {
        return view('admin.devices.edit', compact('device'));
    }

    public function update(Request $request, Device $device)
    {
        $request->validate([
            'name' => 'required|string|max:255',
            'type' => 'required|string|max:50',
            'power_consumption' => 'required|numeric|min:0',
            'units' => 'required|integer|min:1',
            'location' => 'nullable|string|max:255',
            'description' => 'nullable|string',
            'is_always_on' => 'boolean',
            'gpio_pin' => 'nullable|integer|between:0,39',
            'relay_pin' => 'nullable|integer|between:0,39'
        ]);

        $device->update([
            'name' => $request->name,
            'type' => $request->type,
            'power_consumption' => $request->power_consumption,
            'units' => $request->units,
            'location' => $request->location,
            'description' => $request->description,
            'is_always_on' => $request->boolean('is_always_on'),
            'gpio_pin' => $request->gpio_pin,
            'relay_pin' => $request->relay_pin
        ]);

        return redirect()->route('admin.devices.index')
            ->with('success', 'Device berhasil diperbarui.');
    }

    public function destroy(Device $device)
    {
        // Prevent deleting always-on devices
        if ($device->is_always_on) {
            return redirect()->back()
                ->with('error', 'Tidak dapat menghapus device yang selalu aktif.');
        }

        // Turn off device before deleting
        if ($device->is_active) {
            $this->arduinoService->sendCommand($device->type, 'off');
        }

        $device->delete();

        return redirect()->route('admin.devices.index')
            ->with('success', 'Device berhasil dihapus.');
    }

    public function toggle(Device $device)
    {
        try {
            $newState = !$device->is_active;
            
            // Send command to Arduino
            $result = $this->arduinoService->sendCommand(
                $device->type,
                $newState ? 'on' : 'off'
            );

            if ($result['status'] === 'success') {
                $device->update(['is_active' => $newState]);
                
                return response()->json([
                    'status' => 'success',
                    'message' => "Device {$device->name} berhasil " . ($newState ? 'dinyalakan' : 'dimatikan'),
                    'is_active' => $newState,
                    'arduino_response' => $result
                ]);
            } else {
                return response()->json([
                    'status' => 'error',
                    'message' => 'Gagal mengontrol device: ' . $result['message']
                ], 500);
            }

        } catch (\Exception $e) {
            return response()->json([
                'status' => 'error',
                'message' => 'Error: ' . $e->getMessage()
            ], 500);
        }
    }

    public function bulkAction(Request $request)
    {
        $request->validate([
            'action' => 'required|in:turn_on,turn_off,toggle,delete',
            'device_ids' => 'required|array',
            'device_ids.*' => 'exists:devices,id'
        ]);

        $devices = Device::whereIn('id', $request->device_ids)->get();
        $results = [];

        foreach ($devices as $device) {
            try {
                switch ($request->action) {
                    case 'turn_on':
                        $result = $this->arduinoService->sendCommand($device->type, 'on');
                        if ($result['status'] === 'success') {
                            $device->update(['is_active' => true]);
                        }
                        break;

                    case 'turn_off':
                        if (!$device->is_always_on) {
                            $result = $this->arduinoService->sendCommand($device->type, 'off');
                            if ($result['status'] === 'success') {
                                $device->update(['is_active' => false]);
                            }
                        }
                        break;

                    case 'toggle':
                        if (!$device->is_always_on) {
                            $newState = !$device->is_active;
                            $result = $this->arduinoService->sendCommand(
                                $device->type,
                                $newState ? 'on' : 'off'
                            );
                            if ($result['status'] === 'success') {
                                $device->update(['is_active' => $newState]);
                            }
                        }
                        break;

                    case 'delete':
                        if (!$device->is_always_on) {
                            if ($device->is_active) {
                                $this->arduinoService->sendCommand($device->type, 'off');
                            }
                            $device->delete();
                        }
                        break;
                }

                $results[] = [
                    'device' => $device->name,
                    'status' => 'success'
                ];

            } catch (\Exception $e) {
                $results[] = [
                    'device' => $device->name,
                    'status' => 'error',
                    'message' => $e->getMessage()
                ];
            }
        }

        return response()->json([
            'status' => 'success',
            'message' => 'Bulk action berhasil dieksekusi',
            'results' => $results
        ]);
    }

    private function calculateEfficiencyRating(Device $device)
    {
        $monthlyLogs = $device->energyLogs()->thisMonth()->get();
        
        if ($monthlyLogs->isEmpty()) {
            return 'N/A';
        }

        $totalEnergy = $monthlyLogs->sum('energy_consumed');
        $totalPeople = $monthlyLogs->where('people_count', '>', 0)->sum('people_count');
        
        if ($totalPeople == 0) {
            return 'N/A';
        }

        $energyPerPerson = $totalEnergy / $totalPeople;

        if ($energyPerPerson < 50) return 'Excellent';
        if ($energyPerPerson < 100) return 'Good';
        if ($energyPerPerson < 150) return 'Average';
        return 'Poor';
    }
}