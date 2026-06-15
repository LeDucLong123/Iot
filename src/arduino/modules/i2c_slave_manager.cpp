#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include "i2c_slave_manager.h"

// Định nghĩa các chân trên Arduino Uno
#define PIN_LED_PHONG_KHACH 7
#define PIN_LED_RFID_GREEN 8
#define PIN_LED_RFID_RED 9
#define PIN_SERVO 12
#define PIN_LED_BEP 6
#define PIN_LED_PHONG_NGU 5
#define PIN_LED_NHA_VE_SINH 4
#define PIN_SERVO_WINDOW 11



static Servo doorServo;
static Servo windowServo;

// Biến volatile dùng trong ngắt I2C
static volatile uint8_t nextCmd = 0;
static volatile uint8_t nextData = 0;
static volatile bool hasNewCommand = false;

// Hàm ISR khi nhận dữ liệu từ Master
void receiveEvent(int howMany)
{
    if (howMany >= 2)
    {
        nextCmd = Wire.read();
        nextData = Wire.read();
        hasNewCommand = true;
    }
    // Đọc bỏ các byte dư thừa nếu có
    while (Wire.available())
    {
        Wire.read();
    }
}

void initI2CSlave()
{
    // Cấu hình chân output
    pinMode(PIN_LED_PHONG_KHACH, OUTPUT);
    pinMode(PIN_LED_RFID_GREEN, OUTPUT);
    pinMode(PIN_LED_RFID_RED, OUTPUT);
    pinMode(PIN_LED_BEP, OUTPUT);
    pinMode(PIN_LED_PHONG_NGU, OUTPUT);
    pinMode(PIN_LED_NHA_VE_SINH, OUTPUT);

    digitalWrite(PIN_LED_PHONG_KHACH, LOW);
    digitalWrite(PIN_LED_RFID_GREEN, LOW);
    digitalWrite(PIN_LED_RFID_RED, LOW);
    digitalWrite(PIN_LED_BEP, LOW);
    digitalWrite(PIN_LED_PHONG_NGU, LOW);
    digitalWrite(PIN_LED_NHA_VE_SINH, LOW);

    // Khởi tạo Servo
    doorServo.attach(PIN_SERVO);
    doorServo.write(0); // Khóa ban đầu

    windowServo.attach(PIN_SERVO_WINDOW);
    windowServo.write(0); // Cửa sổ đóng ban đầu

    // Khởi tạo I2C Slave địa chỉ 0x08
    Wire.begin(0x08);

    // Vô hiệu hóa điện trở kéo lên nội bộ 5V trên A4 (SDA) và A5 (SCL) để chạy áp 3.3V an toàn
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    pinMode(SDA, INPUT);
    pinMode(SCL, INPUT);
#endif

    // Đăng ký callback nhận gói tin từ I2C Master
    Wire.onReceive(receiveEvent);

    Serial.begin(9600);
    Serial.println("Arduino Uno I2C Slave initialized on address 0x08");
}

void i2cSlaveLoop()
{
    static bool ledRedActive = false;
    static unsigned long ledRedTime = 0;
    const unsigned long ledRedDuration = 3000; // 3 giây tự động tắt LED đỏ

    // Xử lý lệnh mới từ ngắt I2C
    if (hasNewCommand)
    {
        // Copy dữ liệu trong vùng tranh chấp (Critical Section)
        noInterrupts();
        uint8_t cmd = nextCmd;
        uint8_t data = nextData;
        hasNewCommand = false;
        interrupts();

        Serial.print("Received I2C Command: 0x");
        Serial.print(cmd, HEX);
        Serial.print(" | Data: ");
        Serial.println(data);

        switch (cmd)
        {
        case 0x01: // Lệnh điều khiển Servo chốt cửa & LED Xanh
            if (data == 90)
            {
                doorServo.write(90);
                digitalWrite(PIN_LED_RFID_GREEN, HIGH);
                Serial.println("Servo rotated 90 degrees (Open), LED Green ON");
            }
            else if (data == 0)
            {
                doorServo.write(0);
                digitalWrite(PIN_LED_RFID_GREEN, LOW);
                Serial.println("Servo rotated 0 degrees (Close), LED Green OFF");
            }
            break;

        case 0x02: // Lệnh nháy LED Đỏ báo thẻ sai
            if (data == 1)
            {
                digitalWrite(PIN_LED_RFID_RED, HIGH);
                ledRedActive = true;
                ledRedTime = millis();
                Serial.println("LED Red ON for unauthorized card");
            }
            break;

        case 0x03: // Lệnh điều khiển LED Phòng Khách
            digitalWrite(PIN_LED_PHONG_KHACH, data == 1 ? HIGH : LOW);
            Serial.print("Living Room LED turned ");
            Serial.println(data == 1 ? "ON" : "OFF");
            break;

        case 0x06: // Lệnh điều khiển Đèn Bếp
            digitalWrite(PIN_LED_BEP, data == 1 ? HIGH : LOW);
            Serial.print("Kitchen LED turned ");
            Serial.println(data == 1 ? "ON" : "OFF");
            break;

        case 0x07: // Lệnh điều khiển Đèn Phòng Ngủ
            digitalWrite(PIN_LED_PHONG_NGU, data == 1 ? HIGH : LOW);
            Serial.print("Bedroom LED turned ");
            Serial.println(data == 1 ? "ON" : "OFF");
            break;

        case 0x08: // Lệnh điều khiển Đèn Nhà Vệ Sinh
            digitalWrite(PIN_LED_NHA_VE_SINH, data == 1 ? HIGH : LOW);
            Serial.print("Bathroom LED turned ");
            Serial.println(data == 1 ? "ON" : "OFF");
            break;

        case 0x05: // Lệnh điều khiển Servo cửa sổ
            windowServo.write(data);
            Serial.print("Window Servo rotated to ");
            Serial.print(data);
            Serial.println(" degrees");
            break;

        default:
            Serial.print("Unknown command: 0x");
            Serial.println(cmd, HEX);
            break;
        }
    }

    // Tự động tắt LED đỏ sau 3 giây
    if (ledRedActive && (millis() - ledRedTime >= ledRedDuration))
    {
        digitalWrite(PIN_LED_RFID_RED, LOW);
        ledRedActive = false;
        Serial.println("LED Red auto-turned OFF");
    }
}
