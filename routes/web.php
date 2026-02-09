<?php

use App\Http\Controllers\Auth\LoginController;
use App\Http\Controllers\DashboardController;
use App\Http\Controllers\Admin\AdminController;
use App\Http\Controllers\Admin\DeviceController;
use App\Http\Controllers\Admin\UserController;
use App\Http\Controllers\Admin\ACRuleController;
use App\Http\Controllers\Admin\EnergyLogController;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
*/

// Public routes - Direct access to dashboard (no login required for now)
Route::get('/', function () {
    return redirect()->route('dashboard');
});

// Static HTML dashboard route
Route::get('/index.html', function () {
    return view('dashboard-static');
});

Route::get('/dashboard.html', function () {
    return view('dashboard-static');
});

// Authentication routes (disabled for now)
// Route::get('/login', [LoginController::class, 'showLoginForm'])->name('login');
// Route::post('/login', [LoginController::class, 'login']);
// Route::post('/logout', [LoginController::class, 'logout'])->name('logout');

// Public routes (no authentication required for now)
// Route::middleware(['auth'])->group(function () {
    
    // Manual Input Pages
    Route::get('/manual-input', function () {
        return view('manual-input');
    })->name('manual-input');
    
    Route::get('/manual-input-bulk', function () {
        return view('manual-input-bulk');
    })->name('manual-input-bulk');
    
    // Main Dashboard
    Route::get('/dashboard', [DashboardController::class, 'index'])->name('dashboard');
    Route::get('/dashboard/analytics', [DashboardController::class, 'analytics'])->name('dashboard.analytics');
    Route::get('/dashboard/history', [DashboardController::class, 'history'])->name('dashboard.history');
    Route::get('/dashboard/devices', [DashboardController::class, 'devices'])->name('dashboard.devices');
    Route::get('/dashboard/automation', [DashboardController::class, 'automation'])->name('dashboard.automation');
    Route::get('/dashboard/settings', [DashboardController::class, 'settings'])->name('dashboard.settings');
    
    // Reports
    Route::get('/reports', [App\Http\Controllers\ReportController::class, 'index'])->name('reports.index');
    Route::post('/reports/energy', [App\Http\Controllers\ReportController::class, 'energyReport'])->name('reports.energy');
    Route::get('/reports/daily', [App\Http\Controllers\ReportController::class, 'dailyReport'])->name('reports.daily');
    Route::get('/reports/weekly', [App\Http\Controllers\ReportController::class, 'weeklyReport'])->name('reports.weekly');
    Route::get('/reports/monthly', [App\Http\Controllers\ReportController::class, 'monthlyReport'])->name('reports.monthly');
    Route::post('/reports/device', [App\Http\Controllers\ReportController::class, 'deviceReport'])->name('reports.device');
    Route::post('/reports/efficiency', [App\Http\Controllers\ReportController::class, 'efficiencyReport'])->name('reports.efficiency');
    
    // Admin routes (no admin middleware for now)
    // Route::middleware(['admin'])->prefix('admin')->name('admin.')->group(function () {
    Route::prefix('admin')->name('admin.')->group(function () {
        
        // Admin Dashboard
        Route::get('/', [AdminController::class, 'index'])->name('dashboard');
        Route::get('/overview', [AdminController::class, 'overview'])->name('overview');
        
        // User Management
        Route::resource('users', UserController::class);
        Route::post('/users/{user}/toggle-status', [UserController::class, 'toggleStatus'])->name('users.toggle-status');
        
        // Device Management
        Route::resource('devices', DeviceController::class);
        Route::post('/devices/{device}/toggle', [DeviceController::class, 'toggle'])->name('devices.toggle');
        Route::post('/devices/bulk-action', [DeviceController::class, 'bulkAction'])->name('devices.bulk-action');
        
        // AC Automation Rules
        Route::resource('ac-rules', ACRuleController::class);
        Route::post('/ac-rules/{rule}/toggle', [ACRuleController::class, 'toggle'])->name('ac-rules.toggle');
        Route::post('/ac-rules/import', [ACRuleController::class, 'import'])->name('ac-rules.import');
        Route::get('/ac-rules/export', [ACRuleController::class, 'export'])->name('ac-rules.export');
        
        // Energy Logs
        Route::get('/energy-logs', [EnergyLogController::class, 'index'])->name('energy-logs.index');
        Route::get('/energy-logs/{log}', [EnergyLogController::class, 'show'])->name('energy-logs.show');
        Route::delete('/energy-logs/{log}', [EnergyLogController::class, 'destroy'])->name('energy-logs.destroy');
        Route::post('/energy-logs/cleanup', [EnergyLogController::class, 'cleanup'])->name('energy-logs.cleanup');
        Route::get('/energy-logs/export', [EnergyLogController::class, 'export'])->name('energy-logs.export');
        
        // System Settings
        Route::get('/settings', [AdminController::class, 'settings'])->name('settings');
        Route::post('/settings', [AdminController::class, 'updateSettings'])->name('settings.update');
        
        // Arduino Management
        Route::get('/arduino', [AdminController::class, 'arduino'])->name('arduino');
        Route::post('/arduino/test-connection', [AdminController::class, 'testArduinoConnection'])->name('arduino.test');
        Route::post('/arduino/sync', [AdminController::class, 'syncArduino'])->name('arduino.sync');
        Route::post('/arduino/emergency-shutdown', [AdminController::class, 'emergencyShutdown'])->name('arduino.emergency');
        
        // Database Management
        Route::get('/database', [AdminController::class, 'database'])->name('database');
        Route::post('/database/backup', [AdminController::class, 'backupDatabase'])->name('database.backup');
        Route::post('/database/cleanup', [AdminController::class, 'cleanupDatabase'])->name('database.cleanup');
        Route::post('/database/reset', [AdminController::class, 'resetDatabase'])->name('database.reset');
        
        // System Logs
        Route::get('/logs', [AdminController::class, 'logs'])->name('logs');
        Route::post('/logs/clear', [AdminController::class, 'clearLogs'])->name('logs.clear');
    });
// });