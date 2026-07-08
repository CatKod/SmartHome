# SmartHome IoT - HỆ THỐNG NHÀ THÔNG MINH KẾT HỢP HUB TRUNG TÂM

Hệ thống Nhà thông minh (Smart Home IoT) là một giải pháp toàn diện phục vụ việc giám sát môi trường, bảo mật an ninh và tự động hóa thiết bị. Dự án sử dụng vi điều khiển **ESP32** làm các Node thu thập/điều khiển thiết bị ngoại vi qua Wi-Fi và vi điều khiển **STM32F429I** đóng vai trò là một **Smart Hub trung tâm** để quản lý, hiển thị và xử lý cục bộ.

---

## TÍNH NĂNG CHÍNH VÀ HỆ THỐNG CẢM BIẾN

Hệ thống được thiết kế theo dạng module, cho phép tích hợp và thu thập dữ liệu từ hệ thống cảm biến đa dạng, chia làm các nhóm chức năng cốt lõi:

### 1. Giám sát môi trường & Sức khoẻ
* **Nhiệt độ & Độ ẩm:** Sử dụng cảm biến **DHT11/DHT22** để theo dõi chất lượng không khí trong phòng.
* **Cường độ ánh sáng:** Sử dụng quang trở **LDR** hoặc cảm biến kỹ thuật số **BH1750** (đo chính xác độ Lux) phục vụ bài toán tự động hóa chiếu sáng.
* **Chất lượng không khí (Tùy chọn nâng cấp):** Tích hợp cảm biến **MQ-135** để đo nồng độ khí CO2, benzene, hoặc bụi mịn nhằm cảnh báo ô nhiễm trong phòng closed-room.

### 2. An toàn & Phòng chống thiên tai, sự cố
* **Phát hiện rò rỉ khí Gas/Khói:** Sử dụng cảm biến **MQ-2** để giám sát nguy cơ cháy nổ từ bếp Gas hoặc chập điện.
* **Phát hiện lửa:** Sử dụng cảm biến hồng ngoại phát hiện bước sóng của ngọn lửa nhằm đưa ra cảnh báo hỏa hoạn sớm nhất.
* **Phát hiện ngập nước / Trời mưa:** Sử dụng cảm biến **Rain Sensor** hoặc cảm biến mực nước ở ban công/phòng giặt để tự động đóng cửa sổ hoặc kéo rèm khi trời mưa.

### 3. An ninh & Kiểm soát ra vào
* **Phát hiện chuyển động:** Sử dụng cảm biến hồng ngoại **PIR (HC-SR501)** để nhận biết có người di chuyển vào vùng cấm khi kích hoạt chế độ "Báo động chống trộm".
* **Giám sát cửa ra vào:** Tích hợp **Cảm biến má từ (Magnetic Switch)** ở mép cửa để phát hiện trạng thái cửa đang đóng hay bị cạy mở trái phép.
* **Đo khoảng cách hành lang:** Sử dụng cảm biến siêu âm **SRF05 / HC-SR04** để hỗ trợ quét vùng tiếp cận gần khu vực cửa bảo mật.

### 4. Tự động hoá & Điều khiển thiết bị
* Điều khiển đóng/ngắt các thiết bị công suất cao như **Đèn**, **Quạt**, **Bơm nước** thông qua cụm **Module Relay**.
* **Chế độ Tự động:** Hệ thống tự đưa ra quyết định dựa trên dữ liệu từ các cảm biến trên (Ví dụ: Trời tối tự bật đèn, rò rỉ gas tự bật quạt hút mùi, trời mưa tự đóng rèm cửa).
* **Chế độ Thủ công:** Cho phép người dùng can thiệp trực tiếp bất cứ lúc nào thông qua nút bấm vật lý hoặc các nút chạm cảm ứng trên màn hình LCD.

### 5. Kết nối & Cảnh báo
* **Hiển thị tại chỗ:** Toàn bộ thông số môi trường và trạng thái On/Off của thiết bị được hiển thị trực quan trên màn hình LCD/Touch (TouchGFX của STM32).
* **Cảnh báo tức thời:** Tích hợp còi **Buzzer** và hệ thống **LED báo động** để kích hoạt âm thanh/ánh sáng lập tức khi xảy ra sự cố nguy hiểm (phát hiện có người đột nhập hoặc rò rỉ khí gas).
* **Kết nối đám mây:** Sử dụng giao thức **MQTT** hoặc **HTTP** để đồng bộ dữ liệu thời gian thực và nhận lệnh điều khiển từ Web Server hoặc Ứng dụng điện thoại.

---

## THÀNH PHẦN PHẦN CỨNG

| Thành phần | Linh kiện khuyến nghị | Chức năng |
| :--- | :--- | :--- |
| **Khối xử lý trung tâm** | STM32F429I-DISCO | Thu thập dữ liệu tổng, quản lý giao diện màn hình chính |
| **Khối kết nối ngoại vi** | ESP32 WROOM / NodeMCU | Đọc cảm biến, kích relay và truyền nhận dữ liệu qua Wi-Fi |
| **Khối cảm biến mở rộng** | DHT11/22, LDR, PIR, MQ-2, Flame, Rain, Má từ | Thu thập dữ liệu môi trường, an ninh và an toàn cháy nổ diện rộng |
| **Khối cơ cấu chấp hành** | Module Relay 4-Channel, Buzzer, Servo | Đóng cắt thiết bị, phát âm thanh cảnh báo, giả lập đóng mở cửa |

---

## DANH SÁCH CẢM BIẾN VÀ LINH KIỆN HỆ THỐNG

| Tên cảm biến / Linh kiện | Mã linh kiện (Model) | Giao thức / Chuẩn kết nối | Chức năng trong hệ thống |
| :--- | :--- | :--- | :--- |
| **1. Môi trường & Sức khỏe** | | | | 
| Cảm biến Nhiệt độ & Độ ẩm | **DHT11 / DHT22** | Single-Wire (Digital) | Theo dõi chất lượng, nhiệt độ, độ ẩm không khí trong phòng. |
| Cảm biến Ánh sáng (Quang trở) | **LDR (GL5516)** | Analog (ADC) / Digital | Nhận biết trạng thái sáng/tối để tự động hóa bật tắt đèn. |
| Cảm biến Cường độ ánh sáng | **BH1750 / GY-302** | I2C (Digital) | Đo chính xác cường độ rọi ánh sáng môi trường theo đơn vị Lux. |
| Cảm biến Chất lượng không khí | **MQ-135** | Analog (ADC) / Digital | Đo nồng độ khí CO2, các khí độc để cảnh báo ô nhiễm phòng kín. |
| | | | |
| **2. An toàn & Phòng chống sự cố** | | | |
| Cảm biến Khói & Khí Gas | **MQ-2** | Analog (ADC) / Digital | Giám sát và phát hiện sớm nguy cơ rò rỉ khí gas hoặc khói chập điện. |
| Cảm biến Phát hiện lửa | **Flame Sensor (HW-492)**| Analog (ADC) / Digital | Phát hiện bước sóng hồng ngoại của ngọn lửa để báo động hỏa hoạn. |
| Cảm biến Mực nước / Nước mưa | **Rain Sensor (FC-37)** | Analog (ADC) / Digital | Phát hiện trời mưa hoặc ngập nước tại ban công, phòng giặt. |
| | | | |
| **3. An ninh & Kiểm soát ra vào** | | | | 
| Cảm biến Chuyển động hồng ngoại | **HC-SR01 / HC-SR501**| Digital (High/Low) | Phát hiện có người di chuyển trong vùng cấm ở chế độ chống trộm. |
| Cảm biến Má từ giám sát cửa | **Magnetic Switch (MC-38)**| Digital (Đóng/Mở mạch) | Phát hiện trạng thái cửa bị cạy mở trái phép hoặc chưa đóng. |
| Cảm biến Siêu âm đo khoảng cách | **HC-SR04 / SRF05** | Trigger / Echo (Digital) | Đo khoảng cách vật cản, quét vùng tiếp cận hành lang an ninh. |
| Khóa cửa thông minh / Đầu đọc thẻ| **RFID-RC522** | SPI (Digital) | Đọc UID của thẻ từ để xác thực quyền ra vào cửa. |
| | | | |
| **4. Cơ cấu chấp hành & Cảnh báo** | | | |
| Mạch đóng ngắt nguồn cách ly | **Module Relay (SRD-05VDC)**| Digital (Kích mức Thấp/Cao)| Điều khiển đóng/ngắt nguồn điện cho Đèn, Quạt, Bơm nước. |
| Còi báo động vật lý | **Active Buzzer 5V** | PWM / Digital | Phát ra âm thanh cảnh báo tần số cao khi xảy ra sự cố đột nhập, cháy nổ. |
| Đèn chỉ thị trạng thái | **Hệ thống LED (5mm)** | Digital GPIO | Phát tín hiệu ánh sáng cảnh báo tức thời theo từng cấp độ nguy hiểm. |

## SƠ ĐỒ KIẾN TRÚC HỆ THỐNG

```text
TẦNG THIẾT BỊ NGOẠI VI (EDGE LAYER)
                                             ┌──────────────────┐
 [Cụm Cảm Biến Đa Năng] ───(Đọc dữ liệu)──>  │     ESP32-S3     │
 [Relay / Còi Báo Động] <──(Điều khiển)────  │  (Node Ngoại Vi) │
                                             └──────────────────┘
                                                      │
                                       UART / SPI     │ (Giao tiếp phần cứng)
                                                      │
TẦNG TRUNG TÂM ĐIỀU KHIỂN (GATEWAY)                   ▼
                                             ┌──────────────────┐
 [Màn hình LCD TouchGFX] <──(Giao diện)────  │    STM32F429I    │
 [Khóa Cửa Từ Khóa RFID] ───(Kiểm soát)───>  │ (Smart Home Hub) │
                                             └──────────────────┘
                                                      │
                                                      │  Wi-Fi (MQTT / HTTP)
                                                      │
TẦNG MÁY CHỦ & ỨNG DỤNG (CLOUD & USER)                ▼
 ┌──────────────────┐                        ┌──────────────────┐
 │    Mobile App    │      RESTful API       │  Backend Server  │
 │  (Flutter App)   │ <────────────────────> │  (Node.js / TS)  │
 └──────────────────┘      WebSockets        └──────────────────┘
```

---

## CẤU TRÚC THƯ MỤC DỰ ÁN

Mã nguồn của hệ thống được tổ chức thành các phân vùng module độc lập và rõ ràng:

* **`_ /APP/smarthome_app`**: Ứng dụng di động đa nền tảng (Android/iOS) được phát triển bằng **Flutter (Dart)**, đảm nhận vai trò làm giao diện Dashboard hiển thị thông số và điều khiển thiết bị từ xa cho người dùng.
* **`_ /backend`**: Hệ thống máy chủ trung tâm (Server) lập trình bằng **TypeScript (Node.js)**, chịu trách nhiệm quản lý cơ sở dữ liệu, cung cấp RESTful API và các cổng kết nối thời gian thực.
* **`_ /ESP32/esp32s3`**: Mã nguồn chương trình cho Node cảm biến ngoại vi sử dụng vi điều khiển **ESP32-S3** (viết bằng C trên nền ESP-IDF), chịu trách nhiệm đọc dữ liệu cảm biến và truyền dữ liệu.
* **`_ /STM32/SmartHome`**: Dự án Smart Gateway Hub chạy trên kit **STM32F429I-DISCO**, tích hợp cấu hình đồ họa **TouchGFX** hiển thị màn hình LCD tại chỗ và quản lý đa nhiệm thời gian thực bằng hệ điều hành **FreeRTOS**.

---

## HƯỚNG NÂNG CẤP VÀ PHÁT TRIỂN

- **Mở rộng Giao diện:** Thiết kế App di động chuyên dụng qua nền tảng Blynk hoặc tự xây dựng Local Web Server chạy trực tiếp trên ESP32.
- **Điều khiển bằng Giọng nói:** Tích hợp trợ lý ảo (Google Assistant, Alexa) để bật/tắt thiết bị bằng giọng nói.
- **Mạng lưới cảm biến (Mesh):** Triển khai giao thức **ESP-NOW** hoặc **Mạng Mesh** để liên kết nhiều node cảm biến không dây trong nhà mà không phụ thuộc vào Router Wi-Fi.
- **Kiểm soát ra vào:** Tích hợp module **RFID RC522** để làm khóa cửa thông minh bằng thẻ từ bảo mật.
- **Quản lý năng lượng:** Thêm cảm biến dòng điện (như PZEM-004T) để đo lường, giám sát lượng điện năng tiêu thụ của các thiết bị.