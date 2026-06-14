# Hướng dẫn Hệ thống Cảm biến Mưa (LM393) và Tự động Đóng Cửa sổ (Servo D11)

Tài liệu này hướng dẫn chi tiết cách hoạt động, cách đấu nối phần cứng và cách sử dụng hệ thống tự động đóng cửa sổ khi trời mưa thông qua giao tiếp I2C giữa ESP32 (Master) và Arduino Uno (Slave), kết hợp điều khiển và giám sát trên giao diện Home Assistant.

---

## 1. Luồng hoạt động của hệ thống (Workflow)

Hệ thống hoạt động dựa trên sự phối hợp giữa cảm biến đo nước mưa và động cơ servo giả lập cơ chế đóng/mở cánh cửa sổ.

```mermaid
flowchart TD
    Start([Đọc cảm biến mưa GPIO 27]) --> CheckRain{Có mưa? (Chân DO = LOW)}
    
    %% Trời Mưa
    CheckRain -- Có mưa (LOW) --> RainAction[1. Gửi MQTT 'ON' báo mưa\n2. Kiểm tra nếu Cửa sổ đang mở]
    RainAction --> CheckWindowOpen{Cửa sổ đang mở?}
    CheckWindowOpen -- Đúng (OPEN) --> CloseWindow[1. Gửi lệnh I2C [0x05, 0] đóng Servo D11\n2. Gửi MQTT 'CLOSE' trạng thái cửa sổ lên HA]
    CheckWindowOpen -- Sai (CLOSE) --> Loop
    
    %% Trời Không Mưa
    CheckRain -- Không mưa (HIGH) --> DryAction[1. Gửi MQTT 'OFF' báo tạnh mưa\n2. Cho phép điều khiển cửa sổ bình thường]
    DryAction --> Loop
```

### Chi tiết logic vận hành:
*   **Khi trời khô ráo (Không mưa)**:
    *   Cảm biến mưa xuất tín hiệu mức cao (HIGH) về ESP32.
    *   Người dùng toàn quyền điều khiển cửa sổ mở (quay 90 độ) hoặc đóng (quay 0 độ) thông qua nút bấm trên giao diện Home Assistant.
*   **Khi trời đổ mưa**:
    *   Cảm biến mưa dính nước sẽ lập tức xuất tín hiệu mức thấp (LOW) về ESP32.
    *   ESP32 nhận tín hiệu sẽ tự động phát lệnh qua I2C (`[0x05, 0]`) yêu cầu Arduino Uno quay Servo cửa sổ (chân D11) về **0 độ** để khép cửa.
    *   Trạng thái công tắc cửa sổ trên Home Assistant tự động chuyển về `CLOSE` và trạng thái cảm biến mưa chuyển sang hiển thị cảnh báo `Wet` (Ướt).
    *   **Logic an toàn (Safety Lock)**: Trong suốt thời gian trời mưa, nếu người dùng cố tình nhấn mở cửa sổ trên Home Assistant, ESP32 sẽ lập tức **từ chối lệnh**, tự động khép cửa sổ lại và đồng bộ trạng thái về `CLOSE` để bảo vệ tài sản trong nhà.
    *   *Lưu ý*: Cửa chính (nối chân D12 của Arduino Uno) hoàn toàn độc lập và **vẫn mở/đóng bình thường** để gia chủ có thể ra vào nhà.

---

## 2. Hướng dẫn đấu nối phần cứng chi tiết (Thiết bị > Chân - Chân)

### A. Cảm biến mưa LM393 (Nối vào ESP32 - Master)
*   **Cảm biến mưa**
    *   Chân VCC - Chân 3.3V của ESP32.
    *   Chân GND - Chân GND của ESP32.
    *   Chân DO (Digital Output) - Chân **GPIO 27** của ESP32.
    *   *(Bỏ trống chân AO - Analog Output nếu bạn chỉ cần phát hiện có mưa/không mưa bằng tín hiệu số).*

### B. Servo điều khiển cửa sổ (Nối vào Arduino Uno - Slave)
*   **Động cơ Servo cửa sổ (Simulated Window)**
    *   Dây Cam (Tín hiệu) - Chân **D11 (Pin 11)** của Arduino Uno.
    *   Dây Đỏ (VCC) - Chân **5V** của Arduino Uno.
    *   Dây Nâu (GND) - Chân **GND** của Arduino Uno.

---

## 3. Giao thức điều khiển I2C

Gói tin điều khiển Servo cửa sổ gửi từ ESP32 -> Arduino Uno qua địa chỉ `0x08` gồm 2 byte `[0x05, Data]`:
*   `[0x05, 90]`: Mở cửa sổ (Quay Servo ở D11 sang góc 90 độ).
*   `[0x05, 0]`: Đóng cửa sổ (Quay Servo ở D11 sang góc 0 độ).

---

## 4. Cấu hình thực thể trên Home Assistant

Để hiển thị trạng thái cảm biến mưa và nút bấm điều khiển cửa sổ, hãy thêm cấu hình sau vào tệp `configuration.yaml`:

```yaml
mqtt:
  # Cảm biến trạng thái mưa (ON = Mưa, OFF = Không mưa)
  binary_sensor:
    - name: "Cam Bien Mua"
      state_topic: "home/esp32/sensor/rain"
      payload_on: "ON"
      payload_off: "OFF"
      device_class: "moisture" # Hiển thị dạng Wet/Dry (Ướt/Khô) trên HA
      unique_id: esp32_rain_sensor

  # Nút bấm điều khiển đóng mở cửa sổ
  switch:
    - name: "Cua So (Servo)"
      unique_id: window_servo_lock
      command_topic: "home/esp32/window/set"
      state_topic: "home/esp32/window/state"
      payload_on: "OPEN"
      payload_off: "CLOSE"
      state_on: "OPEN"
      state_off: "CLOSE"
      icon: "mdi:window-closed"
```

---

## 5. Hướng dẫn kiểm tra và chạy thử

1.  **Chạy thử khi trời khô ráo**:
    *   Đảm bảo bề mặt cảm biến mưa khô ráo.
    *   Bật/tắt công tắc **Cua So (Servo)** trên màn hình Home Assistant.
    *   Kiểm tra xem Servo cửa sổ nối ở chân **D11** của Uno có quay tương ứng góc 90° (Mở) và 0° (Đóng) hay không.
2.  **Chạy thử khi trời mưa**:
    *   Mở cửa sổ bằng cách bật công tắc trên HA sang `OPEN` (Servo quay 90°).
    *   Nhỏ 1-2 giọt nước lên tấm điện cực của cảm biến mưa.
    *   Quan sát: 
        *   Cảm biến trên HA lập tức đổi sang trạng thái cảnh báo **Wet (Ướt)**.
        *   Động cơ Servo cửa sổ chân **D11** tự động quay về **0°** (Đóng cửa).
        *   Công tắc **Cua So (Servo)** trên HA tự động chuyển về trạng thái **CLOSE**.
    *   Thử bấm công tắc trên HA sang `OPEN` -> Nút bấm sẽ ngay lập tức tự động gạt về `CLOSE` và cửa sổ vẫn đóng chặt.
    *   Lau sạch nước trên cảm biến để đưa hệ thống về trạng thái bình thường.
