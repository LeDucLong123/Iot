#include <Arduino.h>

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "rfid_servo_manager.h"
#include "i2c_master_manager.h"


void setup()
{
    Serial.begin(115200);

    initLeds();
    initI2CMaster();
    initRfidServo();

    connectWifi();

    setupMQTT();
}

void loop()
{
    mqttLoop();
    rfidServoLoop();
}