#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "rfid_servo_manager.h"
#include "i2c_master_manager.h"
#include "dht_manager.h"
#include "rain_manager.h"




WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
    String message;

    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Message: ");
    Serial.println(message);

    if (String(topic) == "home/esp32/led1/set")
    {
        setLed1(message == "ON");
    }
    else if (String(topic) == "home/esp32/led_phong_khach/set")
    {
        sendI2CCommand(I2C_CMD_PHONG_KHACH, message == "ON" ? 1 : 0);
    }
    else if (String(topic) == "home/esp32/led_bep/set")
    {
        sendI2CCommand(I2C_CMD_BEP, message == "ON" ? 1 : 0);
    }
    else if (String(topic) == "home/esp32/led_phong_ngu/set")
    {
        sendI2CCommand(I2C_CMD_PHONG_NGU, message == "ON" ? 1 : 0);
    }
    else if (String(topic) == "home/esp32/led_nha_ve_sinh/set")
    {
        bool turnOn = (message == "ON");
        sendI2CCommand(I2C_CMD_NHA_VE_SINH, turnOn ? 1 : 0);
        publishBathroomLightState(turnOn);
    }
    else if (String(topic) == "home/esp32/servo/set")
    {
        if (message == "OPEN")
        {
            openDoor();
        }
        else if (message == "CLOSE")
        {
            closeDoor();
        }
    }
    else if (String(topic) == "home/esp32/fan/mode/set")
    {
        setFanAutoMode(message == "auto");
    }
    else if (String(topic) == "home/esp32/fan/set")
    {
        if (!isFanAutoMode())
        {
            setFanState(message == "ON");
        }
    }
    else if (String(topic) == "home/esp32/window/set")
    {
        setWindowOpenState(message == "OPEN");
    }
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.println("Connecting MQTT...");

        if (client.connect("ESP32_LED"))
        {
            Serial.println("Connected");

            client.subscribe("home/esp32/led1/set");
            client.subscribe("home/esp32/led_phong_khach/set");
            client.subscribe("home/esp32/led_bep/set");
            client.subscribe("home/esp32/led_phong_ngu/set");
            client.subscribe("home/esp32/led_nha_ve_sinh/set");
            client.subscribe("home/esp32/servo/set");
            client.subscribe("home/esp32/fan/mode/set");
            client.subscribe("home/esp32/fan/set");
            client.subscribe("home/esp32/window/set");

            // Phản hồi đồng bộ trạng thái ban đầu lên HA
            client.publish("home/esp32/fan/mode/state", isFanAutoMode() ? "auto" : "manual", true);
            client.publish("home/esp32/fan/state", getFanState() ? "ON" : "OFF", true);
            client.publish("home/esp32/window/state", getWindowOpenState() ? "OPEN" : "CLOSE", true);
            client.publish("home/esp32/sensor/rain", isRaining() ? "ON" : "OFF", true);

            Serial.println("Subscribed topics & Synced states");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.println(client.state());

            delay(5000);
        }
    }
}

void setupMQTT()
{
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
}

void mqttLoop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();
}

void publishServoState(const char *state)
{
    if (client.connected())
    {
        client.publish("home/esp32/servo/state", state, true);
    }
}

void publishRfidLog(const char *uid, const char *status)
{
    if (client.connected())
    {
        String jsonPayload = "{\"uid\":\"" + String(uid) + "\",\"status\":\"" + String(status) + "\"}";
        client.publish("home/esp32/rfid/log", jsonPayload.c_str());
    }
}

void publishTemperature(float temp)
{
    if (client.connected())
    {
        client.publish("home/esp32/sensor/temperature", String(temp, 1).c_str(), true);
    }
}

void publishHumidity(float hum)
{
    if (client.connected())
    {
        client.publish("home/esp32/sensor/humidity", String(hum, 1).c_str(), true);
    }
}

void publishFanState(const char *state)
{
    if (client.connected())
    {
        client.publish("home/esp32/fan/state", state, true);
    }
}

void publishFanModeState(const char *mode)
{
    if (client.connected())
    {
        client.publish("home/esp32/fan/mode/state", mode, true);
    }
}

void publishRainState(bool raining)
{
    if (client.connected())
    {
        client.publish("home/esp32/sensor/rain", raining ? "ON" : "OFF", true);
    }
}

void publishWindowState(const char *state)
{
    if (client.connected())
    {
        client.publish("home/esp32/window/state", state, true);
    }
}

void publishBathroomLightState(bool state)
{
    if (client.connected())
    {
        client.publish("home/esp32/led_nha_ve_sinh/state", state ? "ON" : "OFF", true);
    }
}

void publishEarthquakeState(bool isQuake)
{
    if (client.connected())
    {
        client.publish("home/esp32/sensor/earthquake", isQuake ? "ON" : "OFF", true);
    }
}

void publishGasState(bool isLeaking)
{
    if (client.connected())
    {
        client.publish("home/esp32/sensor/gas", isLeaking ? "ON" : "OFF", true);
    }
}