<?php
/**
 * Script Debug untuk History Data
 * Jalankan dengan: php debug_history_data.php
 */

// Simulasi environment Laravel
$_ENV['APP_ENV'] = 'local';
require_once 'vendor/autoload.php';

use Illuminate\Database\Capsule\Manager as Capsule;
use Illuminate\Support\Facades\DB;

// Setup database connection
$capsule = new Capsule;
$capsule->addConnection([
    'driver' => 'mysql',
    'host' => env('DB_HOST', '127.0.0.1'),
    'port' => env('DB_PORT', '3306'),
    'database' => env('DB_DATABASE', 'smart_energy'),
    'username' => env('DB_USERNAME', 'root'),
    'password' => env('DB_PASSWORD', ''),
    'charset' => 'utf8mb4',
    'collation' => 'utf8mb4_unicode_ci',
]);

$capsule->setAsGlobal();
$capsule->bootEloquent();

echo "=== DEBUG HISTORY DATA ===\n\n";

try {
    // 1. Cek total records di database
    $totalRecords = Capsule::table('sensor_data')->count();
    echo "1. Total records di database: {$totalRecords}\n";
    
    // 2. Cek 10 record terbaru
    $latestRecords = Capsule::table('sensor_data')
        ->orderBy('created_at', 'desc')
        ->limit(10)
        ->get(['id', 'device_id', 'people_count', 'ac_status', 'created_at']);
    
    echo "\n2. 10 Record terbaru:\n";
    echo "ID | Device | People | AC Status | Created At\n";
    echo "---|--------|--------|-----------|------------\n";
    
    foreach ($latestRecords as $record) {
        echo sprintf("%d | %s | %d | %s | %s\n", 
            $record->id,
            $record->device_id,
            $record->people_count,
            $record->ac_status,
            $record->created_at
        );
    }
    
    // 3. Cek record hari ini
    $todayRecords = Capsule::table('sensor_data')
        ->whereDate('created_at', today())
        ->count();
    echo "\n3. Record hari ini: {$todayRecords}\n";
    
    // 4. Cek record 1 jam terakhir
    $lastHourRecords = Capsule::table('sensor_data')
        ->where('created_at', '>=', now()->subHour())
        ->count();
    echo "4. Record 1 jam terakhir: {$lastHourRecords}\n";
    
    // 5. Cek struktur tabel
    echo "\n5. Struktur tabel sensor_data:\n";
    $columns = Capsule::select("DESCRIBE sensor_data");
    foreach ($columns as $column) {
        echo "- {$column->Field} ({$column->Type})\n";
    }
    
    // 6. Test insert data baru
    echo "\n6. Test insert data baru...\n";
    $testData = [
        'device_id' => 'ESP32_Test_Debug',
        'location' => 'Debug Test Location',
        'people_count' => 5,
        'ac_status' => 'TEST ON',
        'set_temperature' => 25,
        'room_temperature' => 28.5,
        'humidity' => 65.0,
        'light_level' => 300,
        'proximity_in' => true,
        'proximity_out' => false,
        'wifi_rssi' => -45,
        'status' => 'active',
        'created_at' => now(),
        'updated_at' => now()
    ];
    
    $insertId = Capsule::table('sensor_data')->insertGetId($testData);
    echo "✅ Test data berhasil diinsert dengan ID: {$insertId}\n";
    
    // 7. Hapus test data
    Capsule::table('sensor_data')->where('id', $insertId)->delete();
    echo "✅ Test data berhasil dihapus\n";
    
} catch (Exception $e) {
    echo "❌ Error: " . $e->getMessage() . "\n";
    echo "Stack trace: " . $e->getTraceAsString() . "\n";
}

echo "\n=== DEBUG SELESAI ===\n";