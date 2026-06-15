#include <Arduino.h>

#include "config.h"
#include "rain_manager.h"
#include "mqtt_manager.h"
#include "i2c_master_manager.h"

static bool raining = false;
static bool windowOpen = false;
static unsigned long lastRainCheckTime = 0;
static const unsigned long rainCheckInterval = 500; // Kiểm tra cảm biến mưa mỗi 500ms

static bool waitingToAutoOpen = false;
static unsigned long dryStartTime = 0;
static const unsigned long autoOpenDelay = 5000; // Đợi 5 giây sau khi hết mưa mới mở chắn

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
    // open = true: Bật tấm chắn (che mưa)
    // open = false: Tắt tấm chắn (mở ra cho thoáng)

    if (!open && raining)
    {
        Serial.println("[Rain Manager] Warning: Cannot deactivate shield while it is raining!");
        publishWindowState("OPEN"); // Trả lại trạng thái BẬT trên HA
        return;
    }

    if (windowOpen != open)
    {
        windowOpen = open;
        Serial.print("Shield state changed to: ");
        Serial.println(open ? "ACTIVE (0 deg)" : "INACTIVE (90 deg)");

        // Gửi lệnh I2C điều khiển Servo cửa sổ sang Arduino Uno
        // (Đảo ngược góc: OPEN (không chắn) = 0 độ, CLOSE (chắn mưa) = 90 độ)
        sendI2CCommand(I2C_CMD_WINDOW, open ? 0 : 90);

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

            if (raining)
            {
                waitingToAutoOpen = false; // Hủy lệnh tự mở nếu bỗng nhiên mưa lại
                // Trời mưa -> Bật tấm chắn (Mức TRUE)
                if (!windowOpen)
                {
                    Serial.println("[Rain Manager] Rain detected! Shielding ON...");
                    setWindowOpenState(true);
                }
            }
            else
            {
                // Hết mưa -> Bắt đầu đếm ngược thời gian để tắt tấm chắn
                waitingToAutoOpen = true;
                dryStartTime = currentMillis;
                Serial.println("[Rain Manager] Rain stopped. Waiting 5 seconds before removing shield...");
            }
        }

        // Kiểm tra xem đã hết thời gian chờ (5 giây) để tự tắt chắn chưa
        if (!raining && waitingToAutoOpen)
        {
            if (currentMillis - dryStartTime >= autoOpenDelay)
            {
                waitingToAutoOpen = false;
                // Chỉ tắt chắn nếu như hiện tại đang bật (nếu user tự tắt rồi thì thôi)
                if (windowOpen)
                {
                    Serial.println("[Rain Manager] 5 seconds passed! Removing shield...");
                    setWindowOpenState(false);
                }
            }
        }
    }
}
