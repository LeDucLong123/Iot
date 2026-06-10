#pragma once

#define LED1_PIN 2
#define LED2_PIN 4

#define LED_RFID_GREEN_PIN 25
#define LED_RFID_RED_PIN 26
#define SERVO_PIN 13
#define RFID_SDA_PIN 5
#define RFID_RST_PIN 22

extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;

extern const char *MQTT_SERVER;
extern const int MQTT_PORT;

extern const char *AUTHORIZED_UIDS[];
extern const int AUTHORIZED_UIDS_COUNT;
