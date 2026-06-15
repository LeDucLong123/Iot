#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

#include "lcd_manager.h"
#include "dht_manager.h"

// Địa chỉ I2C 0x27, kích thước 16 cột 2 dòng
static LiquidCrystal_I2C lcd(0x27, 16, 2);

static const char* ntpServer = "pool.ntp.org";
static const long  gmtOffset_sec = 7 * 3600; // Múi giờ Việt Nam (UTC+7)
static const int   daylightOffset_sec = 0;

static unsigned long lastLcdUpdate = 0;
static const unsigned long lcdInterval = 1000; // Cập nhật màn hình mỗi giây

void initLCD()
{
    // Khởi tạo LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Booting..");

    // Bắt đầu quá trình đồng bộ thời gian NTP (Yêu cầu đã kết nối WiFi trước đó)
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    Serial.println("LCD Initialized & NTP Configured");
    
    delay(1000);
    lcd.clear();
}

void lcdLoop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastLcdUpdate >= lcdInterval)
    {
        lastLcdUpdate = currentMillis;

        // ----- DÒNG 1: Hiển thị Ngày Giờ -----
        struct tm timeinfo;
        // getLocalTime() trả về true nếu lấy được thời gian
        if (getLocalTime(&timeinfo, 10)) 
        {
            char timeString[17];
            // Format: DD/MM/YY HH:MM (Vd: 14/06/26 19:30)
            snprintf(timeString, sizeof(timeString), "%02d/%02d/%02d %02d:%02d", 
                    timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year % 100,
                    timeinfo.tm_hour, timeinfo.tm_min);
            
            lcd.setCursor(0, 0);
            lcd.print(timeString);
            // Ghi đè khoảng trắng nếu chuỗi ngắn hơn 16 để tránh lỗi hiển thị đè chữ cũ
            for (int i = strlen(timeString); i < 16; i++) lcd.print(" ");
        }
        else
        {
            lcd.setCursor(0, 0);
            lcd.print("Syncing Time... ");
        }

        // ----- DÒNG 2: Hiển thị Nhiệt độ & Độ ẩm -----
        float t = getTemperature();
        float h = getHumidity();
        char sensorStr[17];
        
        if (t > 0 || h > 0) 
        {
            // Format: T:30.0C H:65.0%
            snprintf(sensorStr, sizeof(sensorStr), "T:%.1fC H:%.1f%%", t, h);
        }
        else
        {
            snprintf(sensorStr, sizeof(sensorStr), "Reading DHT...  ");
        }
        
        lcd.setCursor(0, 1);
        lcd.print(sensorStr);
        for (int i = strlen(sensorStr); i < 16; i++) lcd.print(" ");
    }
}
