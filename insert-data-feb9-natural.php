<?php

// Insert data untuk tanggal 9 Februari 2026 dengan pola masuk-keluar natural
$url = 'http://127.0.0.1:8000/api/sensor/data';
$date = '2026-02-09';

// Fungsi untuk generate data realistis
function generateRealisticData($peopleCount, $hour) {
    $baseTemp = 26 + ($peopleCount * 0.15);
    $roomTemperature = round($baseTemp + (rand(-10, 10) / 10), 1);
    $roomTemperature = max(25, min(29, $roomTemperature));
    
    $baseHumidity = 65 + ($peopleCount * 0.3);
    $humidity = round($baseHumidity + (rand(-20, 20) / 10), 1);
    $humidity = max(60, min(75, $humidity));
    
    if ($peopleCount == 0) {
        $lightLevel = rand(50, 150);
    } else {
        $baseLux = 400;
        if ($hour >= 7 && $hour <= 10) {
            $baseLux = 600;
        } elseif ($hour >= 11 && $hour <= 13) {
            $baseLux = 500;
        } else {
            $baseLux = 400;
        }
        $lightLevel = $baseLux + rand(-100, 100);
        $lightLevel = max(200, min(800, $lightLevel));
    }
    
    return [
        'room_temperature' => $roomTemperature,
        'humidity' => $humidity,
        'light_level' => $lightLevel
    ];
}

echo "🚀 Insert data untuk tanggal $date (Hari Ini - Natural Pattern)\n";
echo str_repeat("=", 80) . "\n\n";

// Data masuk-keluar berdasarkan input user + lanjutan natural
$events = [
    // Data dari user
    ['time' => '07:39', 'in' => 2, 'out' => 0],
    ['time' => '08:24', 'in' => 1, 'out' => 0],
    ['time' => '08:27', 'in' => 1, 'out' => 0],
    ['time' => '08:32', 'in' => 0, 'out' => 1],
    ['time' => '08:34', 'in' => 1, 'out' => 0],
    ['time' => '08:34', 'in' => 0, 'out' => 1],
    ['time' => '08:43', 'in' => 1, 'out' => 0],
    ['time' => '08:58', 'in' => 2, 'out' => 0],
    ['time' => '09:02', 'in' => 0, 'out' => 2],
    ['time' => '09:27', 'in' => 2, 'out' => 0],
    ['time' => '09:30', 'in' => 0, 'out' => 3],
    ['time' => '09:41', 'in' => 4, 'out' => 0],
    ['time' => '09:45', 'in' => 0, 'out' => 2],
    ['time' => '10:03', 'in' => 2, 'out' => 0],
    
    // Lanjutan natural (jam 10-14)
    ['time' => '10:15', 'in' => 3, 'out' => 0],
    ['time' => '10:28', 'in' => 2, 'out' => 1],
    ['time' => '10:42', 'in' => 1, 'out' => 0],
    ['time' => '10:55', 'in' => 0, 'out' => 2],
    
    ['time' => '11:08', 'in' => 1, 'out' => 1],
    ['time' => '11:22', 'in' => 0, 'out' => 3],
    ['time' => '11:35', 'in' => 2, 'out' => 0],
    ['time' => '11:48', 'in' => 0, 'out' => 2],
    ['time' => '11:55', 'in' => 0, 'out' => 1],
    
    ['time' => '12:05', 'in' => 1, 'out' => 0],
    ['time' => '12:18', 'in' => 2, 'out' => 0],
    ['time' => '12:32', 'in' => 1, 'out' => 1],
    ['time' => '12:45', 'in' => 0, 'out' => 1],
    ['time' => '12:58', 'in' => 2, 'out' => 0],
    
    ['time' => '13:10', 'in' => 3, 'out' => 0],
    ['time' => '13:25', 'in' => 2, 'out' => 1],
    ['time' => '13:38', 'in' => 0, 'out' => 2],
    ['time' => '13:52', 'in' => 1, 'out' => 0],
    
    ['time' => '14:05', 'in' => 2, 'out' => 0],
    ['time' => '14:18', 'in' => 3, 'out' => 1],
    ['time' => '14:32', 'in' => 1, 'out' => 2],
    ['time' => '14:45', 'in' => 0, 'out' => 3],
    ['time' => '14:58', 'in' => 0, 'out' => 2],
];

$successCount = 0;
$errorCount = 0;
$currentPeople = 0;

foreach ($events as $event) {
    $time = $event['time'];
    $masuk = $event['in'];
    $keluar = $event['out'];
    
    // Update jumlah orang
    $currentPeople += $masuk;
    $currentPeople -= $keluar;
    if ($currentPeople < 0) $currentPeople = 0;
    
    $hour = intval(explode(':', $time)[0]);
    $minute = explode(':', $time)[1];
    $second = str_pad(rand(10, 59), 2, '0', STR_PAD_LEFT);
    $timestamp = "{$date} {$time}:{$second}";
    
    // Generate data realistis
    $sensorData = generateRealisticData($currentPeople, $hour);
    
    // Logika Lampu
    $lampStatus = $currentPeople > 0 ? 'ON' : 'OFF';
    
    // Logika AC & Suhu
    $acStatus = 'OFF';
    $setTemperature = null;
    
    if ($currentPeople >= 1 && $currentPeople <= 5) {
        $acStatus = '1 AC ON';
        $setTemperature = 25;
    } elseif ($currentPeople >= 6 && $currentPeople <= 9) {
        $acStatus = '2 AC ON';
        $setTemperature = 25;
    } elseif ($currentPeople >= 10 && $currentPeople <= 15) {
        $acStatus = '2 AC ON';
        $setTemperature = 22;
    } elseif ($currentPeople >= 16) {
        $acStatus = '2 AC ON';
        $setTemperature = 20;
    }
    
    $data = [
        'device_id' => 'ESP32_Smart_Energy',
        'location' => 'Ruang Dosen Prodi Teknik Elektro',
        'people_count' => $currentPeople,
        'room_temperature' => $sensorData['room_temperature'],
        'humidity' => $sensorData['humidity'],
        'light_level' => $sensorData['light_level'],
        'ac_status' => $acStatus,
        'set_temperature' => $setTemperature,
        'lamp_status' => $lampStatus,
        'proximity_in' => $masuk > 0,
        'proximity_out' => $keluar > 0,
        'wifi_rssi' => rand(-70, -40),
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
        $masukStr = $masuk > 0 ? "↗️ +$masuk" : "";
        $keluarStr = $keluar > 0 ? "↙️ -$keluar" : "";
        $activity = trim("$masukStr $keluarStr");
        if (empty($activity)) $activity = "—";
        
        echo "✅ $time - $activity → Total: $currentPeople orang - ";
        echo "Suhu: {$sensorData['room_temperature']}°C - ";
        echo "$acStatus\n";
    } else {
        $errorCount++;
        echo "❌ $time - GAGAL\n";
    }
    
    usleep(100000); // Delay 100ms
}

echo "\n" . str_repeat("=", 80) . "\n";
echo "📊 HASIL:\n";
echo "   ✅ Berhasil: $successCount data\n";
echo "   ❌ Gagal: $errorCount data\n";
echo "   👥 Total orang di ruangan: $currentPeople orang\n";
echo "\n🔍 Refresh halaman History: http://127.0.0.1:8000/dashboard\n";
echo "   Tekan Ctrl+F5 untuk hard refresh\n";
