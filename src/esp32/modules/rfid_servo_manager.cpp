#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#include "config.h"
#include "rfid_servo_manager.h"
#include "mqtt_manager.h"
#include "i2c_master_manager.h"

MFRC522 mfrc522(RFID_SDA_PIN, RFID_RST_PIN);

static bool doorOpen = false;
static unsigned long doorOpenTime = 0;
static const unsigned long doorOpenDuration = 3000; // 3 giây tự động đóng cửa

void initRfidServo()
{
    // Khởi tạo SPI và MFRC522 RFID
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("MFRC522 initialized on ESP32");
}

void openDoor()
{
    if (!doorOpen)
    {
        // Gửi lệnh I2C mở cửa sang Arduino Uno (Servo quay 90 độ, bật LED Xanh)
        sendI2CCommand(I2C_CMD_SERVO, 90);
        doorOpen = true;
        doorOpenTime = millis();
        Serial.println("Sent Open Door command to Arduino");
        publishServoState("OPEN");
    }
}

void closeDoor()
{
    if (doorOpen)
    {
        // Gửi lệnh I2C đóng cửa sang Arduino Uno (Servo quay 0 độ, tắt LED Xanh)
        sendI2CCommand(I2C_CMD_SERVO, 0);
        doorOpen = false;
        Serial.println("Sent Close Door command to Arduino");
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
        openDoor();
        publishRfidLog(uidStr.c_str(), "authorized");
    }
    else
    {
        Serial.println("Access Denied");
        // Gửi lệnh I2C bật nháy LED Đỏ sang Arduino Uno
        sendI2CCommand(I2C_CMD_RFID_RED, 1);
        publishRfidLog(uidStr.c_str(), "unauthorized");
    }

    // Dừng đọc thẻ hiện tại để sẵn sàng đọc thẻ tiếp theo
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}
