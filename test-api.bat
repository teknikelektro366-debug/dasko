@echo off
echo ===============================================
echo    TESTING LARAVEL API FOR ESP32 INTEGRATION
echo ===============================================
echo.

REM Check if PHP is available
php --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: PHP tidak ditemukan di PATH
    echo Pastikan PHP sudah terinstall dan ada di PATH
    echo.
    pause
    exit /b 1
)

echo PHP ditemukan, menjalankan test...
echo.

REM Run the test script
php test_api.php

echo.
echo ===============================================
echo Jika ada error, pastikan:
echo 1. Laravel server berjalan (php artisan serve)
echo 2. Database sudah di-migrate
echo 3. URL di test_api.php sudah benar
echo ===============================================
echo.
pause