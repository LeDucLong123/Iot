#pragma once

void initDHT();
void dhtLoop();

float getTemperature();
float getHumidity();

bool isFanAutoMode();
void setFanAutoMode(bool autoMode);

bool getFanState();
void setFanState(bool state);
