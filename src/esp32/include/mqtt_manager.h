#pragma once

#include <PubSubClient.h>

void setupMQTT();
void mqttLoop();

void publishServoState(const char *state);
void publishRfidLog(const char *uid, const char *status);

void publishTemperature(float temp);
void publishHumidity(float hum);
void publishFanState(const char *state);
void publishFanModeState(const char *mode);

void publishRainState(bool raining);
void publishWindowState(const char *state);
void publishBathroomLightState(bool state);
void publishEarthquakeState(bool isQuake);
void publishGasState(bool isLeaking);



