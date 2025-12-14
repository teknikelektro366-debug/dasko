<?php

return [
    /*
    |--------------------------------------------------------------------------
    | Energy Management Configuration
    |--------------------------------------------------------------------------
    |
    | Configuration for smart energy management system
    |
    */

    /*
    |--------------------------------------------------------------------------
    | Working Hours Configuration
    |--------------------------------------------------------------------------
    */
    
    'working_start_hour' => env('WORKING_START_HOUR', 7),
    'working_end_hour' => env('WORKING_END_HOUR', 17),
    'working_days_enabled' => env('WORKING_DAYS_ENABLED', true),
    
    /*
    |--------------------------------------------------------------------------
    | Energy Saving Configuration
    |--------------------------------------------------------------------------
    */
    
    'energy_saving_delay' => env('ENERGY_SAVING_DELAY', 15), // minutes
    'panel_shutdown_delay' => env('PANEL_SHUTDOWN_DELAY', 30), // minutes
    'energy_saving_temp' => env('ENERGY_SAVING_TEMP', 28), // °C
    
    /*
    |--------------------------------------------------------------------------
    | AC Automation Configuration
    |--------------------------------------------------------------------------
    */
    
    'ac_auto_enabled' => env('AC_AUTO_ENABLED', true),
    'ac_gradual_mode_enabled' => env('AC_GRADUAL_MODE_ENABLED', true),
    'ac_gradual_temp_step' => env('AC_GRADUAL_TEMP_STEP', 1), // °C per step
    'ac_gradual_time_step' => env('AC_GRADUAL_TIME_STEP', 5), // minutes per step
    
    /*
    |--------------------------------------------------------------------------
    | Light Automation Configuration
    |--------------------------------------------------------------------------
    */
    
    'light_auto_enabled' => env('LIGHT_AUTO_ENABLED', true),
    'light_threshold_lux' => env('LIGHT_THRESHOLD_LUX', 300), // lux
    'light_delay_minutes' => env('LIGHT_DELAY_MINUTES', 2), // minutes
    
    /*
    |--------------------------------------------------------------------------
    | Cost Calculation
    |--------------------------------------------------------------------------
    */
    
    'electricity_rate_per_kwh' => env('ELECTRICITY_RATE_PER_KWH', 1467), // IDR per kWh
    'peak_hour_multiplier' => env('PEAK_HOUR_MULTIPLIER', 1.2),
    'peak_hours' => [
        'start' => env('PEAK_HOUR_START', 17),
        'end' => env('PEAK_HOUR_END', 22),
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Alert Thresholds
    |--------------------------------------------------------------------------
    */
    
    'daily_energy_threshold' => env('DAILY_ENERGY_THRESHOLD', 50000), // Wh
    'hourly_energy_threshold' => env('HOURLY_ENERGY_THRESHOLD', 5000), // Wh
    'temperature_alert_threshold' => env('TEMP_ALERT_THRESHOLD', 35), // °C
    'humidity_alert_threshold' => env('HUMIDITY_ALERT_THRESHOLD', 80), // %
    
    /*
    |--------------------------------------------------------------------------
    | Device Power Ratings (Watts)
    |--------------------------------------------------------------------------
    */
    
    'device_power_ratings' => [
        'ac' => 1050,
        'lamp' => 22,
        'computer' => 400,
        'tv' => 150,
        'dispenser' => 350,
        'kettle' => 1200,
        'coffee' => 800,
        'fridge' => 120,
        'router' => 15,
        'panel' => 300,
    ],
];