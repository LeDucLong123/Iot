#include <Arduino.h>

#include "config.h"
#include "led_controller.h"

void initLeds()
{
    pinMode(LED1_PIN, OUTPUT);
    digitalWrite(LED1_PIN, LOW);
}

void setLed1(bool state)
{
    digitalWrite(LED1_PIN, state);
}