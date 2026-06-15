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

*   **Màn hình LCD I2C (1602 hoặc 2004)**
    *   Chân GND - Chân GND của ESP32 (hoặc cắm vào Breadboard chung)
    *   Chân VCC - Chân 5V hoặc VIN của ESP32 (Cần 5V để màn hình sáng)
    *   Chân SDA - Chân GPIO 21 của ESP32 (Cắm chung với đường SDA sang Arduino)
    *   Chân SCL - Chân GPIO 22 của ESP32 (Cắm chung với đường SCL sang Arduino)

*   **Cảm biến siêu âm HC-SR04 (Radar Nhà Vệ Sinh)**
    *   Chân VCC - Chân VIN (5V) của ESP32
    *   Chân GND - Chân GND của ESP32
    *   Chân TRIG - Chân GPIO 25 của ESP32
    *   Chân ECHO - Chân GPIO 26 của ESP32

*   **Cảm biến Rung (Cảnh báo động đất)**
    *   Chân VCC - Chân 3V3 (hoặc 5V tùy module) của ESP32
    *   Chân GND - Chân GND của ESP32
    *   Chân DO - Chân GPIO 32 của ESP32

*   **Cảm biến Khí Gas (Cảnh báo rò rỉ - Dùng chung còi)**
    *   Chân VCC - Chân VIN (5V) của ESP32 (MQ cần nguồn 5V để làm nóng màng cảm biến)
    *   Chân GND - Chân GND của ESP32
    *   Chân DO - Chân **GPIO 4 (D4)** của ESP32

*   **Còi Báo Động (Active Buzzer)**
    *   Chân VCC (+) - Chân GPIO 33 của ESP32
    *   Chân GND (-) - Chân GND của ESP32

*   **Quạt Làm Mát (Qua Module L298N)**
    *   Chân Tín hiệu (IN1 / INA) - Chân **GPIO 12 (D12)** của ESP32. (Chân IN2/INB để hở hoặc nối GND).
    *   Chân Nguồn - Lấy nguồn rời cấp vào cổng 12V/5V của L298N, chân GND nối chung với GND của mạch.
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
