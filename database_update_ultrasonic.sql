-- Database update untuk mendukung ultrasonic sensors
-- File: database_update_ultrasonic.sql

-- 1. Tambah kolom baru untuk ultrasonic sensors
ALTER TABLE sensor_data ADD COLUMN IF NOT EXISTS ultrasonic_in BOOLEAN DEFAULT FALSE COMMENT 'Status sensor ultrasonic LUAR (masuk)';
ALTER TABLE sensor_data ADD COLUMN IF NOT EXISTS ultrasonic_out BOOLEAN DEFAULT FALSE COMMENT 'Status sensor ultrasonic DALAM (keluar)';
ALTER TABLE sensor_data ADD COLUMN IF NOT EXISTS update_type VARCHAR(50) DEFAULT 'sensor_reading' COMMENT 'Tipe update: ultrasonic_detection, manual_control, etc';
ALTER TABLE sensor_data ADD COLUMN IF NOT EXISTS sensor_type VARCHAR(50) DEFAULT 'Unknown' COMMENT 'Tipe sensor: HC-SR04_Ultrasonic, Proximity, etc';

-- 2. Update existing data (opsional - untuk data lama)
UPDATE sensor_data 
SET sensor_type = 'Proximity_Sensor', 
    update_type = 'proximity_detection' 
WHERE sensor_type = 'Unknown' OR sensor_type IS NULL;

-- 3. Buat index untuk performa yang lebih baik
CREATE INDEX IF NOT EXISTS idx_sensor_data_sensor_type ON sensor_data(sensor_type);
CREATE INDEX IF NOT EXISTS idx_sensor_data_update_type ON sensor_data(update_type);
CREATE INDEX IF NOT EXISTS idx_sensor_data_ultrasonic ON sensor_data(ultrasonic_in, ultrasonic_out);

-- 4. Buat view untuk data ultrasonic terbaru
CREATE OR REPLACE VIEW latest_ultrasonic_data AS
SELECT 
    id,
    device_id,
    location,
    people_count,
    ac_status,
    set_temperature,
    room_temperature,
    humidity,
    light_level,
    ultrasonic_in,
    ultrasonic_out,
    update_type,
    sensor_type,
    update_reason,
    created_at,
    CASE 
        WHEN ultrasonic_in = 1 THEN 'LUAR AKTIF'
        WHEN ultrasonic_out = 1 THEN 'DALAM AKTIF' 
        ELSE 'TIDAK ADA OBJEK'
    END as ultrasonic_status
FROM sensor_data 
WHERE sensor_type = 'HC-SR04_Ultrasonic'
ORDER BY created_at DESC;

-- 5. Buat stored procedure untuk statistik ultrasonic
DELIMITER //
CREATE OR REPLACE PROCEDURE GetUltrasonicStats(IN date_filter DATE)
BEGIN
    SELECT 
        DATE(created_at) as date,
        COUNT(*) as total_detections,
        SUM(CASE WHEN ultrasonic_in = 1 THEN 1 ELSE 0 END) as entries,
        SUM(CASE WHEN ultrasonic_out = 1 THEN 1 ELSE 0 END) as exits,
        AVG(people_count) as avg_people,
        MAX(people_count) as max_people,
        MIN(people_count) as min_people
    FROM sensor_data 
    WHERE sensor_type = 'HC-SR04_Ultrasonic'
    AND (date_filter IS NULL OR DATE(created_at) = date_filter)
    GROUP BY DATE(created_at)
    ORDER BY date DESC;
END //
DELIMITER ;

-- 6. Sample data untuk testing (opsional)
INSERT INTO sensor_data (
    device_id, location, people_count, ac_status, set_temperature,
    room_temperature, humidity, light_level, ultrasonic_in, ultrasonic_out,
    update_type, sensor_type, update_reason, created_at
) VALUES 
('ESP32_Ultrasonic_Test', 'Lab Test', 1, '1 AC ON', 25, 26.5, 65, 450, 1, 0, 'ultrasonic_detection', 'HC-SR04_Ultrasonic', 'Ultrasonic entry - Count: 1', NOW()),
('ESP32_Ultrasonic_Test', 'Lab Test', 0, 'AC OFF', 0, 26.8, 64, 440, 0, 1, 'ultrasonic_detection', 'HC-SR04_Ultrasonic', 'Ultrasonic exit - Count: 0', NOW() + INTERVAL 30 SECOND);

-- 7. Verify the changes
SELECT 
    COLUMN_NAME, 
    DATA_TYPE, 
    IS_NULLABLE, 
    COLUMN_DEFAULT, 
    COLUMN_COMMENT
FROM INFORMATION_SCHEMA.COLUMNS 
WHERE TABLE_NAME = 'sensor_data' 
AND COLUMN_NAME IN ('ultrasonic_in', 'ultrasonic_out', 'update_type', 'sensor_type')
ORDER BY ORDINAL_POSITION;