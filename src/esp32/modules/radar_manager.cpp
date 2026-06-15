#include <Arduino.h>
#include "radar_manager.h"
#include "config.h"
#include "i2c_master_manager.h"
#include "mqtt_manager.h"

#define TRIG_PIN 25
#define ECHO_PIN 26

static unsigned long lastRadarCheck = 0;
static const unsigned long radarInterval = 500; // Kiểm tra mỗi 500ms

static bool isBathroomOccupied = false;
static unsigned long unoccupiedStartTime = 0;
static const unsigned long turnOffDelay = 5000; // Độ trễ 5 giây trước khi tắt đèn

void initRadar()
{
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    Serial.println("Radar HC-SR04 Initialized on Trig=25, Echo=26");
}

void radarLoop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastRadarCheck >= radarInterval)
    {
        lastRadarCheck = currentMillis;

        // Phát xung siêu âm
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        // Đọc thời gian phản hồi của xung (timeout 30ms ~ 5 mét)
        long duration = pulseIn(ECHO_PIN, HIGH, 30000);

        // Tính khoảng cách (cm). Tốc độ âm thanh = 0.0343 cm/us
        float distance = (duration / 2.0) * 0.0343;

        bool personDetected = false;

        if (duration > 0 && distance < 10.0) 
        {
            personDetected = true;
        }

        if (personDetected)
        {
            if (!isBathroomOccupied)
            {
                isBathroomOccupied = true;
                Serial.printf("Radar: Person detected (%.1f cm). Turning ON bathroom light.\n", distance);
                // Bật đèn qua I2C
                sendI2CCommand(I2C_CMD_NHA_VE_SINH, 1);
                // Báo cáo trạng thái lên MQTT
                publishBathroomLightState(true);
            }
            unoccupiedStartTime = 0; // Reset bộ đếm tắt đèn
        }
        else
        {
            if (isBathroomOccupied)
            {
                if (unoccupiedStartTime == 0)
                {
                    unoccupiedStartTime = currentMillis;
                }
                else if (currentMillis - unoccupiedStartTime >= turnOffDelay)
                {
                    isBathroomOccupied = false;
                    Serial.println("Radar: Person left. Turning OFF bathroom light.");
                    // Tắt đèn qua I2C
                    sendI2CCommand(I2C_CMD_NHA_VE_SINH, 0);
                    // Báo cáo trạng thái lên MQTT
                    publishBathroomLightState(false);
                }
            }
        }
    }
}
