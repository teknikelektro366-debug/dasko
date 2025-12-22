#!/usr/bin/env python3
"""
Bluetooth to Laravel Bridge
Menerima data dari ESP32 via Bluetooth dan mengirim ke Laravel via LAN
"""

import bluetooth
import json
import requests
import time
import threading
from datetime import datetime
import sys
import os

class BluetoothLaravelBridge:
    def __init__(self):
        self.esp32_mac = None  # Will be auto-detected
        self.esp32_name = "SmartEnergy_ESP32"
        self.laravel_url = "http://localhost:8000/api/sensor-data"  # Sesuaikan dengan URL Laravel Anda
        self.laravel_headers = {
            'Content-Type': 'application/json',
            'Accept': 'application/json',
            'User-Agent': 'ESP32-Bluetooth-Bridge/1.0'
        }
        
        self.bluetooth_socket = None
        self.connected = False
        self.running = False
        
        # Statistics
        self.data_received = 0
        self.data_sent_to_laravel = 0
        self.connection_errors = 0
        self.laravel_errors = 0
        
        print("=== BLUETOOTH TO LARAVEL BRIDGE ===")
        print(f"Target ESP32: {self.esp32_name}")
        print(f"Laravel URL: {self.laravel_url}")
        print("=====================================")
    
    def find_esp32_device(self):
        """Mencari ESP32 device berdasarkan nama"""
        print("🔍 Searching for ESP32 device...")
        
        try:
            nearby_devices = bluetooth.discover_devices(duration=10, lookup_names=True)
            
            for addr, name in nearby_devices:
                print(f"Found device: {name} ({addr})")
                if self.esp32_name in name:
                    self.esp32_mac = addr
                    print(f"✅ Found ESP32: {name} at {addr}")
                    return True
            
            print(f"❌ ESP32 device '{self.esp32_name}' not found")
            return False
            
        except Exception as e:
            print(f"❌ Error searching for devices: {e}")
            return False
    
    def connect_to_esp32(self):
        """Koneksi ke ESP32 via Bluetooth"""
        if not self.esp32_mac:
            if not self.find_esp32_device():
                return False
        
        try:
            print(f"🔗 Connecting to ESP32 at {self.esp32_mac}...")
            
            # Create Bluetooth socket
            self.bluetooth_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
            self.bluetooth_socket.connect((self.esp32_mac, 1))  # Port 1 for RFCOMM
            
            self.connected = True
            print("✅ Connected to ESP32 successfully!")
            
            # Send welcome message
            welcome_cmd = json.dumps({"action": "keep_alive", "source": "computer_bridge"})
            self.bluetooth_socket.send(welcome_cmd.encode() + b'\n')
            
            return True
            
        except Exception as e:
            print(f"❌ Failed to connect to ESP32: {e}")
            self.connection_errors += 1
            return False
    
    def disconnect_from_esp32(self):
        """Disconnect dari ESP32"""
        if self.bluetooth_socket:
            try:
                self.bluetooth_socket.close()
                print("🔌 Disconnected from ESP32")
            except:
                pass
        
        self.connected = False
        self.bluetooth_socket = None
    
    def send_to_laravel(self, data):
        """Kirim data ke Laravel server via LAN"""
        try:
            # Format data untuk Laravel
            laravel_data = {
                "device_id": data.get("device_id", "ESP32_Smart_Energy"),
                "location": data.get("location", "Lab Teknik Tegangan Tinggi"),
                "people_count": data.get("people_count", 0),
                "ac_status": data.get("ac_status", "OFF"),
                "set_temperature": data.get("set_temperature", 0),
                "room_temperature": data.get("room_temperature", 0.0),
                "humidity": data.get("humidity", 0.0),
                "light_level": data.get("light_level", 0),
                "proximity_in": data.get("proximity_in", False),
                "proximity_out": data.get("proximity_out", False),
                "uptime": data.get("uptime", 0),
                "free_heap": data.get("free_heap", 0),
                "status": data.get("status", "active"),
                "data_source": "bluetooth_bridge",
                "bridge_timestamp": datetime.now().isoformat()
            }
            
            print(f"📤 Sending to Laravel: People={laravel_data['people_count']}, AC={laravel_data['ac_status']}, Temp={laravel_data['room_temperature']}°C")
            
            # POST request ke Laravel
            response = requests.post(
                self.laravel_url,
                json=laravel_data,
                headers=self.laravel_headers,
                timeout=10
            )
            
            if response.status_code in [200, 201]:
                print(f"✅ Data sent to Laravel successfully (Status: {response.status_code})")
                self.data_sent_to_laravel += 1
                return True
            else:
                print(f"⚠️ Laravel responded with status {response.status_code}: {response.text}")
                self.laravel_errors += 1
                return False
                
        except requests.exceptions.RequestException as e:
            print(f"❌ Failed to send to Laravel: {e}")
            self.laravel_errors += 1
            return False
        except Exception as e:
            print(f"❌ Unexpected error sending to Laravel: {e}")
            self.laravel_errors += 1
            return False
    
    def handle_esp32_data(self, data):
        """Handle data yang diterima dari ESP32"""
        try:
            json_data = json.loads(data)
            data_type = json_data.get("type", "unknown")
            
            print(f"📨 Received from ESP32: {data_type}")
            
            if data_type == "sensor_data":
                # Data sensor utama - kirim ke Laravel
                self.data_received += 1
                success = self.send_to_laravel(json_data)
                
                if success:
                    # Send acknowledgment ke ESP32
                    ack = json.dumps({
                        "type": "response",
                        "status": "success",
                        "message": "Data sent to Laravel successfully"
                    })
                    self.bluetooth_socket.send(ack.encode() + b'\n')
                
            elif data_type == "notification":
                # Notifikasi real-time (person entered/exited, AC change)
                event = json_data.get("event", "")
                message = json_data.get("message", "")
                
                print(f"🔔 Notification: {message}")
                
                # Jika notifikasi penting, kirim update ke Laravel
                if json_data.get("requires_laravel_update", False):
                    # Request data terbaru dari ESP32
                    get_data_cmd = json.dumps({"action": "get_data"})
                    self.bluetooth_socket.send(get_data_cmd.encode() + b'\n')
                
            elif data_type == "welcome":
                print(f"👋 ESP32 Welcome: {json_data.get('message', '')}")
                
            elif data_type == "response":
                status = json_data.get("status", "")
                message = json_data.get("message", "")
                print(f"📋 ESP32 Response ({status}): {message}")
                
            elif data_type == "system_status":
                print(f"⚙️ ESP32 System Status: {json_data.get('people_count', 0)} people, {json_data.get('temperature', 0)}°C")
                
        except json.JSONDecodeError as e:
            print(f"❌ Invalid JSON from ESP32: {data}")
        except Exception as e:
            print(f"❌ Error handling ESP32 data: {e}")
    
    def listen_to_esp32(self):
        """Listen untuk data dari ESP32"""
        buffer = ""
        
        while self.running and self.connected:
            try:
                # Receive data dari ESP32
                data = self.bluetooth_socket.recv(1024).decode('utf-8')
                buffer += data
                
                # Process complete lines
                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    if line.strip():
                        self.handle_esp32_data(line.strip())
                
            except bluetooth.BluetoothError as e:
                print(f"❌ Bluetooth error: {e}")
                self.connected = False
                break
            except Exception as e:
                print(f"❌ Error receiving data: {e}")
                break
    
    def send_periodic_requests(self):
        """Kirim request data secara berkala ke ESP32"""
        while self.running and self.connected:
            try:
                # Request data setiap 5 detik
                time.sleep(5)
                
                if self.connected:
                    get_data_cmd = json.dumps({"action": "get_data"})
                    self.bluetooth_socket.send(get_data_cmd.encode() + b'\n')
                    
            except Exception as e:
                print(f"❌ Error sending periodic request: {e}")
                break
    
    def print_statistics(self):
        """Print statistik secara berkala"""
        while self.running:
            time.sleep(30)  # Print setiap 30 detik
            
            print("\n=== STATISTICS ===")
            print(f"Data received from ESP32: {self.data_received}")
            print(f"Data sent to Laravel: {self.data_sent_to_laravel}")
            print(f"Connection errors: {self.connection_errors}")
            print(f"Laravel errors: {self.laravel_errors}")
            print(f"Connected: {self.connected}")
            print("==================\n")
    
    def run(self):
        """Main loop"""
        self.running = True
        
        # Start statistics thread
        stats_thread = threading.Thread(target=self.print_statistics, daemon=True)
        stats_thread.start()
        
        while self.running:
            try:
                # Connect to ESP32
                if not self.connected:
                    if self.connect_to_esp32():
                        # Start listening thread
                        listen_thread = threading.Thread(target=self.listen_to_esp32, daemon=True)
                        listen_thread.start()
                        
                        # Start periodic request thread
                        request_thread = threading.Thread(target=self.send_periodic_requests, daemon=True)
                        request_thread.start()
                        
                        print("🚀 Bridge is running! Press Ctrl+C to stop.")
                
                # Keep main thread alive
                time.sleep(1)
                
            except KeyboardInterrupt:
                print("\n🛑 Stopping bridge...")
                self.running = False
                break
            except Exception as e:
                print(f"❌ Unexpected error: {e}")
                time.sleep(5)  # Wait before retry
        
        # Cleanup
        self.disconnect_from_esp32()
        print("👋 Bridge stopped.")

def main():
    """Main function"""
    print("Starting Bluetooth to Laravel Bridge...")
    
    # Check if bluetooth module is available
    try:
        import bluetooth
    except ImportError:
        print("❌ PyBluez module not found!")
        print("Install with: pip install pybluez")
        sys.exit(1)
    
    # Check if requests module is available
    try:
        import requests
    except ImportError:
        print("❌ Requests module not found!")
        print("Install with: pip install requests")
        sys.exit(1)
    
    # Create and run bridge
    bridge = BluetoothLaravelBridge()
    
    try:
        bridge.run()
    except Exception as e:
        print(f"❌ Fatal error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()