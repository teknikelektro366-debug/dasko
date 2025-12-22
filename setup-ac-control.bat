@echo off
echo ================================
echo   SETUP AC CONTROL SYSTEM
echo ================================
echo.

echo [1/4] Running database migrations...
php artisan migrate --force
if %errorlevel% neq 0 (
    echo ERROR: Migration failed!
    pause
    exit /b 1
)
echo ✓ Migrations completed successfully
echo.

echo [2/4] Clearing application cache...
php artisan config:clear
php artisan route:clear
php artisan view:clear
echo ✓ Cache cleared
echo.

echo [3/4] Testing API endpoints...
echo Testing AC control API...
curl -X GET "http://localhost:8000/api/ac/control?device_id=ESP32_Smart_Energy&location=Lab" -H "Accept: application/json"
echo.
echo.

echo [4/4] Starting Laravel development server...
echo Server will start at http://localhost:8000
echo Dashboard available at: http://localhost:8000/dashboard
echo.
echo Press Ctrl+C to stop the server
echo.

start http://localhost:8000/dashboard
php artisan serve --host=0.0.0.0 --port=8000