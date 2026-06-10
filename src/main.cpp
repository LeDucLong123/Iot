#include <Arduino.h>

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "led_controller.h"

void setup()
{
    Serial.begin(115200);

    initLeds();

    connectWifi();

    setupMQTT();
}

void loop()
{
    mqttLoop();
}