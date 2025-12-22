// ===== AC CONTROL FUNCTIONS FOR HOSTING =====
let currentACState = {
    mode: 'auto',
    ac1_status: false,
    ac2_status: false,
    ac1_temperature: 25,
    ac2_temperature: 25
};

// Hosting configuration
const HOSTING_CONFIG = {
    domain: 'dasko.fst.unja.ac.id',
    baseUrl: 'https://dasko.fst.unja.ac.id',
    apiUrl: 'https://dasko.fst.unja.ac.id/api',
    sslEnabled: true
};

function initializeACControl() {
    console.log('Initializing AC Control for hosting:', HOSTING_CONFIG.domain);
    // Load current AC control status
    loadACControlStatus();
}

function loadACControlStatus() {
    const url = `${HOSTING_CONFIG.apiUrl}/ac/control`;
    
    fetch(url, {
        method: 'GET',
        headers: {
            'Accept': 'application/json',
            'X-Requested-With': 'XMLHttpRequest'
        }
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        return response.json();
    })
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
            console.log('✓ AC Control loaded from hosting');
        }
    })
    .catch(error => {
        console.log('No active AC control found, using auto mode:', error.message);
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
        device_id: 'ESP32_Smart_Energy_Production',
        location: 'Lab Teknik Tegangan Tinggi',
        ac1_status: currentACState.ac1_status,
        ac2_status: currentACState.ac2_status,
        ac1_temperature: currentACState.ac1_temperature,
        ac2_temperature: currentACState.ac2_temperature,
        control_mode: 'manual',
        duration_minutes: duration ? parseInt(duration) : null,
        created_by: 'hosting_dashboard'
    };

    // Show loading state
    const applyBtn = document.querySelector('.apply-btn');
    const originalText = applyBtn.innerHTML;
    applyBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Menerapkan...';
    applyBtn.disabled = true;

    fetch(`${HOSTING_CONFIG.apiUrl}/ac/control`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Accept': 'application/json',
            'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content'),
            'X-Requested-With': 'XMLHttpRequest'
        },
        body: JSON.stringify(controlData)
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        return response.json();
    })
    .then(data => {
        if (data.success) {
            showNotification('✓ Pengaturan AC berhasil diterapkan ke hosting!', 'success');
            updateControlStatus(data.data);
        } else {
            throw new Error(data.message || 'Unknown error');
        }
    })
    .catch(error => {
        console.error('Error:', error);
        showNotification('✗ Gagal menerapkan pengaturan: ' + error.message, 'error');
    })
    .finally(() => {
        // Restore button state
        applyBtn.innerHTML = originalText;
        applyBtn.disabled = false;
    });
}

function emergencyStopAC() {
    if (confirm('Apakah Anda yakin ingin mematikan semua AC secara darurat?')) {
        const emergencyBtn = document.querySelector('.emergency-btn');
        const originalText = emergencyBtn.innerHTML;
        emergencyBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Menghentikan...';
        emergencyBtn.disabled = true;

        fetch(`${HOSTING_CONFIG.apiUrl}/ac/emergency-stop`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Accept': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content'),
                'X-Requested-With': 'XMLHttpRequest'
            },
            body: JSON.stringify({
                device_id: 'ESP32_Smart_Energy_Production',
                location: 'Lab Teknik Tegangan Tinggi'
            })
        })
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }
            return response.json();
        })
        .then(data => {
            if (data.success) {
                showNotification('✓ Emergency stop berhasil! Semua AC dimatikan.', 'success');
                // Reset UI
                currentACState.ac1_status = false;
                currentACState.ac2_status = false;
                updateACControlUI();
                updateControlStatus(data.data);
            } else {
                throw new Error(data.message || 'Unknown error');
            }
        })
        .catch(error => {
            console.error('Error:', error);
            showNotification('✗ Gagal melakukan emergency stop: ' + error.message, 'error');
        })
        .finally(() => {
            // Restore button state
            emergencyBtn.innerHTML = originalText;
            emergencyBtn.disabled = false;
        });
    }
}

function returnToAutoMode() {
    fetch(`${HOSTING_CONFIG.apiUrl}/ac/auto-mode`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Accept': 'application/json',
            'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').getAttribute('content'),
            'X-Requested-With': 'XMLHttpRequest'
        },
        body: JSON.stringify({
            device_id: 'ESP32_Smart_Energy_Production',
            location: 'Lab Teknik Tegangan Tinggi'
        })
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        return response.json();
    })
    .then(data => {
        if (data.success) {
            updateControlStatus({
                control_mode: 'auto',
                manual_override: false,
                message: 'Arduino mengontrol AC berdasarkan jumlah orang'
            });
            showNotification('✓ Kembali ke mode otomatis', 'info');
        }
    })
    .catch(error => {
        console.error('Error returning to auto mode:', error);
        showNotification('✗ Gagal kembali ke mode otomatis: ' + error.message, 'error');
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

function showNotification(message, type = 'info') {
    // Create notification element
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.innerHTML = `
        <div class="notification-content">
            <span class="notification-message">${message}</span>
            <button class="notification-close" onclick="this.parentElement.parentElement.remove()">×</button>
        </div>
    `;
    
    // Add styles if not already added
    if (!document.getElementById('notification-styles')) {
        const styles = document.createElement('style');
        styles.id = 'notification-styles';
        styles.textContent = `
            .notification {
                position: fixed;
                top: 20px;
                right: 20px;
                padding: 15px 20px;
                border-radius: 5px;
                color: white;
                font-weight: bold;
                z-index: 10000;
                max-width: 400px;
                box-shadow: 0 4px 6px rgba(0,0,0,0.1);
                animation: slideIn 0.3s ease-out;
            }
            .notification-success { background: #28a745; }
            .notification-error { background: #dc3545; }
            .notification-info { background: #17a2b8; }
            .notification-warning { background: #ffc107; color: #212529; }
            .notification-content {
                display: flex;
                justify-content: space-between;
                align-items: center;
            }
            .notification-close {
                background: none;
                border: none;
                color: inherit;
                font-size: 18px;
                cursor: pointer;
                margin-left: 10px;
            }
            @keyframes slideIn {
                from { transform: translateX(100%); opacity: 0; }
                to { transform: translateX(0); opacity: 1; }
            }
        `;
        document.head.appendChild(styles);
    }
    
    // Add to page
    document.body.appendChild(notification);
    
    // Auto remove after 5 seconds
    setTimeout(() => {
        if (notification.parentElement) {
            notification.remove();
        }
    }, 5000);
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
    console.log('AC Control System - Hosting Production');
    console.log('Domain:', HOSTING_CONFIG.domain);
    console.log('SSL Enabled:', HOSTING_CONFIG.sslEnabled);
    
    // Initialize AC control
    initializeACControl();
    
    // Start auto refresh
    autoRefreshPage();
    
    // Add hosting info to navbar
    const navbar = document.querySelector('.navbar');
    if (navbar) {
        // Add hosting indicator
        const hostingIndicator = document.createElement('span');
        hostingIndicator.innerHTML = '<i class="fas fa-cloud"></i> HOSTING';
        hostingIndicator.style.cssText = 'margin-left: 10px; padding: 2px 8px; background: #28a745; color: white; border-radius: 3px; font-size: 11px; font-weight: bold;';
        navbar.appendChild(hostingIndicator);
        
        // Add manual refresh button
        const refreshBtn = document.createElement('button');
        refreshBtn.innerHTML = '<i class="fas fa-sync-alt"></i> Refresh';
        refreshBtn.onclick = () => window.location.reload();
        refreshBtn.style.cssText = 'margin-left: 10px; padding: 5px 10px; background: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer;';
        navbar.appendChild(refreshBtn);
    }
    
    // Add connection status check
    setInterval(checkHostingConnection, 60000); // Check every minute
});

function checkHostingConnection() {
    fetch(`${HOSTING_CONFIG.apiUrl}/ac/control`, {
        method: 'HEAD',
        headers: {
            'Accept': 'application/json',
            'X-Requested-With': 'XMLHttpRequest'
        }
    })
    .then(response => {
        const indicator = document.querySelector('.navbar span');
        if (indicator && indicator.innerHTML.includes('HOSTING')) {
            if (response.ok) {
                indicator.style.background = '#28a745';
                indicator.innerHTML = '<i class="fas fa-cloud"></i> HOSTING';
            } else {
                indicator.style.background = '#dc3545';
                indicator.innerHTML = '<i class="fas fa-cloud-slash"></i> HOSTING ERROR';
            }
        }
    })
    .catch(error => {
        const indicator = document.querySelector('.navbar span');
        if (indicator && indicator.innerHTML.includes('HOSTING')) {
            indicator.style.background = '#dc3545';
            indicator.innerHTML = '<i class="fas fa-cloud-slash"></i> HOSTING OFFLINE';
        }
    });
}