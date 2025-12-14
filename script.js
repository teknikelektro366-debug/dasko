/* ===== NAVIGATION ANTAR HALAMAN ===== */
function showSection(id, clickedElement) {
    // Sembunyikan semua section
    document.querySelectorAll("section").forEach(sec => sec.style.display = "none");
    
    // Tampilkan section yang dipilih
    document.getElementById(id).style.display = "block";
    
    // Update active menu
    document.querySelectorAll(".sidebar ul li a").forEach(link => {
        link.classList.remove("active");
    });
    
    // Tambahkan class active ke menu yang diklik
    if (clickedElement) {
        clickedElement.classList.add("active");
    }
}

/* ===== SIDEBAR TOGGLE ===== */
function toggleSidebar() {
    const sidebar = document.getElementById('sidebar');
    const toggleIcon = document.getElementById('toggleIcon');
    
    sidebar.classList.toggle('collapsed');
    
    // Update icon
    if (sidebar.classList.contains('collapsed')) {
        toggleIcon.className = 'fas fa-chevron-right';
    } else {
        toggleIcon.className = 'fas fa-bars';
    }
}

/* ===== SIMPLE WAKTU REALTIME WIB ===== */
function updateWIB() {
    const now = new Date();
    let waktuWIB = now.toLocaleString("id-ID", {
        timeZone: "Asia/Jakarta",
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit"
    });
    
    const timeElement = document.getElementById("waktuRealtime");
    if (timeElement) {
        timeElement.innerHTML = '<i class="fas fa-clock"></i> ' + waktuWIB + " (WIB)";
    }
}

// Enhanced time update with smooth animations
setInterval(updateWIB, 1000);
updateWIB();

/* ===== SMOOTH VALUE UPDATE ANIMATIONS ===== */
function updateValueWithAnimation(elementId, newValue, suffix = '') {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    // Add updating animation
    element.classList.add('updating');
    
    setTimeout(() => {
        element.textContent = newValue + suffix;
        element.classList.remove('updating');
    }, 300);
}

/* ===== SMOOTH STATUS UPDATE ANIMATIONS ===== */
function updateStatusWithAnimation(elementId, newStatus, statusClass) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    // Only animate if status actually changed
    if (element.textContent !== newStatus) {
        element.style.transform = 'scale(0.8)';
        element.style.opacity = '0.5';
        
        setTimeout(() => {
            element.textContent = newStatus;
            element.className = statusClass;
            element.style.transform = 'scale(1)';
            element.style.opacity = '1';
        }, 150);
    }
}

/* ===== DATA PERANGKAT BERDASARKAN TABEL ===== */
const deviceData = {
    ac: { name: 'AC Panasonic', units: 2, power: 1050, consumption: 10.8, startTime: null, totalUsage: 0 },
    lamp: { name: 'Lampu TL', units: 12, power: 22, consumption: 2.112, startTime: null, totalUsage: 0 },
    dispenser: { name: 'Dispenser Sanken', units: 1, power: 420, consumption: 3.36, startTime: null, totalUsage: 0 },
    tv: { name: 'Smart TV Horizon', units: 1, power: 170, consumption: 0.17, startTime: null, totalUsage: 0 },
    fridge: { name: 'Kulkas Sharp', units: 1, power: 90, consumption: 0.72, startTime: null, totalUsage: 0, alwaysOn: true },
    computer: { name: 'Komputer', units: 2, power: 180, consumption: 1.44, startTime: null, totalUsage: 0 }, // 90W × 2 units
    router: { name: 'Router Wi-Fi', units: 1, power: 20, consumption: 0.16, startTime: null, totalUsage: 0, alwaysOn: true },
    panel: { name: 'Panel Listrik', units: 1, power: 300, consumption: 0.3, startTime: null, totalUsage: 0, alwaysOn: false },
    kettle: { name: 'Teko Listrik', units: 1, power: 350, consumption: 0.35, startTime: null, totalUsage: 0 },
    coffee: { name: 'Mesin Kopi', units: 1, power: 1350, consumption: 1.35, startTime: null, totalUsage: 0 }
};

/* ===== SISTEM TRACKING HARIAN (7 AM - 5 PM) ===== */
let dailyData = {
    date: new Date().toDateString(),
    startTime: null,
    endTime: null,
    devices: {},
    totalPeople: 0,
    avgTemp: [],
    isWorkingHours: false
};

// Initialize device tracking
Object.keys(deviceData).forEach(deviceId => {
    dailyData.devices[deviceId] = {
        usage: 0,
        startTime: null
    };
});

let historyData = JSON.parse(localStorage.getItem('energyHistory')) || [];

// Cek apakah dalam jam kerja (7 AM - 5 PM) dan hari kerja (Senin-Jumat)
let workingDaysEnabled = true;

function isWorkingDay() {
    if (!workingDaysEnabled) return true; // Jika disabled, anggap setiap hari adalah hari kerja
    
    const now = new Date();
    const dayOfWeek = now.getDay(); // 0 = Minggu, 1 = Senin, ..., 6 = Sabtu
    return dayOfWeek >= 1 && dayOfWeek <= 5; // Senin (1) sampai Jumat (5)
}

function isWorkingHours() {
    const now = new Date();
    const hour = now.getHours();
    
    // Jam operasional: 07:00 - 17:00
    const isOperatingTime = hour >= 7 && hour < 17;
    const isWorkingDayToday = isWorkingDay();
    
    return isOperatingTime && isWorkingDayToday;
}

// Reset data harian jika hari sudah berganti
function checkDailyReset() {
    const today = new Date().toDateString();
    if (dailyData.date !== today) {
        // Simpan data kemarin ke history jika ada
        if (dailyData.lampUsage > 0 || dailyData.acUsage > 0) {
            saveToHistory();
        }
        
        // Reset data untuk hari baru
        dailyData = {
            date: today,
            startTime: null,
            endTime: null,
            devices: {},
            totalPeople: 0,
            avgTemp: [],
            isWorkingHours: false
        };
        
        // Initialize device tracking
        Object.keys(deviceData).forEach(deviceId => {
            dailyData.devices[deviceId] = {
                usage: 0,
                startTime: null
            };
        });
    }
}

// Simpan data ke history
function saveToHistory() {
    const avgTemp = dailyData.avgTemp.length > 0 
        ? Math.round(dailyData.avgTemp.reduce((a, b) => a + b, 0) / dailyData.avgTemp.length)
        : 27;
    
    const stats = getDeviceUsageStats();
    
    const historyEntry = {
        date: dailyData.date,
        avgTemp: avgTemp,
        totalEnergi: stats.totalEnergy,
        totalPeople: dailyData.totalPeople,
        devices: {},
        timestamp: new Date().toISOString()
    };
    
    // Save individual device data with detailed tracking
    Object.keys(deviceData).forEach(deviceId => {
        const deviceUsage = dailyData.devices[deviceId].usage;
        const deviceEnergy = hitungEnergiDevice(deviceId, deviceUsage);
        
        historyEntry.devices[deviceId] = {
            usage: Math.round(deviceUsage),
            energy: deviceEnergy,
            percentage: stats.totalEnergy > 0 ? ((deviceEnergy / stats.totalEnergy) * 100).toFixed(1) : 0,
            deviceName: deviceData[deviceId].name,
            power: deviceData[deviceId].power
        };
    });
    
    // Calculate breakdown by category
    historyEntry.breakdown = {
        ac: {
            totalUsage: historyEntry.devices.ac.usage,
            totalEnergy: historyEntry.devices.ac.energy,
            percentage: historyEntry.devices.ac.percentage
        },
        lamp: {
            totalUsage: historyEntry.devices.lamp.usage,
            totalEnergy: historyEntry.devices.lamp.energy,
            percentage: historyEntry.devices.lamp.percentage
        },
        otherDevices: {
            totalUsage: ['computer', 'tv', 'dispenser', 'kettle', 'coffee'].reduce((sum, deviceId) => 
                sum + (historyEntry.devices[deviceId]?.usage || 0), 0),
            totalEnergy: ['computer', 'tv', 'dispenser', 'kettle', 'coffee'].reduce((sum, deviceId) => 
                sum + (historyEntry.devices[deviceId]?.energy || 0), 0),
            devices: ['computer', 'tv', 'dispenser', 'kettle', 'coffee'].map(deviceId => ({
                id: deviceId,
                name: deviceData[deviceId].name,
                usage: historyEntry.devices[deviceId]?.usage || 0,
                energy: historyEntry.devices[deviceId]?.energy || 0
            }))
        },
        alwaysOn: {
            totalEnergy: ['fridge', 'router', 'panel'].reduce((sum, deviceId) => 
                sum + (historyEntry.devices[deviceId]?.energy || 0), 0),
            devices: ['fridge', 'router', 'panel'].map(deviceId => ({
                id: deviceId,
                name: deviceData[deviceId].name,
                energy: historyEntry.devices[deviceId]?.energy || 0
            }))
        }
    };
    
    // Legacy fields for backward compatibility
    historyEntry.lampMinutes = historyEntry.devices.lamp ? historyEntry.devices.lamp.usage : 0;
    historyEntry.acMinutes = historyEntry.devices.ac ? historyEntry.devices.ac.usage : 0;
    historyEntry.energiLampu = historyEntry.devices.lamp ? historyEntry.devices.lamp.energy : 0;
    historyEntry.energiAC = historyEntry.devices.ac ? historyEntry.devices.ac.energy : 0;
    
    // Tambah ke array history
    historyData.unshift(historyEntry);
    
    // Batasi history maksimal 30 hari
    if (historyData.length > 30) {
        historyData = historyData.slice(0, 30);
    }
    
    // Simpan ke localStorage
    localStorage.setItem('energyHistory', JSON.stringify(historyData));
    
    // Update tampilan history
    updateHistoryDisplay();
    
    // Log untuk debugging
    console.log('History saved:', historyEntry);
}

/* ===== UNIVERSAL DEVICE TOGGLE ===== */
async function toggleDevice(deviceId) {
    checkDailyReset();
    
    const device = deviceData[deviceId];
    if (!device) return;
    
    const statusElement = document.getElementById(deviceId + "Status");
    if (!statusElement) return;
    
    // Special handling for panel listrik
    if (deviceId === 'panel') {
        togglePanelListrik();
        return;
    }
    
    // Check if panel is off - if so, prevent other devices from turning on
    const panelStatus = document.getElementById("panelStatus");
    if (panelStatus && panelStatus.innerHTML === "MATI") {
        const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
        if (!isCurrentlyOn) {
            showNotification(`Tidak dapat menyalakan ${device.name} - Panel listrik mati`, 'warning');
            return;
        }
    }
    
    const workingHours = isWorkingHours();
    const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
    const newState = !isCurrentlyOn;
    
    // Kirim perintah ke Arduino jika terhubung
    if (useArduinoData) {
        const success = await sendArduinoCommand(deviceId, newState ? 'on' : 'off');
        if (!success) {
            showNotification(`Gagal mengirim perintah ke ${device.name}`, 'warning');
            return;
        }
    }
    
    if (isCurrentlyOn) {
        // Turn OFF
        statusElement.innerHTML = "MATI";
        statusElement.className = "status-off";
        
        // Calculate usage only during working hours (except always-on devices)
        if (dailyData.devices[deviceId].startTime && workingHours && !device.alwaysOn) {
            let duration = (Date.now() - dailyData.devices[deviceId].startTime) / (1000 * 60);
            dailyData.devices[deviceId].usage += duration;
        }
        dailyData.devices[deviceId].startTime = null;
        
        // Show notification
        showNotification(`${device.name} dimatikan`, 'info');
    } else {
        // Turn ON
        statusElement.innerHTML = "MENYALA";
        statusElement.className = "status-on";
        
        // Start tracking only during working hours (except always-on devices)
        if (workingHours || device.alwaysOn) {
            dailyData.devices[deviceId].startTime = Date.now();
        }
        
        // Show notification
        showNotification(`${device.name} dinyalakan`, 'success');
    }
    
    updateCurrentUsageDisplay();
}

/* ===== PANEL LISTRIK CONTROL ===== */
function togglePanelListrik() {
    const panelStatus = document.getElementById("panelStatus");
    if (!panelStatus) return;
    
    const workingHours = isWorkingHours();
    const isCurrentlyOn = panelStatus.innerHTML === "MENYALA";
    
    if (isCurrentlyOn) {
        // Turn OFF Panel - This will turn off ALL devices
        panelStatus.innerHTML = "MATI";
        panelStatus.className = "status-off";
        
        // Turn off all electrical devices
        turnOffAllDevices();
        
        // Disable all device controls
        disableAllDeviceControls();
        
        // Set people count to 0 (no one can be in the room without electricity)
        document.getElementById("jumlahOrang").innerHTML = "0";
        
        // Calculate usage for panel
        if (dailyData.devices.panel.startTime && workingHours) {
            let duration = (Date.now() - dailyData.devices.panel.startTime) / (1000 * 60);
            dailyData.devices.panel.usage += duration;
        }
        dailyData.devices.panel.startTime = null;
        
        showNotification('Panel Listrik dimatikan - Semua kontrol dinonaktifkan', 'warning');
    } else {
        // Turn ON Panel
        panelStatus.innerHTML = "MENYALA";
        panelStatus.className = "status-on";
        
        // Enable all device controls
        enableAllDeviceControls();
        
        // Start tracking panel usage
        if (workingHours) {
            dailyData.devices.panel.startTime = Date.now();
        }
        
        showNotification('Panel Listrik dinyalakan - Semua kontrol dapat digunakan', 'success');
    }
    
    updateCurrentUsageDisplay();
}

/* ===== TURN OFF ALL DEVICES ===== */
function turnOffAllDevices() {
    // Turn off AC units
    if (acUnits.unit1) toggleACUnit(1);
    if (acUnits.unit2) toggleACUnit(2);
    
    // Turn off lamp circuits
    if (lampCircuits.circuit1) toggleLampCircuit(1);
    if (lampCircuits.circuit2) toggleLampCircuit(2);
    
    // Turn off other devices
    const otherDevices = ['computer', 'tv', 'dispenser', 'kettle', 'coffee'];
    otherDevices.forEach(deviceId => {
        const statusElement = document.getElementById(deviceId + "Status");
        if (statusElement && statusElement.innerHTML === "MENYALA") {
            statusElement.innerHTML = "MATI";
            statusElement.className = "status-off";
            
            // Calculate usage if device was running
            if (dailyData.devices[deviceId].startTime) {
                let duration = (Date.now() - dailyData.devices[deviceId].startTime) / (1000 * 60);
                dailyData.devices[deviceId].usage += duration;
                dailyData.devices[deviceId].startTime = null;
            }
        }
    });
    
    // Update sidebar status for other devices
    setTimeout(updateOtherDevicesStatus, 100);
}

/* ===== CHECK PANEL STATUS ===== */
function isPanelOn() {
    const panelStatus = document.getElementById("panelStatus");
    return panelStatus && panelStatus.innerHTML === "MENYALA";
}

/* ===== DISABLE ALL DEVICE CONTROLS ===== */
function disableAllDeviceControls() {
    // Disable AC controls
    const acButtons = document.querySelectorAll('.ac-card button, .ac-unit .mini-btn');
    acButtons.forEach(button => {
        button.disabled = true;
        button.style.opacity = '0.5';
        button.style.cursor = 'not-allowed';
        button.title = 'Panel listrik mati - kontrol tidak tersedia';
    });
    
    // Disable lamp controls
    const lampButtons = document.querySelectorAll('.lamp-card button, .lamp-circuit .mini-btn');
    lampButtons.forEach(button => {
        button.disabled = true;
        button.style.opacity = '0.5';
        button.style.cursor = 'not-allowed';
        button.title = 'Panel listrik mati - kontrol tidak tersedia';
    });
    
    // Disable other device controls in sidebar
    const otherDeviceButtons = document.querySelectorAll('.other-devices-card-sidebar .mini-btn, .device-item-sidebar .mini-btn');
    otherDeviceButtons.forEach(button => {
        button.disabled = true;
        button.style.opacity = '0.5';
        button.style.cursor = 'not-allowed';
        button.title = 'Panel listrik mati - kontrol tidak tersedia';
    });
    
    // Disable device controls in devices section
    const deviceSectionButtons = document.querySelectorAll('.devices-grid .device-controls button');
    deviceSectionButtons.forEach(button => {
        if (!button.closest('.device-card').querySelector('h3').textContent.includes('Panel Listrik')) {
            button.disabled = true;
            button.style.opacity = '0.5';
            button.style.cursor = 'not-allowed';
            button.title = 'Panel listrik mati - kontrol tidak tersedia';
        }
    });
    
    // Disable temperature controls
    const tempButtons = document.querySelectorAll('.temp-btn');
    tempButtons.forEach(button => {
        button.disabled = true;
        button.style.opacity = '0.5';
        button.style.cursor = 'not-allowed';
        button.title = 'Panel listrik mati - kontrol tidak tersedia';
    });
    
    // Add visual indicator to device cards (except panel card)
    const deviceCards = document.querySelectorAll('.device-card');
    deviceCards.forEach(card => {
        const cardTitle = card.querySelector('.title');
        if (cardTitle && !cardTitle.textContent.includes('Panel Listrik')) {
            card.style.opacity = '0.6';
            card.style.filter = 'grayscale(50%)';
            
            // Add disabled overlay
            if (!card.querySelector('.disabled-overlay')) {
                const overlay = document.createElement('div');
                overlay.className = 'disabled-overlay';
                overlay.style.cssText = `
                    position: absolute;
                    top: 0;
                    left: 0;
                    right: 0;
                    bottom: 0;
                    background: rgba(0,0,0,0.3);
                    border-radius: 20px;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    color: #fff;
                    font-weight: bold;
                    font-size: 14px;
                    z-index: 10;
                    pointer-events: none;
                `;
                overlay.innerHTML = '<i class="fas fa-lock"></i> Panel Mati';
                card.style.position = 'relative';
                card.appendChild(overlay);
            }
        }
    });
}

/* ===== ENABLE ALL DEVICE CONTROLS ===== */
function enableAllDeviceControls() {
    // Enable AC controls
    const acButtons = document.querySelectorAll('.ac-card button, .ac-unit .mini-btn');
    acButtons.forEach(button => {
        button.disabled = false;
        button.style.opacity = '1';
        button.style.cursor = 'pointer';
        button.title = '';
    });
    
    // Enable lamp controls
    const lampButtons = document.querySelectorAll('.lamp-card button, .lamp-circuit .mini-btn');
    lampButtons.forEach(button => {
        button.disabled = false;
        button.style.opacity = '1';
        button.style.cursor = 'pointer';
        button.title = '';
    });
    
    // Enable other device controls
    const otherDeviceButtons = document.querySelectorAll('.other-devices-card-sidebar .mini-btn, .device-item-sidebar .mini-btn');
    otherDeviceButtons.forEach(button => {
        button.disabled = false;
        button.style.opacity = '1';
        button.style.cursor = 'pointer';
        button.title = '';
    });
    
    // Enable temperature controls
    const tempButtons = document.querySelectorAll('.temp-btn');
    tempButtons.forEach(button => {
        button.disabled = false;
        button.style.opacity = '1';
        button.style.cursor = 'pointer';
        button.title = '';
    });
    
    // Remove visual indicator from device cards
    const deviceCards = document.querySelectorAll('.device-card');
    deviceCards.forEach(card => {
        card.style.opacity = '1';
        card.style.filter = 'none';
        
        // Remove disabled overlay if exists
        const overlay = card.querySelector('.disabled-overlay');
        if (overlay) {
            overlay.remove();
        }
    });
}

/* ===== LEGACY FUNCTIONS FOR BACKWARD COMPATIBILITY ===== */
function toggleLampu() {
    toggleDevice('lamp');
}

function toggleAC() {
    toggleDevice('ac');
}

let suhuAC = 24;

function tempUp() {
    suhuAC++;
    document.getElementById("acTempValue").innerHTML = suhuAC + " °C";
}

function tempDown() {
    suhuAC--;
    if (suhuAC < 16) suhuAC = 16;
    document.getElementById("acTempValue").innerHTML = suhuAC + " °C";
}

/* ===== CHART.JS (Analytics Mingguan) ===== */
const ctx = document.getElementById("myChart");

if (ctx) {
    new Chart(ctx, {
        type: 'line',
        data: {
            labels: ['Sen', 'Sel', 'Rab', 'Kam', 'Jum', 'Sab', 'Min'],
            datasets: [{
                label: 'Energi Terpakai (Wh)',
                data: [320, 290, 410, 280, 350, 220, 150],
                borderWidth: 2,
                borderColor: '#1e5eff',
                backgroundColor: 'rgba(30,94,255,0.2)'
            }]
        },
        options: {
            responsive: true,
            plugins: {
                legend: { position: "top" }
            }
        }
    });
}

/* ===== SENSOR DUMMY (Optional) ===== */
setInterval(() => {
    document.getElementById("suhuValue").innerHTML = "27 °C";
    document.getElementById("humidityValue").innerHTML = "65 %";
}, 3000);

/* ===== FUNGSI PERHITUNGAN ENERGI ===== */
function hitungEnergiDevice(deviceId, menitPemakaian) {
    const device = deviceData[deviceId];
    if (!device) return 0;
    
    // Energi (Wh) = Daya (W) × Waktu (jam)
    let jamPemakaian = menitPemakaian / 60;
    return Math.round(device.power * jamPemakaian);
}

function hitungTotalEnergiHarian() {
    let totalEnergi = 0;
    
    Object.keys(deviceData).forEach(deviceId => {
        const usage = dailyData.devices[deviceId].usage;
        totalEnergi += hitungEnergiDevice(deviceId, usage);
    });
    
    return totalEnergi;
}

function getDeviceUsageStats() {
    const stats = {};
    let totalDevicesOn = 0;
    
    Object.keys(deviceData).forEach(deviceId => {
        const statusElement = document.getElementById(deviceId + "Status");
        const isOn = statusElement && statusElement.innerHTML === "MENYALA";
        
        if (isOn) totalDevicesOn++;
        
        stats[deviceId] = {
            usage: dailyData.devices[deviceId].usage,
            energy: hitungEnergiDevice(deviceId, dailyData.devices[deviceId].usage),
            isOn: isOn
        };
    });
    
    stats.totalDevicesOn = totalDevicesOn;
    stats.totalEnergy = hitungTotalEnergiHarian();
    
    return stats;
}

/* ===== UPDATE DISPLAY PEMAKAIAN SAAT INI ===== */
function updateCurrentUsageDisplay() {
    const stats = getDeviceUsageStats();
    
    // Update energy save display
    let energiHemat = Math.max(0, 100 - (stats.totalEnergy / 100));
    const energySaveEl = document.getElementById("energySave");
    if (energySaveEl) {
        energySaveEl.innerHTML = Math.round(energiHemat) + "%";
    }
    
    // Update total devices on
    const totalDevicesEl = document.getElementById("totalDevicesOn");
    if (totalDevicesEl) {
        totalDevicesEl.textContent = stats.totalDevicesOn;
    }
    
    // Update total energy
    const totalEnergyEl = document.getElementById("dailyTotalEnergy");
    if (totalEnergyEl) {
        totalEnergyEl.textContent = stats.totalEnergy + " Wh";
    }
}

/* ===== UPDATE HISTORY DISPLAY ===== */
function updateHistoryDisplay() {
    const historyBody = document.getElementById("history-body");
    if (!historyBody) return;
    
    historyBody.innerHTML = "";
    
    historyData.forEach((entry, index) => {
        const row = document.createElement("tr");
        const date = new Date(entry.date);
        const formattedDate = date.toLocaleDateString('id-ID');
        
        row.innerHTML = `
            <td>${index + 1}</td>
            <td>${formattedDate}</td>
            <td>${entry.avgTemp} °C</td>
            <td>${entry.acMinutes}</td>
            <td>${entry.lampMinutes}</td>
            <td>${entry.energiAC}</td>
            <td>${entry.energiLampu}</td>
            <td><strong>${entry.totalEnergi}</strong></td>
            <td>${entry.totalPeople}</td>
        `;
        
        historyBody.appendChild(row);
    });
}

/* ===== MONITORING JAM KERJA ===== */
function checkWorkingHoursStatus() {
    const workingHours = isWorkingHours();
    const statusElement = document.querySelector('.system-status span');
    const now = new Date();
    
    if (workingHours) {
        if (statusElement) {
            statusElement.textContent = "Jam Kerja Aktif";
        }
        
        // Auto-save setiap 30 menit selama jam kerja
        if (now.getMinutes() % 30 === 0 && now.getSeconds() === 0) {
            updateCurrentUsageDisplay();
        }
    } else {
        if (statusElement) {
            const dayStatus = getWorkingDayStatus();
            if (!isWorkingDay() && workingDaysEnabled) {
                statusElement.textContent = `${dayStatus}`;
            } else {
                statusElement.textContent = "Di Luar Jam Kerja";
            }
        }
        
        // Auto-save data saat jam kerja berakhir (jam 5 sore) hanya pada hari kerja
        if (now.getHours() === 17 && now.getMinutes() === 0 && now.getSeconds() === 0 && isWorkingDay()) {
            const stats = getDeviceUsageStats();
            if (stats.totalEnergy > 0) {
                saveToHistory();
                showNotification("Data harian telah disimpan ke history", "success");
            }
        }
    }
}

/* ===== UPDATE WORKING HOURS DISPLAY FROM ARDUINO WITH ANIMATIONS ===== */
function updateWorkingHoursDisplay(workingHoursData, currentTimeData) {
    const workingHoursStatusEl = document.getElementById("workingHoursStatus");
    const workingHoursRangeEl = document.getElementById("workingHoursRange");
    const workingDayInfoEl = document.getElementById("workingDayInfo");
    const currentTimeDisplayEl = document.getElementById("currentTimeDisplay");
    
    if (workingHoursData) {
        // Update working hours status with animation
        if (workingHoursStatusEl) {
            const newStatus = workingHoursData.is_working_hours ? "Jam Kerja Aktif" : (workingHoursData.status || "Di Luar Jam Kerja");
            const newClass = workingHoursData.is_working_hours ? "status-on" : "status-off";
            updateStatusWithAnimation("workingHoursStatus", newStatus, newClass);
        }
        
        // Update working hours range with smooth transition
        if (workingHoursRangeEl) {
            const startHour = String(workingHoursData.start_hour).padStart(2, '0');
            const endHour = String(workingHoursData.end_hour).padStart(2, '0');
            const newRange = `${startHour}:00 - ${endHour}:00 WIB`;
            
            if (workingHoursRangeEl.textContent !== newRange) {
                workingHoursRangeEl.style.transform = 'scale(0.95)';
                workingHoursRangeEl.style.opacity = '0.7';
                
                setTimeout(() => {
                    workingHoursRangeEl.textContent = newRange;
                    workingHoursRangeEl.style.transform = 'scale(1)';
                    workingHoursRangeEl.style.opacity = '1';
                }, 150);
            }
        }
        
        // Update working day info with smooth transition
        if (workingDayInfoEl) {
            const newText = workingHoursData.enabled ? "Senin - Jumat" : "7 Hari Seminggu";
            const newColor = workingHoursData.enabled ? "#1e5eff" : "#28a745";
            
            if (workingDayInfoEl.textContent !== newText) {
                workingDayInfoEl.style.transform = 'scale(0.95)';
                workingDayInfoEl.style.opacity = '0.7';
                
                setTimeout(() => {
                    workingDayInfoEl.textContent = newText;
                    workingDayInfoEl.style.color = newColor;
                    workingDayInfoEl.style.transform = 'scale(1)';
                    workingDayInfoEl.style.opacity = '1';
                }, 150);
            }
        }
    }
    
    // Update current time display with animation
    if (currentTimeData && currentTimeDisplayEl) {
        const newTime = `${currentTimeData.formatted} (${currentTimeData.day_name})`;
        
        if (currentTimeDisplayEl.textContent !== newTime) {
            currentTimeDisplayEl.classList.add('updating');
            
            setTimeout(() => {
                currentTimeDisplayEl.textContent = newTime;
                currentTimeDisplayEl.classList.remove('updating');
            }, 200);
        }
    }
}

/* ===== AC GRADUAL MODE DISPLAY ===== */
function updateACGradualModeDisplay(gradualModeData) {
    // Find or create AC gradual mode status element
    let gradualStatusEl = document.getElementById('acGradualModeStatus');
    
    if (!gradualStatusEl) {
        // Create AC gradual mode status display if it doesn't exist
        const acCard = document.querySelector('.ac-card');
        if (acCard) {
            const gradualModeDiv = document.createElement('div');
            gradualModeDiv.className = 'ac-gradual-mode-status';
            gradualModeDiv.innerHTML = `
                <div class="gradual-mode-header">
                    <i class="fas fa-thermometer-half"></i>
                    <span>Mode Gradual AC</span>
                </div>
                <div id="acGradualModeStatus" class="gradual-mode-content"></div>
            `;
            acCard.appendChild(gradualModeDiv);
            gradualStatusEl = document.getElementById('acGradualModeStatus');
        }
    }
    
    if (gradualStatusEl) {
        if (gradualModeData.active) {
            gradualStatusEl.innerHTML = `
                <div class="gradual-active">
                    <div class="gradual-progress">
                        <div class="progress-bar">
                            <div class="progress-fill" style="width: ${gradualModeData.progress_percent}%"></div>
                        </div>
                        <span class="progress-text">${gradualModeData.progress_percent.toFixed(1)}%</span>
                    </div>
                    <div class="gradual-info">
                        <div class="info-item">
                            <span>Waktu Berlalu:</span>
                            <span>${gradualModeData.elapsed_minutes} menit</span>
                        </div>
                        <div class="info-item">
                            <span>Sisa Waktu:</span>
                            <span>${gradualModeData.remaining_minutes} menit</span>
                        </div>
                        <div class="info-item">
                            <span>Suhu Awal:</span>
                            <span>${gradualModeData.original_temp}°C</span>
                        </div>
                        <div class="info-item">
                            <span>Target Suhu:</span>
                            <span>${gradualModeData.target_room_temp}°C</span>
                        </div>
                    </div>
                    <div class="gradual-description">
                        Menaikkan suhu AC secara bertahap ke suhu ruang untuk menghemat energi
                    </div>
                </div>
            `;
            gradualStatusEl.className = 'gradual-mode-content active';
        } else {
            gradualStatusEl.innerHTML = `
                <div class="gradual-inactive">
                    <i class="fas fa-check-circle"></i>
                    <span>Mode Normal - AC beroperasi sesuai kebutuhan</span>
                </div>
            `;
            gradualStatusEl.className = 'gradual-mode-content inactive';
        }
    }
}

/* ===== ARDUINO WORKING HOURS API FUNCTIONS ===== */
async function toggleArduinoWorkingDays() {
    if (!useArduinoData) {
        toggleWorkingDays();
        return;
    }
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/working-hours`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': 'Bearer ' + (localStorage.getItem('authToken') || '')
            },
            body: JSON.stringify({
                action: 'toggle_working_days'
            })
        });
        
        if (response.ok) {
            const result = await response.json();
            showNotification(result.message, 'success');
            updateRealTimeData(); // Refresh data
        } else {
            showNotification('Gagal mengubah pengaturan hari kerja', 'warning');
        }
    } catch (error) {
        console.error('Error toggling working days:', error);
        showNotification('Error koneksi ke Arduino', 'warning');
    }
}

async function toggleArduinoScheduledShutdown() {
    if (!useArduinoData) {
        toggleScheduledShutdown();
        return;
    }
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/working-hours`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': 'Bearer ' + (localStorage.getItem('authToken') || '')
            },
            body: JSON.stringify({
                action: 'toggle_scheduled_shutdown'
            })
        });
        
        if (response.ok) {
            const result = await response.json();
            showNotification(result.message, 'success');
            updateRealTimeData(); // Refresh data
        } else {
            showNotification('Gagal mengubah pengaturan jadwal otomatis', 'warning');
        }
    } catch (error) {
        console.error('Error toggling scheduled shutdown:', error);
        showNotification('Error koneksi ke Arduino', 'warning');
    }
}

/* ===== AC AUTOMATION STATUS DISPLAY ===== */
function updateACAutomationStatus(acData) {
    // Highlight the current rule being applied in the table
    const tableRows = document.querySelectorAll('.ac-rules-table tbody tr');
    const peopleCount = acData.proximity_count || 0;
    const outdoorTemp = acData.outdoor_temp || 27;
    
    // Remove previous highlights
    tableRows.forEach(row => {
        row.classList.remove('current-rule');
    });
    
    // Determine which rule is currently active and highlight it (sesuai Tabel 9 PERSIS)
    let activeRowId = null;
    
    if (peopleCount === 0) {
        activeRowId = 'rule-0-people'; // No people - gradual mode
    } else if (peopleCount >= 1 && peopleCount <= 4) {
        activeRowId = 'rule-1-4-people'; // 1-4 people - AC off
    } else if (peopleCount >= 5 && peopleCount <= 10) {
        if (outdoorTemp >= 30.0 && outdoorTemp <= 32.0) {
            activeRowId = 'rule-5-10-30-32'; // 5-10 people, 30-32°C, 1 AC, 25°C
        } else if (outdoorTemp >= 32.1 && outdoorTemp <= 35.0) {
            activeRowId = 'rule-5-10-32-35'; // 5-10 people, 32-35°C, 1 AC, 26°C
        }
    } else if (peopleCount >= 10 && peopleCount <= 13) {
        if (outdoorTemp >= 30.0 && outdoorTemp <= 32.0) {
            activeRowId = 'rule-10-13-30-32'; // 10-13 people, 30-32°C, 2 AC, 24°C
        }
    } else if (peopleCount >= 10 && peopleCount <= 20) {
        if (outdoorTemp >= 32.1 && outdoorTemp <= 35.0) {
            activeRowId = 'rule-10-20-32-35'; // 10-20 people, 32-35°C, 2 AC, 25°C
        }
    } else if (peopleCount >= 21) {
        activeRowId = 'rule-21-plus'; // 21+ people - 2 AC, 23°C
    }
    
    // Highlight the active row
    if (activeRowId) {
        const activeRow = document.getElementById(activeRowId);
        if (activeRow) {
            activeRow.classList.add('current-rule');
        }
    }
    
    // Update AC status info if element exists
    const acStatusInfo = document.getElementById('acCurrentRuleInfo');
    if (acStatusInfo) {
        let ruleText = "Tidak ada aturan yang cocok";
        
        if (activeRowId) {
            const ruleDescriptions = {
                'rule-0-people': "Mode Gradual - Tidak ada orang",
                'rule-1-4-people': "1-4 orang - AC mati (di bawah threshold)",
                'rule-5-10-30-32': `5-10 orang, ${outdoorTemp.toFixed(1)}°C - 1 AC, target 25°C`,
                'rule-5-10-32-35': `5-10 orang, ${outdoorTemp.toFixed(1)}°C - 1 AC, target 26°C`,
                'rule-10-13-30-32': `10-13 orang, ${outdoorTemp.toFixed(1)}°C - 2 AC, target 24°C`,
                'rule-10-20-32-35': `10-20 orang, ${outdoorTemp.toFixed(1)}°C - 2 AC, target 25°C`,
                'rule-21-plus': `${peopleCount} orang - 2 AC, target 23°C (beban maksimal)`
            };
            ruleText = ruleDescriptions[activeRowId] || "Aturan tidak dikenali";
        }
        
        acStatusInfo.textContent = ruleText;
    }
}

/* ===== AC GRADUAL MODE CONTROL ===== */
async function startACGradualMode() {
    if (!useArduinoData) {
        showNotification('Mode gradual AC hanya tersedia saat terhubung ke Arduino', 'info');
        return;
    }
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/ac-auto`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': 'Bearer ' + (localStorage.getItem('authToken') || '')
            },
            body: JSON.stringify({
                action: 'start_gradual_mode'
            })
        });
        
        if (response.ok) {
            const result = await response.json();
            showNotification(result.message, 'success');
            updateRealTimeData(); // Refresh data
        } else {
            const error = await response.json();
            showNotification(error.message || 'Gagal memulai mode gradual AC', 'warning');
        }
    } catch (error) {
        console.error('Error starting AC gradual mode:', error);
        showNotification('Error koneksi ke Arduino', 'warning');
    }
}

async function stopACGradualMode() {
    if (!useArduinoData) {
        return;
    }
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/ac-auto`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': 'Bearer ' + (localStorage.getItem('authToken') || '')
            },
            body: JSON.stringify({
                action: 'stop_gradual_mode'
            })
        });
        
        if (response.ok) {
            const result = await response.json();
            showNotification(result.message, 'success');
            updateRealTimeData(); // Refresh data
        } else {
            showNotification('Gagal menghentikan mode gradual AC', 'warning');
        }
    } catch (error) {
        console.error('Error stopping AC gradual mode:', error);
        showNotification('Error koneksi ke Arduino', 'warning');
    }
}

// Cek status jam kerja setiap detik
setInterval(checkWorkingHoursStatus, 1000);

/* ===== INISIALISASI SAAT HALAMAN DIMUAT ===== */
document.addEventListener('DOMContentLoaded', function() {
    // Load history dari localStorage
    updateHistoryDisplay();
    
    // Cek reset harian
    checkDailyReset();
    
    // Update display awal
    updateCurrentUsageDisplay();
});
/* ===== TOGGLE SWITCHES ===== */
document.addEventListener('DOMContentLoaded', function() {
    // Initialize automation toggle switches with correct states
    initializeAutomationToggles();
    
    // Load saved theme preference
    loadThemePreference();
});

function initializeAutomationToggles() {
    // Set initial states for automation toggles based on their variables
    const automationToggles = document.querySelectorAll('.automation-rule .toggle-switch');
    
    automationToggles.forEach(toggle => {
        const onclickAttr = toggle.getAttribute('onclick');
        
        if (onclickAttr) {
            if (onclickAttr.includes('toggleScheduledShutdown')) {
                if (scheduledShutdownEnabled) {
                    toggle.classList.add('active');
                } else {
                    toggle.classList.remove('active');
                }
            } else if (onclickAttr.includes('toggleWorkingDays')) {
                if (workingDaysEnabled) {
                    toggle.classList.add('active');
                } else {
                    toggle.classList.remove('active');
                }
            } else if (onclickAttr.includes('toggleAutoLampControl')) {
                if (autoLampControlEnabled) {
                    toggle.classList.add('active');
                } else {
                    toggle.classList.remove('active');
                }
            } else if (onclickAttr.includes('togglePresenceDetection')) {
                if (presenceDetectionEnabled) {
                    toggle.classList.add('active');
                } else {
                    toggle.classList.remove('active');
                }
            } else if (onclickAttr.includes('toggleAutoShutdown')) {
                if (autoShutdownEnabled) {
                    toggle.classList.add('active');
                } else {
                    toggle.classList.remove('active');
                }
            }
        }
    });
}

/* ===== DARK/LIGHT MODE TOGGLE ===== */
function toggleDarkMode() {
    const body = document.body;
    const darkModeToggle = document.getElementById('darkModeToggle');
    
    // Toggle the visual state first
    darkModeToggle.classList.toggle('active');
    
    // Set mode based on toggle state
    if (darkModeToggle.classList.contains('active')) {
        // Dark mode (toggle ke kanan = aktif)
        body.classList.remove('light-mode');
        localStorage.setItem('theme', 'dark');
        showNotification('Mode gelap diaktifkan', 'info');
    } else {
        // Light mode (toggle ke kiri = nonaktif)
        body.classList.add('light-mode');
        localStorage.setItem('theme', 'light');
        showNotification('Mode terang diaktifkan', 'info');
    }
}

function loadThemePreference() {
    const savedTheme = localStorage.getItem('theme');
    const darkModeToggle = document.getElementById('darkModeToggle');
    
    if (savedTheme === 'light') {
        document.body.classList.add('light-mode');
        darkModeToggle.classList.remove('active');
    } else {
        // Default to dark mode
        document.body.classList.remove('light-mode');
        darkModeToggle.classList.add('active');
    }
}

/* ===== ARDUINO API CONNECTION ===== */
let arduinoApiUrl = 'http://192.168.1.100'; // Ganti dengan IP Arduino Anda
let useArduinoData = false; // Set true untuk menggunakan data dari Arduino

// Fungsi untuk mengambil data dari Arduino
async function fetchArduinoData() {
    if (!useArduinoData) return null;
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/sensors`);
        if (response.ok) {
            const data = await response.json();
            return data;
        }
    } catch (error) {
        console.log('Arduino connection failed, using simulation data');
        useArduinoData = false;
    }
    return null;
}

// Fungsi untuk mengirim perintah ke Arduino
async function sendArduinoCommand(device, action) {
    if (!useArduinoData) return false;
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/control`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                device: device,
                action: action
            })
        });
        return response.ok;
    } catch (error) {
        console.log('Failed to send command to Arduino:', error);
        return false;
    }
}

/* ===== SIMULASI DATA REAL-TIME ===== */
let weatherCondition = "cerah"; // cerah, berawan, hujan
let weatherChangeTimer = 0;

async function updateRealTimeData() {
    checkDailyReset();
    
    // Coba ambil data dari Arduino terlebih dahulu
    const arduinoData = await fetchArduinoData();
    
    let jumlahOrang = 0;
    let suhuBaru = 27;
    let humidityBaru = 65;
    let ldrValue = 500;
    
    if (arduinoData) {
        // Gunakan data dari Arduino
        jumlahOrang = arduinoData.people_count || 0;
        suhuBaru = arduinoData.temperature || 27;
        humidityBaru = arduinoData.humidity || 65;
        ldrValue = arduinoData.light_intensity || 500;
        
        // Update status perangkat dari Arduino
        if (arduinoData.devices) {
            updateDeviceStatusFromArduino(arduinoData.devices);
        }
        
        // Update working hours display from Arduino
        if (arduinoData.working_hours) {
            updateWorkingHoursDisplay(arduinoData.working_hours, arduinoData.current_time);
        }
        
        // Update AC gradual mode display from Arduino
        if (arduinoData.ac_automation && arduinoData.ac_automation.gradual_mode) {
            updateACGradualModeDisplay(arduinoData.ac_automation.gradual_mode);
        }
        
        // Update AC automation status display
        if (arduinoData.ac_automation) {
            updateACAutomationStatus(arduinoData.ac_automation);
        }
    } else {
        // Gunakan data simulasi
        if (isPanelOn()) {
            jumlahOrang = Math.floor(Math.random() * 10);
        }
        
        // Simulasi suhu dengan variasi kecil
        const suhuVariasi = (Math.random() - 0.5) * 2;
        suhuBaru = Math.round((27 + suhuVariasi) * 10) / 10;
        
        // Simulasi kelembaban
        const baseHumidity = weatherCondition === "hujan" ? 80 : 65;
        const humidityVariasi = (Math.random() - 0.5) * 10;
        humidityBaru = Math.round(baseHumidity + humidityVariasi);
        
        // Simulasi LDR berdasarkan cuaca dan waktu
        const now = new Date();
        const hour = now.getHours();
        let baseLightIntensity;
        
        if (hour >= 7 && hour < 9) {
            baseLightIntensity = 400;
        } else if (hour >= 9 && hour < 15) {
            baseLightIntensity = 800;
        } else if (hour >= 15 && hour < 17) {
            baseLightIntensity = 500;
        } else {
            baseLightIntensity = 100;
        }
        
        let weatherMultiplier = 1;
        if (weatherCondition === "berawan") {
            weatherMultiplier = 0.6;
        } else if (weatherCondition === "hujan") {
            weatherMultiplier = 0.3;
        }
        
        ldrValue = Math.floor(baseLightIntensity * weatherMultiplier + (Math.random() * 100));
    }
    
    // Update tampilan with smooth animations
    updateValueWithAnimation("jumlahOrang", jumlahOrang);
    updateValueWithAnimation("suhuValue", suhuBaru, " °C");
    updateValueWithAnimation("humidityValue", humidityBaru, " %");
    
    const weatherIcon = weatherCondition === "cerah" ? "☀️" : weatherCondition === "berawan" ? "☁️" : "🌧️";
    updateValueWithAnimation("ldrValue", ldrValue + ` lux ${weatherIcon}`);
    
    // Tambahkan ke total people harian jika dalam jam kerja
    if (isWorkingHours() && jumlahOrang > 0) {
        dailyData.totalPeople = Math.max(dailyData.totalPeople, jumlahOrang);
    }
    
    // Simulasi perubahan cuaca setiap 30 detik
    weatherChangeTimer++;
    if (weatherChangeTimer >= 6) { // 6 * 5 detik = 30 detik
        const weatherRandom = Math.random();
        if (weatherRandom < 0.6) {
            weatherCondition = "cerah";
        } else if (weatherRandom < 0.8) {
            weatherCondition = "berawan";
        } else {
            weatherCondition = "hujan";
        }
        weatherChangeTimer = 0;
    }
    
    // Update intensitas cahaya berdasarkan cuaca dan waktu
    const now = new Date();
    const hour = now.getHours();
    let baseLightIntensity;
    
    // Simulasi perubahan cuaca setiap 30 detik (hanya jika tidak menggunakan data Arduino)
    if (!arduinoData) {
        weatherChangeTimer++;
        if (weatherChangeTimer >= 6) { // 6 * 5 detik = 30 detik
            const weatherRandom = Math.random();
            if (weatherRandom < 0.6) {
                weatherCondition = "cerah";
            } else if (weatherRandom < 0.8) {
                weatherCondition = "berawan";
            } else {
                weatherCondition = "hujan";
            }
            weatherChangeTimer = 0;
        }
    }
    
    // Auto AC Control berdasarkan deteksi kehadiran
    if (presenceDetectionEnabled && isWorkingHours() && isPanelOn()) {
        checkAutoACControl(jumlahOrang);
    }
    
    // Update current usage display
    updateCurrentUsageDisplay();
}

/* ===== UPDATE DEVICE STATUS FROM ARDUINO WITH ANIMATIONS ===== */
function updateDeviceStatusFromArduino(devices) {
    // Update AC status with animations
    if (devices.ac1 !== undefined) {
        acUnits.unit1 = devices.ac1;
        const newStatus = devices.ac1 ? "MENYALA" : "MATI";
        const newClass = devices.ac1 ? "mini-status status-on" : "mini-status status-off";
        updateStatusWithAnimation("ac1Status", newStatus, newClass);
    }
    
    if (devices.ac2 !== undefined) {
        acUnits.unit2 = devices.ac2;
        const newStatus = devices.ac2 ? "MENYALA" : "MATI";
        const newClass = devices.ac2 ? "mini-status status-on" : "mini-status status-off";
        updateStatusWithAnimation("ac2Status", newStatus, newClass);
    }
    
    // Update Lamp status
    if (devices.lamp1 !== undefined) {
        lampCircuits.circuit1 = devices.lamp1;
        const lamp1Status = document.getElementById("lamp1Status");
        if (lamp1Status) {
            lamp1Status.innerHTML = devices.lamp1 ? "MENYALA" : "MATI";
            lamp1Status.className = devices.lamp1 ? "mini-status status-on" : "mini-status status-off";
        }
    }
    
    if (devices.lamp2 !== undefined) {
        lampCircuits.circuit2 = devices.lamp2;
        const lamp2Status = document.getElementById("lamp2Status");
        if (lamp2Status) {
            lamp2Status.innerHTML = devices.lamp2 ? "MENYALA" : "MATI";
            lamp2Status.className = devices.lamp2 ? "mini-status status-on" : "mini-status status-off";
        }
    }
    
    // Update other devices
    const otherDevices = ['computer', 'tv', 'dispenser', 'kettle', 'coffee', 'fridge', 'router', 'panel'];
    otherDevices.forEach(deviceId => {
        if (devices[deviceId] !== undefined) {
            const statusElement = document.getElementById(deviceId + "Status");
            if (statusElement) {
                statusElement.innerHTML = devices[deviceId] ? "MENYALA" : "MATI";
                statusElement.className = devices[deviceId] ? "status-on" : "status-off";
            }
        }
    });
    
    // Update main status displays
    updateACStatus();
    updateLampStatus();
    updateOtherDevicesStatus();
}

// Update data setiap 5 detik
setInterval(updateRealTimeData, 5000);

/* ===== WAKTU DENGAN ICON ===== */
function updateWIBWithIcon() {
    const now = new Date();
    let waktuWIB = now.toLocaleString("id-ID", {
        timeZone: "Asia/Jakarta",
        year: "numeric",
        month: "long",
        day: "numeric",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit"
    });
    document.getElementById("waktuRealtime").innerHTML = '<i class="fas fa-clock"></i> ' + waktuWIB + " (WIB)";
}
setInterval(updateWIBWithIcon, 1000);
updateWIBWithIcon();

/* ===== ENHANCED NOTIFIKASI SISTEM WITH ANIMATIONS ===== */
function showNotification(message, type = 'info') {
    // Buat elemen notifikasi
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.innerHTML = `
        <i class="fas fa-${type === 'success' ? 'check-circle' : type === 'warning' ? 'exclamation-triangle' : 'info-circle'}"></i>
        <span>${message}</span>
        <button onclick="removeNotification(this.parentElement)"><i class="fas fa-times"></i></button>
    `;
    
    // Set initial state for animation
    notification.style.transform = 'translateX(100%)';
    notification.style.opacity = '0';
    
    // Tambahkan ke body
    document.body.appendChild(notification);
    
    // Animate in
    setTimeout(() => {
        notification.style.transform = 'translateX(0)';
        notification.style.opacity = '1';
    }, 10);
    
    // Auto remove setelah 5 detik
    setTimeout(() => {
        removeNotification(notification);
    }, 5000);
}

function removeNotification(notification) {
    if (notification && notification.parentElement) {
        // Animate out
        notification.style.transform = 'translateX(100%)';
        notification.style.opacity = '0';
        
        // Remove after animation
        setTimeout(() => {
            if (notification.parentElement) {
                notification.remove();
            }
        }, 300);
    }
}

/* ===== SIMPLE SIDEBAR TOGGLE WITHOUT EFFECTS ===== */
function toggleSidebar() {
    const sidebar = document.getElementById('sidebar');
    const toggleIcon = document.getElementById('toggleIcon');
    
    sidebar.classList.toggle('collapsed');
    
    // Update icon with simple transition
    if (sidebar.classList.contains('collapsed')) {
        toggleIcon.className = 'fas fa-chevron-right';
    } else {
        toggleIcon.className = 'fas fa-bars';
    }
}

/* ===== EVENT LISTENERS UNTUK INTERAKSI ===== */
document.addEventListener('DOMContentLoaded', function() {
    // Simulasi notifikasi saat lampu/AC dinyalakan
    const originalToggleLampu = toggleLampu;
    const originalToggleAC = toggleAC;
    
    toggleLampu = function() {
        originalToggleLampu();
        const status = document.getElementById("lampStatus").innerHTML;
        showNotification(`Lampu ${status.toLowerCase()}`, 'info');
    };
    
    toggleAC = function() {
        originalToggleAC();
        const status = document.getElementById("acStatus").innerHTML;
        showNotification(`AC ${status.toLowerCase()}`, 'info');
    };
});
/* ===== HISTORY TAB MANAGEMENT ===== */
function showHistoryTab(tabName) {
    // Sembunyikan semua tabel
    document.querySelectorAll('.history-table-container').forEach(container => {
        container.style.display = 'none';
    });
    
    // Hapus class active dari semua tab button
    document.querySelectorAll('.tab-button').forEach(button => {
        button.classList.remove('active');
    });
    
    // Tampilkan tabel yang dipilih
    document.getElementById(tabName + '-table').style.display = 'block';
    
    // Tambahkan class active ke button yang diklik
    event.target.classList.add('active');
}

/* ===== UPDATE SEPARATED HISTORY TABLES ===== */
function updateSeparatedHistoryDisplay() {
    updateSummaryTable();
    updateLampTable();
    updateACTable();
    updateOtherDevicesTable();
    updateEnvironmentTable();
}

function updateSummaryTable() {
    const summaryBody = document.getElementById("summary-body");
    if (!summaryBody) return;
    
    summaryBody.innerHTML = "";
    
    historyData.forEach((entry, index) => {
        const row = document.createElement("tr");
        const date = new Date(entry.date);
        const formattedDate = date.toLocaleDateString('id-ID');
        const totalTime = entry.lampMinutes + entry.acMinutes;
        
        // Hitung efisiensi berdasarkan rasio energi per orang
        let efficiency = "average";
        const energyPerPerson = entry.totalEnergi / Math.max(entry.totalPeople, 1);
        if (energyPerPerson < 10) efficiency = "excellent";
        else if (energyPerPerson < 15) efficiency = "good";
        else if (energyPerPerson > 25) efficiency = "poor";
        
        const efficiencyText = {
            excellent: "Sangat Baik",
            good: "Baik", 
            average: "Sedang",
            poor: "Buruk"
        };
        
        row.innerHTML = `
            <td>${index + 1}</td>
            <td>${formattedDate}</td>
            <td><strong>${entry.totalEnergi}</strong></td>
            <td>${totalTime} menit</td>
            <td>${entry.totalPeople}</td>
            <td><span class="efficiency ${efficiency}">${efficiencyText[efficiency]}</span></td>
        `;
        
        summaryBody.appendChild(row);
    });
}

function updateLampTable() {
    const lampBody = document.getElementById("lamp-body");
    if (!lampBody) return;
    
    lampBody.innerHTML = "";
    
    historyData.forEach((entry, index) => {
        const row = document.createElement("tr");
        const date = new Date(entry.date);
        const formattedDate = date.toLocaleDateString('id-ID');
        const percentage = ((entry.energiLampu / entry.totalEnergi) * 100).toFixed(1);
        const avgLight = Math.floor(Math.random() * 300) + 350; // Simulasi data cahaya
        
        row.innerHTML = `
            <td>${index + 1}</td>
            <td>${formattedDate}</td>
            <td>${entry.lampMinutes}</td>
            <td>${entry.energiLampu}</td>
            <td>${percentage}%</td>
            <td>${avgLight} lux</td>
        `;
        
        lampBody.appendChild(row);
    });
}

function updateACTable() {
    const acBody = document.getElementById("ac-body");
    if (!acBody) return;
    
    acBody.innerHTML = "";
    
    historyData.forEach((entry, index) => {
        const row = document.createElement("tr");
        const date = new Date(entry.date);
        const formattedDate = date.toLocaleDateString('id-ID');
        const percentage = ((entry.energiAC / entry.totalEnergi) * 100).toFixed(1);
        const avgTemp = Math.floor(Math.random() * 4) + 23; // 23-26°C
        
        row.innerHTML = `
            <td>${index + 1}</td>
            <td>${formattedDate}</td>
            <td>${entry.acMinutes}</td>
            <td>${entry.energiAC}</td>
            <td>${percentage}%</td>
            <td>${avgTemp}°C</td>
        `;
        
        acBody.appendChild(row);
    });
}

function updateOtherDevicesTable() {
    const otherDevicesBody = document.getElementById("other-devices-body");
    if (!otherDevicesBody) return;
    
    otherDevicesBody.innerHTML = "";
    
    historyData.forEach((entry, index) => {
        const row = document.createElement("tr");
        const date = new Date(entry.date);
        const formattedDate = date.toLocaleDateString('id-ID');
        
        // Get device usage data or use default values
        const computerUsage = entry.devices?.computer?.usage || Math.floor(Math.random() * 300) + 100;
        const tvUsage = entry.devices?.tv?.usage || Math.floor(Math.random() * 120) + 30;
        const dispenserUsage = entry.devices?.dispenser?.usage || Math.floor(Math.random() * 480) + 120;
        const kettleUsage = entry.devices?.kettle?.usage || Math.floor(Math.random() * 60) + 10;
        const coffeeUsage = entry.devices?.coffee?.usage || Math.floor(Math.random() * 45) + 5;
        
        // Calculate total energy for other devices
        const computerEnergy = hitungEnergiDevice('computer', computerUsage);
        const tvEnergy = hitungEnergiDevice('tv', tvUsage);
        const dispenserEnergy = hitungEnergiDevice('dispenser', dispenserUsage);
        const kettleEnergy = hitungEnergiDevice('kettle', kettleUsage);
        const coffeeEnergy = hitungEnergiDevice('coffee', coffeeUsage);
        const totalOtherEnergy = computerEnergy + tvEnergy + dispenserEnergy + kettleEnergy + coffeeEnergy;
        
        row.innerHTML = `
            <td>${index + 1}</td>
            <td>${formattedDate}</td>
            <td>${computerUsage}</td>
            <td>${tvUsage}</td>
            <td>${dispenserUsage}</td>
            <td>${kettleUsage}</td>
            <td>${coffeeUsage}</td>
            <td><strong>${totalOtherEnergy}</strong></td>
        `;
        
        otherDevicesBody.appendChild(row);
    });
}

function updateEnvironmentTable() {
    const envBody = document.getElementById("environment-body");
    if (!envBody) return;
    
    envBody.innerHTML = "";
    
    historyData.forEach((entry, index) => {
        const row = document.createElement("tr");
        const date = new Date(entry.date);
        const formattedDate = date.toLocaleDateString('id-ID');
        const avgHumidity = Math.floor(Math.random() * 15) + 60; // 60-75%
        const avgLight = Math.floor(Math.random() * 300) + 350; // 350-650 lux
        
        row.innerHTML = `
            <td>${index + 1}</td>
            <td>${formattedDate}</td>
            <td>${entry.avgTemp}°C</td>
            <td>${avgHumidity}%</td>
            <td>${entry.totalPeople}</td>
            <td>${avgLight} lux</td>
        `;
        
        envBody.appendChild(row);
    });
}

/* ===== UPDATE DAILY USAGE DISPLAY ===== */
function updateDailyUsageDisplay() {
    const lampUsageEl = document.getElementById("dailyLampUsage");
    const acUsageEl = document.getElementById("dailyACUsage");
    const totalEnergyEl = document.getElementById("dailyTotalEnergy");
    const workingHoursEl = document.getElementById("workingHoursStatus");
    
    if (lampUsageEl) {
        lampUsageEl.textContent = Math.round(dailyData.lampUsage) + " menit";
    }
    
    if (acUsageEl) {
        acUsageEl.textContent = Math.round(dailyData.acUsage) + " menit";
    }
    
    if (totalEnergyEl) {
        const totalEnergy = hitungTotalEnergiHarian();
        totalEnergyEl.textContent = totalEnergy + " Wh";
    }
    
    if (workingHoursEl) {
        const workingHours = isWorkingHours();
        if (workingHours) {
            workingHoursEl.textContent = "Jam Kerja Aktif";
            workingHoursEl.className = "status-on";
        } else {
            workingHoursEl.textContent = "Di Luar Jam Kerja";
            workingHoursEl.className = "status-off";
        }
    }
}

/* ===== OVERRIDE UPDATE HISTORY DISPLAY ===== */
function updateHistoryDisplay() {
    updateSeparatedHistoryDisplay();
}

/* ===== OVERRIDE UPDATE CURRENT USAGE DISPLAY ===== */
const originalUpdateCurrentUsageDisplay = updateCurrentUsageDisplay;
updateCurrentUsageDisplay = function() {
    originalUpdateCurrentUsageDisplay();
    updateDailyUsageDisplay();
};
/* ===== OTHER DEVICES CONTROL ===== */
const otherDevicesList = ['computer', 'tv', 'fridge', 'router', 'kettle', 'coffee'];

function updateOtherDevicesStatus() {
    let onCount = 0;
    let totalCount = otherDevicesList.length;
    
    otherDevicesList.forEach(deviceId => {
        const statusElement = document.getElementById(deviceId + "Status");
        if (statusElement && statusElement.innerHTML === "MENYALA") {
            onCount++;
        }
        
        // Update mini status display
        const miniStatusElement = statusElement;
        if (miniStatusElement) {
            if (miniStatusElement.innerHTML === "MENYALA") {
                miniStatusElement.innerHTML = "ON";
                miniStatusElement.className = "mini-status status-on";
            } else {
                miniStatusElement.innerHTML = "OFF";
                miniStatusElement.className = "mini-status status-off";
            }
        }
    });
    
    // Update main status
    const mainStatusElement = document.getElementById("otherDevicesStatus");
    if (mainStatusElement) {
        if (onCount === 0) {
            mainStatusElement.innerHTML = "SEMUA MATI";
            mainStatusElement.className = "status-off";
        } else if (onCount === totalCount) {
            mainStatusElement.innerHTML = "SEMUA MENYALA";
            mainStatusElement.className = "status-on";
        } else {
            mainStatusElement.innerHTML = `${onCount}/${totalCount} MENYALA`;
            mainStatusElement.className = "status-partial";
        }
    }
}

function toggleAllOtherDevices() {
    let onCount = 0;
    
    // Count currently on devices
    otherDevicesList.forEach(deviceId => {
        const statusElement = document.getElementById(deviceId + "Status");
        if (statusElement && statusElement.innerHTML === "MENYALA") {
            onCount++;
        }
    });
    
    // If more than half are on, turn all off. Otherwise, turn all on.
    const shouldTurnOn = onCount < (otherDevicesList.length / 2);
    
    otherDevicesList.forEach(deviceId => {
        const statusElement = document.getElementById(deviceId + "Status");
        if (statusElement) {
            const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
            
            // Skip always-on devices for turning off
            const device = deviceData[deviceId];
            if (!shouldTurnOn && device && device.alwaysOn) {
                return;
            }
            
            if (shouldTurnOn && !isCurrentlyOn) {
                toggleDevice(deviceId);
            } else if (!shouldTurnOn && isCurrentlyOn) {
                toggleDevice(deviceId);
            }
        }
    });
    
    const action = shouldTurnOn ? "dinyalakan" : "dimatikan";
    showNotification(`Semua perangkat lainnya ${action}`, 'info');
}

/* ===== OVERRIDE TOGGLE DEVICE TO UPDATE OTHER DEVICES STATUS ===== */
const originalToggleDevice = toggleDevice;
toggleDevice = function(deviceId) {
    originalToggleDevice(deviceId);
    
    // Update other devices status if it's one of the other devices
    if (otherDevicesList.includes(deviceId)) {
        setTimeout(updateOtherDevicesStatus, 100);
    }
};

/* ===== INITIALIZE OTHER DEVICES STATUS ===== */
document.addEventListener('DOMContentLoaded', function() {
    // Set initial status for always-on devices
    const alwaysOnDevices = ['fridge', 'router'];
    alwaysOnDevices.forEach(deviceId => {
        const statusElement = document.getElementById(deviceId + "Status");
        if (statusElement) {
            statusElement.innerHTML = "MENYALA";
            statusElement.className = "status-on";
        }
    });
    
    // Update other devices status
    setTimeout(updateOtherDevicesStatus, 500);
});
/* ===== LAMP CIRCUITS CONTROL ===== */
let lampCircuits = {
    circuit1: false,
    circuit2: false
};

function toggleLampCircuit(circuitNumber) {
    checkDailyReset();
    
    const statusElement = document.getElementById(`lamp${circuitNumber}Status`);
    if (!statusElement) return;
    
    // Check if panel is on before allowing lamp to turn on
    if (!isPanelOn()) {
        const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
        if (!isCurrentlyOn) {
            showNotification(`Tidak dapat menyalakan Jalur ${circuitNumber} lampu - Panel listrik mati`, 'warning');
            return;
        }
    }
    
    const workingHours = isWorkingHours();
    const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
    
    if (isCurrentlyOn) {
        // Turn OFF
        statusElement.innerHTML = "MATI";
        statusElement.className = "mini-status status-off";
        lampCircuits[`circuit${circuitNumber}`] = false;
        
        // Calculate usage for this circuit (6 units)
        if (dailyData.devices.lamp.startTime && workingHours) {
            let duration = (Date.now() - dailyData.devices.lamp.startTime) / (1000 * 60);
            dailyData.devices.lamp.usage += duration * 0.5; // 50% of total lamp usage
        }
        
        showNotification(`Jalur ${circuitNumber} lampu dimatikan (6 unit)`, 'info');
    } else {
        // Turn ON
        statusElement.innerHTML = "MENYALA";
        statusElement.className = "mini-status status-on";
        lampCircuits[`circuit${circuitNumber}`] = true;
        
        // Start tracking if working hours
        if (workingHours) {
            dailyData.devices.lamp.startTime = Date.now();
        }
        
        showNotification(`Jalur ${circuitNumber} lampu dinyalakan (6 unit)`, 'success');
    }
    
    updateLampStatus();
    updateCurrentUsageDisplay();
}

function toggleAllLamps() {
    const bothOn = lampCircuits.circuit1 && lampCircuits.circuit2;
    const shouldTurnOn = !bothOn;
    
    if (shouldTurnOn) {
        // Turn on both circuits if not both are on
        if (!lampCircuits.circuit1) toggleLampCircuit(1);
        if (!lampCircuits.circuit2) toggleLampCircuit(2);
        showNotification('Semua lampu dinyalakan', 'success');
    } else {
        // Turn off both circuits
        if (lampCircuits.circuit1) toggleLampCircuit(1);
        if (lampCircuits.circuit2) toggleLampCircuit(2);
        showNotification('Semua lampu dimatikan', 'info');
    }
}

function updateLampStatus() {
    const mainStatusElement = document.getElementById("lampStatus");
    if (!mainStatusElement) return;
    
    const circuit1On = lampCircuits.circuit1;
    const circuit2On = lampCircuits.circuit2;
    
    if (!circuit1On && !circuit2On) {
        mainStatusElement.innerHTML = "SEMUA MATI";
        mainStatusElement.className = "status-off";
    } else if (circuit1On && circuit2On) {
        mainStatusElement.innerHTML = "SEMUA MENYALA";
        mainStatusElement.className = "status-on";
    } else {
        mainStatusElement.innerHTML = "SEBAGIAN MENYALA";
        mainStatusElement.className = "status-partial";
    }
}

/* ===== AC CIRCUITS CONTROL ===== */
let acUnits = {
    unit1: false,
    unit2: false
};

function toggleACUnit(unitNumber) {
    checkDailyReset();
    
    const statusElement = document.getElementById(`ac${unitNumber}Status`);
    if (!statusElement) return;
    
    // Check if panel is on before allowing AC to turn on
    if (!isPanelOn()) {
        const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
        if (!isCurrentlyOn) {
            showNotification(`Tidak dapat menyalakan AC Unit ${unitNumber} - Panel listrik mati`, 'warning');
            return;
        }
    }
    
    const workingHours = isWorkingHours();
    const isCurrentlyOn = statusElement.innerHTML === "MENYALA";
    
    if (isCurrentlyOn) {
        // Turn OFF
        statusElement.innerHTML = "MATI";
        statusElement.className = "mini-status status-off";
        acUnits[`unit${unitNumber}`] = false;
        
        // Calculate usage for this unit
        if (dailyData.devices.ac.startTime && workingHours) {
            let duration = (Date.now() - dailyData.devices.ac.startTime) / (1000 * 60);
            dailyData.devices.ac.usage += duration * 0.5; // 50% of total AC usage
        }
        
        showNotification(`AC Unit ${unitNumber} dimatikan`, 'info');
    } else {
        // Turn ON
        statusElement.innerHTML = "MENYALA";
        statusElement.className = "mini-status status-on";
        acUnits[`unit${unitNumber}`] = true;
        
        // Start tracking if working hours
        if (workingHours) {
            dailyData.devices.ac.startTime = Date.now();
        }
        
        showNotification(`AC Unit ${unitNumber} dinyalakan`, 'success');
    }
    
    updateACStatus();
    updateCurrentUsageDisplay();
}

function toggleAllAC() {
    const bothOn = acUnits.unit1 && acUnits.unit2;
    const shouldTurnOn = !bothOn;
    
    if (shouldTurnOn) {
        // Turn on both units if not both are on
        if (!acUnits.unit1) toggleACUnit(1);
        if (!acUnits.unit2) toggleACUnit(2);
        showNotification('Semua AC dinyalakan', 'success');
    } else {
        // Turn off both units
        if (acUnits.unit1) toggleACUnit(1);
        if (acUnits.unit2) toggleACUnit(2);
        showNotification('Semua AC dimatikan', 'info');
    }
}

function updateACStatus() {
    const mainStatusElement = document.getElementById("acStatus");
    if (!mainStatusElement) return;
    
    const unit1On = acUnits.unit1;
    const unit2On = acUnits.unit2;
    
    if (!unit1On && !unit2On) {
        mainStatusElement.innerHTML = "SEMUA MATI";
        mainStatusElement.className = "status-off";
    } else if (unit1On && unit2On) {
        mainStatusElement.innerHTML = "SEMUA MENYALA";
        mainStatusElement.className = "status-on";
    } else {
        mainStatusElement.innerHTML = "SEBAGIAN MENYALA";
        mainStatusElement.className = "status-partial";
    }
}

/* ===== OVERRIDE LEGACY FUNCTIONS ===== */
function toggleLampu() {
    toggleAllLamps();
}

function toggleAC() {
    toggleAllAC();
}
/* ===== SISTEM AUTO SHUTDOWN ===== */
let autoShutdownEnabled = true;
let lastPersonDetected = Date.now();
let noPersonTimeout = 15 * 60 * 1000; // 15 menit dalam milliseconds
let shutdownWarningShown = false;

// Perangkat yang akan dimatikan otomatis (termasuk panel listrik)
const autoShutdownDevices = ['ac', 'lamp', 'computer', 'tv', 'dispenser', 'kettle', 'coffee', 'panel'];

function checkAutoShutdown() {
    const workingHours = isWorkingHours();
    const isWorkingDayToday = isWorkingDay();
    const currentPeople = parseInt(document.getElementById("jumlahOrang").innerHTML) || 0;
    const now = Date.now();
    
    // Update waktu terakhir ada orang
    if (currentPeople > 0) {
        lastPersonDetected = now;
        shutdownWarningShown = false;
    }
    
    // Cek kondisi untuk auto shutdown
    let shutdownReason = "";
    let shouldShutdown = false;
    
    if (!workingHours) {
        if (!isWorkingDayToday && workingDaysEnabled) {
            shutdownReason = "hari libur";
            shouldShutdown = true;
        } else {
            shutdownReason = "jam kerja berakhir";
            shouldShutdown = true;
        }
    } else if (now - lastPersonDetected > noPersonTimeout) {
        shutdownReason = "tidak ada orang terdeteksi selama 15 menit";
        shouldShutdown = true;
    }
    
    if (shouldShutdown && autoShutdownEnabled) {
        // Peringatan 5 menit sebelum shutdown (hanya untuk kondisi tidak ada orang)
        if (shutdownReason.includes("tidak ada orang") && !shutdownWarningShown && 
            (now - lastPersonDetected > (noPersonTimeout - 5 * 60 * 1000))) {
            showNotification("Peringatan: Sistem akan mematikan perangkat dalam 5 menit karena tidak ada aktivitas", "warning");
            shutdownWarningShown = true;
        }
        
        // Lakukan shutdown
        if ((shutdownReason.includes("tidak ada orang") && now - lastPersonDetected >= noPersonTimeout) || 
            !workingHours) {
            performAutoShutdown(shutdownReason);
        }
    }
}

function performAutoShutdown(reason) {
    let devicesShutdown = [];
    
    autoShutdownDevices.forEach(deviceId => {
        const device = deviceData[deviceId];
        if (!device) return;
        
        // Skip always-on devices kecuali panel listrik saat shutdown total
        if (device.alwaysOn && deviceId !== 'panel') return;
        
        // Cek apakah perangkat sedang menyala
        let isOn = false;
        
        if (deviceId === 'ac') {
            isOn = acUnits.unit1 || acUnits.unit2;
            if (isOn) {
                if (acUnits.unit1) toggleACUnit(1);
                if (acUnits.unit2) toggleACUnit(2);
                devicesShutdown.push('AC');
            }
        } else if (deviceId === 'lamp') {
            isOn = lampCircuits.circuit1 || lampCircuits.circuit2;
            if (isOn) {
                if (lampCircuits.circuit1) toggleLampCircuit(1);
                if (lampCircuits.circuit2) toggleLampCircuit(2);
                devicesShutdown.push('Lampu');
            }
        } else if (deviceId === 'panel') {
            // Panel listrik dimatikan terakhir
            const statusElement = document.getElementById(deviceId + "Status");
            isOn = statusElement && statusElement.innerHTML === "MENYALA";
            if (isOn) {
                // Matikan panel listrik setelah delay 2 detik
                setTimeout(() => {
                    toggleDevice(deviceId);
                    showNotification("Panel Listrik dimatikan - Shutdown lengkap", "warning");
                }, 2000);
                devicesShutdown.push('Panel Listrik');
            }
        } else {
            const statusElement = document.getElementById(deviceId + "Status");
            isOn = statusElement && statusElement.innerHTML === "MENYALA";
            if (isOn) {
                toggleDevice(deviceId);
                devicesShutdown.push(device.name);
            }
        }
    });
    
    if (devicesShutdown.length > 0) {
        showNotification(`Auto Shutdown: ${devicesShutdown.join(', ')} dimatikan karena ${reason}`, "warning");
        
        // Log shutdown event
        console.log(`Auto Shutdown executed at ${new Date().toLocaleString()}: ${devicesShutdown.join(', ')} - Reason: ${reason}`);
        
        // Simpan data jika jam kerja berakhir
        if (reason.includes("jam kerja") || reason.includes("operasional")) {
            setTimeout(() => {
                saveToHistory();
                showNotification("Data harian telah disimpan ke history", "success");
            }, 3000);
        }
    }
    
    // Reset warning flag
    shutdownWarningShown = false;
}

function toggleAutoShutdown() {
    // Get the toggle element that was clicked
    const clickedToggle = event.target.closest('.toggle-switch') || document.getElementById("autoShutdownToggle");
    
    // Toggle the visual state first
    if (clickedToggle) {
        clickedToggle.classList.toggle("active");
        autoShutdownEnabled = clickedToggle.classList.contains("active");
    } else {
        // Fallback if called programmatically
        autoShutdownEnabled = !autoShutdownEnabled;
    }
    
    const status = autoShutdownEnabled ? "diaktifkan" : "dinonaktifkan";
    showNotification(`Auto Shutdown ${status}`, autoShutdownEnabled ? "success" : "info");
}

// Cek auto shutdown setiap 30 detik
setInterval(checkAutoShutdown, 30000);

/* ===== EMERGENCY OVERRIDE ===== */
function emergencyOverride() {
    // Reset timer untuk mencegah shutdown
    lastPersonDetected = Date.now();
    shutdownWarningShown = false;
    showNotification("Emergency Override: Auto shutdown ditunda selama 15 menit", "info");
}

/* ===== MANUAL SHUTDOWN ALL ===== */
function manualShutdownAll() {
    if (confirm("Apakah Anda yakin ingin mematikan semua perangkat?")) {
        performAutoShutdown("shutdown manual");
    }
}

/* ===== UPDATE WORKING HOURS STATUS WITH AUTO SHUTDOWN INFO ===== */
function updateWorkingHoursStatusWithShutdown() {
    const workingHours = isWorkingHours();
    const isWorkingDayToday = isWorkingDay();
    const workingHoursEl = document.getElementById("workingHoursStatus");
    const currentPeople = parseInt(document.getElementById("jumlahOrang").innerHTML) || 0;
    const timeSinceLastPerson = Math.floor((Date.now() - lastPersonDetected) / (1000 * 60)); // dalam menit
    const dayStatus = getWorkingDayStatus();
    
    if (workingHoursEl) {
        if (workingHours) {
            if (currentPeople > 0) {
                workingHoursEl.textContent = `Jam Kerja Aktif (${dayStatus})`;
                workingHoursEl.className = "status-on";
            } else if (timeSinceLastPerson > 10) {
                workingHoursEl.textContent = `Tidak Ada Orang (${timeSinceLastPerson} menit)`;
                workingHoursEl.className = "status-partial";
            } else {
                workingHoursEl.textContent = `Jam Kerja Aktif (${dayStatus})`;
                workingHoursEl.className = "status-on";
            }
        } else {
            if (!isWorkingDayToday && workingDaysEnabled) {
                workingHoursEl.textContent = `Hari Libur (${dayStatus})`;
                workingHoursEl.className = "status-off";
            } else {
                workingHoursEl.textContent = `Di Luar Jam Kerja (${dayStatus})`;
                workingHoursEl.className = "status-off";
            }
        }
    }
}

// Override fungsi updateDailyUsageDisplay untuk include auto shutdown info
const originalUpdateDailyUsageDisplay = updateDailyUsageDisplay;
updateDailyUsageDisplay = function() {
    originalUpdateDailyUsageDisplay();
    updateWorkingHoursStatusWithShutdown();
};
/* ===== UPDATE AUTO SHUTDOWN STATUS DISPLAY ===== */
function updateAutoShutdownDisplay() {
    const autoShutdownStatusEl = document.getElementById("autoShutdownStatus");
    const noPersonTimeEl = document.getElementById("noPersonTime");
    const shutdownCountdownEl = document.getElementById("shutdownCountdown");
    
    const currentPeople = parseInt(document.getElementById("jumlahOrang").innerHTML) || 0;
    const timeSinceLastPerson = Math.floor((Date.now() - lastPersonDetected) / (1000 * 60)); // dalam menit
    const timeUntilShutdown = Math.max(0, 15 - timeSinceLastPerson); // 15 menit timeout
    const workingHours = isWorkingHours();
    
    // Update auto shutdown status
    if (autoShutdownStatusEl) {
        if (autoShutdownEnabled) {
            autoShutdownStatusEl.textContent = "AKTIF";
            autoShutdownStatusEl.className = "status-on";
        } else {
            autoShutdownStatusEl.textContent = "NONAKTIF";
            autoShutdownStatusEl.className = "status-off";
        }
    }
    
    // Update no person time
    if (noPersonTimeEl) {
        if (currentPeople > 0) {
            noPersonTimeEl.textContent = "0 menit";
        } else {
            noPersonTimeEl.textContent = timeSinceLastPerson + " menit";
        }
    }
    
    // Update shutdown countdown
    if (shutdownCountdownEl) {
        if (!workingHours) {
            shutdownCountdownEl.textContent = "Di Luar Jam Kerja";
            shutdownCountdownEl.style.color = "#dc3545";
        } else if (currentPeople > 0) {
            shutdownCountdownEl.textContent = "Normal";
            shutdownCountdownEl.style.color = "#28a745";
        } else if (timeUntilShutdown > 5) {
            shutdownCountdownEl.textContent = `${timeUntilShutdown} menit lagi`;
            shutdownCountdownEl.style.color = "#ffc107";
        } else if (timeUntilShutdown > 0) {
            shutdownCountdownEl.textContent = `${timeUntilShutdown} menit lagi!`;
            shutdownCountdownEl.style.color = "#dc3545";
        } else {
            shutdownCountdownEl.textContent = "Shutdown Aktif";
            shutdownCountdownEl.style.color = "#dc3545";
        }
    }
}

// Override fungsi updateCurrentUsageDisplay untuk include auto shutdown display
const originalUpdateCurrentUsageDisplayWithShutdown = updateCurrentUsageDisplay;
updateCurrentUsageDisplay = function() {
    originalUpdateCurrentUsageDisplayWithShutdown();
    updateAutoShutdownDisplay();
};
/* ===== JADWAL OTOMATIS SHUTDOWN ===== */
let scheduledShutdownEnabled = true;

function checkScheduledShutdown() {
    if (!scheduledShutdownEnabled) return;
    
    const now = new Date();
    const hour = now.getHours();
    const minute = now.getMinutes();
    const isWorkingDayToday = isWorkingDay();
    
    // Cek apakah waktu adalah 17:00 dan hari kerja (jika working days enabled)
    if (hour === 17 && minute === 0 && now.getSeconds() === 0) {
        if (!workingDaysEnabled || isWorkingDayToday) {
            performAutoShutdown("jadwal otomatis - akhir jam operasional (17:00)");
            showNotification("Jadwal Otomatis: Listrik ruangan dimatikan - akhir jam operasional (17:00)", "info");
        } else {
            console.log("Scheduled shutdown skipped - not a working day");
        }
    }
    
    // Cek apakah di luar jam operasional (sebelum 07:00 atau setelah 17:00)
    const isOutsideOperatingHours = hour < 7 || hour >= 17;
    
    if (isOutsideOperatingHours && (!workingDaysEnabled || isWorkingDayToday)) {
        // Jika ada perangkat yang masih menyala di luar jam operasional, matikan
        const stats = getDeviceUsageStats();
        if (stats.totalDevicesOn > 0) {
            // Hanya matikan jika belum dimatikan dalam 5 menit terakhir
            const lastShutdownTime = localStorage.getItem('lastScheduledShutdown');
            const nowTime = Date.now();
            if (!lastShutdownTime || (nowTime - parseInt(lastShutdownTime)) > 5 * 60 * 1000) {
                performAutoShutdown("di luar jam operasional (07:00-17:00)");
                localStorage.setItem('lastScheduledShutdown', nowTime.toString());
                showNotification("Jadwal Otomatis: Listrik dimatikan - di luar jam operasional", "warning");
            }
        }
    }
}

function toggleScheduledShutdown() {
    if (useArduinoData) {
        toggleArduinoScheduledShutdown();
        return;
    }
    
    // Get the toggle element that was clicked
    const clickedToggle = event.target.closest('.toggle-switch');
    
    // Toggle the visual state first
    if (clickedToggle) {
        clickedToggle.classList.toggle("active");
        scheduledShutdownEnabled = clickedToggle.classList.contains("active");
    } else {
        // Fallback if called programmatically
        scheduledShutdownEnabled = !scheduledShutdownEnabled;
    }
    
    const status = scheduledShutdownEnabled ? "diaktifkan" : "dinonaktifkan";
    showNotification(`Jadwal Otomatis ${status}`, scheduledShutdownEnabled ? "success" : "info");
}

// Cek jadwal otomatis setiap detik
setInterval(checkScheduledShutdown, 1000);
/* ===== GENERATE SAMPLE HISTORY DATA ===== */
function generateSampleHistoryData() {
    if (historyData.length > 0) return; // Jangan generate jika sudah ada data
    
    const sampleData = [];
    const today = new Date();
    
    for (let i = 7; i >= 1; i--) {
        const date = new Date(today);
        date.setDate(date.getDate() - i);
        
        // Generate random but realistic usage data
        const acUsage = Math.floor(Math.random() * 200) + 100; // 100-300 menit
        const lampUsage = Math.floor(Math.random() * 300) + 200; // 200-500 menit
        const computerUsage = Math.floor(Math.random() * 250) + 150; // 150-400 menit
        const tvUsage = Math.floor(Math.random() * 120) + 30; // 30-150 menit
        const dispenserUsage = Math.floor(Math.random() * 400) + 200; // 200-600 menit
        const kettleUsage = Math.floor(Math.random() * 60) + 15; // 15-75 menit
        const coffeeUsage = Math.floor(Math.random() * 45) + 10; // 10-55 menit
        
        // Always-on devices (8 jam kerja = 480 menit)
        const fridgeUsage = 480;
        const routerUsage = 480;
        const panelUsage = 480;
        
        const entry = {
            date: date.toDateString(),
            avgTemp: Math.floor(Math.random() * 4) + 25, // 25-28°C
            totalPeople: Math.floor(Math.random() * 20) + 25, // 25-45 orang
            timestamp: date.toISOString(),
            devices: {
                ac: {
                    usage: acUsage,
                    energy: hitungEnergiDevice('ac', acUsage),
                    deviceName: deviceData.ac.name,
                    power: deviceData.ac.power
                },
                lamp: {
                    usage: lampUsage,
                    energy: hitungEnergiDevice('lamp', lampUsage),
                    deviceName: deviceData.lamp.name,
                    power: deviceData.lamp.power
                },
                computer: {
                    usage: computerUsage,
                    energy: hitungEnergiDevice('computer', computerUsage),
                    deviceName: deviceData.computer.name,
                    power: deviceData.computer.power
                },
                tv: {
                    usage: tvUsage,
                    energy: hitungEnergiDevice('tv', tvUsage),
                    deviceName: deviceData.tv.name,
                    power: deviceData.tv.power
                },
                dispenser: {
                    usage: dispenserUsage,
                    energy: hitungEnergiDevice('dispenser', dispenserUsage),
                    deviceName: deviceData.dispenser.name,
                    power: deviceData.dispenser.power
                },
                kettle: {
                    usage: kettleUsage,
                    energy: hitungEnergiDevice('kettle', kettleUsage),
                    deviceName: deviceData.kettle.name,
                    power: deviceData.kettle.power
                },
                coffee: {
                    usage: coffeeUsage,
                    energy: hitungEnergiDevice('coffee', coffeeUsage),
                    deviceName: deviceData.coffee.name,
                    power: deviceData.coffee.power
                },
                fridge: {
                    usage: fridgeUsage,
                    energy: hitungEnergiDevice('fridge', fridgeUsage),
                    deviceName: deviceData.fridge.name,
                    power: deviceData.fridge.power
                },
                router: {
                    usage: routerUsage,
                    energy: hitungEnergiDevice('router', routerUsage),
                    deviceName: deviceData.router.name,
                    power: deviceData.router.power
                },
                panel: {
                    usage: panelUsage,
                    energy: hitungEnergiDevice('panel', panelUsage),
                    deviceName: deviceData.panel.name,
                    power: deviceData.panel.power
                }
            }
        };
        
        // Calculate total energy
        entry.totalEnergi = Object.values(entry.devices).reduce((sum, device) => sum + device.energy, 0);
        
        // Add percentages
        Object.keys(entry.devices).forEach(deviceId => {
            entry.devices[deviceId].percentage = ((entry.devices[deviceId].energy / entry.totalEnergi) * 100).toFixed(1);
        });
        
        // Legacy fields
        entry.lampMinutes = entry.devices.lamp.usage;
        entry.acMinutes = entry.devices.ac.usage;
        entry.energiLampu = entry.devices.lamp.energy;
        entry.energiAC = entry.devices.ac.energy;
        
        sampleData.push(entry);
    }
    
    historyData = sampleData;
    localStorage.setItem('energyHistory', JSON.stringify(historyData));
    console.log('Sample history data generated:', sampleData.length, 'entries');
}

/* ===== INITIALIZE HISTORY DATA ===== */
document.addEventListener('DOMContentLoaded', function() {
    // Generate sample data if needed
    generateSampleHistoryData();
    
    // Update all history displays
    updateHistoryDisplay();
});
/* ===== WORKING DAYS CONTROL ===== */
function toggleWorkingDays() {
    if (useArduinoData) {
        toggleArduinoWorkingDays();
        return;
    }
    
    // Get the toggle element that was clicked
    const clickedToggle = event.target.closest('.toggle-switch');
    
    // Toggle the visual state first
    if (clickedToggle) {
        clickedToggle.classList.toggle("active");
        workingDaysEnabled = clickedToggle.classList.contains("active");
    } else {
        // Fallback if called programmatically
        workingDaysEnabled = !workingDaysEnabled;
    }
    
    const status = workingDaysEnabled ? "diaktifkan" : "dinonaktifkan";
    showNotification(`Hari Kerja (Senin-Jumat) ${status}`, workingDaysEnabled ? "success" : "info");
    
    // Update working hours status immediately
    updateWorkingHoursStatusWithShutdown();
}

function getDayName(dayNumber) {
    const days = ['Minggu', 'Senin', 'Selasa', 'Rabu', 'Kamis', 'Jumat', 'Sabtu'];
    return days[dayNumber];
}

function getWorkingDayStatus() {
    const now = new Date();
    const dayOfWeek = now.getDay();
    const dayName = getDayName(dayOfWeek);
    const isWorkingDayToday = isWorkingDay();
    
    if (!workingDaysEnabled) {
        return `${dayName} (Mode 7 Hari)`;
    } else if (isWorkingDayToday) {
        return `${dayName} (Hari Kerja)`;
    } else {
        return `${dayName} (Hari Libur)`;
    }
}
/* ===== UPDATE WORKING DAY INFO DISPLAY ===== */
function updateWorkingDayInfoDisplay() {
    const workingDayInfoEl = document.getElementById("workingDayInfo");
    if (workingDayInfoEl) {
        if (workingDaysEnabled) {
            workingDayInfoEl.textContent = "Senin - Jumat";
            workingDayInfoEl.style.color = "#1e5eff";
        } else {
            workingDayInfoEl.textContent = "7 Hari Seminggu";
            workingDayInfoEl.style.color = "#28a745";
        }
    }
}

// Override fungsi updateCurrentUsageDisplay untuk include working day info
const originalUpdateCurrentUsageDisplayWithWorkingDay = updateCurrentUsageDisplay;
updateCurrentUsageDisplay = function() {
    originalUpdateCurrentUsageDisplayWithWorkingDay();
    updateWorkingDayInfoDisplay();
};
/* ===== AUTO LAMP CONTROL BASED ON PEOPLE COUNT ===== */
let autoLampControlEnabled = true;
let lastPeopleCount = 0;

function toggleAutoLampControl() {
    // Get the toggle element that was clicked
    const clickedToggle = event.target.closest('.toggle-switch');
    
    // Toggle the visual state first
    if (clickedToggle) {
        clickedToggle.classList.toggle("active");
        autoLampControlEnabled = clickedToggle.classList.contains("active");
    } else {
        // Fallback if called programmatically
        autoLampControlEnabled = !autoLampControlEnabled;
    }
    
    const status = autoLampControlEnabled ? "diaktifkan" : "dinonaktifkan";
    showNotification(`Kontrol Lampu Otomatis ${status}`, autoLampControlEnabled ? "success" : "info");
}

/* ===== PRESENCE DETECTION CONTROL ===== */
let presenceDetectionEnabled = true;

function togglePresenceDetection() {
    // Get the toggle element that was clicked
    const clickedToggle = event.target.closest('.toggle-switch');
    
    // Toggle the visual state first
    if (clickedToggle) {
        clickedToggle.classList.toggle("active");
        presenceDetectionEnabled = clickedToggle.classList.contains("active");
    } else {
        // Fallback if called programmatically
        presenceDetectionEnabled = !presenceDetectionEnabled;
    }
    
    const status = presenceDetectionEnabled ? "diaktifkan" : "dinonaktifkan";
    showNotification(`Deteksi Kehadiran & AC Otomatis ${status}`, presenceDetectionEnabled ? "success" : "info");
}

/* ===== AUTO AC CONTROL BASED ON PRESENCE ===== */
function checkAutoACControl(currentPeople) {
    if (!presenceDetectionEnabled) return;
    
    const bothACOn = acUnits.unit1 && acUnits.unit2;
    const anyACOn = acUnits.unit1 || acUnits.unit2;
    
    if (currentPeople > 0) {
        // Ada orang di ruangan - nyalakan AC jika belum menyala
        if (!anyACOn) {
            // Nyalakan kedua unit AC
            if (!acUnits.unit1) toggleACUnit(1);
            if (!acUnits.unit2) toggleACUnit(2);
            showNotification(`AC dinyalakan otomatis - ${currentPeople} orang terdeteksi`, 'success');
        }
    } else {
        // Tidak ada orang - matikan AC jika masih menyala
        if (anyACOn) {
            // Matikan kedua unit AC
            if (acUnits.unit1) toggleACUnit(1);
            if (acUnits.unit2) toggleACUnit(2);
            showNotification('AC dimatikan otomatis - tidak ada orang terdeteksi', 'info');
        }
    }
}

/* ===== ARDUINO CONNECTION MANAGEMENT ===== */
function toggleArduinoConnection() {
    const toggle = document.getElementById('arduinoToggle');
    const statusElement = document.getElementById('connectionStatus');
    
    toggle.classList.toggle('active');
    useArduinoData = toggle.classList.contains('active');
    
    if (useArduinoData) {
        // Test connection
        testArduinoConnection();
        showNotification('Koneksi Arduino diaktifkan', 'info');
    } else {
        statusElement.textContent = 'Terputus';
        statusElement.className = 'connection-status disconnected';
        showNotification('Koneksi Arduino dinonaktifkan', 'info');
    }
    
    // Save setting
    localStorage.setItem('useArduinoData', useArduinoData);
}

function updateArduinoIP() {
    const ipInput = document.getElementById('arduinoIP');
    arduinoApiUrl = `http://${ipInput.value}`;
    
    // Save setting
    localStorage.setItem('arduinoIP', ipInput.value);
    
    // Test connection if enabled
    if (useArduinoData) {
        testArduinoConnection();
    }
    
    showNotification('IP Arduino diperbarui', 'info');
}

async function testArduinoConnection() {
    const statusElement = document.getElementById('connectionStatus');
    
    statusElement.textContent = 'Menghubungkan...';
    statusElement.className = 'connection-status connecting';
    
    try {
        const response = await fetch(`${arduinoApiUrl}/api/status`, {
            method: 'GET',
            timeout: 5000
        });
        
        if (response.ok) {
            statusElement.textContent = 'Terhubung';
            statusElement.className = 'connection-status connected';
            showNotification('Koneksi Arduino berhasil', 'success');
        } else {
            throw new Error('Connection failed');
        }
    } catch (error) {
        statusElement.textContent = 'Gagal Terhubung';
        statusElement.className = 'connection-status disconnected';
        useArduinoData = false;
        document.getElementById('arduinoToggle').classList.remove('active');
        showNotification('Gagal terhubung ke Arduino', 'warning');
    }
}

// Load Arduino settings on page load
document.addEventListener('DOMContentLoaded', function() {
    const savedUseArduino = localStorage.getItem('useArduinoData');
    const savedIP = localStorage.getItem('arduinoIP');
    
    if (savedUseArduino === 'true') {
        useArduinoData = true;
        document.getElementById('arduinoToggle').classList.add('active');
        testArduinoConnection();
    }
    
    if (savedIP) {
        document.getElementById('arduinoIP').value = savedIP;
        arduinoApiUrl = `http://${savedIP}`;
    }
});

function checkAutoLampControl() {
    if (!autoLampControlEnabled || !isWorkingHours()) return;
    
    const currentPeople = parseInt(document.getElementById("jumlahOrang").innerHTML) || 0;
    const lightIntensity = parseInt(document.getElementById("ldrValue").innerHTML) || 1000;
    
    // Threshold intensitas cahaya (lux) - di bawah 300 lux dianggap gelap
    const darkThreshold = 300;
    const isDark = lightIntensity < darkThreshold;
    
    // Hanya lakukan perubahan jika ada perubahan signifikan
    const significantChange = Math.abs(currentPeople - lastPeopleCount) >= 2 || 
                             (lightIntensity < darkThreshold && (!lampCircuits.circuit1 && !lampCircuits.circuit2));
    
    if (!significantChange) return;
    
    lastPeopleCount = currentPeople;
    
    const circuit1On = lampCircuits.circuit1;
    const circuit2On = lampCircuits.circuit2;
    
    if (currentPeople === 0 && !isDark) {
        // Tidak ada orang dan cahaya cukup - matikan semua lampu
        if (circuit1On || circuit2On) {
            if (circuit1On) toggleLampCircuit(1);
            if (circuit2On) toggleLampCircuit(2);
            showNotification("Auto Lampu: Semua lampu dimatikan - tidak ada orang & cahaya cukup", "info");
        }
    } else if (currentPeople === 0 && isDark) {
        // Tidak ada orang tapi gelap - nyalakan 1 jalur untuk keamanan
        if (!circuit1On && !circuit2On) {
            toggleLampCircuit(1);
            showNotification(`Auto Lampu: Jalur 1 dinyalakan - gelap (${lightIntensity} lux)`, "success");
        }
    } else if (currentPeople > 0 && currentPeople < 20) {
        // Kurang dari 20 orang
        if (isDark) {
            // Gelap - nyalakan 1 jalur
            if (!circuit1On && !circuit2On) {
                toggleLampCircuit(1);
                showNotification(`Auto Lampu: Jalur 1 dinyalakan - ${currentPeople} orang & gelap (${lightIntensity} lux)`, "success");
            } else if (circuit1On && circuit2On) {
                // Kedua jalur menyala, matikan jalur 2
                toggleLampCircuit(2);
                showNotification(`Auto Lampu: Jalur 2 dimatikan - ${currentPeople} orang (< 20)`, "info");
            }
        } else {
            // Cahaya cukup - bisa matikan lampu atau tetap 1 jalur
            if (circuit1On && circuit2On) {
                toggleLampCircuit(2);
                showNotification(`Auto Lampu: Jalur 2 dimatikan - cahaya cukup & ${currentPeople} orang`, "info");
            }
        }
    } else if (currentPeople >= 20) {
        // 20 orang atau lebih - nyalakan semua jalur
        if (!circuit1On || !circuit2On) {
            if (!circuit1On) toggleLampCircuit(1);
            if (!circuit2On) toggleLampCircuit(2);
            const reason = isDark ? `gelap (${lightIntensity} lux)` : "banyak orang";
            showNotification(`Auto Lampu: Semua jalur dinyalakan - ${currentPeople} orang & ${reason}`, "success");
        }
    }
}

// Jalankan pengecekan auto lamp control setiap 10 detik
setInterval(checkAutoLampControl, 10000);

// ===== Report Functions =====

// View Report Functions
function viewDailyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        // Laravel environment
        window.open('/reports/daily', '_blank');
    } else {
        showNotification('Viewing daily report...', 'info');
        // Simulate report viewing for standalone HTML
        setTimeout(() => {
            showNotification('Daily report opened!', 'success');
        }, 1000);
    }
}

function viewWeeklyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        window.open('/reports/weekly', '_blank');
    } else {
        showNotification('Viewing weekly report...', 'info');
        setTimeout(() => {
            showNotification('Weekly report opened!', 'success');
        }, 1000);
    }
}

function viewMonthlyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        window.open('/reports/monthly', '_blank');
    } else {
        showNotification('Viewing monthly report...', 'info');
        setTimeout(() => {
            showNotification('Monthly report opened!', 'success');
        }, 1000);
    }
}

function viewEfficiencyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        // Open efficiency report with default date range (last 30 days)
        const dateFrom = new Date();
        dateFrom.setDate(dateFrom.getDate() - 30);
        const dateTo = new Date();
        
        const form = document.createElement('form');
        form.method = 'POST';
        form.action = '/reports/efficiency';
        form.target = '_blank';
        
        const csrfToken = document.querySelector('meta[name="csrf-token"]');
        if (csrfToken) {
            const csrfInput = document.createElement('input');
            csrfInput.type = 'hidden';
            csrfInput.name = '_token';
            csrfInput.value = csrfToken.getAttribute('content');
            form.appendChild(csrfInput);
        }
        
        const dateFromInput = document.createElement('input');
        dateFromInput.type = 'hidden';
        dateFromInput.name = 'date_from';
        dateFromInput.value = dateFrom.toISOString().split('T')[0];
        form.appendChild(dateFromInput);
        
        const dateToInput = document.createElement('input');
        dateToInput.type = 'hidden';
        dateToInput.name = 'date_to';
        dateToInput.value = dateTo.toISOString().split('T')[0];
        form.appendChild(dateToInput);
        
        document.body.appendChild(form);
        form.submit();
        document.body.removeChild(form);
    } else {
        showNotification('Viewing efficiency report...', 'info');
        setTimeout(() => {
            showNotification('Efficiency report opened!', 'success');
        }, 1000);
    }
}

// Download Report Functions
function downloadDailyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        window.open('/reports/daily?format=download', '_blank');
    } else {
        showNotification('Downloading daily report...', 'info');
        setTimeout(() => {
            showNotification('Daily report downloaded!', 'success');
        }, 2000);
    }
}

function downloadWeeklyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        window.open('/reports/weekly?format=download', '_blank');
    } else {
        showNotification('Downloading weekly report...', 'info');
        setTimeout(() => {
            showNotification('Weekly report downloaded!', 'success');
        }, 2000);
    }
}

function downloadMonthlyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        window.open('/reports/monthly?format=download', '_blank');
    } else {
        showNotification('Downloading monthly report...', 'info');
        setTimeout(() => {
            showNotification('Monthly report downloaded!', 'success');
        }, 2000);
    }
}

function downloadEfficiencyReport() {
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        // Download efficiency report with default date range
        const dateFrom = new Date();
        dateFrom.setDate(dateFrom.getDate() - 30);
        const dateTo = new Date();
        
        const form = document.createElement('form');
        form.method = 'POST';
        form.action = '/reports/efficiency';
        
        const csrfToken = document.querySelector('meta[name="csrf-token"]');
        if (csrfToken) {
            const csrfInput = document.createElement('input');
            csrfInput.type = 'hidden';
            csrfInput.name = '_token';
            csrfInput.value = csrfToken.getAttribute('content');
            form.appendChild(csrfInput);
        }
        
        const dateFromInput = document.createElement('input');
        dateFromInput.type = 'hidden';
        dateFromInput.name = 'date_from';
        dateFromInput.value = dateFrom.toISOString().split('T')[0];
        form.appendChild(dateFromInput);
        
        const dateToInput = document.createElement('input');
        dateToInput.type = 'hidden';
        dateToInput.name = 'date_to';
        dateToInput.value = dateTo.toISOString().split('T')[0];
        form.appendChild(dateToInput);
        
        const formatInput = document.createElement('input');
        formatInput.type = 'hidden';
        formatInput.name = 'format';
        formatInput.value = 'download';
        form.appendChild(formatInput);
        
        document.body.appendChild(form);
        form.submit();
        document.body.removeChild(form);
    } else {
        showNotification('Downloading efficiency report...', 'info');
        setTimeout(() => {
            showNotification('Efficiency report downloaded!', 'success');
        }, 2000);
    }
}

// Custom Report Generator
function generateCustomReport() {
    const dateFrom = document.getElementById('reportDateFrom').value;
    const dateTo = document.getElementById('reportDateTo').value;
    const deviceType = document.getElementById('reportDeviceType').value;
    const format = document.getElementById('reportFormat').value;
    
    if (!dateFrom || !dateTo) {
        showNotification('Please select both start and end dates', 'error');
        return;
    }
    
    if (new Date(dateFrom) > new Date(dateTo)) {
        showNotification('Start date cannot be later than end date', 'error');
        return;
    }
    
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        const form = document.createElement('form');
        form.method = 'POST';
        form.action = '/reports/energy';
        
        const csrfToken = document.querySelector('meta[name="csrf-token"]');
        if (csrfToken) {
            const csrfInput = document.createElement('input');
            csrfInput.type = 'hidden';
            csrfInput.name = '_token';
            csrfInput.value = csrfToken.getAttribute('content');
            form.appendChild(csrfInput);
        }
        
        const inputs = [
            { name: 'date_from', value: dateFrom },
            { name: 'date_to', value: dateTo },
            { name: 'device_type', value: deviceType },
            { name: 'format', value: format }
        ];
        
        inputs.forEach(input => {
            const inputElement = document.createElement('input');
            inputElement.type = 'hidden';
            inputElement.name = input.name;
            inputElement.value = input.value;
            form.appendChild(inputElement);
        });
        
        document.body.appendChild(form);
        form.submit();
        document.body.removeChild(form);
    } else {
        showNotification(`Generating ${format.toUpperCase()} report...`, 'info');
        setTimeout(() => {
            showNotification(`Custom ${format.toUpperCase()} report generated!`, 'success');
        }, 3000);
    }
}

// Quick Download Functions
function downloadTodayReport() {
    downloadDailyReport();
}

function downloadWeekReport() {
    downloadWeeklyReport();
}

function downloadMonthReport() {
    downloadMonthlyReport();
}

// Initialize report form dates when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    // Set default dates for custom report form
    const today = new Date();
    const thirtyDaysAgo = new Date();
    thirtyDaysAgo.setDate(today.getDate() - 30);
    
    const dateFromInput = document.getElementById('reportDateFrom');
    const dateToInput = document.getElementById('reportDateTo');
    
    if (dateFromInput && dateToInput) {
        dateFromInput.value = thirtyDaysAgo.toISOString().split('T')[0];
        dateToInput.value = today.toISOString().split('T')[0];
    }
});