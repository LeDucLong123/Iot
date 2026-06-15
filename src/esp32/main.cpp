#include <Arduino.h>

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "rfid_servo_manager.h"
#include "i2c_master_manager.h"
#include "dht_manager.h"
#include "rain_manager.h"
#include "lcd_manager.h"
#include "radar_manager.h"
#include "earthquake_manager.h"
#include "gas_manager.h"

// Biến quản lý còi chung
#define BUZZER_PIN 33
void updateBuzzer()
{
    // Còi kêu khi có Động đất HOẶC rò rỉ Gas
    bool shouldBuzz = isQuakingState() || isGasLeakingState();
    digitalWrite(BUZZER_PIN, shouldBuzz ? HIGH : LOW);
}




void setup()
{
    Serial.begin(115200);

    initLeds();
    initI2CMaster();
    initRfidServo();
    initDHT();
    initRain();
    initRadar();
    initEarthquake();
    initGas();

    connectWifi();

    // Khởi tạo LCD sau khi kết nối WiFi để lấy giờ NTP
    initLCD();

    setupMQTT();
}

void loop()
{
    mqttLoop();
    rfidServoLoop();
    dhtLoop();
    rainLoop();
    lcdLoop();
    radarLoop();
    earthquakeLoop();
    gasLoop();

    // Cập nhật trạng thái còi báo động
    updateBuzzer();
}