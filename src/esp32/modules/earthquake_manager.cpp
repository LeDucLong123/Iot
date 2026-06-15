#include <Arduino.h>
#include "earthquake_manager.h"
#include "mqtt_manager.h"

#define VIBRATION_SENSOR_PIN 32
#define BUZZER_PIN 33

static bool isQuaking = false;
static unsigned long quakeStartTime = 0;
static const unsigned long buzzerDuration = 5000; // Còi hú 5 giây

void initEarthquake()
{
    pinMode(VIBRATION_SENSOR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW); // Tắt còi ban đầu
    Serial.println("Earthquake Manager Initialized. Vibration=32, Buzzer=33");
}

void earthquakeLoop()
{
    // Đọc trạng thái cảm biến rung. (SW-420 thường xuất mức HIGH khi có rung).
    bool vibrationDetected = (digitalRead(VIBRATION_SENSOR_PIN) == HIGH);

    if (vibrationDetected)
    {
        if (!isQuaking)
        {
            isQuaking = true;
            Serial.println("[Earthquake] Vibration detected! Earthquake Alert!");
            publishEarthquakeState(true);
        }
        // Liên tục cập nhật thời gian rung lắc
        quakeStartTime = millis(); 
    }
    else
    {
        if (isQuaking)
        {
            // Nếu đã hết thời gian rung, chờ 5 giây mới hết cảnh báo
            if (millis() - quakeStartTime >= buzzerDuration)
            {
                isQuaking = false;
                Serial.println("[Earthquake] Vibration stopped. Alert cleared.");
                publishEarthquakeState(false);
            }
        }
    }
}

bool isQuakingState()
{
    return isQuaking;
}
