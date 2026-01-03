<?php
// File: api/sensor/data.php
// Update API endpoint untuk menerima data ultrasonic

header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');

// Database connection
$host = 'localhost';
$dbname = 'smart_energy_db';
$username = 'your_username';
$password = 'your_password';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch(PDOException $e) {
    http_response_code(500);
    echo json_encode(['success' => false, 'error' => 'Database connection failed']);
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Receive data from ESP32
    $input = json_decode(file_get_contents('php://input'), true);
    
    if (!$input) {
        http_response_code(400);
        echo json_encode(['success' => false, 'error' => 'Invalid JSON data']);
        exit;
    }
    
    try {
        // Insert sensor data with ultrasonic fields
        $sql = "INSERT INTO sensor_data (
            device_id, location, people_count, ac_status, set_temperature, 
            room_temperature, humidity, light_level, 
            ultrasonic_in, ultrasonic_out, wifi_rssi, uptime, free_heap, 
            update_reason, update_type, control_mode, manual_override, 
            sensor_type, timestamp, created_at
        ) VALUES (
            :device_id, :location, :people_count, :ac_status, :set_temperature,
            :room_temperature, :humidity, :light_level,
            :ultrasonic_in, :ultrasonic_out, :wifi_rssi, :uptime, :free_heap,
            :update_reason, :update_type, :control_mode, :manual_override,
            :sensor_type, :timestamp, NOW()
        )";
        
        $stmt = $pdo->prepare($sql);
        $stmt->execute([
            ':device_id' => $input['device_id'] ?? 'ESP32_Unknown',
            ':location' => $input['location'] ?? 'Unknown',
            ':people_count' => $input['people_count'] ?? 0,
            ':ac_status' => $input['ac_status'] ?? 'OFF',
            ':set_temperature' => $input['set_temperature'] ?? 0,
            ':room_temperature' => $input['room_temperature'] ?? 0,
            ':humidity' => $input['humidity'] ?? 0,
            ':light_level' => $input['light_level'] ?? 0,
            ':ultrasonic_in' => $input['ultrasonic_in'] ? 1 : 0,      // NEW FIELD
            ':ultrasonic_out' => $input['ultrasonic_out'] ? 1 : 0,    // NEW FIELD
            ':wifi_rssi' => $input['wifi_rssi'] ?? 0,
            ':uptime' => $input['uptime'] ?? 0,
            ':free_heap' => $input['free_heap'] ?? 0,
            ':update_reason' => $input['update_reason'] ?? 'Unknown',
            ':update_type' => $input['update_type'] ?? 'sensor_reading',  // NEW FIELD
            ':control_mode' => $input['control_mode'] ?? 'auto',
            ':manual_override' => $input['manual_override'] ? 1 : 0,
            ':sensor_type' => $input['sensor_type'] ?? 'Unknown',         // NEW FIELD
            ':timestamp' => $input['timestamp'] ?? time()
        ]);
        
        $lastId = $pdo->lastInsertId();
        
        echo json_encode([
            'success' => true,
            'message' => 'Ultrasonic sensor data saved successfully',
            'data' => [
                'id' => $lastId,
                'device_id' => $input['device_id'],
                'people_count' => $input['people_count'],
                'sensor_type' => $input['sensor_type'],
                'ultrasonic_in' => $input['ultrasonic_in'],
                'ultrasonic_out' => $input['ultrasonic_out']
            ]
        ]);
        
    } catch(PDOException $e) {
        http_response_code(500);
        echo json_encode([
            'success' => false, 
            'error' => 'Database error: ' . $e->getMessage()
        ]);
    }
    
} elseif ($_SERVER['REQUEST_METHOD'] === 'GET') {
    // Get latest sensor data for dashboard
    try {
        $sql = "SELECT * FROM sensor_data 
                ORDER BY created_at DESC 
                LIMIT 1";
        
        $stmt = $pdo->query($sql);
        $latestData = $stmt->fetch(PDO::FETCH_ASSOC);
        
        if ($latestData) {
            echo json_encode([
                'success' => true,
                'data' => $latestData
            ]);
        } else {
            echo json_encode([
                'success' => false,
                'message' => 'No data found'
            ]);
        }
        
    } catch(PDOException $e) {
        http_response_code(500);
        echo json_encode([
            'success' => false,
            'error' => 'Database error: ' . $e->getMessage()
        ]);
    }
}
?>