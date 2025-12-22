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
    const mainContent = document.querySelector('.main-content');
    const toggleIcon = document.getElementById('toggleIcon');
    
    sidebar.classList.toggle('collapsed');
    
    // Update main content margin
    if (sidebar.classList.contains('collapsed')) {
        mainContent.classList.add('expanded');
        toggleIcon.className = 'fas fa-chevron-right';
    } else {
        mainContent.classList.remove('expanded');
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

// Update working hours status every minute
setInterval(checkWorkingHoursStatus, 60000);
checkWorkingHoursStatus();

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
    const workingHoursStatusEl = document.getElementById("workingHoursStatus");
    const currentTimeDisplayEl = document.getElementById("currentTimeDisplay");
    const now = new Date();
    
    // Update current time display
    if (currentTimeDisplayEl) {
        const timeStr = now.toLocaleTimeString('id-ID', { 
            hour: '2-digit', 
            minute: '2-digit',
            timeZone: 'Asia/Jakarta'
        });
        const dayName = ['Minggu', 'Senin', 'Selasa', 'Rabu', 'Kamis', 'Jumat', 'Sabtu'][now.getDay()];
        currentTimeDisplayEl.textContent = `${timeStr} (${dayName})`;
    }
    
    // Update working hours status
    if (workingHoursStatusEl) {
        if (workingHours) {
            workingHoursStatusEl.textContent = "Jam Kerja Aktif";
            workingHoursStatusEl.className = "status-on";
        } else {
            const dayStatus = getWorkingDayStatus();
            if (!isWorkingDay() && workingDaysEnabled) {
                workingHoursStatusEl.textContent = `${dayStatus}`;
            } else {
                workingHoursStatusEl.textContent = "Di Luar Jam Kerja";
            }
            workingHoursStatusEl.className = "status-off";
        }
    }
    
    // Auto-save setiap 30 menit selama jam kerja
    if (workingHours && now.getMinutes() % 30 === 0 && now.getSeconds() === 0) {
        updateCurrentUsageDisplay();
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

/* ===== NOTIFICATION SYSTEM ===== */
function showNotification(message, type = 'info') {
    // Create notification element
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.innerHTML = `
        <i class="fas fa-${type === 'success' ? 'check-circle' : type === 'warning' ? 'exclamation-triangle' : 'info-circle'}"></i>
        <span>${message}</span>
        <button onclick="removeNotification(this.parentElement)">
            <i class="fas fa-times"></i>
        </button>
    `;
    
    // Set initial state for animation
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        background: ${type === 'success' ? '#10b981' : type === 'warning' ? '#f59e0b' : '#3b82f6'};
        color: white;
        padding: 15px 20px;
        border-radius: 8px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        z-index: 10000;
        display: flex;
        align-items: center;
        gap: 10px;
        transform: translateX(100%);
        opacity: 0;
        transition: all 0.3s ease;
        max-width: 400px;
    `;
    
    // Add to body
    document.body.appendChild(notification);
    
    // Animate in
    setTimeout(() => {
        notification.style.transform = 'translateX(0)';
        notification.style.opacity = '1';
    }, 10);
    
    // Auto remove after 5 seconds
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

/* ===== HISTORY TAB MANAGEMENT ===== */
function showHistoryTab(tabName) {
    // Hide all tables
    const tables = document.querySelectorAll('.history-table-container');
    tables.forEach(table => {
        table.style.display = 'none';
    });
    
    // Show selected table
    const targetTable = document.getElementById(tabName + '-table');
    if (targetTable) {
        targetTable.style.display = 'block';
    } else {
        document.getElementById('summary-table').style.display = 'block';
    }
    
    // Update active tab
    const tabs = document.querySelectorAll('.tab-button');
    tabs.forEach(tab => {
        tab.classList.remove('active');
    });
    event.target.classList.add('active');
}

/* ===== AUTOMATION FUNCTIONS ===== */
function toggleScheduledShutdown() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function toggleWorkingDays() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function toggleAutoLampControl() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function togglePresenceDetection() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function toggleAutoShutdown() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function emergencyOverride() {
    if (confirm('Apakah Anda yakin ingin melakukan Emergency Override? Ini akan menonaktifkan semua otomasi untuk 1 jam.')) {
        alert('Emergency Override diaktifkan - Semua otomasi dinonaktifkan selama 1 jam');
    }
}

function manualShutdownAll() {
    if (confirm('Apakah Anda yakin ingin mematikan semua perangkat?')) {
        alert('Semua perangkat akan dimatikan dalam 10 detik...');
    }
}

function startACGradualMode() {
    alert('Mode Gradual AC diaktifkan - Suhu akan dinaikkan bertahap ke 28°C dalam 15 menit');
}

/* ===== REPORT FUNCTIONS ===== */
function viewDailyReport() {
    alert('Menampilkan laporan harian...');
}

function downloadDailyReport() {
    alert('Mengunduh laporan harian PDF...');
}

function viewWeeklyReport() {
    alert('Menampilkan laporan mingguan...');
}

function downloadWeeklyReport() {
    alert('Mengunduh laporan mingguan PDF...');
}

function viewMonthlyReport() {
    alert('Menampilkan laporan bulanan...');
}

function downloadMonthlyReport() {
    alert('Mengunduh laporan bulanan PDF...');
}

function viewEfficiencyReport() {
    alert('Menampilkan laporan efisiensi...');
}

function downloadEfficiencyReport() {
    alert('Mengunduh laporan efisiensi PDF...');
}

function generateCustomReport() {
    alert('Generating custom report...');
}

function downloadTodayReport() {
    alert('Downloading today report...');
}

function downloadWeekReport() {
    alert('Downloading week report...');
}

function downloadMonthReport() {
    alert('Downloading month report...');
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

/* ===== OTHER SETTINGS FUNCTIONS ===== */

function toggleArduinoConnection() {
    const toggle = document.getElementById('arduinoToggle');
    toggle.classList.toggle('active');
    
    const status = document.getElementById('connectionStatus');
    if (toggle.classList.contains('active')) {
        status.textContent = 'Terhubung';
        status.className = 'connection-status connected';
    } else {
        status.textContent = 'Terputus';
        status.className = 'connection-status disconnected';
    }
}

function updateArduinoIP() {
    const ip = document.getElementById('arduinoIP').value;
    alert('IP Arduino diperbarui ke: ' + ip);
}

/* ===== ARDUINO CONNECTION ===== */
let useArduinoData = false;

async function sendArduinoCommand(device, action) {
    // Placeholder for Arduino communication
    return true;
}

/* ===== INITIALIZE ===== */
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
    
    // Load saved theme preference
    loadThemePreference();
    
    // Update displays
    setTimeout(() => {
        updateOtherDevicesStatus();
        updateCurrentUsageDisplay();
    }, 500);
});

function getWorkingDayStatus() {
    const now = new Date();
    const dayNames = ['Minggu', 'Senin', 'Selasa', 'Rabu', 'Kamis', 'Jumat', 'Sabtu'];
    return dayNames[now.getDay()];
}