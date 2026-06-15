#pragma once

#define LED1_PIN 2

// Cấu hình I2C
#define I2C_SLAVE_ADDR 0x08

#define I2C_CMD_SERVO 0x01
#define I2C_CMD_RFID_RED 0x02
#define I2C_CMD_PHONG_KHACH 0x03
#define I2C_CMD_WINDOW 0x05
#define I2C_CMD_BEP 0x06
#define I2C_CMD_PHONG_NGU 0x07
#define I2C_CMD_NHA_VE_SINH 0x08

#define DHT_PIN 15
#define DHT_TYPE DHT11
#define FAN_PIN 12
#define RAIN_SENSOR_PIN 27
#define GAS_SENSOR_PIN 4

#define RFID_SDA_PIN 5
#define RFID_RST_PIN 14

extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;

extern const char *MQTT_SERVER;
extern const int MQTT_PORT;

extern const char *AUTHORIZED_UIDS[];
extern const int AUTHORIZED_UIDS_COUNT;

