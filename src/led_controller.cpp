#include <Arduino.h>

#include "config.h"
#include "led_controller.h"

void initLeds()
{
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
}

void setLed1(bool state)
{
    digitalWrite(LED1_PIN, state);
}

void setLed2(bool state)
{
    digitalWrite(LED2_PIN, state);
}