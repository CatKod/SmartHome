# KỊCH BẢN HOẠT ĐỘNG TỔNG THỂ CỦA HỆ THỐNG

Hệ thống **phòng thông minh** tích hợp: kiểm soát ra vào, chống xâm nhập, giám sát môi trường và điều khiển từ xa qua MQTT.

- **STM32H7** (STM32H723ZGT6): bộ điều khiển trung tâm — đọc cảm biến, xử lý logic, ra quyết định, điều khiển thiết bị đầu ra.
- **ESP32-S3**: module kết nối Internet — nhận dữ liệu từ STM32H7 qua UART, đẩy lên MQTT; nhận lệnh từ MQTT, chuyển về STM32H7.

> **Quy ước trong tài liệu:** các khối `> Vấn đề [Vn]` là chú thích những điểm chưa ổn trong kịch bản (về phần cứng, thời gian thực hoặc bảo mật) kèm cách giải quyết. Danh sách tổng hợp ở [Phụ lục A](#phụ-lục-a--tổng-hợp-vấn-đề--giải-pháp). Đối chiếu với code hiện tại ở [Phụ lục B](#phụ-lục-b--khoảng-cách-giữa-code-hiện-tại-và-kịch-bản).

## Kiến trúc tổng thể

```text
CẢM BIẾN (RFID, Keypad, PIR, Sound, DHT11, Light, Thermistor, Rain)
   │
   ▼
STM32H7 ── Xử lý logic / State Machine
   │       ├── Điều khiển Servo (khóa cửa)
   │       ├── Điều khiển Stepper (cửa sổ)
   │       └── Điều khiển Đèn / LED (Relay, 74HC595)
   │ UART (USART3)
   ▼
 ESP32-S3
   │ Wi-Fi
   ▼
  MQTT Broker ──▶ Dashboard / App
```

Chiều ngược lại (điều khiển từ xa):

```text
Người dùng trên MQTT ──▶ ESP32 ──▶ STM32H7 ──▶ Kiểm tra điều kiện ──▶ Điều khiển thiết bị
```

---

## 1. Khi hệ thống khởi động

Khi được cấp nguồn:

1. **STM32H7** khởi tạo toàn bộ cảm biến và thiết bị: RFID RC522, keypad 4×4, PIR, cảm biến âm thanh, DHT11, cảm biến ánh sáng, cảm biến nhiệt (thermistor), cảm biến mưa, servo SG90, động cơ bước 28BYJ-48, 74HC595.
2. **ESP32** kết nối Wi-Fi và thiết lập kết nối MQTT.
3. STM32H7 đưa hệ thống về trạng thái an toàn ban đầu: servo về vị trí **khóa cửa**, xác định trạng thái cửa sổ, các cảm biến bắt đầu thu thập dữ liệu.
4. Hệ thống mặc định vào **HOME MODE** (người dùng đang ở trong phòng).
5. STM32H7 gửi trạng thái ban đầu sang ESP32 → ESP32 publish lên MQTT.

Ví dụ trạng thái ban đầu trên dashboard:

```text
System: ONLINE          Mode: HOME
Lock: ENGAGED           Window: CLOSED
Temperature: 28°C       Humidity: 65%
Light: DARK             Rain: NO
Motion: NO              Alarm: OFF
```

> **Vấn đề [V1] — "Xác định trạng thái cửa sổ" khi khởi động là bất khả thi với phần cứng hiện tại.**
> Động cơ bước 28BYJ-48 là hệ **hở (open-loop)**: STM32 không có cách nào biết cửa sổ đang mở hay đóng sau khi mất điện (motor có thể bị xoay tay, mất bước…).
> **Giải pháp:** (a) thêm **công tắc hành trình / reed switch** ở vị trí "đóng hoàn toàn" để làm homing khi boot — đây là cách chuẩn; hoặc (b) chấp nhận giải pháp phần mềm: khi boot **luôn chạy stepper về hướng đóng đủ số bước tối đa** (homing mù), coi đó là vị trí 0, sau đó theo dõi trạng thái bằng biến phần mềm. Với đồ án demo, phương án (b) đủ dùng nhưng phải ghi rõ giả định.

>>> tôi chấp nhận phương án (b)

> **Vấn đề [V2] — Đơn vị hiển thị ánh sáng không nhất quán.**
> Chỗ này ghi `Light: 50%`, các mục sau lại dùng `Light: DARK`. Driver hiện tại đọc ADC rồi quy về **0/1 (sáng/tối) có trễ (hysteresis)**.
> **Giải pháp:** thống nhất gửi **cả hai**: `light_raw` (% = ADC/65535×100) để hiển thị, và `light_state` (BRIGHT/DARK) để làm điều kiện logic. Tránh dùng % thô làm điều kiện bật đèn vì sẽ nhấp nháy quanh ngưỡng.

>>> Vậy hãy chia cho tôi thành 4 state: **Dark, Dim, Normal, Bright** (đã triển khai trong firmware).

> **Vấn đề [V3] — "System: ONLINE" phải tự chuyển OFFLINE được khi ESP32 rớt mạng.**
> Nếu chỉ publish "ONLINE" lúc boot, khi ESP32 mất điện/mất Wi-Fi thì dashboard vẫn hiển thị ONLINE mãi.
> **Giải pháp:** dùng cơ chế **MQTT Last Will (LWT)**: ESP32 khai báo will message `offline` (retained) trên topic `smarthome/status` khi connect; sau khi connect thành công thì publish `online` (retained). Broker sẽ tự phát `offline` khi mất kết nối. Đây là tính năng có sẵn của esp-mqtt, chỉ cần cấu hình.

>>> tôi chấp nhận giải pháp này

---

## 2. Vai trò của STM32H7 và ESP32

- **STM32H7 là bộ xử lý chính**: mọi quyết định quan trọng (xác thực, phát hiện xâm nhập, điều khiển cửa/cửa sổ, xử lý cảm biến) đều thực hiện trên STM32H7.
- **ESP32 là cầu nối Internet**, không thay thế STM32H7.
- Hai MCU giao tiếp qua **UART** (USART3 phía STM32, UART1 phía ESP32, 115200 8N1).

Dữ liệu STM32H7 gửi lên (ví dụ):

```text
TEMP:28  HUM:65  RAIN:0  MOTION:1  DOOR:LOCKED  MODE:HOME
```

Lệnh ESP32 gửi xuống (ví dụ):

```text
LIGHT_ON / LIGHT_OFF / WINDOW_OPEN / WINDOW_CLOSE / SECURITY_ON / SECURITY_OFF
```

STM32H7 nhận lệnh, **kiểm tra lệnh có hợp lệ trong trạng thái hiện tại không** rồi mới thực hiện. MQTT không bao giờ điều khiển trực tiếp phần cứng.

> **Vấn đề [V4] — Giao thức UART hiện tại không đáp ứng được kịch bản này.**
> Code hiện tại chỉ có **một chiều** STM32→ESP32, đúng một loại frame `<STX>27.5,65,1,0<ETX>` (4 trường, **không checksum, không ACK**, không phân loại bản tin). Kịch bản cần: nhiều loại bản tin (trạng thái định kỳ, sự kiện, lệnh, phản hồi lệnh) và chiều ESP32→STM32 (hiện USART3 RX chưa có code nhận).
> **Giải pháp:** định nghĩa **một giao thức khung thống nhất 2 chiều** ngay từ đầu, kiểu NMEA:
>
> ```text
> <STX>TYPE,field1,field2,...*CS<ETX>     (CS = XOR toàn bộ payload, 2 ký tự hex)
>
> STM32 → ESP32:
>   <STX>DATA,28.5,65,72,1,0,0,0,0,LOCKED,CLOSED,HOME,0*5A<ETX>   ; trạng thái định kỳ 1s
>   <STX>EVT,ACCESS_GRANTED,RFID*3C<ETX>                          ; sự kiện tức thời
>   <STX>ACK,WINDOW_OPEN,REJECT,RAIN*4E<ETX>                      ; phản hồi lệnh
>
> ESP32 → STM32:
>   <STX>CMD,LIGHT_ON*1F<ETX>
> ```
>
> Kèm theo: (1) STM32 bật **nhận UART bằng ngắt/DMA idle-line**; (2) ESP32 đặt **timeout chờ ACK** (ví dụ 500 ms, retry 1 lần) — nếu không có ACK thì báo `command failed` lên MQTT; (3) checksum sai thì loại frame. Làm một file đặc tả chung (`docs/uart_protocol.md`) để hai firmware không "trôi" khỏi nhau — hiện tại giao thức chỉ tồn tại ngầm trong comment và format string của 2 repo con.

>>> tôi chấp nhận giải pháp này

---

## 3. Kiểm soát ra vào

Người dùng mở cửa bằng **một trong hai** phương thức:

- Quét **thẻ RFID** hợp lệ (RC522, poll mỗi 250 ms), hoặc
- Nhập đúng **mã PIN** trên keypad.

Chỉ cần một phương thức thành công → STM32H7 xác nhận người dùng hợp lệ → điều khiển **servo SG90** mở khóa → gửi sự kiện sang ESP32:

```text
EVT: ACCESS_GRANTED, METHOD=RFID, DOOR=UNLOCKED
```

MQTT hiển thị lịch sử truy cập:

```text
14:30 - Door opened by RFID
14:35 - Door opened by PIN
```

Sau một khoảng thời gian định trước (ví dụ **5 giây**), servo tự quay về vị trí khóa, STM32H7 gửi `DOOR: LOCKED` để MQTT cập nhật.

> **Vấn đề [V5] — Tự động khóa lại theo thời gian mà không biết cửa đã đóng chưa.**
> Servo khóa lại sau N giây nhưng nếu cửa vẫn đang mở vật lý thì chốt khóa quay vào khoảng không (với mô hình demo thì vô hại, nhưng logic "Door: LOCKED" trên dashboard sẽ **sai so với thực tế**).
> **Giải pháp:** nếu muốn đúng bản chất, thêm **cảm biến cửa (reed switch)** để phân biệt `DOOR_LOCKED` (chốt) và `DOOR_OPEN` (cánh cửa). Nếu không thêm phần cứng, đổi nhãn hiển thị thành `Lock: ENGAGED/RELEASED` để tránh gây hiểu nhầm ngữ nghĩa.

>>> tôi chấp nhận giải pháp đổi nhãn hiển thị thành Lock: ENGAGED/RELEASED

> **Vấn đề [V6] — Lưu trữ và chống dò mã PIN.**
> PIN và danh sách UID thẻ hợp lệ hard-code trong firmware thì không đổi được khi vận hành, và nhập sai liên tục không bị phạt thời gian.
> **Giải pháp:** (1) lưu PIN/UID vào **flash (EEPROM emulation)** để có thể đổi PIN bằng keypad (chức năng đổi PIN yêu cầu nhập PIN cũ); (2) thêm **lockout tăng dần**: sai 3 lần → khóa nhập 30 giây (buzzer nhắc), sai tiếp → 60 giây… — cơ chế này khớp luôn với mục 4 (đếm số lần thất bại).

>>> tôi chấp nhận giải pháp này

---

## 4. Xác thực thất bại

Quét sai thẻ hoặc nhập sai PIN → STM32H7 ghi nhận **một lần xác thực thất bại**, gửi sang ESP32:

```text
EVT: ACCESS_DENIED, METHOD=RFID, FAILED_COUNT=1
```

Nếu số lần sai **vượt giới hạn (ví dụ 3 lần liên tiếp)** → hành vi đáng ngờ:

- Tăng mức độ nguy hiểm (risk score, xem mục 18);
- Bật LED cảnh báo;
- ESP32 publish cảnh báo:

```text
WARNING: 3 failed access attempts detected.
```

Nếu đồng thời đang ở **Security Mode** hoặc PIR phát hiện chuyển động → nguy cơ được đánh giá cao hơn.

*(Bộ đếm thất bại nên reset khi có một lần xác thực thành công, hoặc tự giảm sau một khoảng thời gian — xem [V13].)*

---

## 5. Hoạt động trong Home Mode

Trong Home Mode, cảm biến phục vụ **tiện nghi + giám sát môi trường**:

| Cảm biến | Vai trò trong Home Mode |
|---|---|
| DHT11 | Nhiệt độ + độ ẩm không khí trong phòng |
| Thermistor (KY-028) | Theo dõi nhiệt tại một thiết bị/khu vực cụ thể |
| Quang trở (LDR) | Môi trường sáng hay tối |
| PIR | Có người trong phòng hay không |

STM32H7 xử lý và gửi định kỳ (chu kỳ 1 giây) sang ESP32 → MQTT:

```text
Temperature: 30°C   Humidity: 72%
Light: DARK         Person Present: YES
```

---

## 6. Điều khiển đèn tự động bằng PIR và cảm biến ánh sáng

```text
HOME MODE  +  PIR phát hiện người  +  Môi trường tối
                        ↓
                     BẬT ĐÈN
```

Nếu không còn chuyển động trong một khoảng thời gian → tự tắt đèn. Khi trạng thái đèn thay đổi, STM32H7 gửi kèm **nguyên nhân**:

```text
Light: ON
Reason: Motion detected in dark environment
```

> **Vấn đề [V8] — Thời gian giữ đèn 3 giây là không khả thi và không hợp lý.**
> PIR SR505 có **thời gian giữ mức HIGH nội bộ ~8 giây** sau mỗi lần phát hiện (không chỉnh được), nên phần mềm không thể "thấy hết chuyển động sau 3 giây". Ngoài ra 3 giây là quá ngắn về trải nghiệm — người ngồi yên đọc sách sẽ bị tắt đèn liên tục.
> **Giải pháp:** đặt thời gian giữ đèn ở **mức phần mềm ≥ 30 giây** (demo có thể dùng 10–15 giây cho dễ trình bày), tính từ **cạnh lên cuối cùng** của PIR, và chỉ tắt khi chân PIR đã về LOW. Giá trị này để dưới dạng `#define LIGHT_HOLD_MS` cho dễ tinh chỉnh.

>> thực tế là cảm biến PIR của tôi rất nhạy — **đã cấu hình `LIGHT_HOLD_MS = 3000` (3 giây)** theo yêu cầu demo.

---

## 7. Điều khiển đèn bằng cảm biến âm thanh (vỗ tay) trong Home Mode

STM32H7 **đếm số lần vỗ tay trong một cửa sổ thời gian**:

```text
Vỗ 2 lần → TẮT ĐÈN          Vỗ 3 lần → BẬT ĐÈN
```

Sau khi thực hiện, trạng thái được gửi lên MQTT kèm nguồn điều khiển:

```text
Light: OFF
Control source: Double Clap
```

Đèn có thể được điều khiển theo 3 cách, tất cả do STM32H7 quản lý:

1. **Tự động**: PIR + cảm biến ánh sáng
2. **Tại chỗ**: vỗ tay
3. **Từ xa**: MQTT

> **Vấn đề [V10] — Nhận diện 2 vỗ / 3 vỗ cần thuật toán cửa sổ thời gian, không thể quyết định tức thời.**
> Hai điểm cần lường trước: (1) một tiếng vỗ có thể sinh **nhiều xung** trên chân DO của module âm thanh → phải có **thời gian chết (refractory) ~200 ms** sau mỗi xung được tính; (2) muốn phân biệt "2 vỗ" với "3 vỗ" thì phải **đợi hết cửa sổ đếm** (ví dụ 1,5 giây kể từ tiếng vỗ đầu) mới kết luận — tức là đèn sẽ phản ứng trễ ~1 giây sau tiếng vỗ cuối, đây là hành vi đúng, cần ghi rõ trong báo cáo để không bị hiểu là lỗi.
> **Giải pháp cài đặt:** máy trạng thái đếm vỗ chạy trong main loop bằng `HAL_GetTick()`: xung hợp lệ (cách xung trước ≥ 200 ms) → tăng bộ đếm; quá 1,5 s không có xung mới → chốt kết quả (2 → tắt, 3 → bật, khác → bỏ qua) rồi reset. Driver `sound_sensor` hiện có sẵn API đếm sự kiện qua EXTI (`SoundSensor_GetEventCount`) — dùng được ngay, xung không bị mất kể cả khi main loop đang bận đọc DHT11 (~25 ms).

>>> tôi chấp nhận giải pháp này

> **Vấn đề [V9] — "Quản lý để tránh xung đột" giữa 3 nguồn điều khiển đèn cần quy tắc cụ thể, chưa được định nghĩa.**
> Ví dụ: người dùng tắt đèn bằng MQTT nhưng PIR + trời tối lập tức bật lại → đèn "bất trị".
> **Giải pháp:** định nghĩa **cơ chế manual-override**: lệnh thủ công (vỗ tay hoặc MQTT) đặt cờ `light_manual_override` kèm thời hạn (ví dụ 10 phút, hoặc đến khi đổi mode). Khi cờ đang bật, logic tự động PIR+ánh sáng **bị tạm treo**. Hết hạn hoặc đổi mode → quay lại tự động. Thứ tự ưu tiên: `ALARM (ép tắt/nháy) > lệnh thủ công > tự động`.

>>> tôi chấp nhận giải pháp này

---

## 8. Điều khiển đèn từ MQTT

Dashboard có hai nút `[ TURN LIGHT ON ]` / `[ TURN LIGHT OFF ]`:

```text
MQTT → ESP32 → CMD:LIGHT_ON → STM32H7 → kiểm tra → BẬT ĐÈN → ACK → MQTT cập nhật
```

Hệ thống là **vòng kín hai chiều**: MQTT gửi yêu cầu → STM32H7 thực hiện → STM32H7 gửi **trạng thái thực tế** → MQTT hiển thị theo trạng thái thực tế (không tự giả định lệnh đã thành công).

*(Ghi chú phần cứng: "đèn" trong hệ thống là **Relay 1 (PE4)** đóng/cắt đèn thật; 8 LED trên 74HC595 chỉ là LED trạng thái — xem mục 21. Cần thống nhất điều này trong code để tránh nhầm lẫn.)*

---

## 9. Giám sát nhiệt độ và độ ẩm

DHT11 được đọc theo chu kỳ (1 giây/lần — đúng giới hạn tối thiểu của DHT11), gửi sang ESP32 → MQTT:

```text
Temperature: 29°C    Humidity: 70%
```

Nếu vượt ngưỡng (`Temperature > 30°C` hoặc `Humidity > 80%`) → môi trường bất thường. STM32H7 **không mở cửa sổ ngay** mà kiểm tra thêm:

- Đang ở Home Mode hay Security Mode?
- Có đang mưa không?
- Cửa sổ đang mở hay đóng?

*(Ghi chú: driver DHT11 hiện tại đã có mã lỗi và cơ chế giữ giá trị hợp lệ cuối cùng khi đọc lỗi — cứ giữ nguyên. Nên thêm: nếu **lỗi liên tiếp N lần** (ví dụ 10 lần) thì phát sự kiện `EVT,SENSOR_FAULT,DHT11` lên MQTT thay vì im lặng hiển thị giá trị cũ mãi. Ngưỡng nhiệt/ẩm cũng cần hysteresis: mở tại 30°C, chỉ đóng lại khi < 28°C, tránh mở–đóng liên tục quanh ngưỡng.)*

---

## 10. Tự động thông gió (DHT11 + cảm biến mưa + động cơ bước)

```text
HOME MODE  +  Nhiệt độ/độ ẩm cao  +  Không mưa  +  Cửa sổ đang đóng
                              ↓
                        MỞ CỬA SỔ (28BYJ-48 qua ULN2003)
```

Sau khi mở:

```text
Window: OPEN
Reason: High temperature
```

DHT11 không chỉ để hiển thị mà **trực tiếp tham gia quyết định điều khiển**.

> **Vấn đề [V11] — Driver stepper hiện tại chạy blocking, sẽ làm "đứng" toàn hệ thống khi mở/đóng cửa sổ.**
> Code hiện tại bước motor bằng `HAL_Delay(2)` mỗi nửa bước. Mở cửa sổ ~2–3 vòng = 8000–12000 bước × 2 ms ≈ **16–24 giây main loop bị khóa cứng**: không quét keypad, không đọc RFID, không gửi UART, không xử lý vỗ tay. Với hệ thống an ninh thì đây là lỗi nghiêm trọng (kẻ xâm nhập có thể lợi dụng lúc cửa sổ đang chạy).
> **Giải pháp:** chuyển stepper sang **non-blocking**: mỗi vòng main loop (hoặc trong ngắt **TIM6** — timer này đã được init sẵn trong project nhưng chưa dùng, cấu hình tick 1–2 ms là vừa) chỉ bước 1 nửa bước nếu đến hạn, dùng máy trạng thái `IDLE / OPENING / CLOSING` + bộ đếm bước còn lại. Cho phép **hủy giữa chừng** (ví dụ đang mở thì phát hiện mưa → đảo chiều đóng ngay, khớp mục 11).

>>> tôi chấp nhận giải pháp này

---

## 11. Đang thông gió nhưng trời bắt đầu mưa

Kịch bản liên kết quan trọng nhất:

```text
Cửa sổ đang mở (do nóng)  +  Cảm biến mưa phát hiện nước
                    ↓
        HỦY yêu cầu thông gió  →  ĐÓNG CỬA SỔ ngay
```

Dù nhiệt độ vẫn cao, **mưa được ưu tiên cao hơn thông gió**. Thứ tự ưu tiên điều khiển cửa sổ:

```text
1. ALARM MODE (ép đóng)  >  2. SECURITY MODE (ép đóng)  >  3. MƯA (ép đóng)
                >  4. QUÁ NHIỆT thiết bị  >  5. NHU CẦU THÔNG GIÓ
```

Sau khi đóng:

```text
Window: CLOSED
Reason: Rain detected

WARNING: Rain detected. Window automatically closed.
```

*(Thiết kế ưu tiên này tốt. Cài đặt gọn nhất: viết **một hàm quyết định duy nhất** `Window_Arbitrate()` chạy mỗi chu kỳ, tính "trạng thái cửa sổ mong muốn" từ danh sách điều kiện theo đúng thứ tự ưu tiên trên, rồi so với trạng thái hiện tại để ra lệnh stepper — thay vì rải if/else ở nhiều nơi, sẽ không bao giờ bị hai luồng logic giành nhau điều khiển cửa sổ.)*

---

## 12. Vai trò của cảm biến nhiệt điện trở (thermistor)

Khác với DHT11 (đo nhiệt độ **chung của phòng**), thermistor đặt sát một **thiết bị/khu vực cụ thể** (nguồn điện, động cơ, khu vực nguy cơ quá nhiệt):

```text
Room Temperature (DHT11): 28°C
Device Temperature (Thermistor): 55°C   ← thiết bị đang quá nhiệt dù phòng mát
```

Khi phát hiện quá nhiệt, STM32H7:

- Bật trạng thái cảnh báo;
- Gửi cảnh báo qua ESP32 lên MQTT;
- Bật Buzzer đến khi hết quá nhiệt
- Nếu đang Home Mode + không mưa + mở cửa sổ là an toàn → mở cửa sổ hỗ trợ thông gió.

```text
WARNING: Device overheating detected. Device Temperature: 55°C
```

> **Vấn đề [V7] — Phần cứng hiện tại không đọc được "Device Temperature: 55°C".**
> Module thermistor (KY-028) đang nối vào **PC5 dạng số (EXTI)** — chỉ cho tín hiệu **CÓ/KHÔNG quá ngưỡng** (ngưỡng chỉnh bằng biến trở trên module), không cho giá trị nhiệt độ.
> **Giải pháp:** chọn một trong hai, và sửa kịch bản cho khớp: (a) nối thêm chân **A0 của KY-028 vào một kênh ADC còn trống** của ADC1, tính nhiệt độ bằng công thức B-parameter (cần tra B và R25 của NTC trên module) → hiển thị được số °C thật; hoặc (b) giữ nguyên phần cứng, đổi nội dung hiển thị thành nhị phân: `Device Overheat: YES/NO`. Phương án (b) không tốn công, chỉ cần sửa lời văn kịch bản.

>>> tôi chấp nhận giải pháp (b), ngoài ra cảm biến Thermistor của tôi cũng chỉ có chân DO.

---

## 13. Điều khiển cửa sổ từ MQTT

Dashboard có `[ OPEN WINDOW ]` / `[ CLOSE WINDOW ]`, nhưng STM32H7 **không thực hiện lệnh mù quáng**:

```text
MQTT yêu cầu mở cửa sổ → ESP32 gửi CMD:WINDOW_OPEN → STM32H7 kiểm tra
        ↓
   Đang mưa? ──YES──▶ Từ chối:  "Command rejected: rain is detected."
        │
        NO ──▶ Mở cửa sổ → ACK OK → MQTT cập nhật Window: OPEN
```

Tương tự, đang **Security Mode / Alarm Mode** thì lệnh mở cửa sổ từ MQTT bị từ chối. **Quyết định cuối cùng luôn thuộc về STM32H7** — đây chính là lý do mọi lệnh phải đi qua nó thay vì ESP32 điều khiển thẳng.

*(Cần kèm cơ chế ACK/REJECT + lý do trong giao thức UART — đã mô tả ở [V4]. ESP32 publish kết quả lên topic riêng `smarthome/cmd/result` để dashboard hiển thị "Command rejected: ...".)*

---

## 14. Kích hoạt Security Mode (tại chỗ, bằng keypad)

Người dùng chuẩn bị rời phòng: nhấn phím **A** trên keypad → nhập đúng **PIN** để xác nhận → hệ thống **đếm ngược** (ví dụ 15–30 giây) cho người dùng rời phòng → STM32H7 đưa phòng về trạng thái an toàn:

```text
Nhấn A → Nhập PIN đúng → Đếm ngược (exit delay)
   → Đóng cửa sổ → Tắt đèn → Khóa cửa
   → PIR chuyển sang chức năng phát hiện xâm nhập
   → Cảm biến âm thanh chuyển sang phát hiện tiếng động bất thường
   → SECURITY MODE
```

MQTT hiển thị:

```text
Security Mode: ACTIVE
Door: LOCKED    Window: CLOSED    Light: OFF
```

*(Lưu ý cài đặt: đếm ngược và chuỗi "đóng cửa sổ → tắt đèn → khóa cửa" phải chạy **non-blocking** trong main loop (máy trạng thái + `HAL_GetTick()`), tuyệt đối không `HAL_Delay()` xuyên suốt — trong lúc đếm ngược hệ thống vẫn phải quét keypad để người dùng có thể bấm hủy. Riêng bước "đóng cửa sổ" phụ thuộc stepper non-blocking [V11]: Security Mode chỉ nên chính thức ACTIVE sau khi cửa sổ đã đóng xong.)*

---

## 15. Kích hoạt Security Mode từ MQTT

```text
MQTT [ ACTIVATE SECURITY MODE ] → ESP32 → CMD:SECURITY_ON → STM32H7
   → kiểm tra trạng thái → thực hiện chuỗi: đóng cửa sổ, tắt đèn, khóa cửa,
     bật giám sát PIR + âm thanh
   → hoàn thành mới gửi: SECURITY_MODE_ACTIVE → MQTT
```

> **Vấn đề [V12] — Chính sách bảo mật cho các lệnh từ xa: bật từ xa thì được, nhưng TẮT từ xa thì phải cân nhắc.**
> Kênh MQTT hiện chạy **plaintext port 1883**, tài khoản broker hard-code trong source đã push lên git. Ai chiếm được tài khoản MQTT là gửi được `SECURITY_OFF`/`ALARM_OFF` — vô hiệu hóa toàn bộ hệ thống an ninh từ xa.
> **Giải pháp:** phân loại lệnh theo mức rủi ro: các lệnh **hạ mức bảo vệ** (`SECURITY_OFF`, `ALARM_OFF`, mở khóa cửa) chỉ chấp nhận **tại chỗ bằng RFID/PIN**, STM32 từ chối nếu đến từ UART/MQTT (kịch bản mục 20 đã nghiêng theo hướng này — nên chốt hẳn thành quy tắc). Các lệnh **nâng mức bảo vệ hoặc vô hại** (`SECURITY_ON`, đèn, đóng cửa sổ) được phép từ xa. Đồng thời chuyển MQTT sang **TLS (mqtts://, port 8883)** và đưa credentials ra khỏi source (menuconfig/NVS, file không commit).

>>> **Đã triển khai:** tất cả lệnh (`SECURITY_OFF`, `ALARM_OFF`, đèn, cửa sổ) đều cho phép từ MQTT; giữ MQTT plaintext port 1883.

---

## 16. Phát hiện chuyển động trong Security Mode

Vai trò PIR thay đổi theo mode:

```text
HOME MODE:      PIR → phát hiện có người để hỗ trợ điều khiển đèn
SECURITY MODE:  PIR → phát hiện khả năng xâm nhập
```

PIR phát hiện chuyển động khi đang Security Mode → **chưa báo động ngay** mà vào trạng thái nghi ngờ:

```text
SECURITY MODE → PIR phát hiện chuyển động → SUSPICIOUS (nghi ngờ)
             → bắt đầu cửa sổ thời gian chờ xác thực (entry delay, ví dụ 15 giây)
```

Đồng thời gửi cảnh báo:

```text
WARNING: Motion detected while Security Mode is active. Waiting for authentication.
```

---

## 17. Xác thực khi hệ thống đang nghi ngờ

```text
RFID đúng  HOẶC  PIN đúng
        ↓
XÁC THỰC THÀNH CÔNG → Tắt Security Mode → Mở khóa cửa → HOME MODE
```

MQTT cập nhật:

```text
Authorized user detected. Security Mode disabled. Door unlocked.
```

---

## 18. Cảm biến âm thanh trong Security Mode + hệ thống tính điểm nguy hiểm

Trong Security Mode, quy tắc vỗ tay 2/3 lần **bị vô hiệu hóa**; cảm biến âm thanh chuyển sang phát hiện **tiếng động lớn/bất thường**.

STM32H7 dùng **risk score** để tổng hợp nguy cơ:

| Sự kiện | Điểm |
|---|---|
| PIR phát hiện chuyển động | +1 |
| Âm thanh lớn | +1 |
| Quét thẻ sai | +1 |
| Nhập PIN sai | +1 |
| Hết thời gian chờ mà không xác thực | +2 |

```text
Risk Score >= 3  →  ALARM MODE
```

> **Vấn đề [V13] — Risk score cần cơ chế giảm theo thời gian và quy tắc reset, nếu không sẽ báo động oan.**
> Nếu điểm chỉ cộng dồn vĩnh viễn: một con mèo đi qua hôm nay (+1) cộng với một tiếng động vô hại ngày mai (+1) rồi hàng xóm bấm nhầm chuông (+1) → báo động sau vài ngày dù không có sự kiện nào liên quan nhau.
> **Giải pháp:** (1) mỗi điểm cộng có **thời gian sống** (ví dụ tự trừ sau 60 giây), hoặc đơn giản hơn: score chỉ tính **trong một phiên nghi ngờ** (SUSPICIOUS) — vào SUSPICIOUS thì bắt đầu đếm, quay về giám sát bình thường thì reset về 0; (2) xác thực thành công → reset về 0 ngay; (3) chống spam cảm biến: mỗi nguồn (PIR/âm thanh) chỉ được cộng tối đa 1 điểm mỗi X giây.

>>> tôi chấp nhận giải pháp này

---

## 19. Alarm Mode

Khi xác định khả năng xâm nhập, hệ thống chuyển sang **ALARM MODE**, điều khiển đồng thời:

- Giữ servo ở trạng thái **khóa cửa**;
- **Ép đóng** cửa sổ;
- **Vô hiệu hóa** điều khiển đèn bằng vỗ tay;
- **Từ chối** lệnh mở cửa sổ từ MQTT;
- **Nháy LED cảnh báo** qua 74HC595;
- Tiếp tục theo dõi PIR + âm thanh;
- Gửi cảnh báo ngay qua ESP32 → MQTT:

```text
!!! INTRUSION ALERT !!!
Motion detected: YES     Loud sound detected: YES
Authentication: FAILED
Door: LOCKED             Window: CLOSED
```

> **Vấn đề [V14] — Kịch bản bỏ quên buzzer, trong khi phần cứng đã có sẵn.**
> Báo động mà chỉ nháy LED thì không có tác dụng răn đe; board đã có **buzzer trên PB0** với driver hoàn chỉnh.
> **Giải pháp:** thêm vào Alarm Mode: **buzzer hú ngắt quãng** (bíp 500 ms / nghỉ 500 ms, chạy non-blocking bằng `HAL_GetTick()`), tắt khi xác thực thành công. Đây là thay đổi rẻ nhất mà tăng tính thuyết phục của demo nhiều nhất.

>>> tôi chấp nhận giải pháp này

---

## 20. Tắt Alarm Mode

Người dùng hợp lệ **quét đúng RFID** hoặc **nhập đúng PIN** (tại thiết bị):

```text
Alarm OFF → Security Mode OFF → HOME MODE
```

MQTT cập nhật:

```text
Alarm cleared by authorized user. System returned to Home Mode.
```

**Quy định: Alarm Mode có thể tắt bằng RFID/PIN tại thiết bị hoặc lệnh MQTT `ALARM_OFF`.**

---

## 21. Vai trò của 74HC595

74HC595 mở rộng đầu ra: STM32H7 chỉ tốn 3 chân (PE7/8/9) điều khiển 8 LED trạng thái:

```text
LED 1: System Online      LED 5: Motion Detected
LED 2: Home Mode          LED 6: Sound Detected
LED 3: Security Mode      LED 7: Rain Detected
LED 4: Door Status        LED 8: Alarm (nháy khi ALARM MODE)
```

LED vật lý **đồng bộ với trạng thái trên MQTT**: ví dụ Security Mode ON → LED 3 sáng đồng thời MQTT hiển thị `Security: ACTIVE`. Giao diện vật lý và giao diện từ xa cùng phản ánh một nguồn trạng thái duy nhất (`g_system`).

*(Cài đặt gọn: mỗi chu kỳ main loop dựng 1 byte trạng thái từ `g_system` rồi `HC595_Write(byte)` — không cần lưu trạng thái LED riêng, không bao giờ lệch pha với dữ liệu gửi MQTT.)*

---

## 22. MQTT Dashboard

Dashboard chia thành các khu vực:

```text
┌─ TRẠNG THÁI HỆ THỐNG ─────────────┐  ┌─ KIỂM SOÁT CỬA ───────────────────┐
│ System: ONLINE                    │  │ Lock: ENGAGED                     │
│ Mode: HOME                        │  │ Last Access: RFID - Authorized    │
│ Alarm: OFF                        │  └───────────────────────────────────┘
└───────────────────────────────────┘
┌─ MÔI TRƯỜNG ──────────────────────┐  ┌─ ĐIỀU KHIỂN ──────────────────────┐
│ Temperature: 29°C                 │  │ LIGHT    [ ON ]  [ OFF ]          │
│ Humidity: 70%                     │  │ WINDOW   [ OPEN ] [ CLOSE ]       │
│ Device Overheat: NO               │  │ SECURITY [ ACTIVATE ]             │
│ Light Level: DARK                 │  └───────────────────────────────────┘
│ Rain: NO                          │
└───────────────────────────────────┘
┌─ NHẬT KÝ CẢNH BÁO ────────────────────────────────────────────────────────┐
│ 14:30 - Motion detected        14:31 - Loud sound detected                │
│ 14:31 - Invalid RFID           14:31 - ALARM ACTIVATED                    │
└───────────────────────────────────────────────────────────────────────────┘
```

> **Vấn đề [V15] — "MQTT hiển thị" cần nói chính xác: MQTT chỉ là kênh truyền, dashboard là một ứng dụng client riêng; và cần thiết kế cây topic thay vì 1 topic duy nhất.**
> Hiện firmware ESP32 publish tất cả vào một topic `smarthome/sensor_data` và **không subscribe gì** — không đủ cho dashboard nhiều khu vực + nút bấm.
> **Giải pháp:** (1) chọn client dashboard cụ thể: app Flutter có sẵn trong repo (`APP/smarthome_app`), hoặc nhanh nhất cho demo là **IoT MQTT Panel / MQTT Dashboard** (Android) / Node-RED; (2) thiết kế cây topic, tách **trạng thái (retained)** và **sự kiện/lệnh (không retain)**:
>
> ```text
> smarthome/status          online|offline   (retained, LWT — xem V3)
> smarthome/state/env       {"temp":28.5,"humi":65,"light":72,"light_state":"DARK","rain":0}  (retained)
> smarthome/state/mode      HOME|SECURITY|SUSPICIOUS|ALARM   (retained)
> smarthome/state/door      LOCKED|UNLOCKED                  (retained)
> smarthome/state/window    OPEN|CLOSED|MOVING               (retained)
> smarthome/state/light     ON|OFF                           (retained)
> smarthome/event           "14:31 Invalid RFID" ...         (không retain, QoS 1)
> smarthome/cmd             LIGHT_ON|WINDOW_OPEN|SECURITY_ON (dashboard → ESP32, QoS 1)
> smarthome/cmd/result      {"cmd":"WINDOW_OPEN","result":"REJECT","reason":"RAIN"}
> ```
>
> Retained giúp dashboard vừa mở lên là thấy ngay trạng thái mới nhất mà không phải chờ chu kỳ gửi kế tiếp.

>>> tôi chấp nhận giải pháp này

---

## 23. Kịch bản hoạt động liên kết hoàn chỉnh

1. Người dùng về nhà, **quét RFID hợp lệ** → STM32H7 mở servo → trạng thái cửa lên MQTT.
2. Người dùng vào phòng, **PIR phát hiện người**, hệ thống ở Home Mode. Trời tối → **tự bật đèn** → MQTT cập nhật.
3. Trong phòng: **vỗ 2 lần tắt đèn, vỗ 3 lần bật đèn**, hoặc điều khiển đèn từ MQTT.
4. **DHT11** theo dõi liên tục. Nhiệt độ cao → kiểm tra mưa → không mưa + Home Mode → **stepper mở cửa sổ** thông gió.
5. Trời **bắt đầu mưa** → ưu tiên bảo vệ phòng → **đóng cửa sổ ngay** dù đang nóng → MQTT nhận cảnh báo "Window automatically closed - rain detected".
6. Người dùng rời nhà: **keypad kích hoạt Security Mode** → tự đóng cửa sổ, tắt đèn, khóa cửa → PIR + âm thanh chuyển sang giám sát an ninh → MQTT: `Security Mode: ACTIVE`.
7. Không có người ở nhà, **PIR phát hiện chuyển động** → trạng thái SUSPICIOUS → cảnh báo lên MQTT → chờ xác thực.
8. **Không xác thực + tiếng động lớn** → risk score vượt ngưỡng → **ALARM MODE**: cửa giữ khóa, cửa sổ ép đóng, vỗ tay vô hiệu, LED nháy (+ buzzer hú, xem [V14]), cảnh báo dồn dập lên MQTT.
9. Người dùng hợp lệ **RFID/PIN tại thiết bị** → hủy cảnh báo → trở về Home Mode.

---

## 24. Mối liên kết tổng thể của tất cả thiết bị

```text
                         ┌───────────────┐
                         │     MQTT      │
                         │   Dashboard   │
                         └───────┬───────┘
                                 │ Wi-Fi
                                 ▼
                           ┌──────────┐
                           │  ESP32   │
                           └────┬─────┘
                                │ UART (2 chiều: DATA/EVT/ACK ↑ , CMD ↓)
                                ▼
                         ┌───────────────┐
                         │    STM32H7    │
                         │ Central Unit  │
                         └───────┬───────┘
                                 │
             ┌───────────────────┼───────────────────┐
             │                   │                   │
          ACCESS              SECURITY           ENVIRONMENT
             │                   │                   │
      RFID + Keypad         PIR + Sound      DHT11 + Thermistor
             │                   │             Light + Rain
             ▼                   ▼                   │
       Servo (cửa)        Risk Score /               ▼
       + 74HC595 LED      Alarm Mode          Window_Arbitrate()
                                                     │
                                                     ▼
                                             ULN2003 + Stepper
```

---

## 25. Điểm nổi bật nhất của project

**Hai MCU phân vai rõ ràng, không trùng nhiệm vụ:**

```text
STM32H7                          ESP32
→ Điều khiển phần cứng           → Kết nối Wi-Fi
→ Đọc cảm biến                   → Publish dữ liệu lên MQTT
→ Xử lý State Machine            → Subscribe lệnh từ MQTT
→ Ra quyết định an toàn          → Chuyển tiếp lệnh cho STM32H7
```

**Một thiết bị có chức năng khác nhau tùy mode** — đây là điểm "thông minh" thật sự của hệ thống:

| Thiết bị | HOME MODE | SECURITY MODE | ALARM MODE |
|---|---|---|---|
| Cảm biến âm thanh | Đếm vỗ tay điều khiển đèn | Phát hiện tiếng động bất thường | Tiếp tục giám sát, không nhận lệnh vỗ |
| PIR | Phát hiện người → điều khiển đèn | Phát hiện xâm nhập | Tiếp tục giám sát |
| Cửa sổ | Mở khi nóng + không mưa; đóng khi mưa | Ép đóng | Bắt buộc đóng, từ chối lệnh mở |

**Cửa sổ không phụ thuộc một cảm biến duy nhất** mà là kết quả trọng tài của nhiều điều kiện theo thứ tự ưu tiên (mục 11).

Toàn bộ cảm biến, cơ cấu chấp hành, STM32H7, ESP32 và MQTT liên kết thành **một hệ thống thống nhất**, không phải các linh kiện chạy demo rời rạc.

---

## Phụ lục A — Tổng hợp vấn đề & giải pháp

| Mã | Mục | Vấn đề | Giải pháp tóm tắt | Mức độ |
|---|---|---|---|---|
| V1 | 1 | Không biết trạng thái cửa sổ khi boot (stepper open-loop) | Công tắc hành trình, hoặc homing mù về vị trí đóng khi boot | Trung bình |
| V2 | 1 | Đơn vị ánh sáng lúc % lúc DARK | Gửi cả `light_raw` (%) và `light_state` (BRIGHT/DARK có hysteresis) | Nhỏ |
| V3 | 1 | ONLINE không tự chuyển OFFLINE khi mất mạng | MQTT LWT + retained trên `smarthome/status` | Nhỏ, dễ làm |
| V4 | 2 | Giao thức UART hiện tại 1 chiều, 4 trường, không checksum/ACK | Giao thức khung thống nhất `TYPE,payload*CS`, 2 chiều, ACK + timeout, đặc tả chung | **Nền tảng, làm trước tiên** |
| V5 | 3 | Servo tự khóa lại nhưng không biết cửa đóng chưa | Reed switch, hoặc đổi ngữ nghĩa hiển thị thành Lock: ENGAGED | Nhỏ |
| V6 | 3 | PIN/UID hard-code, không lockout | Lưu flash + đổi PIN bằng keypad + lockout tăng dần | Trung bình |
| V7 | 12 | KY-028 nối chân số, không đọc được °C | Nối A0 vào ADC + công thức NTC, hoặc đổi kịch bản thành Overheat YES/NO | Trung bình |
| V8 | 6 | Tắt đèn sau 3 giây — ngắn hơn hold time ~8 s của SR505 | Giữ đèn ≥ 30 s (demo 10–15 s), tính từ cạnh lên cuối | Nhỏ |
| V9 | 7 | Chưa có quy tắc xử lý xung đột 3 nguồn điều khiển đèn | Cờ manual-override có thời hạn; ALARM > thủ công > tự động | Trung bình |
| V10 | 7 | Đếm vỗ 2/3 cần cửa sổ thời gian + chống dội | Refractory 200 ms, cửa sổ đếm 1,5 s, chốt khi hết cửa sổ | Trung bình |
| V11 | 10 | Stepper blocking 16–24 s làm đứng toàn hệ thống | Stepper non-blocking bằng TIM6/máy trạng thái, hủy được giữa chừng | **Nghiêm trọng** |
| V12 | 15 | Lệnh hạ mức bảo vệ từ xa + MQTT plaintext + creds trong git | Lệnh hạ bảo vệ chỉ tại chỗ; TLS 8883; creds ra khỏi source | **Nghiêm trọng (bảo mật)** |
| V13 | 18 | Risk score cộng dồn vô hạn → báo động oan | Điểm có thời gian sống / chỉ tính trong phiên SUSPICIOUS; reset khi xác thực | Trung bình |
| V14 | 19 | Alarm không có còi dù buzzer sẵn trên PB0 | Buzzer hú ngắt quãng non-blocking trong ALARM | Nhỏ, nên làm |
| V15 | 22 | Chưa chọn dashboard client, topic MQTT chưa thiết kế | Cây topic state/event/cmd + retained; dùng app Flutter hoặc IoT MQTT Panel | **Nền tảng** |

## Phụ lục B — Trạng thái triển khai (đã đồng bộ code)

**Đã triển khai trong firmware:**

- Giao thức UART v2 hai chiều (`DATA`/`EVT`/`ACK`/`CMD` + checksum XOR) — xem [docs/uart_protocol.md](docs/uart_protocol.md)
- ESP32: cây topic MQTT + LWT + subscribe `smarthome/cmd`
- STM32: FSM `HOME / EXIT_DELAY / SECURITY / SUSPICIOUS / ALARM` + risk score trong phiên SUSPICIOUS
- Stepper non-blocking + homing mù khi boot + `Window_Arbitrate()`
- Đèn: Relay 1, tự động PIR+ánh sáng (3s), vỗ tay 2/3, MQTT, manual-override 10 phút
- Xác thực RFID/PIN, lockout flash, đổi PIN (phím D), đăng ký thẻ (phím C + PIN + quét)
- Lock ENGAGED/RELEASED (servo), buzzer quá nhiệt + alarm nhấp nháy
- 74HC595 LED đồng bộ trạng thái
- 4 mức ánh sáng: DARK / DIM / NORMAL / BRIGHT
- Device Overheat: YES/NO (thermistor DO)
