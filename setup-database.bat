@echo off
echo ========================================
echo Smart Energy Management Database Setup
echo ========================================
echo.

echo [1/4] Running database migrations...
php artisan migrate:fresh
if %errorlevel% neq 0 (
    echo ERROR: Migration failed!
    pause
    exit /b 1
)

echo.
echo [2/4] Seeding database with dummy data...
php artisan db:seed
if %errorlevel% neq 0 (
    echo ERROR: Seeding failed!
    pause
    exit /b 1
)

echo.
echo [3/4] Clearing application cache...
php artisan cache:clear
php artisan config:clear
php artisan route:clear
php artisan view:clear

echo.
echo [4/4] Database setup completed successfully!
echo.
echo You can now access the application at: http://localhost:8000
echo.
echo Available demo data:
echo - 12 devices (AC, Lampu, Komputer, TV, dll.)
echo - 7 AC automation rules
echo - 24 hours of sensor readings
echo - Energy consumption logs
echo.
pause