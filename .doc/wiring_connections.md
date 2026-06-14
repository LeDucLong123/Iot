# Hướng dẫn Đấu nối Chân Thiết bị (Wiring Connections Reference)

Tài liệu này tổng hợp toàn bộ các kết nối chân giữa ESP32, Arduino Uno và các thiết bị ngoại vi trong dự án. Định dạng mô tả theo kiểu: **Thiết bị > [Chân thiết bị] - [Chân trên ESP32 hoặc Arduino]**.

---

## 1. Giao tiếp I2C giữa ESP32 và Arduino Uno (Kết nối trực tiếp, không dùng LLC)

*   **Đường truyền I2C (Bắt buộc nối chung chân GND)**
    *   Chân SCL - Chân GPIO 22 của ESP32 - Chân A5 của Arduino Uno
    *   Chân SDA - Chân GPIO 21 của ESP32 - Chân A4 của Arduino Uno
    *   Chân GND - Chân GND của ESP32 - Chân GND của Arduino Uno

*   **Điện trở kéo lên ngoài (2 điện trở 4.7kΩ hoặc 10kΩ)**
    *   Điện trở 1 (SDA Pull-up) - Một đầu nối vào đường SDA (chỗ nối GPIO 21 và A4) - Đầu còn lại nối vào chân 3.3V của ESP32
    *   Điện trở 2 (SCL Pull-up) - Một đầu nối vào đường SCL (chỗ nối GPIO 22 và A5) - Đầu còn lại nối vào chân 3.3V của ESP32

---

## 2. Các thiết bị nối vào ESP32 (Master)

*   **Đầu đọc RFID MFRC522**
    *   Chân VCC - Chân 3.3V của ESP32
    *   Chân GND - Chân GND của ESP32
    *   Chân SDA (SS) - Chân GPIO 5 của ESP32
    *   Chân SCK - Chân GPIO 18 của ESP32
    *   Chân MOSI - Chân GPIO 23 của ESP32
    *   Chân MISO - Chân GPIO 19 của ESP32
    *   Chân RST - Chân GPIO 14 của ESP32

*   **Cảm biến Nhiệt độ / Độ ẩm DHT11**
    *   Chân VCC - Chân 3.3V của ESP32
    *   Chân GND - Chân GND của ESP32
    *   Chân DATA (Tín hiệu) - Chân GPIO 15 của ESP32

*   **Cảm biến Mưa LM393**
    *   Chân VCC - Chân 3.3V của ESP32
    *   Chân GND - Chân GND của ESP32
    *   Chân DO (Digital Output) - Chân GPIO 27 của ESP32

*   **Đèn LED 1 (Báo trạng thái trên board ESP32)**
    *   Cực dương (+) - Điện trở 220Ω - Chân GPIO 2 của ESP32
    *   Cực âm (-) - Chân GND của ESP32

---

## 3. Các thiết bị nối vào Arduino Uno (Slave)

*   **Động cơ Servo Cửa chính (Main Door)**
    *   Dây Cam (Tín hiệu) - Chân D12 (Pin 12) của Arduino Uno
    *   Dây Đỏ (VCC) - Chân 5V của Arduino Uno
    *   Dây Nâu (GND) - Chân GND của Arduino Uno

*   **Động cơ Servo Cửa sổ (Window)**
    *   Dây Cam (Tín hiệu) - Chân D11 (Pin 11) của Arduino Uno
    *   Dây Đỏ (VCC) - Chân 5V của Arduino Uno
    *   Dây Nâu (GND) - Chân GND của Arduino Uno

*   **Đèn LED Phòng Khách (Cũ là LED 2)**
    *   Cực dương (+) - Điện trở 220Ω - Chân D7 (Pin 7) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno

*   **Đèn LED RFID Xanh (Báo quẹt thẻ đúng)**
    *   Cực dương (+) - Điện trở 220Ω - Chân D8 (Pin 8) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno

*   **Đèn LED RFID Đỏ (Báo quẹt thẻ sai)**
    *   Cực dương (+) - Điện trở 220Ω - Chân D9 (Pin 9) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno

*   **Còi báo / Loa (Buzzer báo quẹt thẻ sai)**
    *   Chân tín hiệu (+) - Chân D6 (Pin 6) của Arduino Uno
    *   Chân GND (-) - Chân GND của Arduino Uno

*   **Đèn LED Quạt giả lập**
    *   Cực dương (+) - Điện trở 220Ω - Chân D5 (Pin 5) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno
