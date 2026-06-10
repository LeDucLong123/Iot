#include <WiFi.h>
#include <PubSubClient.h>

#define LED1_PIN 2
#define LED2_PIN 4

const char *ssid = "402 Van La";
const char *password = "0984378397";

const char *mqtt_server = "192.168.42.7";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
    String message;

    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Message: ");
    Serial.println(message);

    // Điều khiển LED 1
    if (String(topic) == "home/esp32/led1/set")
    {
        if (message == "ON")
        {
            digitalWrite(LED1_PIN, HIGH);
        }
        else if (message == "OFF")
        {
            digitalWrite(LED1_PIN, LOW);
        }
    }

    // Điều khiển LED 2
    else if (String(topic) == "home/esp32/led2/set")
    {
        if (message == "ON")
        {
            digitalWrite(LED2_PIN, HIGH);
        }
        else if (message == "OFF")
        {
            digitalWrite(LED2_PIN, LOW);
        }
    }
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.println("Connecting MQTT...");

        if (client.connect("ESP32_LED"))
        {
            Serial.println("Connected");

            // Đăng ký nhận lệnh cho cả 2 đèn
            client.subscribe("home/esp32/led1/set");
            client.subscribe("home/esp32/led2/set");

            Serial.println("Subscribed topics");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.println(client.state());

            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);

    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);

    WiFi.begin(ssid, password);

    Serial.print("Connecting WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();
}