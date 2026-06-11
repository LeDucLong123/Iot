#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"
#include "mqtt_manager.h"
#include "led_controller.h"
#include "rfid_servo_manager.h"
#include "i2c_master_manager.h"


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
            client.subscribe("home/esp32/servo/set");

            Serial.println("Subscribed topics");
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