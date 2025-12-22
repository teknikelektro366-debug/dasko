<?php

return [
    /*
    |--------------------------------------------------------------------------
    | Production Hosting Configuration
    |--------------------------------------------------------------------------
    |
    | Configuration for production hosting environment
    | Domain: https://dasko.fst.unja.ac.id/
    |
    */

    'domain' => env('HOSTING_DOMAIN', 'dasko.fst.unja.ac.id'),
    'url' => env('HOSTING_URL', 'https://dasko.fst.unja.ac.id'),
    'ssl_enabled' => env('HOSTING_SSL_ENABLED', true),
    
    /*
    |--------------------------------------------------------------------------
    | API Configuration
    |--------------------------------------------------------------------------
    */
    
    'api' => [
        'base_url' => env('HOSTING_URL', 'https://dasko.fst.unja.ac.id') . '/api',
        'sensor_endpoint' => '/sensor/data',
        'ac_control_endpoint' => '/ac/control',
        'timeout' => 30, // seconds
        'retry_attempts' => 3,
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Security Configuration
    |--------------------------------------------------------------------------
    */
    
    'security' => [
        'allowed_devices' => [
            'ESP32_Smart_Energy_Production',
            'ESP32_Smart_Energy_ChangeDetection',
            'ESP32_Smart_Energy',
        ],
        'allowed_locations' => [
            'Lab Teknik Tegangan Tinggi',
            'Ruang Dosen Prodi Teknik Elektro',
        ],
        'rate_limit' => [
            'requests_per_minute' => 60,
            'requests_per_hour' => 1000,
        ],
        'require_https' => true,
        'verify_ssl' => true,
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Database Configuration
    |--------------------------------------------------------------------------
    */
    
    'database' => [
        'cleanup_days' => 90, // Keep data for 90 days
        'backup_enabled' => true,
        'backup_frequency' => 'daily',
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Monitoring Configuration
    |--------------------------------------------------------------------------
    */
    
    'monitoring' => [
        'health_check_enabled' => true,
        'health_check_interval' => 300, // 5 minutes
        'alert_email' => env('MONITORING_EMAIL', 'admin@fst.unja.ac.id'),
        'log_level' => env('LOG_LEVEL', 'info'),
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Arduino Configuration
    |--------------------------------------------------------------------------
    */
    
    'arduino' => [
        'connection_timeout' => 30, // seconds
        'data_timeout' => 600, // 10 minutes - consider device offline
        'force_update_interval' => 300, // 5 minutes
        'change_detection' => [
            'temperature_threshold' => 0.5,
            'humidity_threshold' => 2.0,
            'light_threshold' => 50,
        ],
    ],
    
    /*
    |--------------------------------------------------------------------------
    | AC Control Configuration
    |--------------------------------------------------------------------------
    */
    
    'ac_control' => [
        'default_mode' => 'auto',
        'manual_timeout' => 240, // 4 hours default
        'temperature_limits' => [
            'min' => 16,
            'max' => 30,
        ],
        'auto_rules' => [
            ['people' => 0, 'ac1' => false, 'ac2' => false, 'temp' => 0],
            ['people' => 5, 'ac1' => true, 'ac2' => false, 'temp' => 25],
            ['people' => 10, 'ac1' => true, 'ac2' => false, 'temp' => 22],
            ['people' => 15, 'ac1' => true, 'ac2' => true, 'temp' => 22],
            ['people' => 999, 'ac1' => true, 'ac2' => true, 'temp' => 20],
        ],
    ],
    
    /*
    |--------------------------------------------------------------------------
    | Performance Configuration
    |--------------------------------------------------------------------------
    */
    
    'performance' => [
        'cache_enabled' => true,
        'cache_ttl' => 300, // 5 minutes
        'compression_enabled' => true,
        'cdn_enabled' => false,
    ],
];