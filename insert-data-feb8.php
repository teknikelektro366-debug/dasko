<?php

// Insert data untuk tanggal 8 Februari 2026 dengan variasi per jam
$url = 'http://127.0.0.1:8000/api/sensor/data';
$date = '2026-02-08';

// Data jumlah orang per jam dengan variasi menit
$hourlyData = [
    // Jam 07
    ['hour' => '07', 'minute' => '00', 'people' => 3],
    
    // Jam 08 - 4 data
    ['hour' => '08', 'minute' => '05', 'people' => 5],
    ['hour' => '08', 'minute' => '20', 'people' => 9],
    ['hour' => '08', 'minute' => '35', 'people' => 8],
    ['hour' => '08', 'minute' => '50', 'people' => 11],
    
    // Jam 09 - 4 data
    ['hour' => '09', 'minute' => '10', 'people' => 8],
    ['hour' => '09', 'minute' => '25', 'people' => 9],
    ['hour' => '09', 'minute' => '40', 'people' => 13],
    ['hour' => '09', 'minute' => '55', 'people' => 10],
    
    // Jam 10 - 5 data
    ['hour' => '10', 'minute' => '05', 'people' => 9],
    ['hour' => '10', 'minute' => '18', 'people' => 11],
    ['hour' => '10', 'minute' => '32', 'people' => 15],
    ['hour' => '10', 'minute' => '45', 'people' => 11],
    ['hour' => '10', 'minute' => '58', 'people' => 10],
    
    // Jam 11 - 6 data
    ['hour' => '11', 'minute' => '08', 'people' => 9],
    ['hour' => '11', 'minute' => '18', 'people' => 10],
    ['hour' => '11', 'minute' => '28', 'people' => 6],
    ['hour' => '11', 'minute' => '38', 'people' => 5],
    ['hour' => '11', 'minute' => '48', 'people' => 3],
    ['hour' => '11', 'minute' => '58', 'people' => 2],
    
    // Jam 12 - 4 data
    ['hour' => '12', 'minute' => '10', 'people' => 5],
    ['hour' => '12', 'minute' => '25', 'people' => 8],
    ['hour' => '12', 'minute' => '40', 'people' => 9],
    ['hour' => '12', 'minute' => '55', 'people' => 7],
    
    // Jam 13 - 4 data
    ['hour' => '13', 'minute' => '12', 'people' => 10],
    ['hour' => '13', 'minute' => '28', 'people' => 12],
    ['hour' => '13', 'minute' => '42', 'people' => 9],
    ['hour' => '13', 'minute' => '56', 'people' => 8],
    
    // Jam 14 - 5 data
    ['hour' => '14', 'minute' => '08', 'people' => 10],
    ['hour' => '14', 'minute' => '22', 'people' => 12],
    ['hour' => '14', 'minute' => '35', 'people' => 18],
    ['hour' => '14', 'minute' => '48', 'people' => 19],
    ['hour' => '14', 'minute' => '58', 'people' => 20],
];

echo "🚀 Mulai insert data untuk tanggal $date\n";
echo "   Total: " . count($hourlyData) . " data (Jam 07:00-14:58)\n";
echo str_repeat("=", 70) . "\n\n";

$successCount = 0;
$errorCount = 0;

foreach ($hourlyData as $item) {
    $peopleCount = $item['people'];
    $second = str_pad(rand(10, 59), 2, '0', STR_PAD_LEFT); // Random detik
    $timestamp = "{$date} {$item['hour']}:{$item['minute']}:{$second}";
    
    // Logika Lampu
    $lampStatus = $peopleCount > 0 ? 'ON' : 'OFF';
    
    // Logika AC & Suhu
    $acStatus = 'OFF';
    $setTemperature = null;
    
    if ($peopleCount >= 1 && $peopleCount <= 5) {
        $acStatus = '1 AC ON';
        $setTemperature = 25;
    } elseif ($peopleCount >= 6 && $peopleCount <= 9) {
        $acStatus = '2 AC ON';
        $setTemperature = 25;
    } elseif ($peopleCount >= 10 && $peopleCount <= 15) {
        $acStatus = '2 AC ON';
        $setTemperature = 22;
    } elseif ($peopleCount >= 16) {
        $acStatus = '2 AC ON';
        $setTemperature = 20;
    }
    
    $data = [
        'device_id' => 'ESP32_Smart_Energy',
        'location' => 'Ruang Dosen Prodi Teknik Elektro',
        'people_count' => $peopleCount,
        'room_temperature' => 27,
        'humidity' => 65,
        'light_level' => $peopleCount > 0 ? 500 : 0,
        'ac_status' => $acStatus,
        'set_temperature' => $setTemperature,
        'lamp_status' => $lampStatus,
        'proximity_in' => false,
        'proximity_out' => false,
        'wifi_rssi' => -50,
        'status' => 'active',
        'created_at' => $timestamp
    ];
    
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
    curl_setopt($ch, CURLOPT_HTTPHEADER, [
        'Content-Type: application/json',
        'Accept: application/json'
    ]);
    
    $response = curl_exec($ch);
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);
    
    $result = json_decode($response, true);
    
    if ($httpCode == 201 && $result && isset($result['success']) && $result['success']) {
        $successCount++;
        echo "✅ {$item['hour']}:{$item['minute']}:{$second} - {$peopleCount} orang - $lampStatus - $acStatus";
        if ($setTemperature) echo " ({$setTemperature}°C)";
        echo " - ID: {$result['data']['id']}\n";
    } else {
        $errorCount++;
        echo "❌ {$item['hour']}:{$item['minute']} - GAGAL: " . ($result['message'] ?? 'Unknown error') . "\n";
    }
    
    // Delay 100ms
    usleep(100000);
}

echo "\n" . str_repeat("=", 70) . "\n";
echo "📊 HASIL:\n";
echo "   ✅ Berhasil: $successCount data\n";
echo "   ❌ Gagal: $errorCount data\n";
echo "\n🔍 Cek di History: http://127.0.0.1:8000/dashboard (tab History)\n";
echo "   Refresh halaman untuk melihat data terbaru!\n";
