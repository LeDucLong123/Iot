#pragma once

#include <Arduino.h>

void initI2CMaster();
void sendI2CCommand(uint8_t cmd, uint8_t data);
