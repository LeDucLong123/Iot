#include <Arduino.h>
#include <DHT.h>

#include "config.h"
#include "dht_manager.h"
#include "mqtt_manager.h"
#include "i2c_master_manager.h"

static DHT dht(DHT_PIN, DHT_TYPE);

static bool fanAutoMode = false;
static bool fanState = false;
static float temperature = 0.0;
static float humidity = 0.0;
static unsigned long lastDHTReadTime = 0;
static const unsigned long dhtInterval = 5000; // Đọc cảm biến mỗi 5 giây

const float TEMP_THRESHOLD = 30.0; // Ngưỡng nhiệt độ bật quạt

void initDHT()
{
    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(FAN_PIN, LOW); // Tắt quạt ban đầu
    dht.begin();
    Serial.println("DHT Sensor & Fan initialized");
}

float getTemperature()
{
    return temperature;
}

float getHumidity()
{
    return humidity;
}

bool isFanAutoMode()
{
    return fanAutoMode;
}

void setFanAutoMode(bool autoMode)
{
    if (fanAutoMode != autoMode)
    {
        fanAutoMode = autoMode;
        Serial.print("Fan mode changed to: ");
        Serial.println(autoMode ? "Auto" : "Manual");
        publishFanModeState(autoMode ? "auto" : "manual");
    }
}

bool getFanState()
{
    return fanState;
}

void setFanState(bool state)
{
    if (fanState != state)
    {
        fanState = state;
        Serial.print("Fan state changed to: ");
        Serial.println(state ? "ON" : "OFF");

        // Điều khiển chân D12 của ESP32 để bật/tắt Relay hoặc Transistor nối với quạt
        digitalWrite(FAN_PIN, state ? HIGH : LOW);

        // Phản hồi trạng thái lên Home Assistant
        publishFanState(state ? "ON" : "OFF");
    }
}

void dhtLoop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastDHTReadTime >= dhtInterval)
    {
        lastDHTReadTime = currentMillis;

        float t = dht.readTemperature();
        float h = dht.readHumidity();

        if (isnan(t) || isnan(h))
        {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }

        temperature = t;
        humidity = h;

        Serial.printf("DHT11 - Temp: %.1f °C, Hum: %.1f %%\n", t, h);

        // Publish dữ liệu lên MQTT
        publishTemperature(t);
        publishHumidity(h);

        // Logic tự động bật/tắt quạt dựa trên nhiệt độ
        if (fanAutoMode)
        {
            if (t > TEMP_THRESHOLD)
            {
                if (!fanState)
                {
                    Serial.println("[Auto Mode] Temperature high, turning fan ON");
                    setFanState(true);
                }
            }
            else
            {
                if (fanState)
                {
                    Serial.println("[Auto Mode] Temperature normal, turning fan OFF");
                    setFanState(false);
                }
            }
        }
    }
}
