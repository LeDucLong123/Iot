#include "config.h"

const char *WIFI_SSID = "402 Van La";
const char *WIFI_PASSWORD = "0984378397";

const char *MQTT_SERVER = "192.168.42.7";
const int MQTT_PORT = 1883;

// Danh sách các UID thẻ hợp lệ (dạng viết hoa, cách nhau bởi dấu cách)
const char *AUTHORIZED_UIDS[] = {
    "12 34 56 78",
    "AB CD EF 12",
    "70 A6 AC 55",
    "E0 58 58 9B"};
const int AUTHORIZED_UIDS_COUNT = sizeof(AUTHORIZED_UIDS) / sizeof(AUTHORIZED_UIDS[0]);
