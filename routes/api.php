<?php

use App\Http\Controllers\Api\ArduinoController;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| API Routes
|--------------------------------------------------------------------------
*/

Route::middleware('auth:sanctum')->get('/user', function (Request $request) {
    return $request->user();
});

// Arduino API routes (no authentication required for Arduino device)
Route::prefix('arduino')->name('arduino.')->group(function () {
    
    // Sensor data endpoints
    Route::post('/sensor-data', [ArduinoController::class, 'receiveSensorData'])->name('sensor-data');
    Route::get('/sensor-data', [ArduinoController::class, 'getSensorData'])->name('get-sensor-data');
    
    // Device control endpoints
    Route::post('/control', [ArduinoController::class, 'controlDevice'])->name('control');
    
    // AC automation endpoints
    Route::get('/ac-automation', [ArduinoController::class, 'getACAutomation'])->name('ac-automation');
    Route::post('/ac-automation', [ArduinoController::class, 'updateACAutomation'])->name('update-ac-automation');
    
    // System status
    Route::get('/status', [ArduinoController::class, 'getSystemStatus'])->name('status');
});

// API routes (no authentication required for now)
// Route::middleware(['auth:sanctum'])->group(function () {
    
    // Dashboard API endpoints
    Route::get('/dashboard/stats', [ArduinoController::class, 'getDashboardStats'])->name('api.dashboard.stats');
    Route::get('/dashboard/real-time', [ArduinoController::class, 'getRealTimeData'])->name('api.dashboard.real-time');
    
    // Device management API
    Route::post('/devices/{device}/control', [ArduinoController::class, 'controlDevice'])->name('api.devices.control');
    Route::get('/devices/status', [ArduinoController::class, 'getDevicesStatus'])->name('api.devices.status');
    
    // Energy monitoring API
    Route::get('/energy/current', [ArduinoController::class, 'getCurrentEnergyUsage'])->name('api.energy.current');
    Route::get('/energy/history', [ArduinoController::class, 'getEnergyHistory'])->name('api.energy.history');
    
    // AC automation API
    Route::post('/ac/manual-control', [ArduinoController::class, 'manualACControl'])->name('api.ac.manual');
    Route::post('/ac/automation/toggle', [ArduinoController::class, 'toggleACAutomation'])->name('api.ac.toggle');
// });