# Hướng dẫn Hệ thống Cảm biến Nhiệt độ/Độ ẩm (DHT11) và Quạt giả lập (I2C)

Tài liệu này hướng dẫn chi tiết cách hoạt động, cách đấu nối phần cứng và cách sử dụng hệ thống đo nhiệt độ, độ ẩm tự động điều khiển quạt giả lập (LED D5) thông qua giao tiếp I2C giữa ESP32 (Master) và Arduino Uno (Slave), tích hợp giao diện quản lý trên Home Assistant.

---

## 1. Luồng hoạt động của hệ thống (Workflow)

Hệ thống hỗ trợ 2 chế độ điều khiển quạt làm mát độc lập: **Tự động (Auto Mode)** và **Thủ công (Manual Mode)**.

```mermaid
flowchart TD
    Start([Đọc cảm biến DHT11]) --> ReadVal[Lấy giá trị Nhiệt độ T]
    ReadVal --> Publish[Publish T và H lên MQTT]
    Publish --> CheckMode{Chế độ hoạt động?}
    
    %% Nhánh Tự động (Auto Mode)
    CheckMode -- Tự động (Auto) --> CheckTemp{Nhiệt độ T > 30°C ?}
    CheckTemp -- Đúng (>30°C) --> FanON[1. Gửi I2C [0x04, 1] bật LED quạt\n2. Publish trạng thái quạt ON về HA]
    CheckTemp -- Sai (<=30°C) --> FanOFF[1. Gửi I2C [0x04, 0] tắt LED quạt\n2. Publish trạng thái quạt OFF về HA]
    
    %% Nhánh Thủ công (Manual Mode)
    CheckMode -- Thủ công (Manual) --> Ignore[Bỏ qua nhiệt độ, chỉ bật/tắt quạt theo tin nhắn MQTT nhận từ HA]
```

*   **Chế độ Tự động (Auto Mode)**:
    *   ESP32 liên tục đọc cảm biến DHT11 mỗi 5 giây và gửi giá trị lên Home Assistant.
    *   Nếu nhiệt độ đo được **vượt quá 30.0°C**, ESP32 tự động gửi lệnh I2C `[0x04, 1]` yêu cầu Arduino Uno bật LED quạt (chân D5) và cập nhật trạng thái quạt trên HA thành `ON`.
    *   Khi nhiệt độ **hạ xuống hoặc bằng 30.0°C**, ESP32 tự động gửi lệnh I2C `[0x04, 0]` tắt LED quạt và cập nhật trạng thái quạt trên HA thành `OFF`.
*   **Chế độ Thủ công (Manual Mode)**:
    *   Hệ thống bỏ qua giá trị nhiệt độ để điều khiển quạt.
    *   Người dùng có thể bấm trực tiếp nút bấm trên giao diện Home Assistant để bật/tắt quạt giả lập thông qua gói tin I2C gửi từ ESP32 xuống Arduino Uno.

---

## 2. Hướng dẫn đấu nối phần cứng chi tiết (Thiết bị > Chân - Chân)

### A. Cảm biến nhiệt độ/độ ẩm DHT11 (Nối vào ESP32 - Master)
*   **Cảm biến DHT11**
    *   Chân VCC - Chân 3.3V của ESP32.
    *   Chân GND - Chân GND của ESP32.
    *   Chân DATA (Tín hiệu) - Chân **GPIO 15** của ESP32.
    *   *Lưu ý*: Nếu dùng cảm biến DHT11 dạng linh kiện 4 chân trần, cần mắc thêm 1 điện trở 4.7kΩ hoặc 10kΩ kéo từ chân DATA lên nguồn 3.3V. Nếu dùng module DHT11 3 chân hàn sẵn trên mạch nhỏ thì không cần cắm thêm trở này.

### B. LED giả lập quạt làm mát (Nối vào Arduino Uno - Slave)
*   **LED Giả Quạt**
    *   Cực dương (+) - Điện trở 220Ω - Chân **D5 (Pin 5)** của Arduino Uno.
    *   Cực âm (-) - Chân **GND** của Arduino Uno.

---

## 3. Giao thức điều khiển I2C mới

Gói tin điều khiển quạt gửi từ ESP32 -> Arduino Uno qua địa chỉ `0x08` gồm 2 byte `[0x04, Data]`:
*   `[0x04, 1]`: Bật quạt (Kích chân D5 của Arduino Uno lên HIGH).
*   `[0x04, 0]`: Tắt quạt (Kéo chân D5 của Arduino Uno xuống LOW).

---

## 4. Cấu hình thực thể trên Home Assistant

Thêm cấu hình sau vào tệp `configuration.yaml` để hiển thị dữ liệu cảm biến và các nút bấm điều khiển lên giao diện Home Assistant:

```yaml
mqtt:
  sensor:
    # Cảm biến đo nhiệt độ
    - name: "Nhiet Do"
      state_topic: "home/esp32/sensor/temperature"
      unit_of_measurement: "°C"
      device_class: "temperature"
      unique_id: esp32_temperature

    # Cảm biến đo độ ẩm
    - name: "Do Am"
      state_topic: "home/esp32/sensor/humidity"
      unit_of_measurement: "%"
      device_class: "humidity"
      unique_id: esp32_humidity

  switch:
    # Nút bấm bật/tắt quạt thủ công
    - name: "Quat Lam Mat"
      unique_id: esp32_fan_switch
      command_topic: "home/esp32/fan/set"
      state_topic: "home/esp32/fan/state"
      payload_on: "ON"
      payload_off: "OFF"
      icon: "mdi:fan"

    # Công tắc chọn chế độ (Gạt bật = Auto, Gạt tắt = Manual)
    - name: "Che Do Quat Tu Dong"
      unique_id: esp32_fan_mode_switch
      command_topic: "home/esp32/fan/mode/set"
      state_topic: "home/esp32/fan/mode/state"
      payload_on: "auto"
      payload_off: "manual"
      state_on: "auto"
      state_off: "manual"
      icon: "mdi:robot"
```

---

## 5. Hướng dẫn vận hành và kiểm tra nhanh

1.  **Chạy thử chế độ Tự động (Auto)**:
    *   Gạt công tắc **Che Do Quat Tu Dong** trên giao diện Home Assistant sang vị trí **Bật (Auto)**.
    *   Sử dụng bật lửa, máy sấy tóc hoặc thở hơi ấm vào cảm biến DHT11 để đẩy nhiệt độ vượt qua **30.0°C**.
    *   Đèn LED giả quạt (D5) trên Arduino Uno sẽ tự động sáng lên, đồng thời nút bấm **Quat Lam Mat** trên HA chuyển sang màu vàng (ON).
    *   Khi cảm biến nguội đi dưới **30.0°C**, LED quạt và nút bấm trên HA sẽ tự động tắt.
2.  **Chạy thử chế độ Thủ công (Manual)**:
    *   Gạt công tắc **Che Do Quat Tu Dong** trên giao diện Home Assistant sang vị trí **Tắt (Manual)**.
    *   Lúc này, nhiệt độ cảm biến thay đổi sẽ không tự động kích hoạt quạt nữa. Bạn có thể nhấn trực tiếp nút **Quat Lam Mat** trên giao diện Lovelace của HA để bật/tắt LED quạt chân D5 tùy ý.
