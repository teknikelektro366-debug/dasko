<?php
// KONFIGURASI HOSTING UNTUK ESP32 ENHANCED PROXY DETECTION
// File: hosting_config.php

// Database configuration
$host = 'localhost';
$dbname = 'esp32_sensor_db';
$username = 'your_db_username';
$password = 'your_db_password';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch(PDOException $e) {
    die("Connection failed: " . $e->getMessage());
}

// CORS headers for ESP32
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type, X-Requested-With, X-Device-ID, X-Location, X-Proxy-Connection, X-Proxy-Type, X-Real-IP');
header('Content-Type: application/json');

// Handle preflight OPTIONS request
if ($_SERVER['REQUEST_METHOD'] == 'OPTIONS') {
    http_response_code(200);
    exit();
}

// Get request data
$input = json_decode(file_get_contents('php://input'), true);
$method = $_SERVER['REQUEST_METHOD'];
$request_uri = $_SERVER['REQUEST_URI'];

// Parse request path
$path = parse_url($request_uri, PHP_URL_PATH);
$path_parts = explode('/', trim($path, '/'));

// API Routes
if ($path_parts[0] === 'api') {
    switch ($path_parts[1]) {
        case 'sensor':
            if ($path_parts[2] === 'data') {
                handleSensorData($pdo, $method, $input);
            }
            break;
            
        case 'ac':
            if ($path_parts[2] === 'control') {
                handleACControl($pdo, $method, $input);
            }
            break;
            
        case 'proxy':
            if ($path_parts[2] === 'detection') {
                handleProxyDetection($pdo, $method, $input);
            }
            break;
            
        case 'health':
            handleHealthCheck();
            break;
            
        default:
            http_response_code(404);
            echo json_encode(['success' => false, 'message' => 'Endpoint not found']);
            break;
    }
} else {
    http_response_code(404);
    echo json_encode(['success' => false, 'message' => 'API not found']);
}

// ENHANCED PROXY DETECTION HANDLER
function handleProxyDetection($pdo, $method, $input) {
    if ($method !== 'POST') {
        http_response_code(405);
        echo json_encode(['success' => false, 'message' => 'Method not allowed']);
        return;
    }
    
    // Get client information
    $client_ip = getClientIP();
    $real_ip = getRealIP();
    $proxy_detected = detectProxy();
    $proxy_type = getProxyType();
    
    // Device information from ESP32
    $device_id = $input['device_id'] ?? 'unknown';
    $location = $input['location'] ?? 'unknown';
    $local_ip = $input['local_ip'] ?? '';
    $gateway_ip = $input['gateway_ip'] ?? '';
    $dns_ip = $input['dns_ip'] ?? '';
    $rssi = $input['rssi'] ?? 0;
    
    // Log proxy detection
    try {
        $stmt = $pdo->prepare("
            INSERT INTO proxy_detection_log 
            (device_id, location, client_ip, real_ip, proxy_detected, proxy_type, 
             local_ip, gateway_ip, dns_ip, rssi, user_agent, created_at) 
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NOW())
        ");
        
        $stmt->execute([
            $device_id, $location, $client_ip, $real_ip, $proxy_detected ? 1 : 0,
            $proxy_type, $local_ip, $gateway_ip, $dns_ip, $rssi, $_SERVER['HTTP_USER_AGENT'] ?? ''
        ]);
        
        $log_id = $pdo->lastInsertId();
        
        // Response for ESP32
        $response = [
            'success' => true,
            'data' => [
                'proxy_detected' => $proxy_detected,
                'proxy_type' => $proxy_type,
                'real_ip' => $real_ip,
                'proxy_ip' => $proxy_detected ? $client_ip : '',
                'client_ip' => $client_ip,
                'log_id' => $log_id,
                'timestamp' => date('Y-m-d H:i:s')
            ],
            'message' => 'Proxy detection completed'
        ];
        
        echo json_encode($response);
        
    } catch(PDOException $e) {
        http_response_code(500);
        echo json_encode([
            'success' => false, 
            'message' => 'Database error: ' . $e->getMessage()
        ]);
    }
}

// ENHANCED SENSOR DATA HANDLER
function handleSensorData($pdo, $method, $input) {
    if ($method !== 'POST') {
        http_response_code(405);
        echo json_encode(['success' => false, 'message' => 'Method not allowed']);
        return;
    }
    
    // Enhanced data extraction with proxy information
    $device_id = $input['device_id'] ?? 'unknown';
    $location = $input['location'] ?? 'unknown';
    $people_count = $input['people_count'] ?? 0;
    $ac_status = $input['ac_status'] ?? 'OFF';
    $set_temperature = $input['set_temperature'] ?? 0;
    $room_temperature = $input['room_temperature'] ?? 0;
    $humidity = $input['humidity'] ?? 0;
    $light_level = $input['light_level'] ?? 0;
    $proximity_in = $input['proximity_in'] ?? false;
    $proximity_out = $input['proximity_out'] ?? false;
    $wifi_rssi = $input['wifi_rssi'] ?? 0;
    $uptime = $input['uptime'] ?? 0;
    $free_heap = $input['free_heap'] ?? 0;
    $update_reason = $input['update_reason'] ?? 'unknown';
    
    // Enhanced proxy information
    $proxy_detected = $input['proxy_detected'] ?? false;
    $proxy_type = $input['proxy_type'] ?? 'none';
    $connection_type = $input['connection_type'] ?? 'direct';
    $hosting_reachable = $input['hosting_reachable'] ?? true;
    $consecutive_failures = $input['consecutive_failures'] ?? 0;
    $real_ip = $input['real_ip'] ?? '';
    $proxy_ip = $input['proxy_ip'] ?? '';
    
    try {
        $stmt = $pdo->prepare("
            INSERT INTO sensor_data 
            (device_id, location, people_count, ac_status, set_temperature, room_temperature, 
             humidity, light_level, proximity_in, proximity_out, wifi_rssi, uptime, free_heap, 
             update_reason, proxy_detected, proxy_type, connection_type, hosting_reachable, 
             consecutive_failures, real_ip, proxy_ip, client_ip, created_at) 
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NOW())
        ");
        
        $stmt->execute([
            $device_id, $location, $people_count, $ac_status, $set_temperature, $room_temperature,
            $humidity, $light_level, $proximity_in ? 1 : 0, $proximity_out ? 1 : 0, $wifi_rssi, 
            $uptime, $free_heap, $update_reason, $proxy_detected ? 1 : 0, $proxy_type, 
            $connection_type, $hosting_reachable ? 1 : 0, $consecutive_failures, $real_ip, 
            $proxy_ip, getClientIP()
        ]);
        
        $data_id = $pdo->lastInsertId();
        
        // Enhanced response
        $response = [
            'success' => true,
            'data' => [
                'id' => $data_id,
                'device_id' => $device_id,
                'people_count' => $people_count,
                'ac_status' => $ac_status,
                'proxy_status' => $proxy_detected ? 'detected' : 'direct',
                'connection_quality' => $consecutive_failures == 0 ? 'good' : 'poor',
                'timestamp' => date('Y-m-d H:i:s')
            ],
            'message' => 'Enhanced sensor data saved successfully'
        ];
        
        echo json_encode($response);
        
    } catch(PDOException $e) {
        http_response_code(500);
        echo json_encode([
            'success' => false, 
            'message' => 'Database error: ' . $e->getMessage()
        ]);
    }
}

// AC CONTROL HANDLER
function handleACControl($pdo, $method, $input) {
    if ($method !== 'GET') {
        http_response_code(405);
        echo json_encode(['success' => false, 'message' => 'Method not allowed']);
        return;
    }
    
    $device_id = $_GET['device_id'] ?? 'unknown';
    $location = $_GET['location'] ?? 'unknown';
    
    try {
        // Check for active AC control commands
        $stmt = $pdo->prepare("
            SELECT * FROM ac_control 
            WHERE device_id = ? AND (expires_at IS NULL OR expires_at > NOW()) 
            ORDER BY created_at DESC LIMIT 1
        ");
        $stmt->execute([$device_id]);
        $control = $stmt->fetch(PDO::FETCH_ASSOC);
        
        if ($control) {
            $response = [
                'success' => true,
                'data' => [
                    'control_mode' => $control['control_mode'],
                    'manual_override' => (bool)$control['manual_override'],
                    'ac1_status' => (bool)$control['ac1_status'],
                    'ac2_status' => (bool)$control['ac2_status'],
                    'ac1_temperature' => (int)$control['ac1_temperature'],
                    'ac2_temperature' => (int)$control['ac2_temperature'],
                    'created_by' => $control['created_by'],
                    'expires_at' => $control['expires_at']
                ],
                'message' => 'AC control command found'
            ];
        } else {
            $response = [
                'success' => true,
                'data' => [
                    'control_mode' => 'auto',
                    'manual_override' => false,
                    'ac1_status' => false,
                    'ac2_status' => false,
                    'ac1_temperature' => 25,
                    'ac2_temperature' => 25,
                    'created_by' => 'system',
                    'expires_at' => null
                ],
                'message' => 'No active AC control, using auto mode'
            ];
        }
        
        echo json_encode($response);
        
    } catch(PDOException $e) {
        http_response_code(500);
        echo json_encode([
            'success' => false, 
            'message' => 'Database error: ' . $e->getMessage()
        ]);
    }
}

// HEALTH CHECK HANDLER
function handleHealthCheck() {
    $response = [
        'success' => true,
        'data' => [
            'status' => 'healthy',
            'timestamp' => date('Y-m-d H:i:s'),
            'server' => $_SERVER['SERVER_NAME'] ?? 'unknown',
            'php_version' => PHP_VERSION
        ],
        'message' => 'Hosting server is healthy'
    ];
    
    echo json_encode($response);
}

// UTILITY FUNCTIONS FOR PROXY DETECTION
function getClientIP() {
    $ip_keys = ['HTTP_X_FORWARDED_FOR', 'HTTP_X_REAL_IP', 'HTTP_CLIENT_IP', 'REMOTE_ADDR'];
    
    foreach ($ip_keys as $key) {
        if (!empty($_SERVER[$key])) {
            $ips = explode(',', $_SERVER[$key]);
            return trim($ips[0]);
        }
    }
    
    return $_SERVER['REMOTE_ADDR'] ?? 'unknown';
}

function getRealIP() {
    // Try to get real IP from various headers
    if (!empty($_SERVER['HTTP_X_REAL_IP'])) {
        return $_SERVER['HTTP_X_REAL_IP'];
    }
    
    if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
        $ips = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR']);
        return trim(end($ips)); // Last IP is usually the real client IP
    }
    
    return $_SERVER['REMOTE_ADDR'] ?? 'unknown';
}

function detectProxy() {
    // Check for common proxy headers
    $proxy_headers = [
        'HTTP_X_FORWARDED_FOR',
        'HTTP_X_REAL_IP',
        'HTTP_X_FORWARDED_HOST',
        'HTTP_X_FORWARDED_PROTO',
        'HTTP_VIA',
        'HTTP_PROXY_CONNECTION'
    ];
    
    foreach ($proxy_headers as $header) {
        if (!empty($_SERVER[$header])) {
            return true;
        }
    }
    
    return false;
}

function getProxyType() {
    if (!empty($_SERVER['HTTP_VIA'])) {
        return 'http';
    }
    
    if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
        return 'transparent';
    }
    
    if (!empty($_SERVER['HTTP_PROXY_CONNECTION'])) {
        return 'http';
    }
    
    return detectProxy() ? 'unknown' : 'none';
}
?>