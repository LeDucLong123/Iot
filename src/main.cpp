#include <Arduino.h>

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "rfid_servo_manager.h"

void setup()
{
    Serial.begin(115200);

    initLeds();
    initRfidServo();

    connectWifi();

    setupMQTT();
}

void loop()
{
    mqttLoop();
    rfidServoLoop();
}