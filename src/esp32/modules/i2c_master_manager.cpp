#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "i2c_master_manager.h"

void initI2CMaster()
{
    // Mở cổng I2C Master với SDA = GPIO 21 và SCL = GPIO 22
    Wire.begin(21, 22);
    Wire.setClock(5000); // 100 kHz
    Serial.println("I2C Master initialized on SDA=21, SCL=22");
}

void sendI2CCommand(uint8_t cmd, uint8_t data)
{
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(cmd);
    Wire.write(data);
    uint8_t error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.printf("[I2C] Sent command 0x%02X with data %d successfully\n", cmd, data);
    }
    else
    {
        Serial.printf("[I2C] Failed to send command 0x%02X. Wire status: %d\n", cmd, error);
    }
}
