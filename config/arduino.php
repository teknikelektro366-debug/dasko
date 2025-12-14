<?php

return [
    /*
    |--------------------------------------------------------------------------
    | Arduino Configuration
    |--------------------------------------------------------------------------
    |
    | Configuration for Arduino ESP32 communication
    |
    */

    'ip' => env('ARDUINO_IP', '192.168.1.100'),
    'username' => env('ARDUINO_USERNAME', 'admin'),
    'password' => env('ARDUINO_PASSWORD', 'elektro2024'),
    'timeout' => env('ARDUINO_TIMEOUT', 5),
    
    /*
    |--------------------------------------------------------------------------
    | Device GPIO Mapping
    |--------------------------------------------------------------------------
    |
    | GPIO pin mapping for each device type
    |
    */
    
    'gpio_mapping' => [
        'ac1' => ['gpio' => 2, 'relay' => 3],
        'ac2' => ['gpio' => 4, 'relay' => 5],
        'lamp1' => ['gpio' => 6, 'relay' => 7],
        'lamp2' => ['gpio' => 8, 'relay' => 9],
        'computer' => ['gpio' => 10, 'relay' => 11],
        'tv' => ['gpio' => 12, 'relay' => 13],
        'dispenser' => ['gpio' => 14, 'relay' => 15],
        'kettle' => ['gpio' => 16, 'relay' => 17],
        'coffee' => ['gpio' => 18, 'relay' => 19],
        'panel' => ['gpio' => 20, 'relay' => 21],
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Sensor Configuration
    |--------------------------------------------------------------------------
    |
    | Sensor pin mapping and configuration
    |
    */
    
    'sensors' => [
        'temperature_humidity' => ['pin' => 22, 'type' => 'DHT22'],
        'light' => ['pin' => 23, 'type' => 'LDR'],
        'people_counter' => ['pin' => 24, 'type' => 'PIR'],
        'proximity' => ['pin' => 25, 'type' => 'HC-SR04'],
    ],
    
    /*
    |--------------------------------------------------------------------------
    | IR Remote Codes
    |--------------------------------------------------------------------------
    |
    | IR remote control codes for AC units
    |
    */
    
    'ir_codes' => [
        'ac_power_on' => '0x20DF10EF',
        'ac_power_off' => '0x20DF906F',
        'ac_temp_up' => '0x20DF40BF',
        'ac_temp_down' => '0x20DF00FF',
        'ac_mode_cool' => '0x20DFC03F',
        'ac_mode_fan' => '0x20DF20DF',
        'ac_fan_speed' => '0x20DFA05F',
    ],
];