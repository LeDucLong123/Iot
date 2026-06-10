#include <Arduino.h>
#include <WiFi.h>

#include "config.h"

void connectWifi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}