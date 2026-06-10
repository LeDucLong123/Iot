#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#include "config.h"
#include "rfid_servo_manager.h"
#include "mqtt_manager.h"

MFRC522 mfrc522(RFID_SDA_PIN, RFID_RST_PIN);
Servo myServo;

static bool doorOpen = false;
static unsigned long doorOpenTime = 0;
static const unsigned long doorOpenDuration = 3000; // 3 giây tự động đóng

static bool rfidRedActive = false;
static unsigned long rfidRedTime = 0;
static const unsigned long ledRedDuration = 3000; // 3 giây tắt LED đỏ

void initRfidServo()
{
    // Cấu hình chân LED RFID
    pinMode(LED_RFID_GREEN_PIN, OUTPUT);
    pinMode(LED_RFID_RED_PIN, OUTPUT);
    digitalWrite(LED_RFID_GREEN_PIN, LOW);
    digitalWrite(LED_RFID_RED_PIN, LOW);

    // Khởi tạo SPI và MFRC522 RFID
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("MFRC522 initialized");

    // Khởi tạo Servo
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myServo.setPeriodHertz(50); // Servo SG90 tiêu chuẩn chạy 50Hz
    myServo.attach(SERVO_PIN, 500, 2400); // Gán chân và khoảng xung rộng
    myServo.write(0); // Mặc định ở trạng thái khóa (0 độ)
    Serial.println("Servo initialized at 0 degrees");
}

void openDoor()
{
    if (!doorOpen)
    {
        myServo.write(90); // Quay servo 90 độ để mở
        doorOpen = true;
        doorOpenTime = millis();
        Serial.println("Door opened (90 degrees)");
        publishServoState("OPEN");
    }
}

void closeDoor()
{
    if (doorOpen)
    {
        myServo.write(0); // Quay servo về 0 độ để đóng
        doorOpen = false;
        Serial.println("Door closed (0 degrees)");
        publishServoState("CLOSE");
    }
}

bool isDoorOpen()
{
    return doorOpen;
}

void rfidServoLoop()
{
    // Tự động đóng cửa sau 3 giây
    if (doorOpen && (millis() - doorOpenTime >= doorOpenDuration))
    {
        closeDoor();
        digitalWrite(LED_RFID_GREEN_PIN, LOW); // Tắt đèn xanh báo hiệu
    }

    // Tự động tắt LED đỏ sau 3 giây
    if (rfidRedActive && (millis() - rfidRedTime >= ledRedDuration))
    {
        digitalWrite(LED_RFID_RED_PIN, LOW);
        rfidRedActive = false;
    }

    // Kiểm tra xem có thẻ RFID mới quẹt hay không
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    // Đọc UID của thẻ và chuyển đổi thành chuỗi định dạng HEX hoa
    String uidStr = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
            uidStr += "0";
        }
        uidStr += String(mfrc522.uid.uidByte[i], HEX);
        if (i < mfrc522.uid.size - 1)
        {
            uidStr += " ";
        }
    }
    uidStr.toUpperCase();

    Serial.print("RFID Card Scanned. UID: ");
    Serial.println(uidStr);

    // Kiểm tra tính hợp lệ của thẻ
    bool authorized = false;
    for (int i = 0; i < AUTHORIZED_UIDS_COUNT; i++)
    {
        if (uidStr == AUTHORIZED_UIDS[i])
        {
            authorized = true;
            break;
        }
    }

    if (authorized)
    {
        Serial.println("Access Authorized");
        digitalWrite(LED_RFID_GREEN_PIN, HIGH);
        digitalWrite(LED_RFID_RED_PIN, LOW); // Đảm bảo tắt LED đỏ
        rfidRedActive = false;

        openDoor();
        publishRfidLog(uidStr.c_str(), "authorized");
    }
    else
    {
        Serial.println("Access Denied");
        digitalWrite(LED_RFID_RED_PIN, HIGH);
        rfidRedActive = true;
        rfidRedTime = millis();

        publishRfidLog(uidStr.c_str(), "unauthorized");
    }

    // Dừng đọc thẻ hiện tại để sẵn sàng đọc thẻ tiếp theo
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}
