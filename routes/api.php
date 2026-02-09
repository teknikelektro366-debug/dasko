<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\Api\SensorDataController;
use App\Http\Controllers\Api\AcControlController;
use App\Http\Controllers\Api\SimpleReportController;

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
    Route::get('/daily', [SimpleReportController::class, 'dailyReport']);
    
    // Weekly report
    Route::get('/weekly', [SimpleReportController::class, 'weeklyReport']);
    
    // Monthly report
    Route::get('/monthly', [SimpleReportController::class, 'monthlyReport']);
    
    // Efficiency report (redirect to monthly for now)
    Route::get('/efficiency', [SimpleReportController::class, 'monthlyReport']);
    
    // Custom report (redirect to daily for now)
    Route::get('/custom', [SimpleReportController::class, 'dailyReport']);
    
    // PDF Download Routes
    Route::get('/pdf/daily', [\App\Http\Controllers\PdfReportController::class, 'dailyReport']);
    Route::get('/pdf/weekly', [\App\Http\Controllers\PdfReportController::class, 'weeklyReport']);
    Route::get('/pdf/monthly', [\App\Http\Controllers\PdfReportController::class, 'monthlyReport']);
    Route::get('/pdf/efficiency', [\App\Http\Controllers\PdfReportController::class, 'efficiencyReport']);
    
    // Custom Report Route
    Route::get('/custom', [\App\Http\Controllers\PdfReportController::class, 'customReport']);
});

// Energy Management Routes
Route::prefix('energy')->group(function () {
    Route::get('/daily', [\App\Http\Controllers\EnergyController::class, 'getDailyEnergy']);
    Route::get('/monthly', [\App\Http\Controllers\EnergyController::class, 'getMonthlyEnergy']);
    Route::get('/efficiency', [\App\Http\Controllers\EnergyController::class, 'getEfficiency']);
    Route::get('/devices', [\App\Http\Controllers\EnergyController::class, 'getDevicePowerData']);
    Route::get('/savings/daily', [\App\Http\Controllers\EnergyController::class, 'getDailySavings']);
    Route::get('/savings/monthly', [\App\Http\Controllers\EnergyController::class, 'getMonthlySavings']);
});