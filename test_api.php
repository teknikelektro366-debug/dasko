<?php
/**
 * Test Script untuk API Sensor Data
 * Jalankan dengan: php test_api.php
 */

// Konfigurasi
$baseUrl = 'http://localhost:8000'; // Ganti dengan URL Laravel server Anda
$apiEndpoint = $baseUrl . '/api/sensor/data';

echo "=== TESTING LARAVEL API SENSOR DATA ===\n";
echo "API Endpoint: $apiEndpoint\n\n";

// Test data seperti yang dikirim ESP32
$testData = [
    'device_id' => 'ESP32_Smart_Energy',
    'location' => 'Lab Teknik Tegangan Tinggi',
    'people_count' => 3,
    'ac_status' => '1 AC ON',
    'set_temperature' => 25,
    'room_temperature' => 28.5,
    'humidity' => 65.2,
    'light_level' => 450,
    'proximity_in' => false,
    'proximity_out' => true,
    'wifi_rssi' => -45,
    'uptime' => 12345,
    'free_heap' => 234567,
    'timestamp' => time() * 1000,
    'status' => 'active'
];

echo "1. Testing POST /api/sensor/data\n";
echo "Sending test data...\n";

// Kirim POST request
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $apiEndpoint);
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($testData));
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'Content-Type: application/json',
    'Accept: application/json',
    'User-Agent: ESP32-SmartEnergy-Test/1.0'
]);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_TIMEOUT, 10);

$response = curl_exec($ch);
$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
$error = curl_error($ch);
curl_close($ch);

if ($error) {
    echo "❌ CURL Error: $error\n";
    exit(1);
}

echo "Response Code: $httpCode\n";
echo "Response Body: $response\n\n";

if ($httpCode == 200 || $httpCode == 201) {
    echo "✅ POST Test PASSED - Data berhasil dikirim\n\n";
} else {
    echo "❌ POST Test FAILED\n\n";
}

// Test GET latest data
echo "2. Testing GET /api/sensor/latest\n";
$latestUrl = $baseUrl . '/api/sensor/latest';

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $latestUrl);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_TIMEOUT, 10);

$response = curl_exec($ch);
$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

echo "Response Code: $httpCode\n";
echo "Response Body: $response\n\n";

if ($httpCode == 200) {
    echo "✅ GET Test PASSED - Data berhasil diambil\n\n";
    
    $data = json_decode($response, true);
    if ($data && $data['success']) {
        echo "📊 Latest Data Summary:\n";
        echo "- Device ID: " . ($data['data']['device_id'] ?? 'N/A') . "\n";
        echo "- People Count: " . ($data['data']['people_count'] ?? 'N/A') . "\n";
        echo "- AC Status: " . ($data['data']['ac_status'] ?? 'N/A') . "\n";
        echo "- Temperature: " . ($data['data']['room_temperature'] ?? 'N/A') . "°C\n";
        echo "- Humidity: " . ($data['data']['humidity'] ?? 'N/A') . "%\n";
        echo "- Created: " . ($data['data']['created_at'] ?? 'N/A') . "\n";
    }
} else {
    echo "❌ GET Test FAILED\n\n";
}

// Test database connection
echo "3. Testing Database Connection\n";
$dbTestUrl = $baseUrl . '/api/sensor/history?per_page=1';

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $dbTestUrl);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_TIMEOUT, 10);

$response = curl_exec($ch);
$httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
curl_close($ch);

if ($httpCode == 200) {
    $data = json_decode($response, true);
    if ($data && $data['success']) {
        echo "✅ Database Test PASSED\n";
        echo "Total Records: " . ($data['pagination']['total'] ?? 0) . "\n";
    } else {
        echo "❌ Database Test FAILED - Invalid response\n";
    }
} else {
    echo "❌ Database Test FAILED - HTTP $httpCode\n";
}

echo "\n=== TEST COMPLETED ===\n";
echo "Jika semua test PASSED, API siap digunakan dengan ESP32!\n";
?>