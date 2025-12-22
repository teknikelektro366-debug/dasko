@echo off
echo ================================
echo   TEST AC CONTROL API
echo ================================
echo.

set BASE_URL=http://localhost:8000/api/ac
set DEVICE_ID=ESP32_Smart_Energy_ChangeDetection
set LOCATION=Lab Teknik Tegangan Tinggi

echo [1] Testing GET AC Control Status...
echo URL: %BASE_URL%/control
curl -X GET "%BASE_URL%/control?device_id=%DEVICE_ID%&location=%LOCATION%" -H "Accept: application/json"
echo.
echo.

echo [2] Testing SET Manual AC Control...
echo URL: %BASE_URL%/control
curl -X POST "%BASE_URL%/control" ^
  -H "Content-Type: application/json" ^
  -H "Accept: application/json" ^
  -d "{\"device_id\":\"%DEVICE_ID%\",\"location\":\"%LOCATION%\",\"ac1_status\":true,\"ac2_status\":false,\"ac1_temperature\":24,\"ac2_temperature\":25,\"control_mode\":\"manual\",\"duration_minutes\":30,\"created_by\":\"test_user\"}"
echo.
echo.

echo [3] Testing GET AC Control Status (after manual set)...
curl -X GET "%BASE_URL%/control?device_id=%DEVICE_ID%&location=%LOCATION%" -H "Accept: application/json"
echo.
echo.

echo [4] Testing Emergency Stop...
echo URL: %BASE_URL%/emergency-stop
curl -X POST "%BASE_URL%/emergency-stop" ^
  -H "Content-Type: application/json" ^
  -H "Accept: application/json" ^
  -d "{\"device_id\":\"%DEVICE_ID%\",\"location\":\"%LOCATION%\"}"
echo.
echo.

echo [5] Testing Return to Auto Mode...
echo URL: %BASE_URL%/auto-mode
curl -X POST "%BASE_URL%/auto-mode" ^
  -H "Content-Type: application/json" ^
  -H "Accept: application/json" ^
  -d "{\"device_id\":\"%DEVICE_ID%\",\"location\":\"%LOCATION%\"}"
echo.
echo.

echo [6] Testing AC Control History...
echo URL: %BASE_URL%/history
curl -X GET "%BASE_URL%/history?device_id=%DEVICE_ID%&location=%LOCATION%&per_page=5" -H "Accept: application/json"
echo.
echo.

echo ================================
echo   API TESTING COMPLETED
echo ================================
echo.
echo Check the responses above for any errors.
echo All endpoints should return JSON with "success": true
echo.
pause