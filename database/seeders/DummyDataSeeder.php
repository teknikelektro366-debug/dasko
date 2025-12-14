<?php

namespace Database\Seeders;

use Illuminate\Database\Seeder;
use App\Models\Device;
use App\Models\SensorReading;
use App\Models\EnergyLog;
use App\Models\ACAutomationRule;
use Carbon\Carbon;

class DummyDataSeeder extends Seeder
{
    public function run()
    {
        // Create devices based on index.html
        $devices = [
            [
                'name' => 'AC Panasonic Unit 1',
                'type' => Device::TYPE_AC,
                'power_consumption' => 1050.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 2,
                'relay_pin' => 3,
                'description' => 'AC Panasonic CS-PN12WKJ Unit 1'
            ],
            [
                'name' => 'AC Panasonic Unit 2',
                'type' => Device::TYPE_AC,
                'power_consumption' => 1050.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 4,
                'relay_pin' => 5,
                'description' => 'AC Panasonic CS-PN12WKJ Unit 2'
            ],
            [
                'name' => 'Lampu TL Jalur 1',
                'type' => Device::TYPE_LAMP,
                'power_consumption' => 132.00, // 22W x 6 units
                'units' => 6,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => true,
                'is_always_on' => false,
                'gpio_pin' => 6,
                'relay_pin' => 7,
                'description' => 'Lampu TL 22W - Jalur 1 (6 Unit)'
            ],
            [
                'name' => 'Lampu TL Jalur 2',
                'type' => Device::TYPE_LAMP,
                'power_consumption' => 132.00, // 22W x 6 units
                'units' => 6,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => true,
                'is_always_on' => false,
                'gpio_pin' => 8,
                'relay_pin' => 9,
                'description' => 'Lampu TL 22W - Jalur 2 (6 Unit)'
            ],
            [
                'name' => 'Komputer Lab',
                'type' => Device::TYPE_COMPUTER,
                'power_consumption' => 400.00,
                'units' => 2,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 10,
                'relay_pin' => 11,
                'description' => 'Komputer Lab (2 Unit)'
            ],
            [
                'name' => 'Smart TV Horizon',
                'type' => Device::TYPE_TV,
                'power_consumption' => 150.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 12,
                'relay_pin' => 13,
                'description' => 'Smart TV Horizon untuk presentasi'
            ],
            [
                'name' => 'Dispenser Sanken',
                'type' => Device::TYPE_DISPENSER,
                'power_consumption' => 350.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 14,
                'relay_pin' => 15,
                'description' => 'Dispenser air panas/dingin'
            ],
            [
                'name' => 'Teko Listrik',
                'type' => Device::TYPE_KETTLE,
                'power_consumption' => 1200.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 16,
                'relay_pin' => 17,
                'description' => 'Teko listrik untuk air panas'
            ],
            [
                'name' => 'Mesin Kopi',
                'type' => Device::TYPE_COFFEE,
                'power_consumption' => 800.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => false,
                'is_always_on' => false,
                'gpio_pin' => 18,
                'relay_pin' => 19,
                'description' => 'Mesin kopi otomatis'
            ],
            [
                'name' => 'Kulkas Sharp',
                'type' => Device::TYPE_FRIDGE,
                'power_consumption' => 120.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => true,
                'is_always_on' => true,
                'gpio_pin' => null,
                'relay_pin' => null,
                'description' => 'Kulkas Sharp - Always On'
            ],
            [
                'name' => 'Router Wi-Fi',
                'type' => Device::TYPE_ROUTER,
                'power_consumption' => 15.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => true,
                'is_always_on' => true,
                'gpio_pin' => null,
                'relay_pin' => null,
                'description' => 'Router Wi-Fi - Always On'
            ],
            [
                'name' => 'Panel Listrik',
                'type' => Device::TYPE_PANEL,
                'power_consumption' => 300.00,
                'units' => 1,
                'location' => 'Lab Teknik Tegangan Tinggi',
                'is_active' => true,
                'is_always_on' => false,
                'gpio_pin' => 20,
                'relay_pin' => 21,
                'description' => 'Panel listrik utama'
            ]
        ];

        foreach ($devices as $deviceData) {
            Device::create($deviceData);
        }

        // Create AC Automation Rules based on index.html
        $acRules = [
            [
                'min_people' => 0,
                'max_people' => 0,
                'min_outdoor_temp' => 0.0,
                'max_outdoor_temp' => 50.0,
                'ac_units_needed' => 0,
                'target_temperature' => 28,
                'description' => 'Tidak ada orang → Mode Gradual (naikkan suhu bertahap ke 28°C, lalu mati)',
                'is_active' => true
            ],
            [
                'min_people' => 1,
                'max_people' => 4,
                'min_outdoor_temp' => 0.0,
                'max_outdoor_temp' => 50.0,
                'ac_units_needed' => 0,
                'target_temperature' => 0,
                'description' => 'Di bawah threshold → AC tidak perlu menyala',
                'is_active' => true
            ],
            [
                'min_people' => 5,
                'max_people' => 10,
                'min_outdoor_temp' => 30.0,
                'max_outdoor_temp' => 32.0,
                'ac_units_needed' => 1,
                'target_temperature' => 25,
                'description' => 'Beban ringan → hanya satu AC',
                'is_active' => true
            ],
            [
                'min_people' => 5,
                'max_people' => 10,
                'min_outdoor_temp' => 32.1,
                'max_outdoor_temp' => 35.0,
                'ac_units_needed' => 1,
                'target_temperature' => 26,
                'description' => 'Suhu luar tinggi → setpoint lebih longgar',
                'is_active' => true
            ],
            [
                'min_people' => 10,
                'max_people' => 13,
                'min_outdoor_temp' => 30.0,
                'max_outdoor_temp' => 32.0,
                'ac_units_needed' => 2,
                'target_temperature' => 24,
                'description' => 'Beban meningkat → aktifkan kedua AC',
                'is_active' => true
            ],
            [
                'min_people' => 10,
                'max_people' => 20,
                'min_outdoor_temp' => 32.1,
                'max_outdoor_temp' => 35.0,
                'ac_units_needed' => 2,
                'target_temperature' => 25,
                'description' => 'Kondisi luar panas → pendinginan lebih ringan',
                'is_active' => true
            ],
            [
                'min_people' => 21,
                'max_people' => 50,
                'min_outdoor_temp' => 0.0,
                'max_outdoor_temp' => 50.0,
                'ac_units_needed' => 2,
                'target_temperature' => 23,
                'description' => 'Beban maksimal → pendinginan agresif',
                'is_active' => true
            ]
        ];

        foreach ($acRules as $ruleData) {
            ACAutomationRule::create($ruleData);
        }

        // Create dummy sensor readings for today
        $now = Carbon::now();
        $devices = Device::all();

        // Create sensor readings for the last 24 hours
        for ($i = 0; $i < 24; $i++) {
            $time = $now->copy()->subHours($i);
            
            // Temperature sensor
            SensorReading::create([
                'device_id' => null,
                'sensor_type' => SensorReading::TYPE_TEMPERATURE,
                'sensor_name' => 'DHT22_Outdoor',
                'value' => rand(270, 350) / 10, // 27.0 - 35.0°C
                'unit' => '°C',
                'location' => 'Lab Teknik Tegangan Tinggi',
                'gpio_pin' => 22,
                'reading_time' => $time,
                'created_at' => $time,
                'updated_at' => $time
            ]);

            // Humidity sensor
            SensorReading::create([
                'device_id' => null,
                'sensor_type' => SensorReading::TYPE_HUMIDITY,
                'sensor_name' => 'DHT22_Humidity',
                'value' => rand(500, 800) / 10, // 50.0 - 80.0%
                'unit' => '%',
                'location' => 'Lab Teknik Tegangan Tinggi',
                'gpio_pin' => 22,
                'reading_time' => $time,
                'created_at' => $time,
                'updated_at' => $time
            ]);

            // Light sensor
            SensorReading::create([
                'device_id' => null,
                'sensor_type' => SensorReading::TYPE_LIGHT,
                'sensor_name' => 'LDR_Sensor',
                'value' => rand(100, 1000), // 100 - 1000 lux
                'unit' => 'lux',
                'location' => 'Lab Teknik Tegangan Tinggi',
                'gpio_pin' => 23,
                'reading_time' => $time,
                'created_at' => $time,
                'updated_at' => $time
            ]);

            // People count sensor (working hours simulation)
            $isWorkingHours = $time->hour >= 7 && $time->hour <= 17 && $time->isWeekday();
            $peopleCount = $isWorkingHours ? rand(0, 15) : 0;
            
            SensorReading::create([
                'device_id' => null,
                'sensor_type' => SensorReading::TYPE_PEOPLE_COUNT,
                'sensor_name' => 'PIR_Counter',
                'value' => $peopleCount,
                'unit' => 'people',
                'location' => 'Lab Teknik Tegangan Tinggi',
                'gpio_pin' => 24,
                'reading_time' => $time,
                'created_at' => $time,
                'updated_at' => $time
            ]);
        }

        // Create dummy energy logs for active devices
        $activeDevices = Device::where('is_active', true)->get();
        
        foreach ($activeDevices as $device) {
            for ($i = 0; $i < 10; $i++) {
                $startTime = $now->copy()->subHours(rand(1, 24));
                $duration = rand(30, 180); // 30-180 minutes
                $endTime = $startTime->copy()->addMinutes($duration);
                
                $energyConsumed = ($device->power_consumption * $device->units * $duration) / 60; // Wh
                
                EnergyLog::create([
                    'device_id' => $device->id,
                    'energy_consumed' => $energyConsumed,
                    'duration_minutes' => $duration,
                    'start_time' => $startTime,
                    'end_time' => $endTime,
                    'people_count' => rand(0, 10),
                    'outdoor_temperature' => rand(270, 350) / 10,
                    'ac_target_temperature' => $device->type === Device::TYPE_AC ? rand(23, 26) : null,
                    'energy_saving_mode' => rand(0, 1),
                    'working_hours' => $startTime->hour >= 7 && $startTime->hour <= 17 && $startTime->isWeekday(),
                    'cost_estimate' => $energyConsumed * 0.001467, // IDR per Wh
                    'created_at' => $startTime,
                    'updated_at' => $startTime
                ]);
            }
        }

        $this->command->info('Dummy data seeded successfully!');
    }
}