/**
 * Script Debug untuk Dashboard History
 * Copy paste script ini ke Console browser (F12 > Console)
 */

console.log('🔍 Starting Dashboard History Debug...');

// 1. Test API Endpoints
async function testAllAPIs() {
    console.log('\n=== TESTING API ENDPOINTS ===');
    
    const endpoints = [
        '/api/sensor/history?per_page=10',
        '/api/sensor/data?per_page=10', 
        '/api/sensor/latest'
    ];
    
    for (const endpoint of endpoints) {
        console.log(`\n📡 Testing: ${endpoint}`);
        try {
            const response = await fetch(endpoint, {
                headers: {
                    'Accept': 'application/json',
                    'Content-Type': 'application/json',
                    'X-Requested-With': 'XMLHttpRequest'
                }
            });
            
            console.log(`Status: ${response.status} ${response.statusText}`);
            
            if (response.ok) {
                const data = await response.json();
                console.log(`✅ Success:`, data);
                
                if (data.data && Array.isArray(data.data)) {
                    console.log(`📊 Data count: ${data.data.length}`);
                    if (data.pagination) {
                        console.log(`📄 Total records: ${data.pagination.total}`);
                    }
                } else if (data.data) {
                    console.log(`📊 Single record:`, data.data);
                }
            } else {
                const errorData = await response.json();
                console.log(`❌ Error:`, errorData);
            }
        } catch (error) {
            console.error(`❌ Network Error:`, error);
        }
    }
}

// 2. Test DOM Elements
function testDOMElements() {
    console.log('\n=== TESTING DOM ELEMENTS ===');
    
    const elements = [
        'sensor-data-body',
        'sensorDataLimit', 
        'historyPeriod',
        'deviceFilter',
        'sensorPageInfo'
    ];
    
    elements.forEach(id => {
        const element = document.getElementById(id);
        if (element) {
            console.log(`✅ Found: #${id}`, element);
        } else {
            console.log(`❌ Missing: #${id}`);
        }
    });
}

// 3. Test History Loading Function
function testHistoryLoading() {
    console.log('\n=== TESTING HISTORY LOADING ===');
    
    // Check if loadSensorData function exists
    if (typeof loadSensorData === 'function') {
        console.log('✅ loadSensorData function exists');
        
        // Try to call it
        try {
            console.log('🔄 Calling loadSensorData()...');
            loadSensorData();
            console.log('✅ loadSensorData() called successfully');
        } catch (error) {
            console.error('❌ Error calling loadSensorData():', error);
        }
    } else {
        console.log('❌ loadSensorData function not found');
    }
    
    // Check if showHistoryTab function exists
    if (typeof showHistoryTab === 'function') {
        console.log('✅ showHistoryTab function exists');
    } else {
        console.log('❌ showHistoryTab function not found');
    }
}

// 4. Test Manual Data Send
async function testManualDataSend() {
    console.log('\n=== TESTING MANUAL DATA SEND ===');
    
    const testData = {
        device_id: 'ESP32_Browser_Debug',
        location: 'Browser Debug Test',
        people_count: Math.floor(Math.random() * 10) + 1,
        ac_status: 'DEBUG ON',
        set_temperature: 25,
        room_temperature: 28.5,
        humidity: 65.0,
        light_level: 300,
        proximity_in: true,
        proximity_out: false,
        wifi_rssi: -45,
        status: 'active',
        timestamp: Date.now()
    };
    
    console.log('📤 Sending test data:', testData);
    
    try {
        const response = await fetch('/api/sensor/data', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Accept': 'application/json',
                'X-Requested-With': 'XMLHttpRequest'
            },
            body: JSON.stringify(testData)
        });
        
        console.log(`Response status: ${response.status}`);
        
        if (response.ok) {
            const result = await response.json();
            console.log('✅ Data sent successfully:', result);
            
            if (result.data && result.data.id) {
                console.log(`📝 New record ID: ${result.data.id}`);
                
                // Try to reload history data
                console.log('🔄 Reloading history data...');
                if (typeof loadSensorData === 'function') {
                    setTimeout(() => {
                        loadSensorData();
                        console.log('✅ History data reloaded');
                    }, 1000);
                }
            }
        } else {
            const errorData = await response.json();
            console.log('❌ Send failed:', errorData);
        }
    } catch (error) {
        console.error('❌ Network error:', error);
    }
}

// 5. Check Browser Console Errors
function checkConsoleErrors() {
    console.log('\n=== CHECKING CONSOLE ERRORS ===');
    console.log('Check the Console tab for any red error messages');
    console.log('Common issues to look for:');
    console.log('- Network errors (failed to fetch)');
    console.log('- JavaScript errors (undefined functions/variables)');
    console.log('- CORS errors');
    console.log('- 404/500 HTTP errors');
}

// 6. Check Network Tab
function checkNetworkTab() {
    console.log('\n=== CHECKING NETWORK TAB ===');
    console.log('1. Open DevTools (F12)');
    console.log('2. Go to Network tab');
    console.log('3. Reload the page or click History tab');
    console.log('4. Look for API requests to:');
    console.log('   - /api/sensor/history');
    console.log('   - /api/sensor/data');
    console.log('   - /api/sensor/latest');
    console.log('5. Check if requests are successful (200 status)');
    console.log('6. Check response data');
}

// Run all tests
async function runAllTests() {
    console.log('🚀 Running all debug tests...\n');
    
    testDOMElements();
    testHistoryLoading();
    await testAllAPIs();
    checkConsoleErrors();
    checkNetworkTab();
    
    console.log('\n✅ All tests completed!');
    console.log('\n📋 Next steps:');
    console.log('1. Check the results above');
    console.log('2. If APIs return data but history is empty, it\'s a frontend issue');
    console.log('3. If APIs return no data, it\'s a backend/database issue');
    console.log('4. Run testManualDataSend() to test data insertion');
}

// Auto-run tests
runAllTests();

// Make functions available globally for manual testing
window.debugHistory = {
    testAllAPIs,
    testDOMElements,
    testHistoryLoading,
    testManualDataSend,
    runAllTests
};

console.log('\n🔧 Debug functions available:');
console.log('- debugHistory.testAllAPIs()');
console.log('- debugHistory.testManualDataSend()');
console.log('- debugHistory.runAllTests()');