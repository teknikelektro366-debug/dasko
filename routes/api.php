<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\Api\SensorDataController;
use App\Http\Controllers\Api\AcControlController;
use App\Http\Controllers\Api\ReportController;

/*
|--------------------------------------------------------------------------
| API Routes
|--------------------------------------------------------------------------
|
| Here is where you can register API routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned the "api" middleware group. Make something great!
|
*/

Route::middleware('auth:sanctum')->get('/user', function (Request $request) {
    return $request->user();
});

// API untuk Arduino ESP32 - Tidak perlu authentication
Route::prefix('sensor')->group(function () {
    // Endpoint untuk Arduino mengirim data
    Route::post('/data', [SensorDataController::class, 'store']);
    
    // Endpoint untuk testing - GET method
    Route::get('/data', [SensorDataController::class, 'index']);
    
    // Endpoint untuk mendapatkan data terbaru
    Route::get('/latest', [SensorDataController::class, 'latest']);
    
    // Endpoint untuk data chart
    Route::get('/chart', [SensorDataController::class, 'chartData']);
    
    // Endpoint untuk statistik harian
    Route::get('/stats', [SensorDataController::class, 'dailyStats']);
    
    // Endpoint untuk history data
    Route::get('/history', [SensorDataController::class, 'history']);
    
    // Endpoint untuk cleanup data lama
    Route::post('/cleanup', [SensorDataController::class, 'cleanup']);
});

// API untuk dashboard realtime
Route::prefix('dashboard')->group(function () {
    // Get data realtime untuk dashboard
    Route::get('/realtime', [SensorDataController::class, 'latest']);
    
    // Get chart data untuk dashboard
    Route::get('/chart/{hours?}', [SensorDataController::class, 'chartData']);
    
    // Get daily statistics
    Route::get('/stats/daily', [SensorDataController::class, 'dailyStats']);
});

// API untuk kontrol AC
Route::prefix('ac')->group(function () {
    // Arduino endpoints - untuk mendapatkan perintah kontrol
    Route::get('/control', [AcControlController::class, 'getControl']);
    
    // Dashboard endpoints - untuk mengatur kontrol AC
    Route::post('/control', [AcControlController::class, 'setControl']);
    Route::post('/emergency-stop', [AcControlController::class, 'emergencyStop']);
    Route::post('/auto-mode', [AcControlController::class, 'autoMode']);
    Route::get('/history', [AcControlController::class, 'history']);
});

// API untuk laporan PDF
Route::prefix('reports')->group(function () {
    // Daily report
    Route::get('/daily', [ReportController::class, 'dailyReport']);
    
    // Weekly report
    Route::get('/weekly', [ReportController::class, 'weeklyReport']);
    
    // Monthly report
    Route::get('/monthly', [ReportController::class, 'monthlyReport']);
    
    // Efficiency report
    Route::get('/efficiency', [ReportController::class, 'efficiencyReport']);
    
    // Custom report
    Route::get('/custom', [ReportController::class, 'customReport']);
});