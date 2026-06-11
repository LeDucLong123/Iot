#include <Arduino.h>
#include "i2c_slave_manager.h"

void setup()
{
    initI2CSlave();
}

void loop()
{
    i2cSlaveLoop();
}
