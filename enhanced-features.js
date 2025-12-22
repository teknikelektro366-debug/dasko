/* ===== ENHANCED FEATURES FOR DASKO SMART ENERGY SYSTEM ===== */

/* ===== ADVANCED ANALYTICS ===== */
class EnergyAnalytics {
    constructor() {
        this.data = [];
        this.predictions = [];
        this.efficiency = {
            current: 0,
            target: 85,
            trend: 'stable'
        };
    }

    // Calculate energy efficiency score
    calculateEfficiency(usage, optimal) {
        return Math.min(100, Math.max(0, (optimal / usage) * 100));
    }

    // Predict energy usage based on historical data
    predictUsage(days = 7) {
        if (historyData.length < 3) return [];
        
        const recent = historyData.slice(0, days);
        const avgUsage = recent.reduce((sum, day) => sum + day.totalEnergi, 0) / recent.length;
        
        // Simple linear prediction with seasonal adjustment
        const predictions = [];
        for (let i = 1; i <= 7; i++) {
            const seasonalFactor = this.getSeasonalFactor(i);
            const predicted = Math.round(avgUsage * seasonalFactor);
            predictions.push({
                date: this.getFutureDate(i),
                predicted: predicted,
                confidence: Math.max(60, 90 - (i * 5)) // Confidence decreases over time
            });
        }
        
        return predictions;
    }

    getSeasonalFactor(dayOffset) {
        const today = new Date();
        const futureDate = new Date(today.getTime() + (dayOffset * 24 * 60 * 60 * 1000));
        const dayOfWeek = futureDate.getDay();
        
        // Weekend factor (Saturday = 6, Sunday = 0)
        if (dayOfWeek === 0 || dayOfWeek === 6) {
            return 0.3; // 30% of normal usage on weekends
        }
        
        // Weekday variations
        const weekdayFactors = {
            1: 0.9, // Monday - slower start
            2: 1.0, // Tuesday - normal
            3: 1.1, // Wednesday - peak
            4: 1.0, // Thursday - normal
            5: 0.8  // Friday - early finish
        };
        
        return weekdayFactors[dayOfWeek] || 1.0;
    }

    getFutureDate(daysFromNow) {
        const date = new Date();
        date.setDate(date.getDate() + daysFromNow);
        return date.toISOString().split('T')[0];
    }

    // Generate efficiency recommendations
    getRecommendations() {
        const recommendations = [];
        const stats = getDeviceUsageStats();
        
        // AC efficiency recommendations
        if (stats.ac && stats.ac.energy > 0) {
            const acEfficiency = this.calculateEfficiency(stats.ac.energy, 800); // 800Wh optimal
            if (acEfficiency < 70) {
                recommendations.push({
                    type: 'warning',
                    device: 'AC',
                    message: 'AC menggunakan energi berlebihan. Pertimbangkan menaikkan suhu setting 1-2°C.',
                    potential_saving: '15-25%'
                });
            }
        }

        // Lighting efficiency recommendations
        if (stats.lamp && stats.lamp.energy > 0) {
            const lightEfficiency = this.calculateEfficiency(stats.lamp.energy, 200); // 200Wh optimal
            if (lightEfficiency < 80) {
                recommendations.push({
                    type: 'info',
                    device: 'Lampu',
                    message: 'Lampu menyala terlalu lama. Aktifkan kontrol otomatis berdasarkan cahaya.',
                    potential_saving: '10-20%'
                });
            }
        }

        // General recommendations
        if (stats.totalEnergy > 1500) {
            recommendations.push({
                type: 'warning',
                device: 'Sistem',
                message: 'Konsumsi energi harian melebihi target. Tinjau penggunaan perangkat.',
                potential_saving: '20-30%'
            });
        }

        return recommendations;
    }
}

/* ===== SMART SCHEDULING SYSTEM ===== */
class SmartScheduler {
    constructor() {
        this.schedules = JSON.parse(localStorage.getItem('smartSchedules')) || [];
        this.activeSchedules = new Map();
    }

    // Add new schedule
    addSchedule(schedule) {
        const newSchedule = {
            id: Date.now(),
            name: schedule.name,
            device: schedule.device,
            action: schedule.action, // 'on', 'off', 'auto'
            trigger: schedule.trigger, // 'time', 'presence', 'weather', 'energy'
            conditions: schedule.conditions,
            enabled: true,
            created: new Date().toISOString()
        };
        
        this.schedules.push(newSchedule);
        this.saveSchedules();
        return newSchedule;
    }

    // Check and execute schedules
    checkSchedules() {
        const now = new Date();
        
        this.schedules.forEach(schedule => {
            if (!schedule.enabled) return;
            
            const shouldExecute = this.evaluateConditions(schedule, now);
            
            if (shouldExecute && !this.activeSchedules.has(schedule.id)) {
                this.executeSchedule(schedule);
                this.activeSchedules.set(schedule.id, now);
            } else if (!shouldExecute && this.activeSchedules.has(schedule.id)) {
                this.activeSchedules.delete(schedule.id);
            }
        });
    }

    evaluateConditions(schedule, now) {
        const conditions = schedule.conditions;
        
        switch (schedule.trigger) {
            case 'time':
                return this.checkTimeCondition(conditions, now);
            case 'presence':
                return this.checkPresenceCondition(conditions);
            case 'weather':
                return this.checkWeatherCondition(conditions);
            case 'energy':
                return this.checkEnergyCondition(conditions);
            default:
                return false;
        }
    }

    checkTimeCondition(conditions, now) {
        const currentTime = now.getHours() * 60 + now.getMinutes();
        const startTime = this.parseTime(conditions.startTime);
        const endTime = this.parseTime(conditions.endTime);
        
        if (conditions.days && conditions.days.length > 0) {
            const currentDay = now.getDay();
            if (!conditions.days.includes(currentDay)) return false;
        }
        
        return currentTime >= startTime && currentTime <= endTime;
    }

    checkPresenceCondition(conditions) {
        const currentPeople = parseInt(document.getElementById("jumlahOrang").innerHTML) || 0;
        
        switch (conditions.operator) {
            case 'greater_than':
                return currentPeople > conditions.value;
            case 'less_than':
                return currentPeople < conditions.value;
            case 'equals':
                return currentPeople === conditions.value;
            default:
                return false;
        }
    }

    checkWeatherCondition(conditions) {
        const lightIntensity = parseInt(document.getElementById("ldrValue").innerHTML) || 1000;
        
        switch (conditions.type) {
            case 'dark':
                return lightIntensity < 300;
            case 'bright':
                return lightIntensity > 600;
            case 'cloudy':
                return lightIntensity >= 300 && lightIntensity <= 600;
            default:
                return false;
        }
    }

    checkEnergyCondition(conditions) {
        const stats = getDeviceUsageStats();
        const currentUsage = stats.totalEnergy;
        
        switch (conditions.operator) {
            case 'greater_than':
                return currentUsage > conditions.value;
            case 'less_than':
                return currentUsage < conditions.value;
            default:
                return false;
        }
    }

    executeSchedule(schedule) {
        const deviceId = schedule.device;
        const action = schedule.action;
        
        try {
            switch (action) {
                case 'on':
                    if (deviceId === 'ac') {
                        if (!acUnits.unit1) toggleACUnit(1);
                        if (!acUnits.unit2) toggleACUnit(2);
                    } else if (deviceId === 'lamp') {
                        if (!lampCircuits.circuit1) toggleLampCircuit(1);
                        if (!lampCircuits.circuit2) toggleLampCircuit(2);
                    } else {
                        const statusElement = document.getElementById(deviceId + "Status");
                        if (statusElement && statusElement.innerHTML === "MATI") {
                            toggleDevice(deviceId);
                        }
                    }
                    break;
                    
                case 'off':
                    if (deviceId === 'ac') {
                        if (acUnits.unit1) toggleACUnit(1);
                        if (acUnits.unit2) toggleACUnit(2);
                    } else if (deviceId === 'lamp') {
                        if (lampCircuits.circuit1) toggleLampCircuit(1);
                        if (lampCircuits.circuit2) toggleLampCircuit(2);
                    } else {
                        const statusElement = document.getElementById(deviceId + "Status");
                        if (statusElement && statusElement.innerHTML === "MENYALA") {
                            toggleDevice(deviceId);
                        }
                    }
                    break;
                    
                case 'auto':
                    // Implement auto logic based on device type
                    this.executeAutoAction(deviceId);
                    break;
            }
            
            showNotification(`Jadwal "${schedule.name}" dijalankan: ${deviceId} ${action}`, 'success');
        } catch (error) {
            console.error('Error executing schedule:', error);
            showNotification(`Gagal menjalankan jadwal "${schedule.name}"`, 'error');
        }
    }

    executeAutoAction(deviceId) {
        // Implement intelligent auto actions
        const currentPeople = parseInt(document.getElementById("jumlahOrang").innerHTML) || 0;
        const lightIntensity = parseInt(document.getElementById("ldrValue").innerHTML) || 1000;
        
        if (deviceId === 'lamp') {
            if (currentPeople > 0 && lightIntensity < 400) {
                if (!lampCircuits.circuit1) toggleLampCircuit(1);
                if (currentPeople > 15 && !lampCircuits.circuit2) toggleLampCircuit(2);
            } else if (currentPeople === 0 || lightIntensity > 600) {
                if (lampCircuits.circuit1) toggleLampCircuit(1);
                if (lampCircuits.circuit2) toggleLampCircuit(2);
            }
        } else if (deviceId === 'ac') {
            if (currentPeople > 5) {
                if (!acUnits.unit1) toggleACUnit(1);
                if (currentPeople > 15 && !acUnits.unit2) toggleACUnit(2);
            } else if (currentPeople === 0) {
                if (acUnits.unit1) toggleACUnit(1);
                if (acUnits.unit2) toggleACUnit(2);
            }
        }
    }

    parseTime(timeString) {
        const [hours, minutes] = timeString.split(':').map(Number);
        return hours * 60 + minutes;
    }

    saveSchedules() {
        localStorage.setItem('smartSchedules', JSON.stringify(this.schedules));
    }

    getSchedules() {
        return this.schedules;
    }

    deleteSchedule(id) {
        this.schedules = this.schedules.filter(s => s.id !== id);
        this.activeSchedules.delete(id);
        this.saveSchedules();
    }

    toggleSchedule(id) {
        const schedule = this.schedules.find(s => s.id === id);
        if (schedule) {
            schedule.enabled = !schedule.enabled;
            this.saveSchedules();
        }
    }
}

/* ===== ENERGY COST CALCULATOR ===== */
class EnergyCostCalculator {
    constructor() {
        this.tariff = {
            peak: 1467, // Rp per kWh (peak hours)
            offPeak: 1114, // Rp per kWh (off-peak hours)
            weekend: 1000 // Rp per kWh (weekend)
        };
        this.peakHours = { start: 17, end: 22 }; // 17:00 - 22:00
    }

    calculateDailyCost(energyWh, date = new Date()) {
        const energyKwh = energyWh / 1000;
        const dayOfWeek = date.getDay();
        const hour = date.getHours();
        
        let rate;
        if (dayOfWeek === 0 || dayOfWeek === 6) {
            rate = this.tariff.weekend;
        } else if (hour >= this.peakHours.start && hour <= this.peakHours.end) {
            rate = this.tariff.peak;
        } else {
            rate = this.tariff.offPeak;
        }
        
        return Math.round(energyKwh * rate);
    }

    calculateMonthlyCost() {
        const monthlyData = historyData.slice(0, 30);
        let totalCost = 0;
        
        monthlyData.forEach(day => {
            const date = new Date(day.date);
            const cost = this.calculateDailyCost(day.totalEnergi, date);
            totalCost += cost;
        });
        
        return totalCost;
    }

    calculateSavings(currentUsage, optimizedUsage) {
        const currentCost = this.calculateDailyCost(currentUsage);
        const optimizedCost = this.calculateDailyCost(optimizedUsage);
        
        return {
            energySaved: currentUsage - optimizedUsage,
            costSaved: currentCost - optimizedCost,
            percentageSaved: ((currentUsage - optimizedUsage) / currentUsage * 100).toFixed(1)
        };
    }

    getOptimizedUsage(currentUsage) {
        // Simple optimization: reduce by 15-25% based on efficiency recommendations
        const analytics = new EnergyAnalytics();
        const recommendations = analytics.getRecommendations();
        
        let optimizationFactor = 0.85; // Default 15% reduction
        
        if (recommendations.length > 0) {
            const avgSaving = recommendations.reduce((sum, rec) => {
                const saving = parseFloat(rec.potential_saving.split('-')[0]) / 100;
                return sum + saving;
            }, 0) / recommendations.length;
            
            optimizationFactor = 1 - avgSaving;
        }
        
        return Math.round(currentUsage * optimizationFactor);
    }
}

/* ===== REAL-TIME NOTIFICATIONS SYSTEM ===== */
class NotificationSystem {
    constructor() {
        this.notifications = [];
        this.settings = JSON.parse(localStorage.getItem('notificationSettings')) || {
            energyAlerts: true,
            deviceAlerts: true,
            scheduleAlerts: true,
            efficiencyAlerts: true,
            soundEnabled: false
        };
    }

    addNotification(notification) {
        const newNotification = {
            id: Date.now(),
            ...notification,
            timestamp: new Date().toISOString(),
            read: false
        };
        
        this.notifications.unshift(newNotification);
        
        // Keep only last 50 notifications
        if (this.notifications.length > 50) {
            this.notifications = this.notifications.slice(0, 50);
        }
        
        this.saveNotifications();
        this.displayNotification(newNotification);
        
        return newNotification;
    }

    displayNotification(notification) {
        if (!this.shouldShowNotification(notification.type)) return;
        
        showNotification(notification.message, notification.type);
        
        if (this.settings.soundEnabled) {
            this.playNotificationSound(notification.type);
        }
    }

    shouldShowNotification(type) {
        switch (type) {
            case 'energy':
                return this.settings.energyAlerts;
            case 'device':
                return this.settings.deviceAlerts;
            case 'schedule':
                return this.settings.scheduleAlerts;
            case 'efficiency':
                return this.settings.efficiencyAlerts;
            default:
                return true;
        }
    }

    playNotificationSound(type) {
        // Create audio context for notification sounds
        try {
            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            const oscillator = audioContext.createOscillator();
            const gainNode = audioContext.createGain();
            
            oscillator.connect(gainNode);
            gainNode.connect(audioContext.destination);
            
            // Different frequencies for different notification types
            const frequencies = {
                success: 800,
                info: 600,
                warning: 400,
                error: 300
            };
            
            oscillator.frequency.setValueAtTime(frequencies[type] || 600, audioContext.currentTime);
            oscillator.type = 'sine';
            
            gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
            gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.3);
            
            oscillator.start(audioContext.currentTime);
            oscillator.stop(audioContext.currentTime + 0.3);
        } catch (error) {
            console.log('Audio notification not supported');
        }
    }

    saveNotifications() {
        localStorage.setItem('notifications', JSON.stringify(this.notifications));
    }

    getNotifications(limit = 10) {
        return this.notifications.slice(0, limit);
    }

    markAsRead(id) {
        const notification = this.notifications.find(n => n.id === id);
        if (notification) {
            notification.read = true;
            this.saveNotifications();
        }
    }

    clearAll() {
        this.notifications = [];
        this.saveNotifications();
    }

    updateSettings(newSettings) {
        this.settings = { ...this.settings, ...newSettings };
        localStorage.setItem('notificationSettings', JSON.stringify(this.settings));
    }
}

/* ===== PERFORMANCE MONITOR ===== */
class PerformanceMonitor {
    constructor() {
        this.metrics = {
            pageLoadTime: 0,
            apiResponseTimes: [],
            memoryUsage: 0,
            renderTimes: []
        };
        this.startTime = performance.now();
    }

    recordPageLoad() {
        this.metrics.pageLoadTime = performance.now() - this.startTime;
        console.log(`Page loaded in ${this.metrics.pageLoadTime.toFixed(2)}ms`);
    }

    recordApiResponse(duration) {
        this.metrics.apiResponseTimes.push(duration);
        
        // Keep only last 20 measurements
        if (this.metrics.apiResponseTimes.length > 20) {
            this.metrics.apiResponseTimes.shift();
        }
    }

    recordRenderTime(operation, duration) {
        this.metrics.renderTimes.push({ operation, duration, timestamp: Date.now() });
        
        // Keep only last 50 measurements
        if (this.metrics.renderTimes.length > 50) {
            this.metrics.renderTimes.shift();
        }
    }

    getAverageApiResponseTime() {
        if (this.metrics.apiResponseTimes.length === 0) return 0;
        
        const sum = this.metrics.apiResponseTimes.reduce((a, b) => a + b, 0);
        return sum / this.metrics.apiResponseTimes.length;
    }

    getPerformanceReport() {
        return {
            pageLoadTime: this.metrics.pageLoadTime,
            avgApiResponseTime: this.getAverageApiResponseTime(),
            memoryUsage: this.getMemoryUsage(),
            slowOperations: this.getSlowOperations()
        };
    }

    getMemoryUsage() {
        if (performance.memory) {
            return {
                used: Math.round(performance.memory.usedJSHeapSize / 1024 / 1024),
                total: Math.round(performance.memory.totalJSHeapSize / 1024 / 1024),
                limit: Math.round(performance.memory.jsHeapSizeLimit / 1024 / 1024)
            };
        }
        return null;
    }

    getSlowOperations() {
        return this.metrics.renderTimes
            .filter(item => item.duration > 100) // Operations slower than 100ms
            .sort((a, b) => b.duration - a.duration)
            .slice(0, 5);
    }
}

/* ===== INITIALIZE ENHANCED FEATURES ===== */
const energyAnalytics = new EnergyAnalytics();
const smartScheduler = new SmartScheduler();
const costCalculator = new EnergyCostCalculator();
const notificationSystem = new NotificationSystem();
const performanceMonitor = new PerformanceMonitor();

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    performanceMonitor.recordPageLoad();
    
    // Start smart scheduler
    setInterval(() => {
        smartScheduler.checkSchedules();
    }, 60000); // Check every minute
    
    // Monitor performance
    const observer = new PerformanceObserver((list) => {
        for (const entry of list.getEntries()) {
            if (entry.entryType === 'measure') {
                performanceMonitor.recordRenderTime(entry.name, entry.duration);
            }
        }
    });
    
    try {
        observer.observe({ entryTypes: ['measure'] });
    } catch (error) {
        console.log('Performance Observer not supported');
    }
});

/* ===== EXPORT FUNCTIONS FOR GLOBAL ACCESS ===== */
window.energyAnalytics = energyAnalytics;
window.smartScheduler = smartScheduler;
window.costCalculator = costCalculator;
window.notificationSystem = notificationSystem;
window.performanceMonitor = performanceMonitor;