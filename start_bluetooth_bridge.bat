@echo off
echo ========================================
echo   BLUETOOTH TO LARAVEL BRIDGE STARTER
echo ========================================
echo.

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.7+ from https://python.org
    pause
    exit /b 1
)

echo Python found: 
python --version

REM Check if required modules are installed
echo.
echo Checking required Python modules...

python -c "import bluetooth" >nul 2>&1
if errorlevel 1 (
    echo Installing PyBluez...
    pip install pybluez
    if errorlevel 1 (
        echo ERROR: Failed to install PyBluez
        echo Try: pip install pybluez-win10
        pause
        exit /b 1
    )
) else (
    echo ✓ PyBluez is installed
)

python -c "import requests" >nul 2>&1
if errorlevel 1 (
    echo Installing requests...
    pip install requests
    if errorlevel 1 (
        echo ERROR: Failed to install requests
        pause
        exit /b 1
    )
) else (
    echo ✓ Requests is installed
)

echo.
echo ========================================
echo   STARTING BLUETOOTH BRIDGE
echo ========================================
echo.
echo Instructions:
echo 1. Make sure ESP32 is powered on
echo 2. Make sure Laravel server is running
echo 3. Pair ESP32 with this computer if not already paired
echo.
echo Press any key to start the bridge...
pause >nul

REM Start the bridge
python bluetooth_to_laravel_bridge.py

echo.
echo Bridge stopped. Press any key to exit...
pause >nul