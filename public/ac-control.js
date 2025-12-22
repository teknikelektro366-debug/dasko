// ===== AC CONTROL FUNCTIONS =====
let currentACState = {
    mode: 'auto',
    ac1_status: false,
    ac2_status: false,
    ac1_temperature: 25,
    ac2_temperature: 25
};

function initializeACControl() {
    // Load current AC control status
    loadACControlStatus();
}

function loadACControlStatus() {
    fetch('/api/ac/control')
        .then(response => response.json())
        .then(data => {
            if (data.success && data.data.control_mode !== 'auto') {
                currentACState = {
                    mode: data.data.control_mode,
                    ac1_status: data.data.ac1_status,
                    ac2_status: data.data.ac2_status,
                    ac1_temperature: data.data.ac1_temperature,
                    ac2_temperature: data.data.ac2_temperature
                };
                updateACControlUI();
                updateControlStatus(data.data);
            }
        })
        .catch(error => {
            console.log('No active AC control found, using auto mode');
        });
}

function setControlMode(mode) {
    const autoBtn = document.getElementById('autoModeBtn');
    const manualBtn = document.getElementById('manualModeBtn');
    const manualControls = document.getElementById('manualControls');
    const autoModeInfo = document.getElementById('autoModeInfo');

    if (mode === 'auto') {
        autoBtn.classList.add('active');
        manualBtn.classList.remove('active');
        manualControls.style.display = 'none';
        autoModeInfo.style.display = 'block';
        currentACState.mode = 'auto';
        
        // Return to auto mode via API
        returnToAutoMode();
    } else {
        autoBtn.classList.remove('active');
        manualBtn.classList.add('active');
        manualControls.style.display = 'block';
        autoModeInfo.style.display = 'none';
        currentACState.mode = 'manual';
    }
}

function toggleAC(unit) {
    const toggle = document.getElementById(`ac${unit}Toggle`);
    const status = document.getElementById(`ac${unit}Status`);
    
    if (unit === 1) {
        currentACState.ac1_status = !currentACState.ac1_status;
        toggle.classList.toggle('active', currentACState.ac1_status);
        status.textContent = currentACState.ac1_status ? 'ON' : 'OFF';
    } else {
        currentACState.ac2_status = !currentACState.ac2_status;
        toggle.classList.toggle('active', currentACState.ac2_status);
        status.textContent = currentACState.ac2_status ? 'ON' : 'OFF';
    }
}

function changeTemp(unit, delta) {
    const tempSpan = document.getElementById(`ac${unit}Temp`);
    let currentTemp = parseInt(tempSpan.textContent);
    let newTemp = currentTemp + delta;
    
    // Limit temperature range
    newTemp = Math.max(16, Math.min(30, newTemp));
    
    if (unit === 1) {
        currentACState.ac1_temperature = newTemp;
    } else {
        currentACState.ac2_temperature = newTemp;
    }
    
    tempSpan.textContent = newTemp;
}

function updateACControlUI() {
    // Update toggles
    const ac1Toggle = document.getElementById('ac1Toggle');
    const ac2Toggle = document.getElementById('ac2Toggle');
    
    if (ac1Toggle) {
        ac1Toggle.classList.toggle('active', currentACState.ac1_status);
    }
    if (ac2Toggle) {
        ac2Toggle.classList.toggle('active', currentACState.ac2_status);
    }
    
    // Update status text
    const ac1Status = document.getElementById('ac1Status');
    const ac2Status = document.getElementById('ac2Status');
    
    if (ac1Status) {
        ac1Status.textContent = currentACState.ac1_status ? 'ON' : 'OFF';
    }
    if (ac2Status) {
        ac2Status.textContent = currentACState.ac2_status ? 'ON' : 'OFF';
    }
    
    // Update temperatures
    const ac1Temp = document.getElementById('ac1Temp');
    const ac2Temp = document.getElementById('ac2Temp');
    
    if (ac1Temp) {
        ac1Temp.textContent = currentACState.ac1_temperature;
    }
    if (ac2Temp) {
        ac2Temp.textContent = currentACState.ac2_temperature;
    }
}

function applyACControl() {
    const duration = document.getElementById('controlDuration').value;
    
    const controlData = {
        device_id: 'ESP32_Smart_Energy_ChangeDetection',
        location: 'Lab Teknik Tegangan Tinggi',
        ac1_status: currentACState.ac1_status,
        ac2_status: currentACState.ac2_status,
        ac1_temperature: currentACState.ac1_temperature,
        ac2_temperature: currentACState.ac2_temperature,
        control_mode: 'manual',
        duration_minutes: duration ? parseInt(duration) : null,
        created_by: 'dashboard_user'
    };

    fetch('/api/ac/control', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content')
        },
        body: JSON.stringify(controlData)
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            alert('✓ Pengaturan AC berhasil diterapkan!');
            updateControlStatus(data.data);
        } else {
            alert('✗ Gagal menerapkan pengaturan: ' + data.message);
        }
    })
    .catch(error => {
        console.error('Error:', error);
        alert('✗ Terjadi kesalahan saat menerapkan pengaturan');
    });
}

function emergencyStopAC() {
    if (confirm('Apakah Anda yakin ingin mematikan semua AC secara darurat?')) {
        fetch('/api/ac/emergency-stop', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content')
            },
            body: JSON.stringify({
                device_id: 'ESP32_Smart_Energy_ChangeDetection',
                location: 'Lab Teknik Tegangan Tinggi'
            })
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                alert('✓ Emergency stop berhasil! Semua AC dimatikan.');
                // Reset UI
                currentACState.ac1_status = false;
                currentACState.ac2_status = false;
                updateACControlUI();
                updateControlStatus(data.data);
            } else {
                alert('✗ Gagal melakukan emergency stop: ' + data.message);
            }
        })
        .catch(error => {
            console.error('Error:', error);
            alert('✗ Terjadi kesalahan saat emergency stop');
        });
    }
}

function returnToAutoMode() {
    fetch('/api/ac/auto-mode', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content')
        },
        body: JSON.stringify({
            device_id: 'ESP32_Smart_Energy_ChangeDetection',
            location: 'Lab Teknik Tegangan Tinggi'
        })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            updateControlStatus({
                control_mode: 'auto',
                manual_override: false,
                message: 'Arduino mengontrol AC berdasarkan jumlah orang'
            });
        }
    })
    .catch(error => {
        console.error('Error returning to auto mode:', error);
    });
}

function updateControlStatus(controlData) {
    const statusDisplay = document.getElementById('controlStatus');
    if (!statusDisplay) return;
    
    let statusText = '';
    
    if (controlData.control_mode === 'auto') {
        statusText = 'Mode: Otomatis | Arduino mengontrol AC berdasarkan jumlah orang';
    } else {
        const ac1Text = controlData.ac1_status ? `AC1: ON (${controlData.ac1_temperature}°C)` : 'AC1: OFF';
        const ac2Text = controlData.ac2_status ? `AC2: ON (${controlData.ac2_temperature}°C)` : 'AC2: OFF';
        statusText = `Mode: Manual | ${ac1Text}, ${ac2Text}`;
        
        if (controlData.expires_at) {
            const expiryDate = new Date(controlData.expires_at);
            statusText += ` | Berakhir: ${expiryDate.toLocaleString('id-ID')}`;
        }
    }
    
    statusDisplay.textContent = statusText;
}

// Auto refresh page function
function autoRefreshPage() {
    // Auto refresh setiap 30 detik untuk update data
    setTimeout(function() {
        window.location.reload();
    }, 30000);
}

// Initialize AC control when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    console.log('AC Control System - Initialized');
    
    // Initialize AC control
    initializeACControl();
    
    // Start auto refresh
    autoRefreshPage();
    
    // Add manual refresh button
    const navbar = document.querySelector('.navbar');
    if (navbar) {
        const refreshBtn = document.createElement('button');
        refreshBtn.innerHTML = '<i class="fas fa-sync-alt"></i> Refresh';
        refreshBtn.onclick = () => window.location.reload();
        refreshBtn.style.cssText = 'margin-left: auto; padding: 5px 10px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer;';
        navbar.appendChild(refreshBtn);
    }
});