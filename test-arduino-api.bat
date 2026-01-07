@echo off
echo ===============================================
echo Testing Arduino API Connection
echo ===============================================
echo.

echo 1. Testing GET /api/sensor/data endpoint...
powershell -Command "try { $response = Invoke-WebRequest -Uri 'http://localhost:8000/api/sensor/data' -Method GET; Write-Host 'SUCCESS: GET endpoint working' -ForegroundColor Green; Write-Host 'Status Code:' $response.StatusCode; } catch { Write-Host 'ERROR: GET endpoint failed' -ForegroundColor Red; Write-Host $_.Exception.Message; }"
echo.

echo 2. Testing GET /api/sensor/latest endpoint...
powershell -Command "try { $response = Invoke-WebRequest -Uri 'http://localhost:8000/api/sensor/latest' -Method GET; Write-Host 'SUCCESS: Latest endpoint working' -ForegroundColor Green; Write-Host 'Status Code:' $response.StatusCode; } catch { Write-Host 'ERROR: Latest endpoint failed' -ForegroundColor Red; Write-Host $_.Exception.Message; }"
echo.

echo 3. Testing POST /api/sensor/data endpoint (simulating Arduino)...
powershell -Command "$body = @{ device_id='ESP32_Smart_Energy_UNJA'; location='Ruang Dosen Prodi Teknik Elektro - UNJA'; people_count=3; ac_status='AUTO 1 AC'; set_temperature=25; room_temperature=28.5; humidity=65.2; light_level=450; proximity_in=$false; proximity_out=$false; wifi_rssi=-45; status='active'; timestamp=[int][double]::Parse((Get-Date -UFormat %%s)) } | ConvertTo-Json; try { $response = Invoke-WebRequest -Uri 'http://localhost:8000/api/sensor/data' -Method POST -Body $body -ContentType 'application/json'; Write-Host 'SUCCESS: POST endpoint working' -ForegroundColor Green; Write-Host 'Status Code:' $response.StatusCode; } catch { Write-Host 'ERROR: POST endpoint failed' -ForegroundColor Red; Write-Host $_.Exception.Message; }"
echo.

echo 4. Testing AC Control endpoint...
powershell -Command "try { $response = Invoke-WebRequest -Uri 'http://localhost:8000/api/ac/control?device_id=ESP32_Smart_Energy_UNJA' -Method GET; Write-Host 'SUCCESS: AC Control endpoint working' -ForegroundColor Green; Write-Host 'Status Code:' $response.StatusCode; } catch { Write-Host 'ERROR: AC Control endpoint failed' -ForegroundColor Red; Write-Host $_.Exception.Message; }"
echo.

echo ===============================================
echo API Test Complete!
echo ===============================================
echo.
echo Next Steps:
echo 1. Upload Arduino code to ESP32
echo 2. Configure WiFi credentials in Arduino code
echo 3. Monitor Serial output for connection status
echo 4. Check Laravel dashboard for incoming data
echo.
pause