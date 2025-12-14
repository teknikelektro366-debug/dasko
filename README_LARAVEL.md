# Smart Energy Management System - Laravel Backend

## Overview
Complete Laravel backend implementation for the Smart Energy Management System with comprehensive Admin Dashboard, user authentication, and database management.

## Features Implemented

### 🔐 Authentication System
- **Multi-role authentication** (Admin, Operator, Viewer)
- **Session-based login** with rate limiting
- **Role-based access control** with middleware
- **User management** with profile and permissions

### 👨‍💼 Admin Dashboard
- **System overview** with real-time statistics
- **User management** (CRUD operations, role assignment, status toggle)
- **Device management** (control, monitoring, bulk actions)
- **AC automation rules** management with import/export
- **Energy logs** with filtering, cleanup, and export
- **Arduino control** (connection testing, sync, emergency shutdown)
- **Database management** (backup, cleanup, reset)
- **System settings** configuration
- **System logs** viewing and management

### 📊 Main Dashboard
- **Real-time sensor data** display (temperature, humidity, people count)
- **Working hours status** monitoring
- **Energy saving status** tracking
- **Device status** overview with controls
- **AC automation rules** display with active rule highlighting
- **Live updates** every 5 seconds

### 🔌 Device Management
- **12 device types** (AC units, lights, computers, TV, etc.)
- **Power consumption** tracking and calculation
- **Always-on devices** protection (fridge, router)
- **GPIO and relay pin** configuration
- **Energy logging** with efficiency ratings

### ❄️ AC Automation System
- **Rule-based automation** following Tabel 9 specifications
- **People count and temperature** based control
- **Gradual temperature adjustment** (15-minute mode)
- **Overlap detection** for rules
- **Import/export functionality** for rules

### 📈 Energy Monitoring
- **Real-time energy consumption** tracking
- **Historical data** with filtering and analytics
- **Cost estimation** based on electricity rates
- **Efficiency ratings** (Excellent, Good, Average, Poor)
- **Export capabilities** (CSV format)

### 🤖 Arduino Integration
- **RESTful API endpoints** for Arduino communication
- **Sensor data reception** and storage
- **Device control commands** with feedback
- **Connection status monitoring**
- **Emergency shutdown** capabilities

## Database Schema

### Users Table
- `id`, `name`, `email`, `password`
- `role` (admin, operator, viewer)
- `is_active`, `last_login_at`, `avatar`

### Devices Table
- `id`, `name`, `type`, `power_consumption`, `units`
- `location`, `is_active`, `is_always_on`
- `gpio_pin`, `relay_pin`, `description`

### Sensor Readings Table
- `id`, `device_id`, `sensor_type`, `sensor_name`
- `value`, `unit`, `location`, `gpio_pin`, `reading_time`

### Energy Logs Table
- `id`, `device_id`, `energy_consumed`, `duration_minutes`
- `start_time`, `end_time`, `people_count`, `outdoor_temperature`
- `ac_target_temperature`, `energy_saving_mode`, `working_hours`, `cost_estimate`

### AC Automation Rules Table
- `id`, `min_people`, `max_people`, `min_outdoor_temp`, `max_outdoor_temp`
- `ac_units_needed`, `target_temperature`, `description`, `is_active`

## API Endpoints

### Arduino API (No Authentication)
```
POST /api/arduino/sensor-data     - Receive sensor data
GET  /api/arduino/sensor-data     - Get current sensor data
POST /api/arduino/control         - Control devices
GET  /api/arduino/ac-automation   - Get AC automation status
POST /api/arduino/ac-automation   - Update AC automation
GET  /api/arduino/status          - Get system status
```

### Protected API (Requires Authentication)
```
GET  /api/dashboard/stats         - Dashboard statistics
GET  /api/dashboard/real-time     - Real-time data
POST /api/devices/{device}/control - Control specific device
GET  /api/devices/status          - Get all device status
GET  /api/energy/current          - Current energy usage
GET  /api/energy/history          - Energy history
```

## Installation & Setup

### 1. Install Dependencies
```bash
composer install
```

### 2. Environment Configuration
```bash
cp .env.example .env
php artisan key:generate
```

### 3. Database Setup
```bash
# Configure database in .env file
DB_CONNECTION=mysql
DB_HOST=127.0.0.1
DB_PORT=3306
DB_DATABASE=smart_energy
DB_USERNAME=root
DB_PASSWORD=

# Run migrations and seeders
php artisan migrate
php artisan db:seed
```

### 4. Arduino Configuration
```bash
# Configure Arduino settings in .env
ARDUINO_IP=192.168.1.100
ARDUINO_USERNAME=admin
ARDUINO_PASSWORD=elektro2024
```

### 5. Start Development Server
```bash
php artisan serve
```

## Default User Accounts

| Role | Email | Password | Access Level |
|------|-------|----------|--------------|
| Admin | admin@smartenergy.lab | elektro2024 | Full system access |
| Admin | dosen@smartenergy.lab | dosen2024 | Full system access |
| Operator | operator@smartenergy.lab | operator2024 | Device control + reports |
| Viewer | viewer@smartenergy.lab | viewer2024 | Read-only access |
| Viewer | mahasiswa@smartenergy.lab | mahasiswa2024 | Read-only access |

## System Configuration

### Working Hours
- **Default**: 07:00 - 17:00 WIB (Monday-Friday)
- **Configurable** via admin settings

### Energy Saving
- **Stage 1**: 15 minutes without people → AC to 28°C, lights off
- **Stage 2**: 30 minutes without people → Complete panel shutdown

### AC Automation Rules (Tabel 9)
- **0 people**: Gradual mode to 28°C over 15 minutes
- **5-10 people, 30-32°C**: 1 AC unit, target 25°C
- **5-10 people, 32-35°C**: 1 AC unit, target 26°C
- **10-13 people, 30-32°C**: 2 AC units, target 24°C
- **10-20 people, 32-35°C**: 2 AC units, target 25°C

## Security Features

### Authentication
- **Rate limiting** (5 attempts per minute)
- **Session management** with CSRF protection
- **Password hashing** using bcrypt
- **Role-based middleware** protection

### Admin Protection
- **Cannot delete last admin** user
- **Cannot deactivate self** as admin
- **Confirmation required** for destructive actions
- **Activity logging** for admin actions

### Device Protection
- **Always-on devices** cannot be turned off
- **Emergency shutdown** available for critical situations
- **Device state validation** before operations

## File Structure

```
app/
├── Http/
│   ├── Controllers/
│   │   ├── Auth/LoginController.php
│   │   ├── DashboardController.php
│   │   ├── Admin/
│   │   │   ├── AdminController.php
│   │   │   ├── UserController.php
│   │   │   ├── DeviceController.php
│   │   │   ├── ACRuleController.php
│   │   │   └── EnergyLogController.php
│   │   └── Api/ArduinoController.php
│   └── Middleware/AdminMiddleware.php
├── Models/
│   ├── User.php
│   ├── Device.php
│   ├── SensorReading.php
│   ├── EnergyLog.php
│   └── ACAutomationRule.php
└── Services/ArduinoService.php

resources/views/
├── layouts/
│   ├── app.blade.php
│   └── sidebar.blade.php
├── auth/login.blade.php
├── admin/
│   ├── dashboard.blade.php
│   └── users/index.blade.php
└── dashboard/index.blade.php

database/
├── migrations/
├── seeders/
│   ├── DatabaseSeeder.php
│   ├── UserSeeder.php
│   ├── DeviceSeeder.php
│   └── ACAutomationRuleSeeder.php
```

## Integration with Arduino

The Laravel backend is designed to work seamlessly with the Arduino ESP32 system:

1. **Arduino sends sensor data** → Laravel stores and processes
2. **Laravel sends control commands** → Arduino executes device control
3. **Real-time synchronization** of device states
4. **Automatic AC automation** based on sensor readings
5. **Energy logging** for all device operations

## Next Steps

1. **Deploy to production server**
2. **Configure SSL/HTTPS**
3. **Set up automated backups**
4. **Configure email notifications**
5. **Add WebSocket for real-time updates**
6. **Implement advanced analytics**
7. **Add mobile app API endpoints**

## Support

For technical support or questions about the Laravel implementation, please refer to the system documentation or contact the development team.

---

**Lab Teknik Tegangan Tinggi**  
Smart Energy Management System  
© 2024