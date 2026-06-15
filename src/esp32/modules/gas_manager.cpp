#include <Arduino.h>
#include "config.h"
#include "gas_manager.h"
#include "mqtt_manager.h"

static bool isLeaking = false;

void initGas()
{
    pinMode(GAS_SENSOR_PIN, INPUT);
    Serial.println("Gas Manager Initialized. Sensor=4");
}

void gasLoop()
{
    // Đọc trạng thái cảm biến Gas (MQ-2/MQ-5 thường xuất mức LOW khi có khí gas)
    bool gasDetected = (digitalRead(GAS_SENSOR_PIN) == LOW);

    if (gasDetected)
    {
        if (!isLeaking)
        {
            isLeaking = true;
            Serial.println("[Gas Manager] Gas detected! DANGER!");
            publishGasState(true);
        }
    }
    else
    {
        if (isLeaking)
        {
            isLeaking = false;
            Serial.println("[Gas Manager] Gas cleared. Safe.");
            publishGasState(false);
        }
    }
}

bool isGasLeakingState()
{
    return isLeaking;
}
