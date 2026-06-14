#include <Arduino.h>

#include "config.h"
#include "rain_manager.h"
#include "mqtt_manager.h"
#include "i2c_master_manager.h"

static bool raining = false;
static bool windowOpen = false;
static unsigned long lastRainCheckTime = 0;
static const unsigned long rainCheckInterval = 500; // Kiểm tra cảm biến mưa mỗi 500ms

void initRain()
{
    pinMode(RAIN_SENSOR_PIN, INPUT);
    // Đọc trạng thái mưa ban đầu (cảm biến tích cực mức THẤP - LOW khi có nước)
    raining = (digitalRead(RAIN_SENSOR_PIN) == LOW);
    Serial.printf("Rain sensor initialized. Initial state: %s\n", raining ? "Raining" : "Dry");
}

bool isRaining()
{
    return raining;
}

bool getWindowOpenState()
{
    return windowOpen;
}

void setWindowOpenState(bool open)
{
    if (open && raining)
    {
        Serial.println("[Rain Manager] Warning: Cannot open window while it is raining!");
        publishWindowState("CLOSE"); // Ép trạng thái nút nhấn trên HA về CLOSE
        return;
    }

    if (windowOpen != open)
    {
        windowOpen = open;
        Serial.print("Window state changed to: ");
        Serial.println(open ? "OPEN" : "CLOSE");

        // Gửi lệnh I2C điều khiển Servo cửa sổ sang Arduino Uno
        sendI2CCommand(I2C_CMD_WINDOW, open ? 90 : 0);

        // Phản hồi trạng thái lên Home Assistant
        publishWindowState(open ? "OPEN" : "CLOSE");
    }
}

void rainLoop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastRainCheckTime >= rainCheckInterval)
    {
        lastRainCheckTime = currentMillis;

        // Đọc giá trị số từ DO của cảm biến mưa (càng nhiều nước điện áp ra càng thấp -> LOW)
        bool isNowRaining = (digitalRead(RAIN_SENSOR_PIN) == LOW);

        if (isNowRaining != raining)
        {
            raining = isNowRaining;
            Serial.printf("[Rain Manager] Rain state changed. Is Raining: %s\n", raining ? "YES" : "NO");

            // Publish trạng thái mưa lên MQTT
            publishRainState(raining);

            // Nếu phát hiện trời mưa và cửa sổ đang mở -> Tiến hành tự động đóng cửa sổ
            if (raining && windowOpen)
            {
                Serial.println("[Rain Manager] Rain detected! Force closing the window...");
                setWindowOpenState(false);
            }
        }
    }
}
