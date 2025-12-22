<?php
/**
 * Test Script untuk API Hosting
 * Jalankan dengan: php test_hosting_api.php
 */

// KONFIGURASI - GANTI DENGAN URL HOSTING ANDA
$hostingUrl = 'https://yourdomain.com'; // GANTI INI!
$apiEndpoint = $hostingUrl . '/api/sensor/data';
$healthEndpoint = $hostingUrl . '/api/health';
$latestEndpoint = $hostingUrl . '/api/sensor/latest';

echo "=== TESTING HOSTING API ===\n";
echo "Hosting URL: $hostingUrl\n";
echo "API Endpoint: $apiEndpoint\n\n";

// Test data seperti yang dikirim ESP32
$testData = [
    'device_id' => 'ESP32_Smart_Energy_Production',
    'location' => 'Lab Teknik Tegangan Tinggi',
    'people_count' => 5,
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
    'status' => 'active',
    'version' => 'production-1.0',
    'connection_type' => 'hosting'
];

// Function untuk test HTTP request
function testHttpRequest($url, $method = 'GET', $data = null, $headers = []) {
    $ch = curl_init();
    
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 30);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 10);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
    curl_setopt($ch, CURLOPT_USERAGENT, 'ESP32-SmartEnergy-Test/1.0');
    
    if ($method === 'POST') {
        curl_setopt($ch, CURLOPT_POST, true);
        if ($data) {
            curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
            $headers[] = 'Content-Type: application/json';
        }
    }
    
    if (!empty($headers)) {
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    }
    
    $response = curl_exec($ch);
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    $error = curl_error($ch);
    $info = curl_getinfo($ch);
    
    curl_close($ch);
    
    return [
        'response' => $response,
        'http_code' => $httpCode,
        'error' => $error,
        'info' => $info
    ];
}

// 1. Test Health Check
echo "1. Testing Health Check\n";
echo "URL: $healthEndpoint\n";

$result = testHttpRequest($healthEndpoint);

if ($result['error']) {
    echo "❌ CURL Error: " . $result['error'] . "\n";
    echo "   Possible causes:\n";
    echo "   - Domain tidak bisa diakses\n";
    echo "   - SSL certificate bermasalah\n";
    echo "   - Server down\n\n";
} else {
    echo "Response Code: " . $result['http_code'] . "\n";
    echo "Response Time: " . round($result['info']['total_time'], 2) . "s\n";
    
    if ($result['http_code'] == 200) {
        echo "✅ Health Check PASSED\n";
        $healthData = json_decode($result['response'], true);
        if ($healthData) {
            echo "   Status: " . ($healthData['status'] ?? 'unknown') . "\n";
            echo "   Database: " . ($healthData['database'] ?? 'unknown') . "\n";
        }
    } else {
        echo "❌ Health Check FAILED\n";
        echo "   Response: " . substr($result['response'], 0, 200) . "\n";
    }
}
echo "\n";

// 2. Test POST Data (seperti ESP32)
echo "2. Testing POST /api/sensor/data (ESP32 simulation)\n";
echo "Sending test data...\n";

$result = testHttpRequest($apiEndpoint, 'POST', $testData, [
    'Accept: application/json',
    'X-Requested-With: XMLHttpRequest'
]);

if ($result['error']) {
    echo "❌ CURL Error: " . $result['error'] . "\n";
    echo "   ESP32 akan mengalami error yang sama!\n\n";
} else {
    echo "Response Code: " . $result['http_code'] . "\n";
    echo "Response Time: " . round($result['info']['total_time'], 2) . "s\n";
    echo "Response Body: " . $result['response'] . "\n";
    
    if ($result['http_code'] == 200 || $result['http_code'] == 201) {
        echo "✅ POST Test PASSED - ESP32 akan berhasil mengirim data\n";
        
        $responseData = json_decode($result['response'], true);
        if ($responseData && $responseData['success']) {
            echo "   Data berhasil disimpan dengan ID: " . ($responseData['data']['id'] ?? 'unknown') . "\n";
        }
    } else {
        echo "❌ POST Test FAILED - ESP32 akan gagal mengirim data\n";
        
        // Analisis error
        switch ($result['http_code']) {
            case 404:
                echo "   Error: Route tidak ditemukan\n";
                echo "   Solution: Pastikan route /api/sensor/data exists\n";
                break;
            case 422:
                echo "   Error: Validation failed\n";
                echo "   Solution: Check validation rules di controller\n";
                break;
            case 500:
                echo "   Error: Server error\n";
                echo "   Solution: Check Laravel logs\n";
                break;
            case 403:
                echo "   Error: Forbidden\n";
                echo "   Solution: Check CORS atau middleware\n";
                break;
            default:
                echo "   Error: HTTP " . $result['http_code'] . "\n";
        }
    }
}
echo "\n";

// 3. Test GET Latest Data
echo "3. Testing GET /api/sensor/latest\n";

$result = testHttpRequest($latestEndpoint);

if ($result['error']) {
    echo "❌ CURL Error: " . $result['error'] . "\n\n";
} else {
    echo "Response Code: " . $result['http_code'] . "\n";
    echo "Response Time: " . round($result['info']['total_time'], 2) . "s\n";
    
    if ($result['http_code'] == 200) {
        echo "✅ GET Test PASSED\n";
        
        $data = json_decode($result['response'], true);
        if ($data && $data['success'] && isset($data['data'])) {
            echo "📊 Latest Data Summary:\n";
            echo "   Device ID: " . ($data['data']['device_id'] ?? 'N/A') . "\n";
            echo "   People Count: " . ($data['data']['people_count'] ?? 'N/A') . "\n";
            echo "   AC Status: " . ($data['data']['ac_status'] ?? 'N/A') . "\n";
            echo "   Temperature: " . ($data['data']['room_temperature'] ?? 'N/A') . "°C\n";
            echo "   Humidity: " . ($data['data']['humidity'] ?? 'N/A') . "%\n";
            echo "   Created: " . ($data['data']['created_at'] ?? 'N/A') . "\n";
        }
    } else {
        echo "❌ GET Test FAILED\n";
        echo "   Response: " . substr($result['response'], 0, 200) . "\n";
    }
}
echo "\n";

// 4. Test SSL Certificate
echo "4. Testing SSL Certificate\n";

$sslInfo = testHttpRequest($hostingUrl);
if (isset($sslInfo['info']['ssl_verify_result'])) {
    if ($sslInfo['info']['ssl_verify_result'] == 0) {
        echo "✅ SSL Certificate VALID\n";
        echo "   ESP32 dapat menggunakan HTTPS dengan aman\n";
    } else {
        echo "⚠ SSL Certificate Issue (Code: " . $sslInfo['info']['ssl_verify_result'] . ")\n";
        echo "   ESP32 mungkin perlu setInsecure() atau update certificate\n";
    }
} else {
    echo "ℹ SSL info tidak tersedia\n";
}
echo "\n";

// 5. Performance Test
echo "5. Testing Performance (5 requests)\n";
$times = [];
for ($i = 1; $i <= 5; $i++) {
    $start = microtime(true);
    $result = testHttpRequest($latestEndpoint);
    $end = microtime(true);
    $time = ($end - $start) * 1000; // Convert to milliseconds
    $times[] = $time;
    echo "   Request $i: " . round($time, 2) . "ms (HTTP " . $result['http_code'] . ")\n";
}

$avgTime = array_sum($times) / count($times);
echo "   Average Response Time: " . round($avgTime, 2) . "ms\n";

if ($avgTime < 1000) {
    echo "✅ Performance GOOD - ESP32 akan mendapat response cepat\n";
} elseif ($avgTime < 3000) {
    echo "⚠ Performance OK - ESP32 perlu timeout yang cukup\n";
} else {
    echo "❌ Performance SLOW - ESP32 mungkin timeout\n";
}
echo "\n";

// Summary
echo "=== SUMMARY ===\n";
echo "Hosting URL: $hostingUrl\n";
echo "Ready for ESP32: ";

// Check semua test
$allPassed = true;
if (!empty($result['error'])) $allPassed = false;
if ($result['http_code'] != 200 && $result['http_code'] != 201) $allPassed = false;

if ($allPassed) {
    echo "✅ YES\n";
    echo "\nNext Steps:\n";
    echo "1. Update Arduino code dengan URL: $apiEndpoint\n";
    echo "2. Upload arduino_production_hosting.ino ke ESP32\n";
    echo "3. Monitor serial output untuk konfirmasi\n";
    echo "4. Check dashboard untuk data realtime\n";
} else {
    echo "❌ NO\n";
    echo "\nIssues to Fix:\n";
    echo "1. Pastikan Laravel sudah di-deploy dengan benar\n";
    echo "2. Check database connection di hosting\n";
    echo "3. Pastikan route /api/sensor/data accessible\n";
    echo "4. Test manual dengan browser: $latestEndpoint\n";
}

echo "\n=== TEST COMPLETED ===\n";
?>