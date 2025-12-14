@echo off
echo ========================================
echo Smart Energy Management System Setup
echo ========================================
echo.

echo [1/6] Checking PHP installation...
php --version
if %errorlevel% neq 0 (
    echo ERROR: PHP tidak ditemukan!
    echo Silakan install PHP terlebih dahulu dari https://windows.php.net/download/
    echo Atau install XAMPP dari https://www.apachefriends.org/
    pause
    exit /b 1
)

echo.
echo [2/6] Installing Composer dependencies...
composer install
if %errorlevel% neq 0 (
    echo ERROR: Composer install gagal!
    echo Pastikan Composer sudah terinstall dari https://getcomposer.org/
    pause
    exit /b 1
)

echo.
echo [3/6] Generating application key...
php artisan key:generate

echo.
echo [4/6] Creating storage directories...
if not exist "storage\framework\cache\data" mkdir "storage\framework\cache\data"
if not exist "bootstrap\cache" mkdir "bootstrap\cache"

echo.
echo [5/6] Setting up database...
echo Pastikan MySQL sudah berjalan dan database 'smart_energy' sudah dibuat
echo Tekan Enter untuk melanjutkan atau Ctrl+C untuk batal...
pause > nul

php artisan migrate
if %errorlevel% neq 0 (
    echo WARNING: Migration gagal. Pastikan database sudah dibuat dan konfigurasi .env benar
)

php artisan db:seed
if %errorlevel% neq 0 (
    echo WARNING: Seeding gagal. Database mungkin sudah berisi data
)

echo.
echo [6/6] Setup selesai!
echo.
echo Untuk menjalankan aplikasi:
echo   php artisan serve
echo.
echo Aplikasi akan berjalan di: http://localhost:8000
echo.
pause