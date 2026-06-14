#pragma once

#define LED1_PIN 2

// Cấu hình I2C
#define I2C_SLAVE_ADDR 0x08

// Các mã lệnh I2C gửi xuống Arduino Uno
#define I2C_CMD_SERVO 0x01
#define I2C_CMD_RFID_RED 0x02
#define I2C_CMD_PHONG_KHACH 0x03
#define I2C_CMD_FAN 0x04
#define I2C_CMD_WINDOW 0x05

#define DHT_PIN 15
#define DHT_TYPE DHT11
#define RAIN_SENSOR_PIN 27

#define RFID_SDA_PIN 5
#define RFID_RST_PIN 14

extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;

extern const char *MQTT_SERVER;
extern const int MQTT_PORT;

extern const char *AUTHORIZED_UIDS[];
extern const int AUTHORIZED_UIDS_COUNT;

