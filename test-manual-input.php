<?php

// Test Manual Input API
$url = 'http://127.0.0.1:8000/api/sensor/data';

$data = [
    'device_id' => 'MANUAL_INPUT',
    'location' => 'Ruang Dosen Prodi Teknik Elektro',
    'people_count' => 10,
    'room_temperature' => 27,
    'humidity' => 65,
    'light_level' => 500,
    'ac_status' => 'AC 1+2 ON',
    'set_temperature' => 22,
    'lamp_status' => 'ON',
    'proximity_in' => false,
    'proximity_out' => false,
    'wifi_rssi' => 0,
    'status' => 'active',
    'created_at' => date('Y-m-d H:i:s')
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

echo "HTTP Code: $httpCode\n";
echo "Response: $response\n";

$result = json_decode($response, true);
if ($result && isset($result['success']) && $result['success']) {
    echo "\n✅ SUCCESS! Data berhasil disimpan.\n";
    echo "ID: " . ($result['data']['id'] ?? 'N/A') . "\n";
} else {
    echo "\n❌ FAILED! " . ($result['message'] ?? 'Unknown error') . "\n";
}
