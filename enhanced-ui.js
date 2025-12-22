/* ===== ENHANCED UI COMPONENTS ===== */

/* ===== ENERGY ANALYTICS DASHBOARD ===== */
function createAnalyticsDashboard() {
    const analyticsSection = document.createElement('section');
    analyticsSection.id = 'enhanced-analytics';
    analyticsSection.style.display = 'none';
    analyticsSection.innerHTML = `
        <header>
            <h1><i class="fas fa-chart-line"></i> Analisa Lanjutan</h1>
        </header>
        
        <!-- Energy Efficiency Score -->
        <div class="efficiency-score-card">
            <div class="score-display">
                <div class="score-circle">
                    <svg viewBox="0 0 100 100">
                        <circle cx="50" cy="50" r="45" fill="none" stroke="#e2e8f0" stroke-width="8"/>
                        <circle id="efficiency-progress" cx="50" cy="50" r="45" fill="none" 
                                stroke="url(#efficiency-gradient)" stroke-width="8" 
                                stroke-dasharray="283" stroke-dashoffset="283"
                                transform="rotate(-90 50 50)"/>
                        <defs>
                            <linearGradient id="efficiency-gradient" x1="0%" y1="0%" x2="100%" y2="0%">
                                <stop offset="0%" style="stop-color:#10b981"/>
                                <stop offset="50%" style="stop-color:#3b82f6"/>
                                <stop offset="100%" style="stop-color:#8b5cf6"/>
                            </linearGradient>
                        </defs>
                    </svg>
                    <div class="score-text">
                        <span id="efficiency-score">85</span>
                        <small>Efisiensi</small>
                    </div>
                </div>
            </div>
            <div class="score-details">
                <h3>Skor Efisiensi Energi</h3>
                <p>Berdasarkan penggunaan optimal vs aktual</p>
                <div class="efficiency-breakdown">
                    <div class="breakdown-item">
                        <span>Target Harian:</span>
                        <span id="target-usage">1200 Wh</span>
                    </div>
                    <div class="breakdown-item">
                        <span>Penggunaan Aktual:</span>
                        <span id="actual-usage">1050 Wh</span>
                    </div>
                    <div class="breakdown-item">
                        <span>Penghematan:</span>
                        <span id="energy-savings" class="text-success">150 Wh (12.5%)</span>
                    </div>
                </div>
            </div>
        </div>

        <!-- Predictions Chart -->
        <div class="predictions-card">
            <h3><i class="fas fa-crystal-ball"></i> Prediksi Penggunaan 7 Hari</h3>
            <div class="predictions-chart">
                <canvas id="predictionsChart" width="800" height="400"></canvas>
            </div>
            <div class="prediction-summary">
                <div class="prediction-item">
                    <span>Prediksi Minggu Depan:</span>
                    <span id="weekly-prediction">8,400 Wh</span>
                </div>
                <div class="prediction-item">
                    <span>Confidence Level:</span>
                    <span id="prediction-confidence">78%</span>
                </div>
            </div>
        </div>

        <!-- Recommendations -->
        <div class="recommendations-card">
            <h3><i class="fas fa-lightbulb"></i> Rekomendasi Efisiensi</h3>
            <div id="recommendations-list" class="recommendations-list">
                <!-- Recommendations will be populated by JavaScript -->
            </div>
        </div>

        <!-- Cost Analysis -->
        <div class="cost-analysis-card">
            <h3><i class="fas fa-money-bill-wave"></i> Analisis Biaya</h3>
            <div class="cost-grid">
                <div class="cost-item">
                    <div class="cost-icon">
                        <i class="fas fa-calendar-day"></i>
                    </div>
                    <div class="cost-details">
                        <span class="cost-label">Biaya Hari Ini</span>
                        <span id="daily-cost" class="cost-value">Rp 1,250</span>
                    </div>
                </div>
                <div class="cost-item">
                    <div class="cost-icon">
                        <i class="fas fa-calendar-week"></i>
                    </div>
                    <div class="cost-details">
                        <span class="cost-label">Biaya Minggu Ini</span>
                        <span id="weekly-cost" class="cost-value">Rp 8,750</span>
                    </div>
                </div>
                <div class="cost-item">
                    <div class="cost-icon">
                        <i class="fas fa-calendar-alt"></i>
                    </div>
                    <div class="cost-details">
                        <span class="cost-label">Proyeksi Bulanan</span>
                        <span id="monthly-cost" class="cost-value">Rp 37,500</span>
                    </div>
                </div>
                <div class="cost-item">
                    <div class="cost-icon">
                        <i class="fas fa-piggy-bank"></i>
                    </div>
                    <div class="cost-details">
                        <span class="cost-label">Potensi Penghematan</span>
                        <span id="potential-savings" class="cost-value text-success">Rp 7,500</span>
                    </div>
                </div>
            </div>
        </div>
    `;
    
    // Insert after the existing analytics section
    const existingAnalytics = document.getElementById('analytics');
    existingAnalytics.parentNode.insertBefore(analyticsSection, existingAnalytics.nextSibling);
}

/* ===== SMART SCHEDULER UI ===== */
function createSmartSchedulerUI() {
    const schedulerSection = document.createElement('section');
    schedulerSection.id = 'smart-scheduler';
    schedulerSection.style.display = 'none';
    schedulerSection.innerHTML = `
        <header>
            <h1><i class="fas fa-clock"></i> Penjadwalan Cerdas</h1>
        </header>

        <!-- Add New Schedule -->
        <div class="scheduler-form-card">
            <h3><i class="fas fa-plus"></i> Tambah Jadwal Baru</h3>
            <form id="schedule-form" class="schedule-form">
                <div class="form-row">
                    <div class="form-group">
                        <label>Nama Jadwal:</label>
                        <input type="text" id="schedule-name" placeholder="Contoh: AC Otomatis Pagi" required>
                    </div>
                    <div class="form-group">
                        <label>Perangkat:</label>
                        <select id="schedule-device" required>
                            <option value="">Pilih Perangkat</option>
                            <option value="ac">AC (Semua Unit)</option>
                            <option value="lamp">Lampu (Semua Jalur)</option>
                            <option value="computer">Komputer</option>
                            <option value="tv">Smart TV</option>
                            <option value="dispenser">Dispenser</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label>Aksi:</label>
                        <select id="schedule-action" required>
                            <option value="">Pilih Aksi</option>
                            <option value="on">Nyalakan</option>
                            <option value="off">Matikan</option>
                            <option value="auto">Otomatis</option>
                        </select>
                    </div>
                </div>
                
                <div class="form-row">
                    <div class="form-group">
                        <label>Pemicu:</label>
                        <select id="schedule-trigger" required onchange="updateConditionsForm()">
                            <option value="">Pilih Pemicu</option>
                            <option value="time">Waktu</option>
                            <option value="presence">Kehadiran</option>
                            <option value="weather">Cuaca/Cahaya</option>
                            <option value="energy">Konsumsi Energi</option>
                        </select>
                    </div>
                </div>

                <!-- Dynamic Conditions Form -->
                <div id="conditions-form" class="conditions-form">
                    <!-- Will be populated based on trigger selection -->
                </div>

                <div class="form-actions">
                    <button type="submit" class="btn-primary">
                        <i class="fas fa-save"></i> Simpan Jadwal
                    </button>
                    <button type="reset" class="btn-secondary">
                        <i class="fas fa-times"></i> Reset
                    </button>
                </div>
            </form>
        </div>

        <!-- Active Schedules -->
        <div class="active-schedules-card">
            <h3><i class="fas fa-list"></i> Jadwal Aktif</h3>
            <div id="schedules-list" class="schedules-list">
                <!-- Schedules will be populated by JavaScript -->
            </div>
        </div>
    `;
    
    // Insert after automation section
    const automationSection = document.getElementById('automation');
    automationSection.parentNode.insertBefore(schedulerSection, automationSection.nextSibling);
}

/* ===== NOTIFICATION CENTER UI ===== */
function createNotificationCenter() {
    const notificationCenter = document.createElement('div');
    notificationCenter.id = 'notification-center';
    notificationCenter.className = 'notification-center';
    notificationCenter.innerHTML = `
        <div class="notification-header">
            <h3><i class="fas fa-bell"></i> Pusat Notifikasi</h3>
            <div class="notification-actions">
                <button onclick="toggleNotificationCenter()" class="btn-icon">
                    <i class="fas fa-times"></i>
                </button>
            </div>
        </div>
        
        <div class="notification-filters">
            <button class="filter-btn active" data-filter="all">Semua</button>
            <button class="filter-btn" data-filter="energy">Energi</button>
            <button class="filter-btn" data-filter="device">Perangkat</button>
            <button class="filter-btn" data-filter="schedule">Jadwal</button>
        </div>
        
        <div id="notifications-list" class="notifications-list">
            <!-- Notifications will be populated by JavaScript -->
        </div>
        
        <div class="notification-footer">
            <button onclick="clearAllNotifications()" class="btn-secondary">
                <i class="fas fa-trash"></i> Hapus Semua
            </button>
            <button onclick="openNotificationSettings()" class="btn-primary">
                <i class="fas fa-cog"></i> Pengaturan
            </button>
        </div>
    `;
    
    document.body.appendChild(notificationCenter);
}

/* ===== PERFORMANCE DASHBOARD ===== */
function createPerformanceDashboard() {
    const performanceSection = document.createElement('section');
    performanceSection.id = 'performance-dashboard';
    performanceSection.style.display = 'none';
    performanceSection.innerHTML = `
        <header>
            <h1><i class="fas fa-tachometer-alt"></i> Monitor Performa</h1>
        </header>

        <div class="performance-grid">
            <div class="performance-card">
                <div class="performance-icon">
                    <i class="fas fa-stopwatch"></i>
                </div>
                <div class="performance-details">
                    <h4>Waktu Muat Halaman</h4>
                    <span id="page-load-time" class="performance-value">0ms</span>
                </div>
            </div>
            
            <div class="performance-card">
                <div class="performance-icon">
                    <i class="fas fa-server"></i>
                </div>
                <div class="performance-details">
                    <h4>Rata-rata Response API</h4>
                    <span id="api-response-time" class="performance-value">0ms</span>
                </div>
            </div>
            
            <div class="performance-card">
                <div class="performance-icon">
                    <i class="fas fa-memory"></i>
                </div>
                <div class="performance-details">
                    <h4>Penggunaan Memori</h4>
                    <span id="memory-usage" class="performance-value">0 MB</span>
                </div>
            </div>
            
            <div class="performance-card">
                <div class="performance-icon">
                    <i class="fas fa-exclamation-triangle"></i>
                </div>
                <div class="performance-details">
                    <h4>Operasi Lambat</h4>
                    <span id="slow-operations" class="performance-value">0</span>
                </div>
            </div>
        </div>

        <div class="performance-chart-card">
            <h3><i class="fas fa-chart-area"></i> Grafik Performa Real-time</h3>
            <canvas id="performanceChart" width="800" height="300"></canvas>
        </div>
    `;
    
    // Insert after settings section
    const settingsSection = document.getElementById('settings');
    settingsSection.parentNode.insertBefore(performanceSection, settingsSection.nextSibling);
}

/* ===== UPDATE SIDEBAR WITH NEW SECTIONS ===== */
function addEnhancedMenuItems() {
    const sidebar = document.querySelector('.sidebar ul');
    
    const enhancedMenuItems = `
        <li><a href="#" onclick="showSection('enhanced-analytics', this)" title="Analisa Lanjutan">
            <i class="fas fa-chart-pie"></i> <span class="sidebar-text">Analisa Lanjutan</span>
        </a></li>
        <li><a href="#" onclick="showSection('smart-scheduler', this)" title="Penjadwalan Cerdas">
            <i class="fas fa-clock"></i> <span class="sidebar-text">Jadwal Cerdas</span>
        </a></li>
        <li><a href="#" onclick="showSection('performance-dashboard', this)" title="Monitor Performa">
            <i class="fas fa-tachometer-alt"></i> <span class="sidebar-text">Monitor Performa</span>
        </a></li>
    `;
    
    // Insert before settings menu item
    const settingsMenuItem = sidebar.querySelector('a[onclick*="settings"]').parentElement;
    settingsMenuItem.insertAdjacentHTML('beforebegin', enhancedMenuItems);
}

/* ===== NOTIFICATION BELL ICON ===== */
function addNotificationBell() {
    const navbar = document.querySelector('.navbar');
    const notificationBell = document.createElement('div');
    notificationBell.className = 'notification-bell';
    notificationBell.innerHTML = `
        <button onclick="toggleNotificationCenter()" class="notification-btn">
            <i class="fas fa-bell"></i>
            <span id="notification-count" class="notification-count">0</span>
        </button>
    `;
    
    navbar.style.position = 'relative';
    navbar.appendChild(notificationBell);
}

/* ===== ENHANCED FEATURES FUNCTIONS ===== */

// Update conditions form based on trigger selection
function updateConditionsForm() {
    const trigger = document.getElementById('schedule-trigger').value;
    const conditionsForm = document.getElementById('conditions-form');
    
    let formHTML = '';
    
    switch (trigger) {
        case 'time':
            formHTML = `
                <div class="form-row">
                    <div class="form-group">
                        <label>Waktu Mulai:</label>
                        <input type="time" id="start-time" required>
                    </div>
                    <div class="form-group">
                        <label>Waktu Selesai:</label>
                        <input type="time" id="end-time" required>
                    </div>
                </div>
                <div class="form-group">
                    <label>Hari:</label>
                    <div class="checkbox-group">
                        <label><input type="checkbox" value="1"> Senin</label>
                        <label><input type="checkbox" value="2"> Selasa</label>
                        <label><input type="checkbox" value="3"> Rabu</label>
                        <label><input type="checkbox" value="4"> Kamis</label>
                        <label><input type="checkbox" value="5"> Jumat</label>
                        <label><input type="checkbox" value="6"> Sabtu</label>
                        <label><input type="checkbox" value="0"> Minggu</label>
                    </div>
                </div>
            `;
            break;
            
        case 'presence':
            formHTML = `
                <div class="form-row">
                    <div class="form-group">
                        <label>Kondisi:</label>
                        <select id="presence-operator" required>
                            <option value="greater_than">Lebih dari</option>
                            <option value="less_than">Kurang dari</option>
                            <option value="equals">Sama dengan</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label>Jumlah Orang:</label>
                        <input type="number" id="presence-value" min="0" max="50" required>
                    </div>
                </div>
            `;
            break;
            
        case 'weather':
            formHTML = `
                <div class="form-group">
                    <label>Kondisi Cuaca:</label>
                    <select id="weather-type" required>
                        <option value="dark">Gelap (< 300 lux)</option>
                        <option value="cloudy">Berawan (300-600 lux)</option>
                        <option value="bright">Terang (> 600 lux)</option>
                    </select>
                </div>
            `;
            break;
            
        case 'energy':
            formHTML = `
                <div class="form-row">
                    <div class="form-group">
                        <label>Kondisi:</label>
                        <select id="energy-operator" required>
                            <option value="greater_than">Lebih dari</option>
                            <option value="less_than">Kurang dari</option>
                        </select>
                    </div>
                    <div class="form-group">
                        <label>Konsumsi Energi (Wh):</label>
                        <input type="number" id="energy-value" min="0" max="5000" required>
                    </div>
                </div>
            `;
            break;
    }
    
    conditionsForm.innerHTML = formHTML;
}

// Toggle notification center
function toggleNotificationCenter() {
    const center = document.getElementById('notification-center');
    center.classList.toggle('active');
    
    if (center.classList.contains('active')) {
        updateNotificationsList();
    }
}

// Update notifications list
function updateNotificationsList() {
    const list = document.getElementById('notifications-list');
    const notifications = notificationSystem.getNotifications(20);
    
    if (notifications.length === 0) {
        list.innerHTML = '<div class="no-notifications">Tidak ada notifikasi</div>';
        return;
    }
    
    list.innerHTML = notifications.map(notification => `
        <div class="notification-item ${notification.read ? 'read' : 'unread'}" data-id="${notification.id}">
            <div class="notification-icon ${notification.type}">
                <i class="fas fa-${getNotificationIcon(notification.type)}"></i>
            </div>
            <div class="notification-content">
                <div class="notification-message">${notification.message}</div>
                <div class="notification-time">${formatNotificationTime(notification.timestamp)}</div>
            </div>
            <button onclick="markNotificationAsRead(${notification.id})" class="notification-close">
                <i class="fas fa-times"></i>
            </button>
        </div>
    `).join('');
}

function getNotificationIcon(type) {
    const icons = {
        success: 'check-circle',
        info: 'info-circle',
        warning: 'exclamation-triangle',
        error: 'times-circle',
        energy: 'bolt',
        device: 'microchip',
        schedule: 'clock'
    };
    return icons[type] || 'bell';
}

function formatNotificationTime(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diff = now - date;
    
    if (diff < 60000) return 'Baru saja';
    if (diff < 3600000) return `${Math.floor(diff / 60000)} menit lalu`;
    if (diff < 86400000) return `${Math.floor(diff / 3600000)} jam lalu`;
    return date.toLocaleDateString('id-ID');
}

// Initialize enhanced UI
function initializeEnhancedUI() {
    createAnalyticsDashboard();
    createSmartSchedulerUI();
    createNotificationCenter();
    createPerformanceDashboard();
    addEnhancedMenuItems();
    addNotificationBell();
    
    // Setup event listeners
    setupEnhancedEventListeners();
}

function setupEnhancedEventListeners() {
    // Schedule form submission
    const scheduleForm = document.getElementById('schedule-form');
    if (scheduleForm) {
        scheduleForm.addEventListener('submit', function(e) {
            e.preventDefault();
            handleScheduleSubmission();
        });
    }
    
    // Notification filter buttons
    const filterButtons = document.querySelectorAll('.filter-btn');
    filterButtons.forEach(btn => {
        btn.addEventListener('click', function() {
            filterButtons.forEach(b => b.classList.remove('active'));
            this.classList.add('active');
            filterNotifications(this.dataset.filter);
        });
    });
}

function handleScheduleSubmission() {
    const formData = {
        name: document.getElementById('schedule-name').value,
        device: document.getElementById('schedule-device').value,
        action: document.getElementById('schedule-action').value,
        trigger: document.getElementById('schedule-trigger').value,
        conditions: getConditionsFromForm()
    };
    
    const newSchedule = smartScheduler.addSchedule(formData);
    showNotification(`Jadwal "${newSchedule.name}" berhasil ditambahkan`, 'success');
    
    // Reset form and update schedules list
    document.getElementById('schedule-form').reset();
    updateSchedulesList();
}

function getConditionsFromForm() {
    const trigger = document.getElementById('schedule-trigger').value;
    
    switch (trigger) {
        case 'time':
            const days = Array.from(document.querySelectorAll('input[type="checkbox"]:checked'))
                .map(cb => parseInt(cb.value));
            return {
                startTime: document.getElementById('start-time').value,
                endTime: document.getElementById('end-time').value,
                days: days
            };
            
        case 'presence':
            return {
                operator: document.getElementById('presence-operator').value,
                value: parseInt(document.getElementById('presence-value').value)
            };
            
        case 'weather':
            return {
                type: document.getElementById('weather-type').value
            };
            
        case 'energy':
            return {
                operator: document.getElementById('energy-operator').value,
                value: parseInt(document.getElementById('energy-value').value)
            };
            
        default:
            return {};
    }
}

function updateSchedulesList() {
    const list = document.getElementById('schedules-list');
    const schedules = smartScheduler.getSchedules();
    
    if (schedules.length === 0) {
        list.innerHTML = '<div class="no-schedules">Belum ada jadwal yang dibuat</div>';
        return;
    }
    
    list.innerHTML = schedules.map(schedule => `
        <div class="schedule-item ${schedule.enabled ? 'enabled' : 'disabled'}">
            <div class="schedule-info">
                <h4>${schedule.name}</h4>
                <p>${schedule.device} - ${schedule.action} (${schedule.trigger})</p>
            </div>
            <div class="schedule-actions">
                <button onclick="toggleScheduleStatus(${schedule.id})" class="btn-toggle">
                    <i class="fas fa-${schedule.enabled ? 'pause' : 'play'}"></i>
                </button>
                <button onclick="deleteSchedule(${schedule.id})" class="btn-delete">
                    <i class="fas fa-trash"></i>
                </button>
            </div>
        </div>
    `).join('');
}

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    setTimeout(initializeEnhancedUI, 1000); // Delay to ensure main UI is loaded
});

/* ===== EXPORT FUNCTIONS ===== */
window.updateConditionsForm = updateConditionsForm;
window.toggleNotificationCenter = toggleNotificationCenter;
window.updateNotificationsList = updateNotificationsList;
window.handleScheduleSubmission = handleScheduleSubmission;