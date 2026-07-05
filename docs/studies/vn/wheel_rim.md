# Kiến trúc phần cứng và phần mềm vành lái

> Ngày nghiên cứu: 2026-07-02
> Phạm vi: thiết bị điện tử vành lái / vô lăng, liên kết cơ sở bánh xe, tích hợp máy chủ và kiến trúc bàn đạp liền kề.
> Bằng chứng: các dự án GitHub công cộng và tài liệu công cộng do người yêu cầu cung cấp.
> Ràng buộc: quan sát cộng đồng không phải là thông số kỹ thuật chính thức của Fanatec. Không khai thác firmware độc quyền hoặc bỏ qua bảo mật.
> Tài liệu liên quan: [sim_racing_research.md](./sim_racing_research.md), [wheel_base.md](./wheel_base.md), [accessories.md](./accessories.md), [tools.md](./tools.md), và [repos.md](./repos.md).

## 1. Tóm tắt điều hành

Phần này cung cấp một cái nhìn tổng quan cấp cao về kiến trúc vành lái, thiết lập vai trò của nó như là một nút I / O chứ không phải là một bộ điều khiển phản hồi lực. Nó được dự định để định hướng người đọc trước khi đi sâu vào chi tiết phần cứng và phần mềm.

Một vành lái hiện đại là một nút I / O nhúng quay. Nó quét các nút, mái chèo, bộ mã hóa quay, cần điều khiển và đầu vào ly hợp tương tự; nó nhận được lệnh hiển thị / LED; nó xác định khả năng của nó với cơ sở bánh xe; và nó trao đổi các khung bị chặn qua liên kết điện giải phóng nhanh. Nó không sở hữu điều khiển động cơ phản hồi lực. Cơ sở bánh xe sở hữu liệt kê USB, thu thập trục lái, xử lý phản hồi lực, mô-men xoắn động cơ, an toàn và tổng hợp dữ liệu vành / ngoại vi.

Bằng chứng cộng đồng mạnh nhất cho các vành tương thích Fanatec cũ mô tả đế bánh xe là bộ điều khiển / chủ SPI và vành là thiết bị ngoại vi / nô lệ SPI sử dụng tín hiệu 3.3 V. `Arduino_Fanatec_Wheel` thực hiện trao đổi 33 byte, kiểm tra CRC-8, trường bit nút và giải mã hiển thị trên phần cứng AVR. `Fanatec-Wheel-Barebone-Emulator` thêm các ràng buộc thời gian khởi động, nhiều thiết bị ngoại vi hơn và cảnh báo khả năng tương thích rõ ràng: cách tiếp cận AVR của nó được báo cáo là không tương thích với ClubSport DD / DD +, trong khi các cơ sở cũ hơn thông qua CSL DD và DD1 / DD2 được báo cáo là hoạt động. Đây là ranh giới thế hệ quan trọng, không phải là một điều chỉnh thời gian nhỏ.

Tại ranh giới máy chủ, `hid-fanatecff` hiển thị một kiến trúc riêng biệt: Các hiệu ứng đầu vào / FF của Linux được dịch thành các báo cáo USB HID dành riêng cho thiết bị, trong khi đèn LED, màn hình, điều chỉnh, nhận dạng bánh xe và các chức năng bàn đạp được hiển thị thông qua các sysfs Linux hoặc HIDRAW. `hid-fanatecff-tools` tiêu thụ từ xa trò chơi và điều khiển các đầu ra mở rộng đó. Do đó, chỉ đạo đầu vào / FFB và từ xa bảng điều khiển nên được coi là mặt phẳng dữ liệu riêng biệt nhưng phối hợp.

## 2. Ranh giới hệ thống

Phần này xác định ranh giới vật lý và logic của hệ thống vành lái. Nó minh họa các tương tác giữa vành lái, đế bánh xe và máy tính chủ, điều này rất quan trọng để hiểu chức năng cụ thể nằm ở đâu.

**Hình 2-1: Ranh giới hệ thống và luồng dữ liệu **

```nàng tiên cá
Lưu đồ LR
Trò chơi[Trò chơi / Trình mô phỏng] -->|Hiệu ứng FF| Máy chủ [Trình điều khiển hệ điều hành / Dịch vụ nhà cung cấp]
Trò chơi -->|telemetry| Telemetry[Telemetry Adapter]
Máy chủ <-->|USB HID / báo cáo nhà cung cấp| Base
Đo từ xa -->|LED/giá trị hiển thị| Máy chủ

cơ sở tiểu đồ thị [Cơ sở bánh xe]
USB [Thiết bị USB]
Main[Hệ thống MCU]
RimManager [Quản lý Rim]
FFB [Động cơ FFB]
MotorCtl [Điều khiển động cơ]
An toàn[Giám sát an toàn]
USB <--> Chính
Chính <--> RimManager
Chính -> FFB -> MotorCtl
An toàn -> MotorCtl
kết thúc

RimManager <-->|QR power + dữ liệu đồng bộ| Rim

tiểu đồ thị Vành [Vành lái]
RimMCU [MCU độ trễ thấp]
Đầu vào [Nút / Bộ mã hóa / Mái chèo / Cần điều khiển]
Đầu ra [Đèn LED hiển thị / RPM / Đèn LED trạng thái / Rumble]
Đầu vào -> RimMCU
RimMCU -> Đầu ra
kết thúc

Bàn đạp [Bàn đạp] -> Cổng cơ sở hoặc USB độc lập
```

### 2.1 Quyền sở hữu

| Chức năng | Vành | Cơ sở bánh xe | Phần mềm máy chủ |
|---|---|---|---|
| Quét điện nút / mái chèo | Chính | Nhận / lập bản đồ | Tiêu thụ các điều khiển hợp lý |
| Nhận dạng vành/khả năng | Vật tư | Khám phá/hợp lệ | Có thể hiển thị cấu hình |
| Hiển thị và đèn LED | Ổ đĩa phần cứng | Vận chuyển / điều khiển | Sản xuất các giá trị từ xa |
| Góc trục lái | Không, thông thường bộ mã hóa cơ sở | Chính | Tiêu thụ trục |
| Giải thích hiệu ứng FFB | Không | Chính | Gửi hiệu ứng |
| Dòng điện động cơ / PWM | Không | Chính và an toàn-quan trọng | Không |
| Cập nhật firmware | Rim bootloader nếu có | Coordinator/pass-through | Công cụ cập nhật |
| Cảm biến bàn đạp | Nút / đế bàn đạp riêng biệt | Tổng hợp | Tiêu thụ trục |

## 3. Phương pháp nghiên cứu và chất lượng bằng chứng

Phần này phác thảo các nguồn được sử dụng để lấy được kiến trúc này và các quy tắc để giải thích chúng. Nó cung cấp bối cảnh về độ tin cậy của các kết luận kỹ thuật được trình bày trong suốt tài liệu.

### 3.1 Nguồn được tư vấn

| Nguồn | Vai trò | Niềm tin vào kiến trúc | Giới hạn |
|---|---|---|---|
| `Arduino_Fanatec_Wheel` | DIY rim tương thích với các cơ sở cũ | Trung bình cao cho mô hình AVR / SPI đã được chứng minh | Triển khai cộng đồng; cũ; mô hình cụ thể |
| `Fanatec-Wheel-Barebone-Emulator` | Trình giả lập vành DIY mở rộng | Trung bình cao cho các bài học khởi động / thời gian / ngoại vi | Không tương thích rõ ràng với CS DD / DD + mới hơn |
| `Fanatec-Pinout` | Bộ sưu tập kết nối / pinout cộng đồng | Trung bình cho các giả thuyết điện | Không đầy đủ; DD1-trung tâm; không được nhà sản xuất chấp thuận |
| `ArduinoTec-Pedals` | Bộ điều khiển bàn đạp USB độc lập | Trung bình cho chuỗi tín hiệu bàn đạp | CSP V1-tập trung, không phải bằng chứng giao thức rim |
| `hid-fanatecff` | Nhân Linux Trình điều khiển USB/FF | Cao cho thiết kế phần mềm công cộng của trình điều khiển đó | Phía máy chủ; giao thức thiết bị vẫn dành riêng cho nhà cung cấp |
| `hid-fanatecff-tools` | Game telemetry to wheel outputs | Cao cho kiến trúc công cụ công cộng | Game/tính năng hạn chế; đường dẫn DBus đánh dấu chưa hoàn thành |
| GitHub Tìm kiếm Fanatec | Khám phá | Thấp | Thứ hạng tìm kiếm không phải là bằng chứng kỹ thuật |

### 3.2 Quy tắc diễn giải

- ** Quan sát **: trực tiếp hiện diện trong tài liệu kho lưu trữ hoặc nguồn.
- ** Inferred **: kết luận kỹ thuật được hỗ trợ bởi một số hành vi quan sát được.
- ** Đề nghị **: hướng dẫn thiết kế sản phẩm, không phải là một yêu cầu về nội bộ thương mại.
- Ghim chính xác, ý nghĩa byte, thời gian và ID vẫn dành riêng cho thế hệ cho đến khi được xác minh trên tài liệu phần cứng được phê duyệt.

## 4. Phân tích kho

Phần này đi sâu hơn vào các kho lưu trữ mã nguồn mở cụ thể được phân tích trong giai đoạn nghiên cứu. Nó nêu bật những phát hiện chính, điểm mạnh và điểm yếu của từng dự án, trích xuất các bài học thiết kế sản phẩm áp dụng cho việc triển khai mới.

# 4.1 `lshachar/Arduino_Fanatec_Wheel`

| Khía cạnh | Tìm kiếm |
|---|---|
| Mục tiêu | Bánh xe DIY được công nhận bởi cơ sở Fanatec |
| Bộ điều khiển | Arduino Uno/Nano; biến thể 5 V yêu cầu xử lý cấp độ |
| Rim link | Base-master, rim-slave SPI ở 3.3 V |
| Hành vi khung hình | Bộ đệm 33 byte, CRC-8, ánh xạ bit nút, hiển thị dữ liệu |
| Thiết bị ngoại vi | Nút bấm, D-pad analog, hiển thị chữ và số TM1637 |
| Sức mạnh | Sơ đồ/nguồn tham chiếu cụ thể cho kiến trúc kế thừa |
| Điểm yếu | Triển khai Arduino nguyên khối; gỡ lỗi nối tiếp có thể làm phiền thời gian nút; bằng chứng tương thích cũ |
| Bài học sản phẩm | Firmware sẽ giữ đường dẫn nhanh độc lập với việc ghi nhật ký và hiển thị kết xuất. |

### 4.2 `StuyoP/Fanatec-Wheel-Barebone-Emulator`

| Khía cạnh | Tìm kiếm |
|---|---|
| Mục tiêu | Nút vành tương thích nhỏ gọn với các nút, màn hình, đèn LED |
| Bộ điều khiển | ATmega328P trần ở nguồn gốc 3.3 V; bộ nạp khởi động bị loại bỏ cho tốc độ khởi động |
| Phần mở rộng | Thanh ghi thay đổi và các biến thể ngoại vi tùy chỉnh |
| Sức mạnh | Làm nổi bật trình tự tăng sức mạnh, thời hạn khởi động, dấu chân, tích hợp |
| Điểm yếu | Mỗi build có mã tùy chỉnh; giới hạn AVR; cơ sở không tương thích mới hơn |
| Bài học sản phẩm | Phần mềm mô-đun điều khiển khả năng và thời hạn khởi động được đo lường là bắt buộc. |

# 4,3 `FendtXerion3800 / Fanatec-Pinout`

| Khía cạnh | Tìm kiếm |
|---|---|
| Mục tiêu | Thu thập thông tin kết nối / pinout rải rác |
| Bảo hiểm | Khái niệm cơ bản, phanh tay, cần gạt, bàn đạp, E-stop, phím mô-men xoắn, data/CAN |
| Mô hình quan sát | Một số đầu vào cơ sở kéo lên đến 5 V và khẳng định thấp; đầu vào tương tự thường kéo dài 0-5 V |
| Hạn chế | Sơ đồ không đầy đủ và chủ yếu dựa trên DD1 |
| Bài học sản phẩm | Mỗi pin phải được xác minh bằng điện và chống lại sơ đồ được phê duyệt. |


4.4 "gotzl/hid-fanatecff"

| Khía cạnh | Tìm kiếm |
|---|---|
| Goal | Linux hỗ trợ đầu vào và phản hồi lực cho nhiều thiết bị USB Fanatec |
| Architecture | Mô-đun hạt nhân HID ngoài cây phân chia trên thiết bị, PID / FF và các tệp điều chỉnh |
| FFB | Hiệu ứng Linux tiêu chuẩn được dịch sang HID tùy chỉnh; hẹn giờ mặc định 2 ms không đồng bộ |
| Các tính năng mở rộng | Đèn LED, hiển thị, điều chỉnh, ID bánh xe, phạm vi, tế bào tải, đạp ầm ầm qua sysfs / HIDRAW |
| Khả năng tương thích | ID thiết bị có trạng thái ổn định/thử nghiệm khác nhau |
| Bài học sản phẩm | Hệ thống sẽ tách riêng FF API chung, vận chuyển nhà cung cấp và kiểm soát tính năng nâng cao. |

# 4.5 `gotzl/hid-fanatecff-tools`

| Khía cạnh | Tìm kiếm |
|---|---|
| Mục tiêu | Cầu trò chơi từ xa để mở rộng chức năng bánh xe |
| Đầu vào | UDP hoặc bộ điều hợp ánh xạ chia sẻ bộ nhớ/đặt tên cho mỗi trò chơi |
| Đầu ra | thao tác sysfs display/LED/load/tuning |
| Kiến trúc | Máy chủ chính cộng với bộ điều hợp / luồng máy khách cho mỗi trò chơi |
| Giới hạn | Phạm vi bảo hiểm trò chơi và trường đo từ xa khác nhau; Dịch vụ DBus được đánh dấu không hoạt động |
Phần mềm máy chủ nên bình thường hóa đo từ xa trò chơi trước khi đầu ra thiết bị, cách ly bộ điều hợp và lưu lượng hiển thị giới hạn tốc độ.

# # 4.6 `Alexbox364/F_Interface_AL`

| Khía cạnh | Tìm kiếm |
|---|---|
| Goal | Tự tay lái tùy chỉnh thông qua SPI với thanh ghi số |
| Bộ điều khiển | Arduino Nano cộng với bộ chuyển đổi mức và thanh ghi thay đổi 74HC165 |
| Khả năng mở rộng | Dễ dàng mở rộng phần cứng cho nhiều nút |
| Bài học sản phẩm | Sử dụng các thanh ghi / bộ mở rộng bên ngoài làm giảm số lượng pin MCU cần thiết nhưng yêu cầu quản lý thời gian / lưu trữ cẩn thận. |

## 5. Kiến trúc điện

Phần này nêu chi tiết các chiến lược phân phối và bảo vệ điện cho vành lái. Nó là điều cần thiết để ngăn ngừa thiệt hại điện gây ra bởi các mức điện áp không phù hợp, dòng điện xâm nhập hoặc kết nối không đúng cách.

**Hình 5-1: Cây kiến trúc điện **

```nàng tiên cá
Sơ đồ TD
BasePower [QR Supply] -> ESD [ESD / Surge / Giới hạn hiện tại]
USBPower [Debug USB Supply] -> USBProtect [Giới hạn hiện tại USB]
ESD -> ORing [Ideal-Diode Power Mux]
USBProtect -> ĐẶT HÀNG
ORing -> Reg [3.3 V Điều chỉnh / Giám sát]
Reg -> MCU
Reg -> Cảm biến
Reg -> Logic
Reg --> DisplayRail [Màn hình chuyển đổi tải / Đường ray LED]
Giám sát [Brownout / Reset Supervisor] -> MCU
```

# 5.1 Quy tắc thiết kế điện

Bằng chứng cộng đồng chỉ ra rằng mức điện áp không đúng có thể gây ra thiệt hại vĩnh viễn. Hơn nữa, thời gian khởi động ra lệnh thành công công nhận cơ sở.

- Phần cứng sẽ không bao giờ trực tiếp tham gia cơ sở và đường ray USB 5 V.
- Phần cứng phải sử dụng công tắc tải hoặc MUX diode lý tưởng để cách ly nguồn cung cấp.
- Phần cứng sẽ bị ràng buộc dòng xâm nhập từ tụ điện hiển thị và tải LED.
- MISO và tín hiệu đầu vào phải duy trì trở kháng cao trong khi không được cấp nguồn hoặc đặt lại.
- Thiết kế phải đo và đảm bảo thời gian đáp ứng từ khởi động đến lần đầu tiên hợp lệ qua giới hạn điện áp và nhiệt độ.

## 6. Kiến trúc phần cứng tham khảo

Phần này giải thích thiết kế phần cứng bên trong của bộ điều khiển vành lái. Nó kết nối các đầu vào vật lý (nút, bộ mã hóa) và đầu ra (đèn LED, màn hình) với bộ vi điều khiển trung tâm.

** Hình 6-1: Sơ đồ khối phần cứng điều khiển vành **

```nàng tiên cá
Sơ đồ TD
QR [Kết nối phát hành nhanh] -> Bảo vệ [ESD / Series Resistance / Level Protection]
Bảo vệ --> Nguồn điện [Lựa chọn / Quy định nguồn]
Bảo vệ <-->|SCLK / MOSI / MISO / CS hoặc liên kết đặc hiệu thế hệ| MCU

MCU -> Ma trận [Ma trận nút / GPIO trực tiếp]
MCU -> Bộ mã hóa [Bộ mã hóa quay]
MCU -> Paddles [Paddles kỹ thuật số Shift]
MCU -> Analog [ADC: Ly hợp / Cần điều khiển / Mái chèo tương tự]
MCU -> Expanders [Thay đổi đăng ký / GPIO Expanders]
MCU -> Hiển thị [Bộ điều khiển phân đoạn / OLED / LCD]
MCU -> Đèn LED [Trình điều khiển LED RPM / Cờ]
MCU -> Haptics [Trình điều khiển Rumble tùy chọn]
Gỡ lỗi [SWD / UPDI / ISP / UART] -> MCU
```

### 6.1 Trách nhiệm khối phần cứng

| Khối | Mục đích | Yêu cầu thiết kế |
|---|---|---|
| MCU | Deterministic link service và local I/O | Fast boot; peripheral-slave support; timers; ADC; enough GPIO/DMA |
| Đầu nối QR | Cơ khí, nguồn điện và truyền tín hiệu | Trình tự tiếp xúc, hao mòn, ESD, rung, không có điện áp không an toàn tiếp xúc
| Bảo vệ đầu vào | Hạn chế thoáng qua và tranh chấp | Khả năng tương thích 3.3 V; điện trở loạt; thiết bị ESD; tránh các cạnh chậm |
Lựa chọn nguồn điện Ngăn chặn nguồn cấp dữ liệu ngược Công tắc diode / tải lý tưởng; USB / gỡ lỗi và cách ly nguồn cung cấp cơ sở
| Ma trận / trình mở rộng | Tăng số lượng nút | Chiến lược ma, kéo lên, quét tất định |
| Mặt trước tương tự | Điều kiện Hall / đầu vào điện kế | Bảo vệ đường sắt, bộ lọc RC, đo tỷ lệ, chẩn đoán
| Giao diện hiển thị | Thiết lập hiện tại / đo từ xa | DMA nơi hữu ích; giới hạn khối lượng công việc làm mới |
| Trình điều khiển LED | Đầu ra trực quan được điều khiển hiện tại | Dòng điện trên mỗi kênh, ngân sách nhiệt, độ sáng toàn cầu |
Lái xe xúc giác Lái xe rung động nếu được trang bị Transitor / trình điều khiển, bảo vệ flyback, giới hạn hiện tại
| Cổng lập trình | Sản xuất và phục hồi | Chính sách khóa truy cập và sản xuất được bảo vệ |

### 6.2 Lựa chọn MCU

Nguyên mẫu cộng đồng sử dụng ATmega328P / Arduino Uno / thiết bị lớp Nano. Điều đó chứng minh tính khả thi cho các liên kết cũ hơn nhưng không nên xác định một thiết kế sản xuất mới.

| Yêu cầu MCU | Lý do |
|---|---|
| Hoạt động 3.3 V bản địa | Tránh các nguy cơ thay đổi mức độ và tranh chấp |
| Deterministic SPI ngoại vi với DMA / gián đoạn | Dịch vụ cơ sở thăm dò ý kiến mà không cần chặn |
| Fast reset-to-response | Ngăn chặn cơ sở tuyên bố vành không tương thích |
| Đủ GPIO/ADC/timers | Nút, bộ mã hóa, paddles analog, đèn LED |
| Tăng tốc CRC tùy chọn | Hữu ích, không bắt buộc đối với khung nhỏ |
| Chất lượng dao động bên trong hoặc tinh thể | Đáp ứng thời gian liên kết qua nhiệt độ |
| Khả năng khởi động/cập nhật an toàn | Tính xác thực và phục hồi sản xuất |
| Brownout / thiết lập lại giám sát | Ngăn chặn phản ứng dị dạng trong quá trình sập đường sắt |

> ** Lưu ý: ** MCU phải là Cortex-M0 + / M33 hiện đại hoặc MCU công suất thấp tương đương với 3.3 V I / O gốc, chế độ ngoại vi xác định, DMA và đường dẫn khởi động được ghi lại. Việc lựa chọn phải dựa trên các yêu cầu liên kết được đo lường, không bắt chước thương hiệu.

## 7. Đầu vào và đầu ra

Phần này nêu chi tiết cách các đầu vào vật lý của người dùng được dịch sang trạng thái logic và cách dữ liệu từ xa bên ngoài được hiển thị trên màn hình và đèn LED. Nó bao gồm ranh giới giữa tín hiệu phần cứng và xử lý phần sụn.

### 7.1 Mua lại đầu vào

**Hình 7-1: Đường ống xử lý đầu vào **

```nàng tiên cá
Lưu đồ LR
Điện [GPIO / Matrix / ADC] -> Mẫu [Lấy mẫu định kỳ]
Mẫu -> Xác thực [Kiểm tra trạng thái phạm vi / không thể]
Xác thực -> Bộ lọc [Debounce / Hysteresis]
Bộ lọc -> Bản đồ [Lập bản đồ nút / trục logic]
Bản đồ --> Ảnh chụp nhanh [Ảnh chụp phản ứng vành nguyên tử]
```

| Đầu vào | Phần cứng | Xử lý phần cứng |
|---|---|---|
| Nút nhấn | GPIO hoặc ma trận trực tiếp | Kéo cấu hình, gỡ lỗi, phát hiện bị kẹt |
| Shift paddle | Microswitch hoặc chuyển đổi Hall | Báo cáo độ trễ thấp và báo cáo cạnh / trạng thái |
| Bộ mã hóa quay | Địa chỉ liên lạc / Hội trường Quadrature | Bảng chuyển đổi, từ chối trả lại, tích lũy |
Công tắc Funky / D-pad Công tắc rời rạc hoặc thang điện trở Phân loại hướng và độ trễ
Mái chèo ly hợp tương tự Cảm biến Hall / điện thế kế Bộ lọc ADC, hiệu chuẩn tối thiểu / tối đa, vùng chết
| Cần điều khiển ngón tay cái | Trục Hall / điện thế kép | ADC, hiệu chuẩn trung tâm, ánh xạ tròn / vuông |
| Chuyển đổi chế độ | Đa vị trí kỹ thuật số / analog | Trạng thái chế độ ổn định và ánh xạ khả năng |

Một số đầu vào này là quay hoặc tương tự. Một bộ mã hóa quay báo cáo các bước gia tăng như một cặp tứ giác A / B, trong đó thứ tự pha của hai kênh mã hóa hướng; một mái chèo ly hợp tương tự hoặc cần điều khiển ngón tay cái thay vào đó sử dụng cảm biến Hall hoặc chiết áp đọc qua ADC. Sự so sánh dưới đây tương phản chiết áp dựa trên tiếp xúc với đĩa mã hóa không tiếp xúc và dạng sóng A / B của nó.

![Potentiometer vs. bộ mã hóa quay](../assets/potentiometer_and_encoder.svg)

### 7.2 Kiến trúc đầu ra

| Đầu ra | Trình điều khiển | Nguồn gốc dữ liệu | Chính sách cập nhật |
|---|---|---|---|
| Hiển thị số / phân đoạn | TM1637 hoặc bộ điều khiển khác | Menu thiết lập cơ sở hoặc từ xa máy chủ | Chỉ cập nhật khi thay đổi |
| Đèn LED RPM | GPIO, thanh ghi thay đổi, trình điều khiển LED | Từ xa trò chơi thông qua trình điều khiển cơ sở / máy chủ | Khung giới hạn tốc độ
| Đèn LED cờ / trạng thái | Trình điều khiển có địa chỉ hoặc dòng điện không đổi | Trạng thái trò chơi / cơ sở | Cảnh báo ưu tiên trên trang trí |
| OLED / LCD | Bộ điều khiển hiển thị SPI / QSPI | Từ xa / cấu hình phong phú | DMA, FPS giới hạn, cập nhật một phần |
| Động cơ rung | Trình điều khiển MOSFET / bóng bán dẫn | Trượt bàn đạp / ABS hoặc lệnh cơ sở | PWM cộng với giới hạn nhiệt / dòng điện |

`hid-fanatecff` hiển thị đèn LED bánh xe và hiển thị riêng biệt với đường dẫn đầu vào / FF tiêu chuẩn. `hid-fanatecff-tools` tiêu thụ từ xa UDP / chia sẻ bộ nhớ và viết các điều khiển đó. Việc thực hiện sẽ hỗ trợ phân chia giữa lưu lượng lái / FFB thời gian thực và lưu lượng hiển thị nỗ lực tốt nhất.

## 8. Truyền thông Rim-to-Base

Phần này xem xét việc trao đổi dữ liệu giữa vành lái và đế bánh xe. Nó thảo luận về các giao thức kế thừa được quan sát trong các dự án cộng đồng và xác định các yêu cầu cho một liên kết truyền thông mạnh mẽ trong các thiết kế hiện đại.

### 8.1 Mô hình di sản / cộng đồng được quan sát

Việc triển khai cộng đồng `Arduino_Fanatec_Wheel` cung cấp cái nhìn sâu sắc về các kiến trúc cũ hơn. Những sự kiện này áp dụng cho việc triển khai cộng đồng được thử nghiệm và không phải là một hợp đồng Fanatec phổ quát.

**Hình 8-1: Chuỗi giao dịch SPI Rim-to-Base **

```nàng tiên cá
trình tựSơ đồ
cơ sở tham gia như cơ sở bánh xe (bộ điều khiển)
người tham gia RimISR là Rim Link ISR / DMA
Tham gia RimTask như Rim Background Tasks

RimTask->>RimTask: Quét / xóa nợ đầu vào
RimTask->>RimISR: Xuất bản phản ứng bất biến tiếp theo
Cơ sở->>RimISR: Khẳng định CS
vòng lặp Byte giao dịch cố định
Base->>RimISR: Đồng hồ lệnh / hiển thị byte trên MOSI
RimISR - >> Cơ sở: Trả về danh tính / byte đầu vào trên MISO
kết thúc
Cơ sở->>RimISR: Deassert CS
RimISR->>RimISR: Xác nhận chiều dài nhận được / CRC
RimISR->>RimTask: Xuất lệnh đầu ra đã được xác thực
```

# # 8.2 Ranh giới thế hệ

| Bằng chứng gia đình sản phẩm | Tình trạng cộng đồng | Kết luận kỹ thuật |
|---|---|---|
| Các cơ sở thời CSW cũ hơn | Được trình diễn bởi các dự án Arduino | Trình giả lập SPI cũ có thể được sử dụng làm tài liệu tham khảo trong phòng thí nghiệm |
| CSL DD / DD1 / DD2 | Báo cáo dự án Barebone làm việc | Thời gian và các chi tiết điện sẽ được xác minh trên mỗi firmware cơ sở |
| ClubSport DD / DD+ | Dự án Barebone báo cáo không tương thích | Giả sử thay đổi thời gian, giao thức hoặc bảo mật cho đến khi được làm rõ chính thức |
| Podium DD (2026) | No evidence from the cited legacy emulator projects | Treat as unsupported until an approved current-generation specification or verified implementation exists |
| Cơ sở / vành trong tương lai | Không rõ | Yêu cầu thông số kỹ thuật được phê duyệt và đàm phán khả năng |

Khả năng tương thích thương mại hiện tại có ranh giới cơ học riêng biệt. Bánh xe và đế cửa hàng Fanatec là QR2 theo mặc định kể từ 2026-02-16, trong khi QR1 bị ngưng. QR2 yêu cầu kết hợp các thành phần Base-Side và Wheel-Side; quan sát SPI cũ không thiết lập sự phù hợp cơ học QR2, phê duyệt mô-men xoắn hoặc khả năng tương thích giao thức hiện tại.

![Phát hành nhanh: khớp nối cơ khí và cầu điện / dữ liệu] (./quick_release_exploded.svg)

QR là nơi liên kết điện của vành vật lý vượt qua cơ sở: một nửa Bánh xe-Side trên vành giao phối với một nửa Base-Side trên trục, mang mô-men xoắn cơ học và, cho một vành thông minh, sức mạnh và dữ liệu trên các tiếp điểm chốt lò xo. Cả hai nửa phải là cùng một thế hệ để giao phối, vì vậy một mô hình bu lông vành phù hợp một mình không bao giờ chứng minh khả năng tương thích QR hoặc giao thức.

Cấp phép nền tảng cũng tách biệt với liên kết dữ liệu vành. Khả năng tương thích Xbox đến từ vô lăng hoặc trung tâm được cấp phép, trong khi khả năng tương thích PlayStation đến từ cơ sở bánh xe được cấp phép. Không suy ra thông điệp xác thực giao diện điều khiển từ các dự án SPI cộng đồng.

Sự không tương thích có thể phản ánh độ trễ khởi động, thời gian giao dịch, hành vi điện, khung, xác thực hoặc thay đổi kiến trúc. Phần cứng và phần sụn không được ép xung một cách mù quáng hoặc chụp lại để bỏ qua sự không phù hợp của thế hệ.

### 8.3 Yêu cầu liên kết cho một thiết kế mới

| Yêu cầu | Hành vi được đề xuất |
|---|---|
Mức điện Thiết kế sẽ sử dụng điện áp mục tiêu gốc; một ổ đĩa 5 V sẽ không kết nối với đầu vào 3,3 V.
| Khởi động | Pin MISO phải duy trì trở kháng cao cho đến khi được chọn; liên kết phải sẵn sàng trong thời hạn đo được. |
| Framing | Khung phải bao gồm một tiêu đề cố định, phiên bản, loại, độ dài, tải trọng và CRC nơi quyền sở hữu giao thức cho phép. |
Giao thức nên bao gồm một số thứ tự hoặc bộ đếm giao dịch.
| Xử lý lỗi | Firmware sẽ thả các khung RX không hợp lệ, cung cấp đáp ứng dự phòng xác định và bộ đếm lỗi tăng dần. |
| Khả năng tương thích | Vành sẽ cung cấp danh tính, bitmap khả năng và phiên bản chính / phụ cho cơ sở. |
| Hot-plug | Hệ thống sẽ chịu được tiếp xúc bị trả lại, thực hiện thiết lập lại an toàn khi tắt và ngăn chặn chốt bus. |
| Timeout | Firmware sẽ xóa các điều khiển tạm thời và ngừng thay đổi đầu ra khi liên kết bị lỗi. |

## 9. Kiến trúc phần mềm tham khảo

Phần này mô tả thiết kế phần mềm tổng thể của vi điều khiển chạy bên trong vành lái. Nó tập trung vào việc tách các tác vụ truyền thông thời gian thực khỏi các tác vụ xử lý nền để đảm bảo sự ổn định truyền thông.

**Hình 9-1: Dòng thực thi firmware **

```nàng tiên cá
Sơ đồ TD
Đặt lại -> SafePins [Trạng thái GPIO và MISO an toàn]
SafePins -> Tự kiểm tra [Đồng hồ / RAM / NVM / I / O Tự kiểm tra]
SelfTest --> Nhận dạng [Tải nhận dạng và khả năng]
Danh tính -> Liên kết [Arm Rim-Link Peripheral]

biểu đồ con FastPath [Đường dẫn nhanh bị gián đoạn / DMA]
Liên kết -> RX [Nhận khung cơ sở]
RX -> Xác nhận [Chiều dài / Trình tự / CRC]
Xác thực --> PublishOut [Xuất bản lệnh LED / Hiển thị]
Ảnh chụp nhanh [Ảnh chụp đầu vào được xây dựng sẵn] -> TX [Khung vành truyền]
kết thúc

nền tiểu đồ thị [Nền bị chặn]
Quét [Quét nút / mã hóa]
ADC [Mua lại Analog]
Trả nợ [Debounce / Hysteresis]
Soạn [Soạn ảnh chụp nhanh tiếp theo + CRC]
Render [Hiển thị / LED Rendering]
Sức khỏe [Sức khỏe / Lỗi truy cập]
Quét -> Nợ -> Soạn
ADC -> Nợ
Xuất bản -> Kết xuất
Sức khỏe -> Soạn
kết thúc

TX -> Liên kết
```

### 9,1 Mô-đun

| Mô-đun | Trách nhiệm | Ngữ cảnh thực thi |
|---|---|---|
| Khởi động / khởi động | Ghim an toàn, tự kiểm tra, nhận dạng tải, sẵn sàng liên kết nhanh | Đặt lại đường dẫn |
Liên kết ISR / DMA Di chuyển khung cố định, duy trì chỉ số byte, xử lý ranh giới CS Ưu tiên vành cao nhất
| Trình xác thực khung | Độ dài, CRC, trình tự, sự tỉnh táo của lệnh | Đuôi ISR hoặc nhiệm vụ ưu tiên cao |
| Thu nhận đầu vào | Ma trận quét, GPIO, ADC, bộ mã hóa | Hẹn giờ / tác vụ định kỳ |
| Giải phóng/lọc | Chuyển đổi các mẫu điện thành trạng thái logic ổn định | Nhiệm vụ định kỳ |
| Snapshot composer | Tạo phản hồi bất biến tiếp theo | Giữa các giao dịch |
| Bộ điều phối đầu ra | Giải mã các lệnh hiển thị/LED/haptic đã được xác thực | Nhiệm vụ |
| Danh tính thiết bị | Model, bản sửa đổi, khả năng, phiên bản firmware | Startup/query |
| Chẩn đoán | Lỗi CRC, tràn, lý do thiết lập lại, lỗi đường sắt | Bối cảnh |
| Bộ nạp khởi động | Xác minh/cập nhật/phục hồi ảnh | Chế độ cập nhật an toàn mô-men xoắn riêng biệt |

### 9.2 Mô hình đệm

Firmware sẽ tách các tác vụ nền chậm khỏi thời hạn liên kết nhanh.

- Firmware phải sử dụng bộ đệm kép để truyền.
- Ảnh chụp nhanh TX đang hoạt động sẽ không thay đổi trong một giao dịch cơ sở.
- Nhiệm vụ đầu vào xây dựng ảnh chụp nhanh không hoạt động.
- Việc hoán đổi con trỏ/chỉ số nguyên tử chỉ được thực hiện tại một ranh giới giao dịch.
- Firmware phải xác thực khung RX trước khi cập nhật trạng thái hiển thị hoặc cấu hình.
- Khi CRC / chiều dài thất bại, firmware sẽ giữ lại trạng thái đầu ra không nguy hiểm hợp lệ cuối cùng và bộ đếm chẩn đoán gia tăng.

## 10. Kiến trúc phần mềm máy chủ

Phần này bao gồm phần mềm chạy trên máy chủ (ví dụ: trình điều khiển và tiện ích Linux). Nó cung cấp cái nhìn sâu sắc về cách hệ điều hành và trò chơi tương tác với đế bánh xe và, bằng cách mở rộng, vành lái.

**Hình 10-1: Kiến trúc phần mềm máy chủ **

```nàng tiên cá
Sơ đồ TD
Trò chơi -->|Hiệu ứng FF của Linux| InputAPI[Linux Input / evdev]
InputAPI -> Driver [mô-đun hạt nhân ẩnfanatecff]
Trình điều khiển --> báo cáo USB HID tùy chỉnh, hẹn giờ mặc định 2 ms không đồng bộ
Cơ sở -->|trục/nút| Trình điều khiển --> InputAPI

Trò chơi -->|UDP / shared memory telemetry| Công cụ[hid-fanatecff-tools]
Công cụ -->|sysfs: LEDs/display/tuning| Trình điều khiển

Proton[Wine / Proton HIDRAW] <-->|Mô tả và báo cáo PID| Trình điều khiển
Proton -> |Fanatec SDK các chức năng nâng cao được hỗ trợ | Trình điều khiển
```

### 10.1 Trách nhiệm của tài xế

Lớp | Trách nhiệm |
|---|---|
| Hệ thống con đầu vào Linux | Các trục tiêu chuẩn, các nút và API hiệu ứng FF |
| "hid-fanatecff" core | So khớp thiết bị, xử lý báo cáo, tích hợp mô tả |
| FF dịch | Chuyển đổi hiệu ứng Linux/PID để báo cáo thiết bị cụ thể; hẹn giờ cập nhật không đồng bộ |
Tích hợp sysfs Phạm vi, ID bánh xe, điều chỉnh, đèn LED, hiển thị, tải bàn đạp / đổ nát nơi được hỗ trợ
| Đường dẫn HIDRAW | Cho phép Wine/Proton truy cập trực tiếp kiểu HID và các tính năng phụ thuộc SDK |
| Công cụ đo từ xa | Dịch từ xa UDP/shared-memory theo trò chơi sang giá trị LED/display |

Trình điều khiển máy chủ không phải là firmware vành lái. Nó giao tiếp với cơ sở qua USB; cơ sở sau đó làm trung gian cho vành kèm theo. Sự tách biệt này rất quan trọng khi gỡ lỗi các tính năng cụ thể (ví dụ: "các nút hoạt động nhưng đèn LED thì không").

# 11. Hành vi thời gian thực

Phần này xác định các ưu tiên thực hiện và mục tiêu tần số cho các tác vụ firmware. Những hạn chế về thời gian này đảm bảo vành đáp ứng thời hạn giao tiếp cơ bản trong khi gỡ lỗi đầy đủ đầu vào và hiển thị làm mới.

Các yêu cầu về thời gian được liệt kê dưới đây là các điểm khởi đầu kỹ thuật, không phải là yêu cầu chính thức của Fanatec.

| Hoạt động | Mục tiêu bắt đầu | Ưu tiên | Ghi chú |
|---|---|---|---|
| Liên kết vành ISR / DMA | Mọi giao dịch cơ bản | Cao nhất | Thời hạn bắt nguồn từ hành vi đồng hồ / CS được đo |
| Quét nút / mã hóa | 500–1000 Hz | Độ trễ cao | Độ trễ thấp; bị chặn debounce |
| Analog acquisition | 500–1000 Hz | High | Timer-triggered ADC; filtered snapshot |
| Chụp sáng tác | Một lần mỗi lần quét hoặc khi thay đổi | Cao | Hoàn thành trước khi giao dịch tiếp theo |
| Cập nhật LED | 50–200 Hz | Trung bình | Giới hạn tốc độ; cập nhật khi thay đổi |
| Hiển thị phân đoạn | 20–100 Hz | Trung bình | Con người nhìn thấy được; không liên kết quan trọng |
| Rich LCD | 20-60 FPS | Trung bình / thấp | DMA và cập nhật một phần |
| Chẩn đoán | 1-10 Hz cộng với bộ đếm | Thấp | Không bao giờ nối tiếp đồng bộ trong đường dẫn nhanh |
| Ghi NVM | Trên cam kết rõ ràng | Thấp nhất | Nguyên tử, giới hạn hao mòn, giao dịch bên ngoài |

## 12. Máy nhà nước

Phần này mô tả chính thức các trạng thái hoạt động của vành lái và quá trình giao dịch liên kết. Nó phác thảo cách vành khởi động, chuyển sang trạng thái hoạt động và phục hồi sau lỗi.

# # Vòng đời 12,1

** Hình 12-1: Máy trạng thái vòng đời Rim **

```nàng tiên cá
stateSơ đồ-v2
[*] -> Đặt lại
Đặt lại -> Tự kiểm tra
Tự kiểm tra -> LinkReady: vượt qua
Tự kiểm tra -> Lỗi: thất bại
LinkReady --> Xác định: giao dịch cơ bản
Xác định -> Hoạt động: nhận dạng được chấp nhận
Xác định -> CompatibilityFault: bị từ chối / timeout
Hoạt động -> Bị xuống cấp: Ngưỡng CRC / lỗi
Đã xuống cấp -> Hoạt động: cửa sổ khôi phục sạch
Hoạt động -> Ngắt kết nối: mất điện / liên kết
Suy thoái -> Ngắt kết nối
Ngắt kết nối -> Đặt lại: khôi phục nguồn
CompatibilityFault --> Recovery: yêu cầu dịch vụ
Phục hồi -> Đặt lại: hình ảnh đã xác minh
```

# 12,2 Trạng thái giao dịch

**Hình 12-2: Máy trạng thái giao dịch **

```nàng tiên cá
stateSơ đồ-v2
[*] --> Nhàn rỗi
Nhàn rỗi -> Lựa chọn: CS khẳng định
Chọn -> Chuyển: đồng hồ đầu tiên
Chuyển -> Chuyển: byte tiếp theo
Chuyển -> Xác nhận: CS deasserted
Xác nhận -> Xuất bản: chiều dài và CRC hợp lệ
Xác thực -> Lỗi: không hợp lệ
Xuất bản --> Nhàn rỗi
Lỗi -> Nhàn rỗi: đếm và giữ trạng thái an toàn
```

## 13. Kiến trúc sản phẩm được đề xuất

Phần này củng cố các khuyến nghị kỹ thuật thành một thiết kế hệ thống mạch lạc. Nó được dành cho những người thực hiện thiết kế một vành sẵn sàng sản xuất mới chứ không phải là một dự án có sở thích.

# 13.1 Phần cứng

1. Thiết kế phải sử dụng một MCU 3.3 V gốc với SPI chế độ ngoại vi xác định hoặc giao diện cụ thể cho thế hệ được phê duyệt.
2. Phần cứng phải sử dụng một MUX / giới hạn dòng điện độc lập cho QR và gỡ lỗi nguồn cung cấp USB.
3. Phần cứng sẽ cung cấp tín hiệu QR được bảo vệ đầu vào với hành vi đặt lại trở kháng cao.
4. Thiết kế chỉ nên sử dụng bộ mở rộng GPIO / thanh ghi thay đổi khi độ trễ quét và chế độ lỗi được chấp nhận.
5. Phần cứng phải sử dụng công tắc nguồn LED / hiển thị riêng biệt và ngân sách hiện tại.
6. Các đầu vào tương tự phải được bảo vệ, lọc, hiệu chuẩn và chẩn đoán.
7. Phần cứng phải cung cấp đầu nối gỡ lỗi sản xuất cộng với cơ chế khôi phục được kiểm soát.

# 13.2 Lớp phần mềm

** Hình 13-1: Lớp phần mềm được đề xuất **

```nàng tiên cá
Sơ đồ TD
Ứng dụng [Tính năng ứng dụng] -> Model [Logical Rim Model]
Mô hình --> Đầu vào [Dịch vụ đầu vào]
Mô hình --> Đầu ra [Dịch vụ đầu ra]
Mô hình --> Nhận dạng [Nhận dạng / Khả năng]
Đầu vào -> HAL [HAL / BSP]
Đầu ra --> HAL
Danh tính --> Giao thức [Versioned Rim Protocol Adapter]
Giao thức -> Liên kết [Trình điều khiển liên kết ISR / DMA]
Liên kết -> HAL
Sức khỏe [Chẩn đoán / Watchdog / Đặt lại Lý do] -> Mô hình
Boot [Verified Bootloader] -> Ứng dụng
```

# 13.3 Quy tắc thiết kế không thể thương lượng

- Firmware không thực hiện ghi nhật ký nối tiếp hoặc hiển thị công việc trong ISR liên kết.
- Firmware không cho phép bộ đệm phản hồi có thể thay đổi trong quá trình giao dịch.
- Phần cứng không được kết nối trực tiếp đầu ra 5 V với tín hiệu cơ sở chưa được xác minh.
- Thiết bị phần cứng phải đảm bảo không có sự truyền dữ liệu ngược giữa nguồn USB và QR.
- Phần vững không được giả định rằng một bản sắc / khung kế thừa hoạt động trên tất cả các cơ sở.
- Sự cố mất điện từ xa chủ nhà không phải là báo động nút chặn.
- Lỗi vành không làm thay đổi giới hạn an toàn động cơ cơ sở.
- Một bản cập nhật firmware sẽ không để lại chân bus chủ động điều khiển trong quá trình thiết lập lại / phục hồi.

## 14. An toàn, mạnh mẽ và bảo mật

Phần này nêu bật những rủi ro quan trọng liên quan đến phần cứng và phần sụn. Nó cung cấp các điều khiển cụ thể để giảm thiểu thiệt hại về điện, lỗi giao thức và sử dụng sai tiềm năng.

Một vành lái không nên chứa quyền lực mô-men xoắn. Tuy nhiên, nhận dạng vành sai hoặc điều khiển bị hỏng có thể gây ra thay đổi chế độ bất ngờ, do đó, vấn đề toàn vẹn đầu vào và nhận dạng. Người dùng sẽ không sử dụng trình giả lập cộng đồng để bỏ qua xác thực bảng điều khiển được cấp phép, hạn chế mô-men xoắn, khóa an toàn hoặc bảo mật sản phẩm.

### 14.1 Rủi ro về điện và vận hành

Rủi ro | Kiểm soát |
|---|---|
| Tranh chấp xe buýt | Điện áp tự nhiên, bộ chuyển mức an toàn hướng, đặt lại Z cao, điện trở loạt
| Nguồn cấp điện | Công tắc mux / tải diode lý tưởng; không bao giờ tham gia nguồn cung cấp |
| QR liên hệ bị trả lại | Giám sát Brownout, sự hiện diện bị từ chối, phục hồi giao dịch |
| Khởi động muộn | Khởi động tối thiểu, không có độ trễ khởi động không cần thiết, đáp ứng nhận dạng được tính toán trước |
| Bão CRC | Bộ đếm lỗi, đồng bộ hóa lại, trạng thái suy giảm giới hạn |
| Nút bị kẹt | Chẩn đoán độ bền / độ bền bị kẹt; trạng thái nhìn thấy vật chủ được hỗ trợ |
| Bộ giải mã bị trả lại | Bộ giải mã bảng chuyển tiếp và số lượng chuyển đổi bất hợp pháp |
| Hiển thị đói khối lượng công việc | DMA / giới hạn tốc độ và ưu tiên thấp hơn so với liên kết / đầu vào |
| Cập nhật bị hỏng | Đường dẫn phục hồi và hình ảnh được xác thực / kiểm tra toàn vẹn |
| Cơ sở mới không rõ | Từ chối khả năng tương thích thay vì đoán điện / giao thức không an toàn |

# 15. Chiến lược Xác minh

Phần này phác thảo cách kiểm tra phần cứng và phần sụn để đảm bảo độ tin cậy. Nó nêu chi tiết một trình tự băng ghế dự bị từng bước và một đơn vị trải rộng ma trận thử nghiệm toàn diện cho các cấp hệ thống.

# # Trình tự băng ghế dự bị 15.1

| Bước | Hành động | Ghi chú / ràng buộc |
|---|---|---|
1 Xác minh hướng kết nối và mọi đường ray có nguồn cung cấp giới hạn hiện tại Không có cơ sở được kết nối
2 Xác minh trở kháng pin không được cấp nguồn / đặt lại và không có nguồn cấp dữ liệu USB / QR Ngăn chặn sự tranh chấp điện
| 3 | Xác minh thời gian sẵn sàng khởi động đến liên kết qua điện áp và nhiệt độ | Đảm bảo liệt kê đáng tin cậy |
| 4 | Sử dụng một giao thức mô phỏng hoặc logic-level fixture | Yêu cầu trước khi kết nối cơ sở thương mại |
| 5 | Xác nhận ranh giới CS, chế độ đồng hồ, thiết lập/giữ, độ trễ phản hồi và CRC | Xác nhận độ ổn định liên kết |
| 6 | Tiêm các khung cắt ngắn, đồng hồ bổ sung, lỗi CRC, kết nối lại nhanh và mất điện | Chứng minh xử lý lỗi |
| 7 | Xác thực mọi đầu vào để trả lại, trạng thái bị kẹt, mở / ngắn và kích hoạt đồng thời | Chứng minh độ bền đầu vào |
| 8 | Stress LED / hiển thị lưu lượng truy cập trong khi chứng minh không có giao dịch liên kết bị bỏ lỡ | Chứng minh lịch trình thời gian thực |
9 Kiểm tra gián đoạn cập nhật firmware và phục hồi Đảm bảo độ tin cậy của trường
| 10 | Kiểm tra từng tổ hợp base/firmware được hỗ trợ | Xử lý kết quả như một ma trận tương thích |

### Ma trận thử nghiệm 15.2

| Lớp kiểm tra | Ví dụ |
|---|---|
| Đơn vị máy | CRC, bộ phân tích khung, sơ đồ nút, bảng chuyển tiếp bộ mã hóa, hiệu chuẩn |
| Đơn vị MCU | HAL giả, máy trạng thái, lược đồ NVM |
| Thành phần | Thời gian ngoại vi SPI, hoàn thành DMA, độ chính xác ADC, trình điều khiển hiển thị |
| HIL | Trình mô phỏng giao dịch cơ bản, power bounce, khung sai dạng |
| Hệ thống | Các cơ sở được hỗ trợ, vành, trình điều khiển máy chủ, trò chơi, công cụ từ xa |
| Ngâm | Rung, chu kỳ QR lặp lại, phạm vi nhiệt, lưu lượng nút / LED dài

# 15 Thiết bị đo đạc

- Bộ phân tích logic: CS, SCLK, MOSI, MISO, link-ready GPIO.
- Oscilloscope: đường ray, inrush, reset, level-shifter cạnh, backfeed hiện tại.
- Bộ đếm firmware: giao dịch, lỗi CRC / chiều dài, tràn, quét bỏ lỡ, lý do đặt lại.
- Dấu vết máy chủ: Báo cáo USB, hoạt động sysfs / HIDRAW, nhịp đầu vào từ xa.

## 16. Lộ trình thực hiện

Phần này xác định trình tự logic của các bước cần thiết để xây dựng và xác nhận một sản phẩm vành lái mới. Nó giúp các nhóm dự án theo dõi tiến độ từ nghiên cứu ban đầu đến xác nhận cuối cùng.

| Bước | Hành động | Ghi chú / ràng buộc |
|---|---|---|
| 1 | Có được thông số kỹ thuật điện QR được phê duyệt và danh sách thế hệ cơ sở được hỗ trợ. | Nền tảng cho phần cứng |
2 Yêu cầu chụp: đầu vào, màn hình, đèn LED, trục analog, cập nhật, thời hạn khởi động. Định nghĩa sản phẩm
| 3 | Xây dựng một thiết bị xác minh kết nối / pinout không được cấp nguồn. | An toàn điện trước
| 4 | Triển khai mô hình đầu vào/đầu ra độc lập với giao thức và kiểm tra đơn vị. | Firmware logic |
| 5 | Triển khai bộ chuyển đổi rim-link phía sau một giao diện được phiên bản. | Giao tiếp cơ sở |
| 6 | Đưa lên hành vi sức mạnh / thiết lập lại / trở kháng cao trước khi lưu lượng liên kết. | Ngăn chặn thiệt hại cơ bản |
| 7 | Xác nhận trao đổi khung cố định với một mô phỏng, sau đó cơ sở mục tiêu giới hạn hiện tại. | Kiểm tra liên kết |
| 8 | Chỉ thêm các tính năng hiển thị / LED sau khi tỷ lệ lỗi đầu vào / liên kết bằng không khi bị căng thẳng. | Tránh hồi quy thời gian thực |
| 9 | Thực hiện chẩn đoán, phục hồi cập nhật và báo cáo tương thích. | Tính năng tin cậy |
| 10 | Xuất bản một ma trận base/firmware/rim đã thử nghiệm với các tổ hợp không được hỗ trợ rõ ràng. | Tài liệu người dùng cuối |

## 17. Câu hỏi chưa được giải quyết

Phần này liệt kê các câu hỏi mở và các phụ thuộc phải được giải quyết trước khi hoàn thiện thiết kế sản phẩm.

- Những thế hệ vành lái và cơ sở bánh xe chính xác nào là mục tiêu của sản phẩm?
- Có yêu cầu tương thích với ClubSport DD / DD + không? Nếu có, thông số kỹ thuật giao diện được phê duyệt ở đâu?
- Điện áp / dòng điện của đường ray QR, mức tín hiệu, thứ tự liên lạc và yêu cầu ESD là gì?
- Thời hạn phản hồi đầu tiên được đo là bao lâu?
- Là SPI di sản liên kết vành, một giao thức mới hơn, hoặc nhiều phương tiện vận chuyển thương lượng?
- Những lĩnh vực nhận dạng / năng lực nào được pháp luật và kỹ thuật cho phép đối với một sản phẩm mới?
- Số lượng cần thiết cho các nút, bộ mã hóa, mái chèo tương tự, đèn LED, loại màn hình và haptics?
- Firmware rim có được cập nhật thông qua cơ sở, USB / gỡ lỗi trực tiếp hay cả hai không?
- Nền tảng máy chủ và API từ xa nào phải hỗ trợ đầu ra hiển thị / LED?
- Những yêu cầu an toàn, EMC, môi trường và cấp phép giao diện điều khiển nào được áp dụng?

## 18. Tài liệu tham khảo

Phần này cung cấp các liên kết đến các kho cộng đồng và các tiêu chuẩn kỹ thuật đã thông báo cho tài liệu này.

### 18.1 Kho chính

- [lshachar/Arduino_Fanatec_Wheel] (https://github.com/lshachar/Arduino_Fanatec_Wheel) - vành DIY cũ, thiết bị ngoại vi SPI, nút và màn hình.
- [StuyoP/Fanatec-Wheel-Barebone-Emulator] (https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator) - Trình giả lập vành ATmega trần, các bài học khởi động và tương thích.
- [FendtXerion3800/Fanatec-Pinout] (https://github.com/FendtXerion3800/Fanatec-Pinout) - kết nối cộng đồng và quan sát ngoại vi.
- [jssting/ArduinoTec-Pedals] (https://github.com/jssting/ArduinoTec-Pedals) - chuyển đổi bàn đạp USB độc lập.
- [Alexbox364/F_Interface_AL] (https://github.com/Alexbox364/F_Interface_AL) - Tay lái tùy chỉnh DIY thông qua SPI.
- [GeekyDeaks/fanatec-pedal-emulator] (https://github.com/GeekyDeaks/fanatec-pedal-emulator) - bàn đạp USB của bên thứ ba proxy qua RJ12.
- [StuyoP/Universal-Shifter-Interface-for-Fanatec] (https://github.com/StuyoP/Universal-Shifter-Interface-for-Fanatec) - giao diện shifter dựa trên chuyển mạch thông qua RJ12.
- [gotzl/hid-fanatecff] (https://github.com/gotzl/hid-fanatecff) - Trình điều khiển Linux HID/FF và điều khiển mở rộng.
- [gotzl/hid-fanatecff-tools] (https://github.com/gotzl/hid-fanatecff-tools) - cầu telemetry-to-display/LED.
- [Tìm kiếm kho lưu trữ GitHub Fanatec] (https://github.com/search?q=%20fanatec&type=repositories) - chỉ khám phá; xác thực từng kết quả một cách độc lập.

### 18.2 Tài liệu tham khảo quan trọng Upstream / Liên quan

- [darknao/btClubSportWheel] (https://github.com/darknao/btClubSportWheel) - được trích dẫn bởi các dự án vành DIY là công việc trước đó.
- [Alexbox364/F_Interface_AL] (https://github.com/Alexbox364/F_Interface_AL) - được trích dẫn cho thiết kế Nano/level-converter/shift-register mở rộng.
- [Thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid) - tài liệu HID và PID tiêu chuẩn.
- [Hướng dẫn chuyển đổi Fanatec QR2] (https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) - Thế hệ QR, các biến thể Base-Side / Wheel-Side và nâng cấp.
- [Câu hỏi thường gặp về tay lái Fanatec] (https://help.fanatec.com/hc/en-us/articles/43802514108433-Steering-Wheel-FAQ) - Ngày ngừng sử dụng QR2 và QR1.
- [Tương thích nền tảng Fanatec] (https://www.fanatec.com/us-en/platforms) - Ranh giới cấp phép cơ sở Xbox Wheel / Hub và PlayStation.
- [Đăng ký nguồn hệ sinh thái Fanatec] (./references.md) - ngày nguồn và giới hạn hướng dẫn người mua.
