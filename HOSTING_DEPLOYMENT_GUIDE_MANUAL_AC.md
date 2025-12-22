# Panduan Deployment Hosting dengan Manual AC Control
## Domain: https://dasko.fst.unja.ac.id/

## Overview
Panduan ini menjelaskan cara deploy sistem Smart Energy dengan fitur manual AC control ke hosting production dengan domain `https://dasko.fst.unja.ac.id/`.

## Persiapan Hosting

### 1. **Server Requirements**
- **PHP**: 8.1 atau lebih tinggi
- **MySQL**: 8.0 atau lebih tinggi
- **SSL Certificate**: Let's Encrypt atau SSL berbayar
- **Memory**: Minimum 512MB RAM
- **Storage**: Minimum 1GB disk space
- **Extensions**: OpenSSL, PDO, Mbstring, Tokenizer, XML, Ctype, JSON

### 2. **Domain Configuration**
- **Domain**: `dasko.fst.unja.ac.id`
- **SSL**: HTTPS enabled (wajib)
- **Subdomain**: Bisa menggunakan subdomain jika diperlukan

## File Deployment

### 1. **Upload Files ke Hosting**
Upload semua file Laravel ke hosting:
```
/public_html/
├── app/
├── bootstrap/
├── config/
├── database/
├── public/
├── resources/
├── routes/
├── storage/
├── vendor/
├── .env
├── artisan
├── composer.json
└── composer.lock
```

### 2. **Environment Configuration**
Copy `.env.hosting` ke `.env` dan sesuaikan:
```bash
# Copy environment file
cp .env.hosting .env

# Edit configuration
nano .env
```

**Konfigurasi Penting**:
```env
APP_URL=https://dasko.fst.unja.ac.id
APP_ENV=production
APP_DEBUG=false

# Database
DB_HOST=localhost
DB_DATABASE=smart_energy_production
DB_USERNAME=your_db_username
DB_PASSWORD=your_secure_password

# Hosting
HOSTING_DOMAIN=dasko.fst.unja.ac.id
HOSTING_URL=https://dasko.fst.unja.ac.id
HOSTING_SSL_ENABLED=true
```

### 3. **Database Setup**
```bash
# Create database
mysql -u root -p
CREATE DATABASE smart_energy_production;
CREATE USER 'smart_energy_user'@'localhost' IDENTIFIED BY 'secure_password';
GRANT ALL PRIVILEGES ON smart_energy_production.* TO 'smart_energy_user'@'localhost';
FLUSH PRIVILEGES;
EXIT;

# Run migrations
php artisan migrate --force
```

### 4. **Permissions Setup**
```bash
# Set proper permissions
chmod -R 755 storage/
chmod -R 755 bootstrap/cache/
chown -R www-data:www-data storage/
chown -R www-data:www-data bootstrap/cache/
```

## Arduino Configuration

### 1. **Upload Arduino Code**
Upload file `arduino_production_hosting_manual_ac.ino` ke ESP32:

**Key Features**:
- HTTPS/SSL communication
- Production domain integration
- Manual AC control support
- SSL certificate verification
- Error handling dan retry logic

### 2. **WiFi Configuration**
Update WiFi credentials di Arduino code:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. **API Endpoints**
Arduino akan mengakses:
- **Data API**: `https://dasko.fst.unja.ac.id/api/sensor/data`
- **AC Control**: `https://dasko.fst.unja.ac.id/api/ac/control`

## Dashboard Configuration

### 1. **Update JavaScript**
Ganti `ac-control.js` dengan `ac-control-hosting.js` di dashboard:
```html
<script src="{{ asset('ac-control-hosting.js') }}"></script>
```

### 2. **HTTPS Configuration**
Pastikan semua API calls menggunakan HTTPS:
```javascript
const HOSTING_CONFIG = {
    domain: 'dasko.fst.unja.ac.id',
    baseUrl: 'https://dasko.fst.unja.ac.id',
    apiUrl: 'https://dasko.fst.unja.ac.id/api',
    sslEnabled: true
};
```

## SSL Certificate Setup

### 1. **Let's Encrypt (Gratis)**
```bash
# Install Certbot
sudo apt install certbot python3-certbot-apache

# Generate certificate
sudo certbot --apache -d dasko.fst.unja.ac.id

# Auto renewal
sudo crontab -e
# Add: 0 12 * * * /usr/bin/certbot renew --quiet
```

### 2. **SSL Verification**
Test SSL certificate:
```bash
# Check SSL
openssl s_client -connect dasko.fst.unja.ac.id:443

# Check certificate expiry
echo | openssl s_client -servername dasko.fst.unja.ac.id -connect dasko.fst.unja.ac.id:443 2>/dev/null | openssl x509 -noout -dates
```

## Testing

### 1. **API Testing**
```bash
# Test sensor data API
curl -X POST "https://dasko.fst.unja.ac.id/api/sensor/data" \
  -H "Content-Type: application/json" \
  -d '{"device_id":"ESP32_Smart_Energy_Production","location":"Lab Teknik Tegangan Tinggi","people_count":5,"room_temperature":25.5,"humidity":60.0}'

# Test AC control API
curl -X GET "https://dasko.fst.unja.ac.id/api/ac/control?device_id=ESP32_Smart_Energy_Production"
```

### 2. **Dashboard Testing**
1. Buka: `https://dasko.fst.unja.ac.id/dashboard`
2. Test manual AC control
3. Verify HTTPS connection
4. Check browser console for errors

### 3. **Arduino Testing**
Monitor serial output untuk:
- WiFi connection success
- HTTPS API calls
- SSL certificate verification
- AC control commands received

## Security Configuration

### 1. **Firewall Setup**
```bash
# Allow HTTP/HTTPS
sudo ufw allow 80
sudo ufw allow 443

# Allow SSH (if needed)
sudo ufw allow 22

# Enable firewall
sudo ufw enable
```

### 2. **Rate Limiting**
Configure di `config/hosting.php`:
```php
'rate_limit' => [
    'requests_per_minute' => 60,
    'requests_per_hour' => 1000,
],
```

### 3. **CORS Configuration**
Update `config/cors.php`:
```php
'allowed_origins' => ['https://dasko.fst.unja.ac.id'],
'allowed_methods' => ['GET', 'POST', 'PUT', 'DELETE'],
'allowed_headers' => ['*'],
```

## Monitoring & Maintenance

### 1. **Log Monitoring**
```bash
# Laravel logs
tail -f storage/logs/laravel.log

# Apache logs
tail -f /var/log/apache2/access.log
tail -f /var/log/apache2/error.log
```

### 2. **Database Backup**
```bash
# Create backup script
#!/bin/bash
DATE=$(date +%Y%m%d_%H%M%S)
mysqldump -u smart_energy_user -p smart_energy_production > backup_$DATE.sql
```

### 3. **Health Check**
Setup monitoring untuk:
- SSL certificate expiry
- Database connection
- Arduino connectivity
- API response times

## Troubleshooting

### 1. **SSL Issues**
```bash
# Check SSL configuration
sudo apache2ctl configtest

# Restart Apache
sudo systemctl restart apache2

# Check certificate
sudo certbot certificates
```

### 2. **Database Issues**
```bash
# Check MySQL status
sudo systemctl status mysql

# Check connections
mysql -u smart_energy_user -p -e "SHOW PROCESSLIST;"
```

### 3. **Arduino Connection Issues**
- Verify WiFi credentials
- Check SSL certificate in Arduino code
- Monitor serial output for HTTPS errors
- Verify API endpoints accessibility

### 4. **Dashboard Issues**
- Check browser console for JavaScript errors
- Verify CSRF token
- Check API response in Network tab
- Verify HTTPS mixed content issues

## Performance Optimization

### 1. **Caching**
```bash
# Enable caching
php artisan config:cache
php artisan route:cache
php artisan view:cache
```

### 2. **Database Optimization**
```sql
-- Add indexes
CREATE INDEX idx_device_location ON sensor_data(device_id, location);
CREATE INDEX idx_created_at ON sensor_data(created_at);
CREATE INDEX idx_ac_control_device ON ac_controls(device_id, location);
```

### 3. **Apache Optimization**
```apache
# Enable compression
LoadModule deflate_module modules/mod_deflate.so
<Location />
    SetOutputFilter DEFLATE
</Location>

# Enable caching
LoadModule expires_module modules/mod_expires.so
ExpiresActive On
ExpiresByType text/css "access plus 1 month"
ExpiresByType application/javascript "access plus 1 month"
```

## Backup & Recovery

### 1. **Automated Backup**
```bash
#!/bin/bash
# backup.sh
DATE=$(date +%Y%m%d_%H%M%S)

# Database backup
mysqldump -u smart_energy_user -p smart_energy_production > db_backup_$DATE.sql

# Files backup
tar -czf files_backup_$DATE.tar.gz /path/to/laravel/

# Upload to cloud storage (optional)
# aws s3 cp db_backup_$DATE.sql s3://your-bucket/backups/
```

### 2. **Recovery Process**
```bash
# Restore database
mysql -u smart_energy_user -p smart_energy_production < db_backup_YYYYMMDD_HHMMSS.sql

# Restore files
tar -xzf files_backup_YYYYMMDD_HHMMSS.tar.gz -C /
```

## Final Checklist

### Pre-Deployment:
- [ ] SSL certificate installed and valid
- [ ] Database created and configured
- [ ] Environment variables set correctly
- [ ] File permissions configured
- [ ] Arduino code updated with production URLs

### Post-Deployment:
- [ ] Dashboard accessible via HTTPS
- [ ] API endpoints responding correctly
- [ ] Arduino connecting and sending data
- [ ] Manual AC control working
- [ ] SSL certificate auto-renewal configured
- [ ] Monitoring and alerting setup
- [ ] Backup system configured

### Testing:
- [ ] End-to-end functionality test
- [ ] Load testing (if needed)
- [ ] Security scan
- [ ] Performance monitoring
- [ ] Error handling verification

---

**Domain**: https://dasko.fst.unja.ac.id/
**Status**: Production Ready
**SSL**: Enabled & Verified
**Manual AC Control**: Fully Supported