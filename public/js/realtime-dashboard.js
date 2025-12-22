/**
 * Smart Energy Dashboard - Realtime Module
 * Handles real-time data fetching and dashboard updates
 */

class RealtimeDashboard {
    constructor(options = {}) {
        this.config = {
            apiEndpoint: '/api/sensor/latest',
            updateInterval: 3000, // 3 seconds
            maxRetries: 5,
            retryDelay: 2000,
            debug: false,
            ...options
        };
        
        this.state = {
            isRunning: false,
            consecutiveErrors: 0,
            lastUpdateTime: null,
            updateTimes: [],
            totalUpdates: 0
        };
        
        this.elements = {};
        this.charts = {};
        
        this.init();
    }
    
    init() {
        this.log('Initializing Realtime Dashboard');
        this.cacheElements();
        this.setupEventListeners();
        this.start();
    }
    
    cacheElements() {
        // Cache frequently used DOM elements
        this.elements = {
            // Connection status
            connectionStatus: document.getElementById('connectionStatus'),
            lastUpdate: document.getElementById('lastUpdate'),
            
            // Sensor values
            suhuValue: document.getElementById('suhuValue'),
            humidityValue: document.getElementById('humidityValue'),
            jumlahOrang: document.getElementById('jumlahOrang'),
            ldrValue: document.getElementById('ldrValue'),
            
            // AC status
            acStatusValue: document.getElementById('acStatusValue'),
            acTempValue: document.getElementById('acTempValue'),
            
            // Proximity
            proximityIn: document.getElementById('proximityIn'),
            proximityOut: document.getElementById('proximityOut'),
            
            // Device info
            deviceInfo: document.getElementById('deviceInfo'),
            deviceLocation: document.getElementById('deviceLocation'),
            wifiSignal: document.getElementById('wifiSignal'),
            signalQuality: document.getElementById('signalQuality'),
            
            // Statistics
            totalRecords: document.getElementById('totalRecords'),
            updateFreq: document.getElementById('updateFreq'),
            
            // Timestamps
            tempTimestamp: document.getElementById('tempTimestamp'),
            humidityTimestamp: document.getElementById('humidityTimestamp'),
            peopleTimestamp: document.getElementById('peopleTimestamp'),
            lightTimestamp: document.getElementById('lightTimestamp'),
            acTimestamp: document.getElementById('acTimestamp'),
            acTempTimestamp: document.getElementById('acTempTimestamp'),
            proximityTimestamp: document.getElementById('proximityTimestamp'),
            
            // Cards for animation
            tempCard: document.getElementById('tempCard'),
            humidityCard: document.getElementById('humidityCard'),
            peopleCard: document.getElementById('peopleCard'),
            lightCard: document.getElementById('lightCard'),
            acCard: document.getElementById('acCard'),
            acTempCard: document.getElementById('acTempCard'),
            proximityCard: document.getElementById('proximityCard')
        };
    }
    
    setupEventListeners() {
        // Keyboard shortcuts
        document.addEventListener('keydown', (e) => {
            if (e.ctrlKey) {
                switch(e.key) {
                    case 'r':
                        e.preventDefault();
                        this.toggle();
                        break;
                    case '1':
                        e.preventDefault();
                        this.setUpdateInterval(1000);
                        break;
                    case '3':
                        e.preventDefault();
                        this.setUpdateInterval(3000);
                        break;
                    case '5':
                        e.preventDefault();
                        this.setUpdateInterval(5000);
                        break;
                    case '0':
                        e.preventDefault();
                        this.setUpdateInterval(10000);
                        break;
                }
            }
        });
        
        // Visibility change handling
        document.addEventListener('visibilitychange', () => {
            if (document.hidden) {
                this.log('Page hidden, slowing down updates');
                this.setUpdateInterval(this.config.updateInterval * 2);
            } else {
                this.log('Page visible, resuming normal updates');
                this.setUpdateInterval(this.config.updateInterval);
                this.fetchData(); // Immediate update when page becomes visible
            }
        });
        
        // Window focus/blur
        window.addEventListener('focus', () => {
            this.log('Window focused, immediate update');
            this.fetchData();
        });
    }
    
    async start() {
        if (this.state.isRunning) return;
        
        this.state.isRunning = true;
        this.log('Starting realtime updates');
        this.updateConnectionStatus('connecting', 'Connecting to ESP32...');
        
        await this.fetchData();
        this.scheduleNextUpdate();
    }
    
    stop() {
        this.state.isRunning = false;
        this.log('Stopping realtime updates');
        this.updateConnectionStatus('offline', 'Realtime updates stopped');
        
        if (this.updateTimeout) {
            clearTimeout(this.updateTimeout);
        }
    }
    
    toggle() {
        if (this.state.isRunning) {
            this.stop();
        } else {
            this.start();
        }
    }
    
    async fetchData() {
        if (!this.state.isRunning) return;
        
        try {
            this.log('Fetching sensor data...');
            
            const response = await fetch(this.config.apiEndpoint, {
                method: 'GET',
                headers: {
                    'Accept': 'application/json',
                    'X-Requested-With': 'XMLHttpRequest',
                    'Cache-Control': 'no-cache'
                }
            });
            
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }
            
            const data = await response.json();
            
            if (data.success && data.data) {
                this.updateDashboard(data.data);
                this.state.consecutiveErrors = 0;
                this.state.totalUpdates++;
            } else {
                throw new Error('Invalid data format received');
            }
            
        } catch (error) {
            this.handleError(error);
        }
    }
    
    updateDashboard(sensorData) {
        this.log('Updating dashboard with new data', sensorData);
        
        // Update connection status
        this.updateConnectionStatus('online', 'Receiving data from ESP32');
        
        // Update timestamp tracking
        this.state.lastUpdateTime = new Date();
        this.trackUpdateFrequency();
        
        const timeString = this.state.lastUpdateTime.toLocaleTimeString('id-ID');
        
        // Update sensor values with animations
        this.updateSensorCard('tempCard', 'suhuValue', sensorData.room_temperature, '°C', 'tempTimestamp', timeString);
        this.updateSensorCard('humidityCard', 'humidityValue', sensorData.humidity, '%', 'humidityTimestamp', timeString);
        this.updateSensorCard('peopleCard', 'jumlahOrang', sensorData.people_count, '', 'peopleTimestamp', timeString);
        this.updateSensorCard('lightCard', 'ldrValue', sensorData.light_level, ' lux', 'lightTimestamp', timeString);
        
        // Update AC status
        this.updateSensorCard('acCard', 'acStatusValue', sensorData.ac_status, '', 'acTimestamp', timeString);
        this.updateSensorCard('acTempCard', 'acTempValue', sensorData.set_temperature || '--', sensorData.set_temperature ? '°C' : '', 'acTempTimestamp', timeString);
        
        // Update proximity sensors
        this.updateElement('proximityIn', 'IN: ' + (sensorData.proximity_in ? 'AKTIF' : 'OFF'));
        this.updateElement('proximityOut', 'OUT: ' + (sensorData.proximity_out ? 'AKTIF' : 'OFF'));
        this.updateElement('proximityTimestamp', timeString);
        
        // Update device info
        this.updateElement('deviceInfo', sensorData.device_id || 'ESP32_Smart_Energy');
        this.updateElement('deviceLocation', sensorData.location || 'Lab Teknik Tegangan Tinggi');
        
        // Update WiFi signal
        if (sensorData.wifi_rssi) {
            this.updateElement('wifiSignal', sensorData.wifi_rssi + ' dBm');
            this.updateElement('signalQuality', this.getSignalQuality(sensorData.wifi_rssi));
        }
        
        // Update last update display
        const dateString = this.state.lastUpdateTime.toLocaleDateString('id-ID');
        this.updateElement('lastUpdate', `Terakhir update: ${timeString} - ${dateString}`);
        
        // Update statistics
        this.updateStatistics();
        
        // Trigger custom event
        this.dispatchUpdateEvent(sensorData);
    }
    
    updateSensorCard(cardId, valueId, value, unit, timestampId, timestamp) {
        const card = this.elements[cardId];
        const valueElement = this.elements[valueId];
        const timestampElement = this.elements[timestampId];
        
        if (card && valueElement) {
            // Add updating animation
            card.classList.add('updating');
            setTimeout(() => card.classList.remove('updating'), 1000);
            
            // Update value with smooth transition
            if (value !== null && value !== undefined) {
                const newValue = value + unit;
                if (valueElement.textContent !== newValue) {
                    valueElement.style.opacity = '0.5';
                    setTimeout(() => {
                        valueElement.textContent = newValue;
                        valueElement.style.opacity = '1';
                    }, 200);
                }
            } else {
                valueElement.textContent = '--' + unit;
            }
            
            // Update timestamp
            if (timestampElement) {
                timestampElement.textContent = timestamp;
            }
        }
    }
    
    updateElement(elementId, value) {
        const element = this.elements[elementId];
        if (element && element.textContent !== value) {
            element.textContent = value;
        }
    }
    
    updateConnectionStatus(status, message) {
        const statusElement = this.elements.connectionStatus;
        if (statusElement) {
            statusElement.className = `connection-status ${status}`;
            statusElement.textContent = status === 'online' ? 'Online' : 
                                     status === 'connecting' ? 'Connecting...' : 'Offline';
            
            if (message) {
                statusElement.title = message;
            }
        }
    }
    
    trackUpdateFrequency() {
        const now = Date.now();
        this.state.updateTimes.push(now);
        
        // Keep only last 10 updates
        if (this.state.updateTimes.length > 10) {
            this.state.updateTimes.shift();
        }
    }
    
    updateStatistics() {
        // Update frequency calculation
        if (this.state.updateTimes.length >= 2) {
            const intervals = [];
            for (let i = 1; i < this.state.updateTimes.length; i++) {
                intervals.push((this.state.updateTimes[i] - this.state.updateTimes[i-1]) / 1000);
            }
            const avgInterval = intervals.reduce((a, b) => a + b, 0) / intervals.length;
            this.updateElement('updateFreq', avgInterval.toFixed(1) + ' detik');
        }
        
        // Fetch total records periodically
        if (this.state.totalUpdates % 10 === 0) {
            this.fetchTotalRecords();
        }
    }
    
    async fetchTotalRecords() {
        try {
            const response = await fetch('/api/sensor/history?per_page=1');
            const data = await response.json();
            
            if (data.success && data.pagination) {
                this.updateElement('totalRecords', data.pagination.total.toLocaleString('id-ID'));
            }
        } catch (error) {
            this.log('Error fetching total records:', error);
        }
    }
    
    getSignalQuality(rssi) {
        if (rssi >= -30) return 'Excellent';
        if (rssi >= -50) return 'Good';
        if (rssi >= -60) return 'Fair';
        if (rssi >= -70) return 'Weak';
        return 'Very Weak';
    }
    
    handleError(error) {
        this.log('Error fetching data:', error);
        this.state.consecutiveErrors++;
        
        if (this.state.consecutiveErrors >= this.config.maxRetries) {
            this.updateConnectionStatus('offline', `Connection failed after ${this.config.maxRetries} attempts`);
            // Increase interval when having connection issues
            this.setUpdateInterval(this.config.updateInterval * 2);
        } else {
            this.updateConnectionStatus('connecting', `Retrying... (${this.state.consecutiveErrors}/${this.config.maxRetries})`);
        }
    }
    
    scheduleNextUpdate() {
        if (!this.state.isRunning) return;
        
        const delay = this.state.consecutiveErrors > 0 ? 
                     this.config.retryDelay : 
                     this.config.updateInterval;
        
        this.updateTimeout = setTimeout(() => {
            this.fetchData().then(() => {
                this.scheduleNextUpdate();
            });
        }, delay);
    }
    
    setUpdateInterval(interval) {
        this.config.updateInterval = interval;
        this.log(`Update interval changed to ${interval}ms`);
        
        // Restart with new interval
        if (this.state.isRunning) {
            if (this.updateTimeout) {
                clearTimeout(this.updateTimeout);
            }
            this.scheduleNextUpdate();
        }
    }
    
    dispatchUpdateEvent(data) {
        const event = new CustomEvent('sensorDataUpdate', {
            detail: {
                data: data,
                timestamp: this.state.lastUpdateTime,
                updateCount: this.state.totalUpdates
            }
        });
        document.dispatchEvent(event);
    }
    
    log(message, data = null) {
        if (this.config.debug) {
            console.log(`[RealtimeDashboard] ${message}`, data || '');
        }
    }
    
    // Public API methods
    getStatus() {
        return {
            isRunning: this.state.isRunning,
            lastUpdate: this.state.lastUpdateTime,
            totalUpdates: this.state.totalUpdates,
            consecutiveErrors: this.state.consecutiveErrors,
            updateInterval: this.config.updateInterval
        };
    }
    
    getConfig() {
        return { ...this.config };
    }
    
    updateConfig(newConfig) {
        this.config = { ...this.config, ...newConfig };
        this.log('Configuration updated', this.config);
    }
}

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', function() {
    // Create global instance
    window.realtimeDashboard = new RealtimeDashboard({
        debug: true, // Enable debug logging
        updateInterval: 3000 // 3 seconds
    });
    
    // Add manual refresh button
    const navbar = document.querySelector('.navbar');
    if (navbar) {
        const controlsDiv = document.createElement('div');
        controlsDiv.style.cssText = 'margin-left: auto; display: flex; gap: 10px; align-items: center;';
        
        // Refresh button
        const refreshBtn = document.createElement('button');
        refreshBtn.innerHTML = '<i class="fas fa-sync-alt"></i>';
        refreshBtn.title = 'Manual Refresh (Ctrl+R)';
        refreshBtn.onclick = () => window.realtimeDashboard.fetchData();
        refreshBtn.style.cssText = 'padding: 5px 8px; background: #007bff; color: white; border: none; border-radius: 4px; cursor: pointer;';
        
        // Toggle button
        const toggleBtn = document.createElement('button');
        toggleBtn.innerHTML = '<i class="fas fa-pause"></i>';
        toggleBtn.title = 'Toggle Realtime Updates';
        toggleBtn.onclick = () => {
            window.realtimeDashboard.toggle();
            toggleBtn.innerHTML = window.realtimeDashboard.state.isRunning ? 
                '<i class="fas fa-pause"></i>' : '<i class="fas fa-play"></i>';
        };
        toggleBtn.style.cssText = 'padding: 5px 8px; background: #28a745; color: white; border: none; border-radius: 4px; cursor: pointer;';
        
        // Interval selector
        const intervalSelect = document.createElement('select');
        intervalSelect.innerHTML = `
            <option value="1000">1s</option>
            <option value="3000" selected>3s</option>
            <option value="5000">5s</option>
            <option value="10000">10s</option>
        `;
        intervalSelect.onchange = (e) => window.realtimeDashboard.setUpdateInterval(parseInt(e.target.value));
        intervalSelect.style.cssText = 'padding: 5px; border: 1px solid #ccc; border-radius: 4px;';
        
        controlsDiv.appendChild(refreshBtn);
        controlsDiv.appendChild(toggleBtn);
        controlsDiv.appendChild(intervalSelect);
        navbar.appendChild(controlsDiv);
    }
    
    // Log keyboard shortcuts
    console.log('🚀 Smart Energy Dashboard - Realtime Mode');
    console.log('Keyboard shortcuts:');
    console.log('  Ctrl+R: Toggle realtime updates');
    console.log('  Ctrl+1: 1 second interval');
    console.log('  Ctrl+3: 3 second interval');
    console.log('  Ctrl+5: 5 second interval');
    console.log('  Ctrl+0: 10 second interval');
});

// Export for use in other scripts
if (typeof module !== 'undefined' && module.exports) {
    module.exports = RealtimeDashboard;
}