#pragma once

#include <PubSubClient.h>

void setupMQTT();
void mqttLoop();

void publishServoState(const char *state);
void publishRfidLog(const char *uid, const char *status);

