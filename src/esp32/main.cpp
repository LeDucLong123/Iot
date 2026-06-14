#include <Arduino.h>

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "rfid_servo_manager.h"
#include "i2c_master_manager.h"
#include "dht_manager.h"
#include "rain_manager.h"




void setup()
{
    Serial.begin(115200);

    initLeds();
    initI2CMaster();
    initRfidServo();
    initDHT();
    initRain();

    connectWifi();

    setupMQTT();
}

void loop()
{
    mqttLoop();
    rfidServoLoop();
    dhtLoop();
    rainLoop();
}