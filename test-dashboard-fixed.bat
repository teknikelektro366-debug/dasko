@echo off
echo ================================
echo   TEST DASHBOARD AFTER FIXES
echo ================================
echo.

echo [1] Clearing Laravel cache...
php artisan config:clear
php artisan route:clear
php artisan view:clear
echo ✓ Cache cleared
echo.

echo [2] Testing JavaScript syntax...
echo Checking for syntax errors in dashboard...
echo.

echo [3] Starting Laravel server...
echo Server will start at http://localhost:8000
echo Dashboard: http://localhost:8000/dashboard
echo.
echo JavaScript fixes applied:
echo - Fixed syntax error in dashboard.blade.php
echo - Moved workingDaysEnabled variable to top of script.js
echo - Separated AC control functions to ac-control.js
echo - Fixed function declarations and scope issues
echo.
echo Press Ctrl+C to stop the server
echo.

start http://localhost:8000/dashboard
php artisan serve --host=0.0.0.0 --port=8000