@echo off
echo Starting Laravel server for network access...
echo Server will be accessible from ESP32 at: http://192.168.0.111:8000
echo.
echo Press Ctrl+C to stop the server
echo.
php artisan serve --host=0.0.0.0 --port=8000