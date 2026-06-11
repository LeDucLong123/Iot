# Hướng dẫn Hệ thống Khóa cửa RFID & Servo qua I2C (Không dùng LLC)

Tài liệu này hướng dẫn chi tiết cách hoạt động, cách đấu nối phần cứng trực tiếp (không dùng LLC, lắp thêm điện trở kéo lên ngoài) và cách sử dụng hệ thống I2C Master-Slave giữa ESP32 và Arduino Uno.

---

## 1. Sơ đồ đấu nối phần cứng trực tiếp (Không dùng LLC)

Để giao tiếp I2C trực tiếp giữa ESP32 (3.3V) và Arduino Uno (5V) một cách an toàn mà không cần mạch chuyển đổi mức logic LLC, chúng ta sử dụng phương pháp đấu nối kết hợp **2 điện trở kéo lên ngoài (pull-up resistors) 4.7kΩ hoặc 10kΩ về nguồn 3.3V**.

### Sơ đồ đi dây dạng danh sách kết nối:

#### A. Kết nối I2C giữa ESP32 (Master) và Arduino Uno (Slave)
*   **Chân SCL:** Chân **GPIO 22** của ESP32 nối trực tiếp vào Chân **A5** của Arduino Uno.
*   **Chân SDA:** Chân **GPIO 21** của ESP32 nối trực tiếp vào Chân **A4** của Arduino Uno.
*   **Chân GND (Bắt buộc):** Chân **GND** của ESP32 nối vào Chân **GND** của Arduino Uno.

#### B. Cách đấu nối 2 Điện trở kéo lên ngoài (Để sửa lỗi chập chờn / Timeout)
Chúng ta dùng 2 điện trở có trị số từ **4.7kΩ đến 10kΩ**:
*   **Điện trở 1 (cho đường SDA):**
    *   Một đầu chân điện trở nối vào đường dây **SDA** (chỗ nối giữa chân GPIO 21 của ESP32 và A4 của Arduino Uno).
    *   Đầu chân còn lại của điện trở nối vào chân **3.3V** của ESP32.
*   **Điện trở 2 (cho đường SCL):**
    *   Một đầu chân điện trở nối vào đường dây **SCL** (chỗ nối giữa chân GPIO 22 của ESP32 và A5 của Arduino Uno).
    *   Đầu chân còn lại của điện trở nối vào chân **3.3V** của ESP32.

```text
                  +3.3V (chân 3.3V trên ESP32)
                    |
            +-------+-------+
            |               |
          [10k]           [10k]   <-- 2 điện trở kéo lên (pull-up)
            |               |
SDA: GPIO 21 -------*-------|------------ Chân A4 (Arduino Uno)
                            |
SCL: GPIO 22 ---------------*------------ Chân A5 (Arduino Uno)


```text
GND: GND -------------------------------- GND (Nối chung đất 2 board)
```

> [!IMPORTANT]
> **Lưu ý về chiều dài dây I2C:**
> Giao thức I2C được thiết kế cho khoảng cách ngắn. Thực tế thử nghiệm cho thấy:
> *   Sử dụng dây dẫn dài (khoảng **60cm** trở lên) sẽ làm tăng điện dung đường dây, gây suy hao tín hiệu và dẫn đến chập chờn (lỗi Timeout hoặc không nhận diện được thiết bị).
> *   **Giải pháp:** Rút ngắn chiều dài dây dẫn (nên sử dụng dây cắm ngắn dưới **20cm**) để tín hiệu I2C truyền nhận ổn định và chính xác nhất.

---

### C. Thiết bị kết nối với ESP32 (Master)
*   **Đầu đọc RFID MFRC522**
    *   Chân VCC - Chân 3.3V của ESP32
    *   Chân GND - Chân GND của ESP32
    *   Chân SDA (SS) - Chân GPIO 5 của ESP32
    *   Chân SCK - Chân GPIO 18 của ESP32
    *   Chân MOSI - Chân GPIO 23 của ESP32
    *   Chân MISO - Chân GPIO 19 của ESP32
    *   Chân RST - Chân GPIO 14 của ESP32 *(đã chuyển từ chân 22 sang 14 để nhường SCL)*

*   **LED 1 (Cũ)**
    *   Cực dương (+) - Điện trở 220Ω - Chân GPIO 2 của ESP32
    *   Cực âm (-) - Chân GND của ESP32

---

### D. Thiết bị kết nối với Arduino Uno (Slave)
*   **Động cơ Servo SG90 (Điều khiển chốt cửa)**
    *   Dây Cam (Tín hiệu) - Chân D12 (Pin 12) của Arduino Uno
    *   Dây Đỏ (VCC) - Chân 5V của Arduino Uno
    *   Dây Nâu (GND) - Chân GND của Arduino Uno

*   **LED Phòng Khách (Cũ là LED 2)**
    *   Cực dương (+) - Điện trở 220Ω - Chân D7 (Pin 7) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno

*   **LED RFID Xanh (Báo quẹt thẻ đúng)**
    *   Cực dương (+) - Điện trở 220Ω - Chân D8 (Pin 8) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno

*   **LED RFID Đỏ (Báo quẹt thẻ sai)**
    *   Cực dương (+) - Điện trở 220Ω - Chân D9 (Pin 9) của Arduino Uno
    *   Cực âm (-) - Chân GND của Arduino Uno

*   **Còi báo/Loa (Báo quẹt thẻ sai)**
    *   Chân tín hiệu (+) - Chân D6 (Pin 6) của Arduino Uno
    *   Chân GND (-) - Chân GND của Arduino Uno

---

## 2. Giao thức điều khiển I2C

Địa chỉ I2C của Arduino Uno: `0x08`. Gói tin gồm 2 byte `[Command_ID, Data]`:

*   **Lệnh 0x01 (Điều khiển Servo cửa):**
    *   `[0x01, 90]`: Mở cửa (Servo quay 90° tại D12, LED Xanh sáng tại D8).
    *   `[0x01, 0]`: Đóng cửa (Servo quay 0° tại D12, LED Xanh tắt tại D8).
*   **Lệnh 0x02 (Cảnh báo thẻ lỗi):**
    *   `[0x02, 1]`: Bật nháy LED Đỏ tại D9 trong 3 giây và phát còi cảnh báo lỗi tại D6 trong 1 giây.
*   **Lệnh 0x03 (LED Phòng khách):**
    *   `[0x03, 1]`: Bật LED phòng khách tại D7.
    *   `[0x03, 0]`: Tắt LED phòng khách tại D7.

---

## 3. Cách vận hành hệ thống

1.  **Quét UID thẻ:** Quẹt thẻ lên đầu đọc RFID kết nối với ESP32, xem UID trên Serial Monitor (Baudrate `115200`) của ESP32.
2.  **Đăng ký UID:** Thêm thẻ vào danh sách `AUTHORIZED_UIDS` trong tệp `src/esp32/modules/config.cpp` của ESP32.
3.  **Điều khiển qua Home Assistant:** Sử dụng switch `LED Phong Khach` và `Cua Ra Vao (Servo)` trên giao diện HA để điều khiển từ xa.
