#Hệ sinh thái Sim Racing hiện đại: Cơ sở tri thức nhúng

| Tài liệu | Phiên bản | Ngày | Đối tượng
|---|---|---|---|
| Modern Sim Racing Ecosystem: Embedded Knowledge Base | 1.4 | 2026-07-02 | Fresher/junior trong lĩnh vực đua xe sim, mid level trong hệ thống nhúng |

> **Tin tức:**
> Phạm vi: Chỉ thông tin công cộng và kiến trúc tham chiếu. Không có kỹ thuật đảo ngược firmware độc quyền. Ưu tiên bằng chứng: cơ quan tiêu chuẩn; hướng dẫn sử dụng / hỗ trợ nhà sản xuất; tài liệu tham khảo bán dẫn; triển khai mở công khai; bằng sáng chế. MCU, bus, định dạng gói, tỷ lệ kiểm soát và cơ chế bảo mật dành riêng cho thương hiệu vẫn chưa được biết trừ khi tài liệu công khai xác định rõ ràng chúng.

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Thay đổi |
|---|---|---|
| 1.0 | 2026-07-01 | Dự thảo nghiên cứu ban đầu. |
| 1.1 | 2026-07-01 | Tái cấu trúc cho luồng sư phạm, quy ước ngôn ngữ quy phạm ứng dụng và sơ đồ cập nhật. |
| 1.2 | 2026-07-01 | Các khái niệm nền tảng được hợp nhất, các loại ổ đĩa và thiết lập an toàn từ basic.md. |
| 1.3 | 2026-07-01 | Đã thêm đường dẫn đọc của nhà phát triển và mô hình liên kết tham chiếu rõ ràng cho tài liệu nghiên cứu. |
| 1.4 | 2026-07-02 | Đã thêm các cấp Fanatec hiện tại, quyền sở hữu nền tảng, chuyển đổi QR2, hướng dẫn đường dẫn kết nối và ghi chú tiền tệ nguồn. |

## Điều hướng kiến trúc hệ thống

Tài liệu bao quát này đóng vai trò là gốc của cơ sở kiến thức đua xe mô phỏng. Để lặn sâu vào các hệ thống con cụ thể, hãy tham khảo các tài liệu được kết nối với nhau sau:

| Hệ thống con | Tài liệu | Tiêu điểm chính |
|---|---|---|
| ** Cơ sở bánh xe ** | [`wheel_base.md`] (./wheel_base.md) | Điều khiển động cơ, giai đoạn FFB, trung tâm USB tập trung |
| ** Phản hồi lực (người giải thích) ** | [`force_feedback_explained.md`] (./force_feedback_explained.md) Giải thích FFB hợp nhất: lý thuyết lực, động cơ servo, chuỗi tín hiệu, lực / rung cảm thấy, độ trung thực, điều chỉnh, an toàn |
| ** Vành lái ** | [`wheel_rim.md`](./wheel_rim.md) | Firmware bánh xe nhúng, đầu vào, màn hình tích hợp, SPI |
| ** Bàn đạp ** | [`pedals.md`] (./pedals.md) | Tải ô, ADC, RJ12 ủy nhiệm |
| **Add-Ons** | [`add_ons.md`](./add_ons.md) | Shifters (H-pattern/sequential) và handbrakes |
| **Phụ kiện** | [`accessories.md`](./accessories.md) | Bản phát hành nhanh, bảng điều khiển độc lập, hộp nút |
| ** Buồng lái ** | [`cockpits.md`](./cockpits.md) | Độ cứng cơ khí và các thành phần cấu trúc |
| **Tools** | [`tools.md`](./tools.md) | Tiêu chuẩn, công cụ máy chủ, công cụ firmware, đo lường và xác nhận |
| **Repositories** | [`repos.md`](./repos.md) | Giới hạn bằng chứng và phát hiện thực hiện cộng đồng |
| **Glossary** | [`glossary.md`](./glossary.md) | Thuật ngữ khách hàng, nhãn tương thích, họ mô hình và chữ viết tắt |
| **Sổ đăng ký nguồn ** | [`references.md`](./references.md) | Phân loại nguồn hệ sinh thái, ngày xem xét và xung đột tiền tệ đã biết |

## Đường dẫn đọc dành cho nhà phát triển

Sử dụng đường dẫn này khi giới thiệu một lập trình viên nhúng:

1. Đọc tệp này để biết quyền sở hữu hệ thống và từ vựng an toàn.
2. Đọc [wheel_base.md] (./wheel_base.md) trước bất kỳ công việc FFB, điều khiển động cơ, cập nhật hoặc USB / PID nào.
3. Đọc [wheel_rim.md] (./wheel_rim.md) trước bất kỳ công việc QR, hiển thị, LED hoặc nút bánh xe nào.
4. Đọc [pedals.md](./pedals.md), [add_ons.md](./add_ons.md), và [accessories.md](./accessories.md) cho các nút đầu vào ngoại vi.
5. Đọc [cockpits.md] (./cockpits.md) trước khi diễn giải dữ liệu kiểm tra lực, mô-men xoắn hoặc tải trọng bàn đạp.
6. Sử dụng [tools.md](./tools.md) và [repos.md](./repos.md) để tham khảo xác nhận và ví dụ thực hiện công khai.

---

## 1. Tổng quan hệ thống

Phần này xác định phạm vi và ranh giới của hệ sinh thái đua xe mô phỏng. Nó giải thích mối quan hệ cấp cao giữa vật chủ, đế bánh xe và các thiết bị ngoại vi của nó.

Một hệ sinh thái đua xe sim là một hệ thống người-máy hai chiều. Hệ thống sẽ định tuyến tay lái và điều khiển người lái đến máy chủ. Cơ sở bánh xe sẽ chấp nhận các lệnh haptic và tạo ra mô-men xoắn trục bị chặn. Hệ thống có thể tổng hợp tất cả các phụ kiện thông qua đế bánh xe hoặc nó có thể hỗ trợ các thiết bị ngoại vi USB độc lập.

1.1. Thành phần

Bảng dưới đây mô tả các thành phần chính trong hệ sinh thái và vai trò firmware điển hình của chúng.

| Thành phần | Mục đích | Giao diện điển hình | Vai trò Firmware |
|---|---|---|---|
| PC | Game, driver, configuration, update | USB, network | Host driver/service and updater; open-source Linux kernel drivers (e.g., hid-fanatecff) exist for FFB support |
| Bảng điều khiển | Nền tảng trò chơi / phụ kiện được điều khiển | Đường dẫn USB được cấp phép | Tích hợp được phê duyệt; Giấy phép Xbox nằm trong bánh xe / trung tâm được cấp phép, trong khi giấy phép PlayStation nằm trong cơ sở được cấp phép |
Cơ sở bánh xe Thiết bị truyền động và trung tâm hệ thống Haptic USB cộng với xe buýt nội bộ / ngoại vi HID / PID, FFB, điều khiển động cơ, an toàn
| Vô lăng | Điều khiển, chỉ báo và trung tâm | Liên hệ QR, liên kết nhà cung cấp có dây / không dây hoặc USB tùy thuộc vào hệ sinh thái | Quét, phát hiện, hiển thị, nhận dạng; một bánh xe / trung tâm Fanatec được cấp phép Xbox có thể cung cấp khả năng tương thích với nền tảng Xbox |
| Vành bánh xe | Vòng cơ khí trần gắn vào trung tâm | Bu lông cơ khí | Không có (Thụ động) |
| Phát hành nhanh | Khớp nối mô-men xoắn cơ học; nguồn / dữ liệu tùy chọn | Danh bạ hoặc hệ thống không dây / quy nạp | Sự hiện diện, bắt tay, giải trình tự nguồn |
| Động cơ | Tạo mô-men xoắn vật lý | Biến tần ba pha | Điều khiển và bảo vệ dòng điện / mô-men xoắn |
| Bộ mã hóa | Góc trục / rôto | SPI, SSI, BiSS-C, ABZ, Sin / Cos | Thu nhận, hiệu lực, hiệu chuẩn |
Bàn đạp ga, phanh, ly hợp Cổng cơ sở (ví dụ: RJ12) hoặc USB ADC, bộ lọc, hiệu chuẩn, báo cáo; có thể được ủy quyền thông qua cơ sở để hỗ trợ bảng điều khiển
| Shifter | Gear hoặc các sự kiện tuần tự | Cổng cơ sở (ví dụ: RJ12) hoặc USB | Phân loại và giải mã |
| Phanh tay | Đầu vào phanh liên tục | Cổng cơ sở (ví dụ: RJ12) hoặc USB | ADC, hiệu chuẩn, báo cáo |
| Bảng điều khiển | Hiển thị từ xa / trạng thái | USB, nối tiếp, Ethernet / Wi-Fi | Hiển thị và giám sát liên kết |
| Hộp nút | Điều khiển phụ | USB HID | Quét ma trận/mã hóa |
| Load cell | Force-to-signal transducer | Amplifier và ADC | Tare, span, lọc, chẩn đoán |
| Nguồn điện | Nguồn DC bị cô lập | Đầu nối DC | Màn hình cơ sở trạng thái bus |
Buồng lái Khung gầm kết cấu Cơ khí Thân cứng thụ động

**Hình 1-1: Tổng quan hệ sinh thái hệ thống **

```nàng tiên cá
Sơ đồ TD
Trò chơi [Trò chơi] <--> Driver [Trình điều khiển hệ điều hành / Dịch vụ nhà cung cấp]
Trình điều khiển <--> |USB HID / PID + giao diện nhà cung cấp tùy chọn | Base
Console[Console] <-->|đường dẫn phụ kiện được cấp phép| Base
PSU [Cung cấp DC cách ly] -> Cơ sở
cơ sở tiểu đồ thị [Cơ sở bánh xe]
USB [USB] <--> Chính [MCU chính]
Chính <--> FFB [Động cơ FFB]
FFB -> Arb [Arbiter mô-men xoắn / an toàn]
Arb -> Điều khiển [Điều khiển động cơ]
Bộ mã hóa -> Kiểm soát
Hiện tại [Current Sense] -> Kiểm soát
Điều khiển -> Cổng [Trình điều khiển cổng / Biến tần] -> Động cơ
Lỗi [Đường dẫn lỗi phần cứng] -->|tắt| Cổng
Chính <--> Cổng [Cổng ngoại vi]
kết thúc
Động cơ --- QR [Phát hành nhanh] --- Tay lái [Tay lái]
Tay lái --- Vành [Vành bánh xe trần]
Tay lái -> Nút [Nút / Mái chèo / Bộ mã hóa / Hiển thị]
Tay lái <-->|SPI / dữ liệu có dây hoặc không dây| Chính
Bàn đạp -> Cổng cơ sở RJ12 hoặc USB | Cổng
HPattern [H-pattern Shifter] -> Cổng cơ sở RJ12 hoặc USB | Cổng
Sequential [Sequential Shifter] -> Cổng cơ sở RJ12 hoặc USB | Cổng
Phanh tay -> Cổng cơ sở RJ12 hoặc USB | Cổng
Dash[Dashboard] -->|USB/network| Trình điều khiển
Hộp [Hộp nút] -> |USB HID | Trình điều khiển
```

### 1.2. Hệ sinh thái Fanatec như một ví dụ công cộng

Hệ sinh thái công cộng của Fanatec có tính mô đun cao, được thiết kế để người dùng có thể kết hợp và kết hợp các thành phần (đế bánh xe, vô lăng, bàn đạp) và nâng cấp từng bước. Sản phẩm sử dụng rộng rãi ba tầng:
- **CSL (ClubSport Light) **: Cấp nhập cảnh, thân thiện với ngân sách, thường sử dụng các thành phần nhựa và kim loại cơ bản.
- **ClubSport**: Tầng dành cho những người đam mê thể thao tầm trung, sử dụng vật liệu cao cấp như nhôm và sợi carbon với các thiết bị điện tử tiên tiến hơn.
- ** Podium **: Cấp hàng đầu, cấp chuyên nghiệp, được thiết kế cho mô-men xoắn tối đa, độ bền và tùy biến sử dụng vật liệu cấp công nghiệp.

Nhãn cấp giúp điều hướng nhưng không chứng minh rằng hai sản phẩm tương thích về điện, cơ học hoặc nền tảng.

Đế bánh xe là trung tâm hệ thống trung tâm để thiết lập bảng điều khiển. Bàn đạp, cần số và phanh tay tương thích kết nối với đế, cho thấy một đường dẫn USB được cấp phép đến bảng điều khiển. Trên PC, các thiết bị ngoại vi được hỗ trợ thay vào đó có thể hoạt động như các thiết bị USB độc lập. Gói Ready2Race là gói mua, không phải là tiêu chuẩn giao diện mới.

![Fanatec Ecosystem](../assets/fanatec_ecosystem.jpg)
![Sản phẩm Fanatec](../assets/fanatec_products.jpg)

| Nền tảng | Vị trí giấy phép Fanatec | Quy tắc thực hành |
|---|---|---|
| Windows PC | Không yêu cầu chip bảo mật bảng điều khiển | Xác minh phiên bản Windows, hỗ trợ trò chơi, trình điều khiển / Ứng dụng và đường dẫn kết nối của từng thiết bị. |
| Xbox | Tay lái hoặc trung tâm được cấp phép của Xbox | Bánh xe / trung tâm được cấp phép cho phép các thiết bị ngoại vi cơ sở và kết nối cơ sở tương thích trên Xbox. |
Bánh xe tương thích và các thiết bị ngoại vi được kết nối cơ sở kế thừa hỗ trợ PlayStation thông qua cơ sở đó.

*Lưu ý: Kết hợp cơ sở bánh xe được cấp phép PlayStation với vô lăng được cấp phép Xbox thường tạo ra một thiết lập tương thích chéo hoạt động trên PlayStation, Xbox và PC. *

Kể từ ngày 2026-02-16, Fanatec tuyên bố rằng bánh xe và đế được mua thông qua cửa hàng của họ sử dụng QR2 theo mặc định và QR1 đã ngừng hoạt động. Phần cứng QR1 cũ vẫn có liên quan, nhưng các thế hệ Base-Side và Wheel-Side phải phù hợp và hỗ trợ nâng cấp là dành riêng cho mô hình.

1.3. Các loại ổ đĩa

Cơ sở bánh xe đua Sim thường được phân loại theo hệ thống phân phối mô-men xoắn cơ học của chúng:

- ** Cơ sở điều khiển bánh răng: ** Cơ chế chi phí thấp nhưng giới thiệu phản ứng cơ học.
- ** Cơ sở điều khiển đai: ** Cung cấp giao hàng mượt mà hơn nhưng giới thiệu tuân thủ / kéo dài cơ học.
- ** Cơ sở truyền động trực tiếp: ** Trục động cơ kết nối trực tiếp với vành lái. Cung cấp lỗi truyền thấp nhất và yêu cầu cân nhắc mô-men xoắn và an toàn cao nhất.

1.4. Ranh giới Firmware

Phần mềm sẽ thiết lập ranh giới sở hữu độc lập cho các đầu nối, miền nguồn, bộ mô tả USB, chế độ nền tảng và giới hạn mô-men xoắn. Phần mềm sẽ xác minh danh tính, định tuyến, thời gian, hiệu chuẩn và cập nhật khả năng tương thích trước khi cho phép vận hành.

### 1.5. Các yếu tố hình thức và phong cách lái xe

Thiết bị ngoại vi thường được thiết kế riêng cho các kiểu lái mô phỏng cụ thể:
- ** Công thức: ** Vô lăng hình chữ nhật / bướm được tối ưu hóa để xoay giới hạn.
- ** GT: ** Tay lái hình chữ D hoặc tròn với bộ nút mở rộng.
- ** Rally & Drift: ** Vành bánh xe tròn hoàn hảo ưu tiên xoay nhanh, trượt góc.

## 2. Nguyên tắc cơ bản vật lý và cơ khí

Phần này bao gồm các nguyên tắc vật lý cơ bản của phần cứng đua xe sim, tập trung vào mô-men xoắn, động lực học chuyển động và cảm biến. Nó thu hẹp khoảng cách giữa thiết kế cơ khí và điều khiển hệ thống nhúng.

- ** Mô-men xoắn (N · m) ** là sản phẩm của lực tiếp tuyến và bán kính. Vành lái lớn hơn làm giảm lực tay cần thiết ở mô-men xoắn trục bằng nhau.
- ** quán tính ** chống lại gia tốc góc.
- ** Damping ** chống lại vận tốc.
- ** Friction ** chống lại chuyển động.
- ** Cogging ** là gợn mô-men xoắn từ tính phụ thuộc vào vị trí vốn có của thiết kế động cơ.

## 3. Phân tích sản phẩm

Phần này phân tách hệ thống tổng thể thành các hệ thống con chức năng cụ thể. Nó xác định các khả năng phần cứng và trách nhiệm firmware cho mỗi mô-đun.

3.1. Ma trận hệ thống con

Ma trận hệ thống con phác thảo sự phân bổ trách nhiệm trên các mô-đun phần cứng riêng biệt.

| Hệ thống con | Lớp phần cứng / MCU | Trách nhiệm firmware | Giao tiếp | Nguồn / Cập nhật |
|---|---|---|---|---|
| Đế bánh xe | MCU chính; động cơ tùy chọn MCU / ASIC; bộ mã hóa; biến tần; NVM | USB, FFB, tập hợp đầu vào, an toàn, hiệu chuẩn | USB; SPI / UART / CAN bên trong | DC bên ngoài; Bộ nạp khởi động / phục hồi USB
| Tay lái | MCU công suất thấp, bộ mở rộng GPIO, cảm biến Hall, đèn LED / LCD | Quét, giải mã, giải mã bộ mã hóa, hiển thị, nhận dạng, mở khóa FFB | QR liên kết có dây (SPI thường bị giả mạo bởi trình giả lập), không dây hoặc USB | QR / quy nạp / USB / pin; pass-through / USB / OTA |
| Bàn đạp | Cảm biến (Potentiometers, hiệu ứng Hall), tế bào tải AFE, ADC, MCU tùy chọn | Lấy mẫu, lọc, hiệu chuẩn, HID | Cổng cơ sở analog / kỹ thuật số (RJ12) hoặc USB | Base / USB; không có hoặc cập nhật USB |
| H-pattern shifter | Hai trục Hall / mảng chuyển đổi, MCU tùy chọn | Ngưỡng cổng, trễ, từ chối trạng thái không thể | Analog, GPIO, bus kỹ thuật số, USB | Base / USB |
| Bộ chuyển đổi tuần tự | Hai công tắc hoặc sắp xếp Hall | Giải phóng, ngữ nghĩa cạnh / xung | GPIO, analog, bus, USB | Base / USB |
| Phanh tay | Potentiometer / Hall / load cell, MCU tùy chọn | Bộ lọc, hiệu chuẩn phạm vi, phát hiện mở / ngắn | Analog, bus kỹ thuật số, USB | Base / USB |
Bảng điều khiển | MCU / MPU, LCD / OLED, trình điều khiển LED | Giải mã từ xa, hiển thị, giám sát | USB, UART / CAN, Ethernet / Wi-Fi | USB / phụ trợ; USB / OTA |
| Hộp nút | MCU USB công suất thấp, ma trận / bộ mở rộng | Quét, giải mã, mô tả | USB HID | USB; bộ nạp khởi động |
| Bảng điện | Bảo vệ, DC bus, buck / LDO, cảm giác, biến tần | Trình tự, giám sát, chính sách tái sinh | ADC / GPIO cho bộ điều khiển | DC cách ly bên ngoài
| Bộ điều khiển động cơ | Real-time MCU/DSP/ASIC, ADC/timers | Encoder/current acquisition and bounded PWM | SPI/CAN/PWM from main MCU | Logic and DC bus; base-bundled update |
| Giao diện USB | Tích hợp / bên ngoài PHY, ESD | Sơ yếu lý lịch, báo cáo, vòng đời điểm cuối | Điều khiển USB / ngắt; giao diện nhà cung cấp tùy chọn | Cơ sở tự cấp nguồn với cảm biến VBUS |

**Hình 3-1: Đường dẫn dữ liệu lõi cơ sở bánh xe **

```nàng tiên cá
Lưu đồ LR
Máy chủ <-->|USB| Main[Base System MCU]
Chính -> FFB -> Mô-men xoắn [Torque Arbiter]
Mô-men xoắn -> MotorCtl [Motor MCU / ASIC]
Bộ mã hóa -> MotorCtl
Shunts -> ADC -> MotorCtl
MotorCtl -> PWM -> Cầu -> Động cơ
```

**Hình 3-2: Đường dẫn dữ liệu cảm biến bàn đạp và tương tự **

```nàng tiên cá
Lưu đồ LR
Lực -> LoadCell -> INA [Bộ khuếch đại công cụ] -> ADC
Du lịch [Hall / Potentiometer] -> ADC
ADC -> Cal [Hiệu chuẩn + Lọc] -> Liên kết [Liên kết cơ sở / USB HID]
```

**Hình 3-3: Đường dẫn dữ liệu tay lái **

```nàng tiên cá
Lưu đồ LR
Đầu vào [Nút / Paddles / Bộ mã hóa] -> WheelMCU
WheelMCU -> Hiển thị [Đèn LED / Màn hình]
WheelMCU <-->|SPI / dữ liệu khung + sức khỏe| BaseMCU
QR [Cơ QR] --- Trục
```

### 3.2. Nhận dạng thành phần và sức khỏe

Firmware sẽ xử lý từng thành phần thông minh như một nút phiên bản. Mỗi nút sẽ báo cáo danh tính, khả năng, trạng thái khởi động, trạng thái ứng dụng, tình trạng sức khỏe và trạng thái phục hồi. Firmware sẽ thực hiện xử lý lỗi cho các cảm biến thụ động, bao gồm lỗi cáp, đường ray ngoài giới hạn, giới hạn phạm vi và kiểm tra tính hợp lý của tín hiệu.

## 4. Tổng quan về phản hồi của lực lượng

Phần này mô tả cơ sở lý thuyết của phản hồi lực (FFB). Nó theo dõi cách các sự kiện vật lý ảo được dịch thành mô-men xoắn trục vật lý.

Phản hồi lực chuyển đổi các hiệu ứng vật lý được xác định theo mô phỏng thành mô-men xoắn trục bị chặn trong khi quay trở lại vị trí lái và điều khiển mô phỏng.

### 4.1. Các giai đoạn phản hồi

Các giai đoạn sau đây mô tả con đường từ game engine đến mô-men xoắn vật lý.

Giai đoạn | Trách nhiệm |
|---|---|
| Game | Tính toán lực lái ảo và các sự kiện vật lý |
| API / Driver | Express hiệu ứng thông qua hợp đồng hệ điều hành được hỗ trợ |
| USB Transport | Cung cấp và xác nhận báo cáo |
| Trình quản lý PID | Phân bổ hiệu ứng; duy trì thời lượng, phong bì, điều kiện và trạng thái bắt đầu / dừng |
| FFB Mixer | Kết hợp các hiệu ứng hoạt động và áp dụng các bộ lọc được cấu hình |
| Torque Arbiter | Enforce gain, maximum torque, slew rate, thermal derating, enable state, và freshness limits |
| Điều khiển động cơ | Theo dõi mô-men xoắn và nhu cầu hiện tại bắt nguồn từ phản hồi |
Giai đoạn năng lượng Tạo mô-men xoắn vật lý bằng cách sử dụng động cơ
| An toàn | Phát hiện lỗi phần cứng và chủ động loại bỏ mô-men xoắn

** Hình 4-1: Đường ống phản hồi lực **

```nàng tiên cá
Lưu đồ LR
Trò chơi --> API [OS API / Nhà cung cấp SDK] --> USB --> Phân tích cú pháp
Phân tích cú pháp --> Effects[Effect State / Mixer] --> Arbiter[Torque Arbiter]
Arbiter -> Control [Kiểm soát hiện tại] -> Inverter -> Motor -> Driver [Driver Hands]
Bộ mã hóa -> Kiểm soát
Cảm biến [Hiện tại / Điện áp / Nhiệt độ / Độ tươi] -> Arbiter
Lỗi [Lỗi phần cứng] -> |tắt | Biến tần
```

** Hình 4-2: Đường dẫn dữ liệu Phản hồi Lực lượng Hợp lý (FFB) **

```nàng tiên cá
đồ thị LR
A[Vật lý trò chơi] --> B[Trình điều khiển/API]
B -> C [Máy trộn hiệu ứng]
C -> D [Bộ lọc]
D -> E [Trọng tài mô-men xoắn]
E -> F [Kiểm soát hiện tại]
F -> G [Động cơ]
```

### 4.2. Ràng buộc firmware FFB

Firmware sẽ xác nhận và lên lịch cho tất cả các hiệu ứng đến. Máy trộn FFB sẽ kết hợp các hiệu ứng mà không bị tràn số học. Hệ thống sẽ áp dụng tất cả các giới hạn an toàn và công suất sau giai đoạn trộn. Nếu liên kết máy chủ bị lỗi, hệ thống sẽ thực hiện phân rã mô-men xoắn rõ ràng và vô hiệu hóa chính sách. Firmware sẽ không cho phép bất kỳ lệnh phần mềm nào vượt qua giới hạn vật lý hoặc nhiệt. Cắt xảy ra khi mô-men xoắn yêu cầu vượt quá giới hạn hoạt động, khiến các lực lớn khác nhau sụp đổ đến cùng một mức tối đa và chi tiết bị mất.

## 5. Kiến trúc phần cứng

Phần này nêu chi tiết các thành phần phần cứng bên trong của đế bánh xe trực tiếp. Nó chỉ định vòng điều khiển vật lý và các biện pháp bảo vệ điện tử cần thiết.

# 5.1. Kiến trúc cơ sở ổ đĩa trực tiếp

Kiến trúc cơ sở bánh xe cốt lõi được tách thành miền quản lý hệ thống và miền điều khiển động cơ thời gian thực. Các cơ sở truyền động trực tiếp thường sử dụng động cơ BLDC / PMSM ba pha với phản hồi bộ mã hóa, cảm biến dòng pha, Điều chế độ rộng xung (PWM), trình điều khiển cổng và biến tần.

Động cơ là một PMSM ba pha - một stato thép vết thương xung quanh một rôto nam châm vĩnh cửu trên trục lái - và biến tần là giai đoạn nguồn sáu MOSFET tổng hợp dòng điện ba pha của nó từ bus DC:

! [Phần cắt ngang động cơ servo trực tiếp] (./servo_motor_cross_section.svg)

![Biến tần ba pha điều khiển động cơ](../assets/three_phase_inverter.svg)

Ba nửa cầu của biến tần mỗi bộ đặt điện áp của một pha bằng PWM; hai công tắc trong một chân không bao giờ được bật cùng nhau (thời gian chết ngăn chặn một bus DC ngắn) và các shunt phía thấp trả về phép đo dòng pha mà vòng lặp FOC cần.

** Hình 5-1: Sơ đồ khối kiến trúc phần cứng **

```nàng tiên cá
Sơ đồ TD
USBConn [USB + ESD] -> PHY -> Chính [MCU chính]
Chính <--> NVM [Flash / EEPROM]
Chính <--> Periph [Liên kết bánh xe / bàn đạp / Shifter]
Chính --> Nhu cầu [Nhu cầu mô-men xoắn giới hạn]
Nhu cầu --> MotorMCU [Motor MCU / ASIC]
Bộ mã hóa [Absolute / Incremental Encoder] -> MotorMCU
Shunts [Giai đoạn / DC hiện tại] -> AFE [Bộ khuếch đại + Bộ lọc] -> ADC
ADC -> MotorMCU
MotorMCU -> Hẹn giờ [PWM bổ sung + Thời gian chết]
Hẹn giờ -> Cổng [Trình điều khiển cổng] -> Cầu [Cầu MOSFET 3 pha] -> PMSM
DC [Đầu vào DC] -> Bảo vệ [Bảo vệ / Inrush / DC Bus] -> Cầu
Bảo vệ -> Rails [Logic Rails]
Nhiệt [Động cơ / FET / PCB Nhiệt độ] -> Chính
HWTrip [Comparator / Gate Fault / E-stop] -> | vô hiệu hóa không đồng bộ | Gate
```

| Chặn | Trách nhiệm | Yêu cầu firmware |
|---|---|---|
| Main MCU | Host / ngoại vi giao thức và chính sách hệ thống | Shall enforce scheduling and version compatibility |
| Motor MCU / ASIC | Đường dẫn dòng điện / mô-men xoắn thời gian thực | Đáp ứng thời hạn xác định và phản hồi lỗi |
| PMSM/BLDC | Thiết bị truyền động mô-men xoắn | Hoạt động trong các thông số động cơ và giới hạn nhiệt |
Bộ mã hóa Phản hồi góc / tốc độ Sẽ xác nhận CRC, trạng thái, quấn, hướng, bù và thời gian chờ
| Cảm biến dòng điện | Phản hồi dòng điện pha / DC | Hiệu chỉnh độ lệch, độ lợi, độ bão hòa và căn chỉnh với cửa sổ mẫu PWM |
Bộ hẹn giờ nâng cao Bộ kích hoạt PWM và ADC Sẽ tạo ra các đầu ra bổ sung với hỗ trợ đầu vào thời gian chết và ngắt
| Trình điều khiển / biến tần cổng | Chuyển DC thành ba pha | Mặc định tắt; sẽ phản hồi ngay lập tức các lỗi phần cứng |
| NVM | Firmware, hiệu chuẩn, hồ sơ, hồ sơ lỗi | Sẽ đảm bảo ghi nguyên tử và hỗ trợ cân bằng hao mòn |

### 5.2. Kiểm soát thiết kế tên miền

Hệ thống có thể sử dụng một MCU duy nhất hoặc một kiến trúc phân chia (Main MCU + Motor MCU / ASIC).

Điều khiển hướng trường (FOC) biến đổi góc rôto và các phép đo dòng điện để điều chỉnh dòng sản xuất mô-men xoắn. Phần sụn phải đảm bảo độ chính xác cao và thời gian PWM / ADC được đồng bộ hóa. Đầu vào quá dòng và ngắt ở cấp phần cứng sẽ ghi đè lên điều khiển phần mềm. Phần sụn sẽ kích hoạt đồng bộ ADC trong cửa sổ giữa PWM hợp lệ và sẽ hiệu chỉnh các hiệu chỉnh cảm giác dòng điện trong quá trình khởi tạo.

! [PWM tàu sân bay, chu kỳ làm việc, và điểm mẫu ADC] (./foc_pwm_timing.svg)

"Cửa sổ giữa PWM hợp lệ" là chi tiết thời gian chính: một sóng mang hình tam giác được so sánh với lệnh nhiệm vụ của mỗi pha tạo ra tín hiệu cổng và các mẫu ADC hiện tại ở đỉnh sóng mang - điểm giữa yên tĩnh của thời gian chuyển mạch - do đó việc đọc không bị hỏng do nhiễu cạnh chuyển mạch. Thu phóng thời gian chết hiển thị khoảng cách ngắn cả hai ngăn chặn bắn xuyên qua mỗi lần chuyển đổi.

## 6. Tương tác phần cứng

Phần này phác thảo cách firmware tương tác với các thiết bị ngoại vi phần cứng cụ thể. Nó xác định ánh xạ giữa các giao diện điện và các tính năng vi điều khiển.

### 6.1. Giao diện ngoại vi

Firmware sẽ cấu hình và quản lý các thiết bị ngoại vi MCU để giao tiếp an toàn với phần cứng bên ngoài.

| Kết nối | Thiết bị ngoại vi MCU | Yêu cầu firmware |
|---|---|---|
| Encoder | SPI/SSI/BiSS-C/ABZ, timer, DMA | Shall verify deadline, CRC, wrap, direction, and timeout |
| Bộ khuếch đại dòng điện | ADC, bộ kích hoạt PWM, DMA | Sẽ xác minh cửa sổ lấy mẫu, độ lệch, độ lợi và độ bão hòa |
| MCU PWM đến cổng | Hẹn giờ nâng cao, phá vỡ GPIO | Định cấu hình thời gian chết, thiết lập lại an toàn và phá vỡ độ trễ |
| Cổng lỗi đến MCU | Ngắt đầu vào, GPIO | Sẽ thực hiện tắt phần cứng đầu tiên và chốt bản ghi lỗi |
| Rim đến cơ sở | CAN / SPI / UART / radio | Sẽ xử lý cắm nóng, phục hồi ESD, trình tự nguồn và thời gian chờ |
| Bàn đạp đến ADC / bus | ADC / SPI / I2C | Xác minh các điều kiện mở / ngắn, giới hạn tham chiếu và hiệu chuẩn |
| Nút tới GPIO | GPIO, hẹn giờ | Áp dụng debounce và từ chối ghosting |
| Hiển thị cho SPI | SPI, DMA | Ngân sách băng thông để ngăn chặn đảo ngược ưu tiên |
| Đèn LED | Hẹn giờ, bus nối tiếp | Giới hạn dòng điện và duy trì tốc độ làm mới |
| USB tới máy chủ | Thiết bị USB | Sẽ quản lý VBUS, thiết lập lại, tạm dừng và vòng đời điểm cuối |
| NVM để MCU | QSPI / SPI / I2C | Sẽ thực hiện mặc San lấp mặt bằng, nguyên tử, và schema xác nhận |

**Hình 6-1: Định tuyến ngoại vi phần cứng **

```nàng tiên cá
đồ thị TD
Bộ mã hóa -->|SPI/SSI/BiSS-C/ABZ| MCU
Shunts -> Amp -> |ADC | MCU
MCU -> | PWM | Cổng -> Động cơ
Cổng -->|FAULT/BREAK| MCU
Vành <-->|có dây/không dây| MCU
Bàn đạp -->|ADC/digital| MCU
Nút -->|GPIO| MCU
MCU -->|SPI + DMA| Hiển thị
MCU -->|PWM/SPI/I2C| Đèn LED
Máy chủ <-->|USB| MCU
EStop -> ức chế cổng phần cứng | Cổng
```

### 6.2. Pin và quản lý nhà nước

Đầu ra cho phép cổng sẽ mặc định ở trạng thái không hoạt động trong quá trình thiết lập lại, thực thi bộ nạp khởi động, khôi phục và trong khi các chân không được định cấu hình. Hệ thống sẽ bảo vệ đường ray điện ngoại vi để phụ kiện bên ngoài bị hỏng không thể làm sập đường ray điều khiển chính.

## 7. Kiến trúc truyền thông

Phần này xác định các liên kết truyền thông bên trong và bên ngoài. Nó chỉ định các giao thức vận chuyển, khả năng và yêu cầu toàn vẹn dữ liệu.

7.1. Đặc điểm liên kết

Các liên kết sau đây xác định cách các mô-đun trao đổi dữ liệu.

| Giao diện | Vai trò điển hình | Tô pô | Mô tả |
|---|---|---|---|
| USB 2.0 FS HID | Máy chủ đến thiết bị | Máy chủ/thiết bị | Tiêu chuẩn, tự mô tả; xử lý các trục, nút và FFB |
| USB HS | Hiển thị/dữ liệu nhà cung cấp | Máy chủ/thiết bị | Băng thông cao; tăng độ phức tạp ngăn xếp |
| SPI | MCU để ASIC / mã hóa / hiển thị | Bộ điều khiển / ngoại vi | MHz tốc độ; DMA thân thiện; nhạy cảm với EMI |
| UART | Debug/boot/simple accessory | Peer framing | Universal; yêu cầu địa chỉ phần mềm và frameing |
| CAN / CAN-FD | Mô-đun phân tán | Đa bộ điều khiển | Bus mạnh vi sai; có giao thức trên cao |
| I2C | EEPROM/cảm biến/mở rộng | Bộ điều khiển/mục tiêu | Hai dây; nhạy cảm với khóa bus và điện dung |
| RS-485 | Phụ kiện có cáp | Giao thức được xác định | Vi phân; yêu cầu khung và trọng tài |
| Ethernet | Dash/service | Mạng gói tin | Công cụ chuẩn; ngăn xếp và độ trễ biến đổi |
| BLE | Vành/cấu hình không dây | Trung tâm/ngoại vi | Không dây; tùy thuộc vào giới hạn RF và độ trễ |
| Wi-Fi | Bảng điều khiển / từ xa | Mạng IP | Thông lượng cao; phát sinh gánh nặng điện và an ninh |

### 7.2. Giao tiếp nền tảng máy chủ

Chiến lược truyền thông dựa trên ** Wheel Base hoạt động như một USB Hub tập trung **, với hành vi thích ứng với mô hình bảo mật của nền tảng máy chủ:

#### 7.2.1. PC (Windows/Linux)
Hệ thống sẽ hiển thị các điểm cuối USB tiêu chuẩn để xử lý phản hồi và đầu vào vật lý. Hệ thống sẽ sử dụng ** USB HID ** (Thiết bị giao diện con người) để báo cáo các điều khiển (trục lái, vị trí bàn đạp, nhấn nút) và có thể sử dụng ** USB PID ** (Thiết bị giao diện vật lý) để nhận các hiệu ứng vật lý Force Feedback (FFB) từ công cụ trò chơi. Trình điều khiển nguồn mở (ví dụ: `hid-fanatecff` cho Linux) hoặc phần mềm nhà cung cấp có thể tự do tương tác với giao thức mở này.

** Hình 7-1: Cấu trúc liên kết mô tả USB (PC) **

```nàng tiên cá
Sơ đồ TD
Desc[USB/HID/PID Descriptors] --> Enum[Enumeration]
Enum --> IN[Ngắt IN: trục/nút]
Enum --> OUT[Gián đoạn OUT hoặc SET_REPORT: hiệu ứng]
Enum --> Feature[Feature: capabilities/config]
Enum --> Nhà cung cấp [Giao diện nhà cung cấp tùy chọn]
```

#### 7.2.2. Bảng điều khiển (PlayStation & Xbox)
Bảng điều khiển sử dụng các đường dẫn phụ kiện được cấp phép. Hướng dẫn Fanatec công khai thiết lập nơi sở hữu khả năng tương thích, nhưng nó không xuất bản giao thức mật mã hoặc cho phép giả định thực hiện.

- ** Xbox: ** khả năng tương thích được cung cấp bởi một tay lái hoặc hub được cấp phép Xbox gắn vào đế bánh xe Fanatec tương thích.
- ** PlayStation: ** khả năng tương thích được cung cấp bởi một cơ sở bánh xe Fanatec PlayStation cấp phép.
- ** Tổng hợp ngoại vi: ** Bàn đạp, cần số và phanh tay Fanatec phải kết nối thông qua đế bánh xe để sử dụng bảng điều khiển. Thiết bị ngoại vi USB độc lập là đường dẫn PC trừ khi trang sản phẩm hiện tại nêu rõ ràng khác.

Kiến trúc firmware sẽ mô hình hóa cấp phép nền tảng như một yêu cầu sản phẩm được phê duyệt. Nó sẽ không phát minh, mô phỏng hoặc bỏ qua hành vi xác thực bảng điều khiển chưa được công bố.

# 7.3 Cấu trúc liên kết nội bộ

**Hình 7-2: Cấu trúc liên kết Bus nội bộ **

```nàng tiên cá
đồ thị LR
PC <-->|USB| Chính
Chính <-->|SPI + IRQ| MotorCtl
MotorCtl <-->|SPI/SSI/BiSS-C/ABZ| Bộ mã hóa
Chính <-->|I2C| EEPROM
Trang chủ <-->|CAN/UART/proprietary framed link| Rim
Chính <-->|ADC/GPIO/digital bus| Phụ kiện
```

# 7.4 Đóng khung và tính toàn vẹn

Mỗi liên kết được đóng khung sẽ xác định phiên bản, loại, độ dài, trình tự, tính toàn vẹn tải trọng (ví dụ: CRC) và thời gian chờ mới. Phần mềm cơ sở sẽ thực thi hàng đợi bị chặn, đàm phán tương thích và phục hồi liên kết. Chuyển DMA sẽ thực thi quyền sở hữu bộ đệm rõ ràng, thời hạn chuyển, sự gắn kết bộ đệm và xử lý lỗi.

## 8. Kiến trúc Firmware

Phần này cung cấp thiết kế cấu trúc của ứng dụng firmware. Nó bao gồm các ranh giới mô-đun, máy trạng thái và quản lý vòng đời.

### 8.1. Mô-đun phần mềm

Firmware sẽ tách trách nhiệm để đảm bảo giao diện người dùng và các lớp vận chuyển không thể can thiệp vào đường dẫn điều khiển thời gian thực.

**Hình 8-1: Kiến trúc thành phần Firmware **

```nàng tiên cá
Sơ đồ TD
Boot [Bộ nạp khởi động được bảo vệ] -> Tự [Ứng dụng tự kiểm tra]
subgraph Dịch vụ
USB[USB/PID]
FFB[Trình quản lý hiệu ứng/Mixer]
Đầu vào [Tổng hợp đầu vào]
Rim [Rim / Quản lý ngoại vi]
Cài đặt[Cài đặt/Hiệu chuẩn]
Update[Đại lý cập nhật]
Diag [Chẩn đoán]
An toàn[Giám sát an toàn]
kết thúc
biểu đồ phụ Realtime
Arb [Arbiter mô-men xoắn] -> Động cơ [Điều khiển động cơ] -> PWM
Bộ mã hóa -> Động cơ
Dòng điện --> Động cơ
kết thúc
Tự -> Dịch vụ
USB -> FFB -> Arb
Rim -> Đầu vào -> USB
Cài đặt -> FFB
An toàn -> Arb
```

# 8.2. Ràng buộc mô-đun

Các ràng buộc sau đây sẽ được thực thi trên kiến trúc chương trình cơ sở.

| Mô đun | Yêu cầu |
|---|---|
Bộ nạp khởi động Sẽ xác minh, chọn và khôi phục hình ảnh; sẽ không bao giờ nạp năng lượng cho động cơ
| USB/PID | Sẽ vận chuyển các mô tả và báo cáo hiệu ứng; sẽ không bao giờ ghi vào PWM |
| FFB | Sẽ thực hiện số học bị chặn để trộn hiệu ứng |
| Trọng tài mô-men xoắn | Sẽ là con đường phần mềm duy nhất cho nhu cầu động cơ; sẽ thực thi cho phép, giới hạn và độ tươi |
| Điều khiển động cơ | Không phân tích lưu lượng máy chủ |
| Bộ mã hóa / dòng điện | Phải đính kèm dấu thời gian và trạng thái cho mọi giá trị được lấy mẫu |
| Thiết bị ngoại vi | Quản lý các sự kiện cắm nóng và trạng thái thiết bị cũ |
| Cài đặt | Sẽ không thực hiện chặn ghi flash trong đường dẫn thời gian thực cứng |
| Chẩn đoán | Sẽ ràng buộc bộ đếm / dấu vết; sẽ không chặn vòng lặp điều khiển |
| Cập nhật | Sẽ tắt mô-men xoắn trong toàn bộ quá trình cập nhật |
| An toàn | Sẽ phân loại lỗi và yêu cầu ngăn chặn; sẽ hành động cùng với bảo vệ điện nhanh |

# 8.3 Máy trạng thái hệ thống

Firmware sẽ thực hiện một máy trạng thái xác định để ủy quyền mô-men xoắn. Bộ ức chế phần cứng sẽ luôn có thẩm quyền.

** Hình 8-2: Mô-men xoắn cho phép máy trạng thái **

```nàng tiên cá
stateSơ đồ-v2
[*] -> Đặt lại
Đặt lại -> Khôi phục: yêu cầu hình ảnh / khôi phục không hợp lệ
Đặt lại -> Tự kiểm tra: hình ảnh hợp lệ
Tự kiểm tra -> SafeIdle: vượt qua
Tự kiểm tra -> Lỗi: thất bại
SafeIdle -> Hiệu chỉnh
Hiệu chuẩn -> Sẵn sàng
Sẵn sàng -> Đã bật: máy chủ hoạt động + chính sách hài lòng
Bật -> SafeIdle: vô hiệu hóa / thời gian chờ máy chủ
Bật -> Lỗi: lỗi nghiêm trọng
Lỗi -> SafeIdle: ủy quyền rõ ràng + điều kiện an toàn
Khôi phục -> Đặt lại: đã cài đặt hình ảnh đã xác minh
```

## 9. Dòng dữ liệu

Phần này nêu chi tiết chuyển động từ đầu đến cuối của dữ liệu thông qua hệ thống. Nó bao gồm việc xử lý các đầu vào cảm biến, cập nhật hiệu ứng và phản hồi phần cứng.

# 9.1. Trình tự kết thúc

Các tương tác máy chủ và vòng lặp thời gian thực nội bộ sẽ thực hiện đồng thời mà không làm rách dữ liệu.

** Hình 9-1: Trình tự dòng dữ liệu từ đầu đến cuối **

```nàng tiên cá
trình tựSơ đồ
số tự động
tham gia Enc như bộ mã hóa
MC tham gia như điều khiển động cơ
người tham gia Arb là Trọng tài mô-men xoắn
người tham gia Chính là MCU / USB chính
người tham gia Tổ chức như OS / Driver
tham gia trò chơi như trò chơi
vòng lặp Thời gian động cơ
Enc >> MC: góc + trạng thái
MC->>MC: mẫu hiện tại và cập nhật PWM
kết thúc
MC - >> Chính: góc mạch lạc / ảnh chụp sức khỏe
Chính->>Chính: hợp nhất bàn đạp / nút / chuyển
Main->>Host: Báo cáo đầu vào HID
Host->>Game: điều khiển trình điều khiển
Game->>Host: hiệu ứng tạo/cập nhật
Máy chủ->>Chính: Báo cáo PID
Chính->>Chính: xác nhận / cập nhật trạng thái hiệu ứng
Main->> Arb: mô-men xoắn yêu cầu + độ tươi
Arb->>Arb: enable/limit/derate/slew
Arb->>MC: giới hạn nhu cầu
alt liên kết cũ hoặc lỗi quan trọng
Arb >> MC: kiểm soát không hoặc ngay lập tức ức chế bởi lớp lỗi
kết thúc
```

### 9.2. Đường ống đầu vào

Đầu vào từ các cảm biến phải trải qua các giai đoạn xác nhận và hiệu chuẩn tiêu chuẩn trước khi được báo cáo cho máy chủ.

** Hình 9-2: Đường ống xử lý đầu vào **

```nàng tiên cá
Lưu đồ LR
Raw[GPIO/ADC/Bus] --> Hợp lệ[Range/CRC/Freshness]
Hợp lệ -> Bộ lọc [Debounce / Filter / Hyperesis]
Bộ lọc -> Cal [Hiệu chuẩn / Caling]
Cal --> Ảnh chụp nhanh[Ảnh chụp nguyên tử]
Ảnh chụp -> HID [Báo cáo đầu vào USB]
```

### 9.3. Xử lý dữ liệu cổ phần

Phần mềm phải xác định và áp dụng các chính sách dữ liệu nhà nước nghiêm ngặt. Mỗi dữ liệu được truyền phải bao gồm giá trị, dấu thời gian, hiệu lực, chủ sở hữu và chính sách của nhà nước.

** Bảng 9-1: Các yếu tố tiêu chuẩn Datum **

| Yếu tố | Kiểu | Mô tả |
|---------|------|-------------|
| `value` | Tải trọng | Giá trị số hoặc trạng thái |
| `timestamp` | uint32 | Thời gian dữ liệu được lấy mẫu hoặc tạo ra |
| "validity" | Boolean | Cho biết dữ liệu là đáng tin cậy hay hợp lệ |
| `owner` | Enum | Hệ thống con khởi tạo dữ liệu |
| `stale_policy` | Enum | Yêu cầu thao tác khi dữ liệu vượt quá thời gian chờ |

**Bảng 9-2: Chính sách Stale theo nguồn **

| Nguồn dữ liệu | Chính sách cổ phần |
|---|---|
| Mô-men xoắn / Hiệu ứng | Sẽ trải qua phân rã xác định và dừng lại; sẽ không bao giờ giữ vô thời hạn |
| Bộ mã hóa / Dòng điện | Sẽ kích hoạt lỗi điều khiển ngay lập tức khi không hợp lệ vượt quá dung sai |
| Nút | Sẽ xóa hoặc giữ lại dựa trên ngữ nghĩa giao thức rõ ràng |
| Bàn đạp | Sẽ đánh dấu đầu vào là không hợp lệ hoặc quay trở lại trạng thái báo cáo an toàn được ghi lại |
| Nhiệt độ | Áp dụng giảm tốc bảo thủ hoặc lỗi trên cảm biến không hợp lệ |
| Từ xa vành | Sẽ xóa các điều khiển bị ngắt kết nối và dừng cập nhật màn hình |

Firmware sẽ sử dụng ảnh chụp nhanh nguyên tử hoặc bộ đệm kép giữa Interrupt Service Routines (ISR) và các tác vụ.

## 10. Nhiệm vụ thời gian thực

Phần này xác định bối cảnh thực hiện và yêu cầu thời gian cho các tác vụ hệ thống. Nó đặt các mục tiêu hiệu suất cho các vòng điều khiển quan trọng.

### 10.1. Bối cảnh thực thi

Firmware sẽ chỉ định các ưu tiên đảm bảo vòng lặp phần cứng nhanh và các cơ chế bảo vệ ngăn chặn các tác vụ vận chuyển và nền.

**Hình 10-1: Phân cấp ưu tiên nhiệm vụ **

```nàng tiên cá
Sơ đồ TD
Break [Comparator / Gate Fault] -> P0 [Immediate Inhibit]
ADCIRQ [ADC / PWM IRQ] -> P1 [Điều khiển động cơ]
EncIRQ [Bộ mã hóa hoàn chỉnh / Lỗi] -> P1
USBIRQ [USB IRQ] -> P2 [Vận chuyển giới hạn]
BusIRQ [CAN / SPI / UART IRQ] -> P3 [Hoàn thành bộ đệm]
Nhiệm vụ [Nhiệm vụ RTOS] -> P4 [FFB / Đầu vào / Giám sát]
BG[Background] --> P5[Logs/NVM/UI]
```

### 10.2. Thời gian và thời hạn

Bảng dưới đây cung cấp các mục tiêu tần số phổ biến. Tốc độ chính xác là yêu cầu thực hiện. Tốc độ vật lý trò chơi, nhịp USB, tốc độ đánh giá FFB, tốc độ vòng lặp động cơ, độ trễ từ đầu đến cuối, jitter và băng thông là các số liệu hiệu suất riêng biệt.

| Hoạt động | Dải tần số | Bối cảnh | Hậu quả của Miss |
|---|---|---|---|
| Vòng hiện tại / FOC | 10-40 kHz | Hẹn giờ / ADC ISR hoặc lõi động cơ | Biến dạng mô-men xoắn, lỗi quá dòng |
| Đọc bộ mã hóa | Tốc độ điều khiển | SPI DMA / hẹn giờ ISR | Tính toán góc Stale
| FFB / trọng tài mô-men xoắn | 0,5-2 kHz | Nhiệm vụ ưu tiên cao Jitter, độ trễ pha trong phản hồi
| Giao thông USB | Kết thúc nhịp | Nhiệm vụ ISR + | Báo cáo bị rơi hoặc trì hoãn |
| Rim Link | 100–1000 Hz | DMA + task | Stale control input, delayed display |
| Bàn đạp / Nút | 100–1000 Hz | ADC DMA / tác vụ hẹn giờ | Độ trễ đầu vào cao, tiếng ồn |
| Giám sát an toàn | Giới hạn phần cứng + 10–1000 Hz | Phần cứng / ISR / nhiệm vụ | Tắt máy muộn |
| Chẩn đoán / NVM | Theo yêu cầu | Nhiệm vụ ưu tiên thấp | Không được chặn thực thi điều khiển |

# 10.3 Quy tắc thời gian thực

Phần vững sẽ đo Thời gian thực hiện trường hợp tồi tệ nhất (WCET) dưới lưu lượng truy cập tối đa và tranh chấp DMA. Thời gian thực hiện ISR sẽ bị giới hạn. Phần vững sẽ cấm phân bổ bộ nhớ, xóa / ghi flash và chặn I / O trong các đường dẫn điều khiển động cơ. Hệ thống sẽ phát hiện thời gian vượt quá. Cơ quan giám sát phần cứng sẽ chỉ được phục vụ từ đường dẫn quan trọng đã được xác minh.

## 11. An toàn và Bảo mật

Phần này đề cập đến việc phát hiện lỗi và bảo vệ hệ thống. Nó chỉ định các phản ứng cần thiết đối với các điều kiện nguy hiểm, thỏa hiệp hệ thống và phác thảo các giao thức thiết lập phù hợp.

# 11.1. Thiết lập và yêu cầu an toàn

Phần này xác định các yêu cầu an toàn và vận hành bắt buộc để thiết lập và thử nghiệm thiết bị đua xe mô phỏng. Tuân thủ là rất quan trọng do khả năng mô-men xoắn cao của các hệ thống truyền động trực tiếp.

- Thiết bị phải được gắn chắc chắn trước khi vận hành.
- Nhân viên vận hành phải kiểm tra ống xả nhanh, dây cáp, bộ cấp điện, công tắc ngắt mô-men xoắn trước khi vận hành.
- Nhà điều hành phải sử dụng phần mềm và quy trình cập nhật đã được phê duyệt.
- Hệ thống phải được hiệu chỉnh về tâm lái, tầm lái, bàn đạp.
- Hoạt động ban đầu sẽ bắt đầu ở một thiết lập mô-men xoắn thấp bằng cách sử dụng bộ lọc mặc định.
- Người vận hành phải kiểm tra hướng động cơ, đầu vào và chức năng công tắc tắt mô-men xoắn trước khi sử dụng bình thường.
- Người vận hành phải khớp phạm vi lái phần cứng với phạm vi lái trò chơi.
- Người vận hành nên tăng dần mô-men xoắn và theo dõi hệ thống để cắt, dao động và nhiệt quá mức.
- Nhân viên vận hành phải giữ tay, trẻ em, tóc, quần áo và dây cáp tránh xa các bộ phận quay.
- Người dùng sẽ không bao giờ bỏ qua khóa liên động vật lý hoặc các tính năng bảo mật firmware.
- Phần cứng động cơ cải tiến phải yêu cầu xác minh hướng, tỷ lệ dòng điện, mô-men xoắn giới hạn và cơ chế vô hiệu hóa cổng độc lập trước khi cấp điện.

11.2. Kiểm soát mối nguy hiểm

Hệ thống sẽ bảo vệ người dùng và phần cứng khỏi hành vi bất ngờ.

** Bảng điều kiện: Phản ứng lỗi **

| Điều kiện | Kích hoạt | Hành động |
|---|---|---|
| "Dữ liệu máy chủ cố định" HOẶC "Đã vượt quá giới hạn mô-men xoắn" | Mô-men xoắn bất ngờ | Thực hiện điều khiển bằng không hoặc ức chế ngay lập tức |
| "Encoder polarity differs from driven polarity" | Sai hướng | Từ chối bật và chốt lỗi |
| `Giai đoạn hiện tại > OVERCURRENT_TRIP` | Quá dòng | Phần cứng PWM vô hiệu hóa thông qua đầu vào ngắt so sánh |
| `Inverter Temp > THERMAL_LIMIT` | Overtemperature | Áp dụng giảm nhiệt; nếu vượt quá, tắt PWM |
| `Điện áp DC Bus> OVERVOLTAGE_TRIP` | Quá áp tái sinh | Giảm hoặc vô hiệu hóa mô-men xoắn dựa trên chính sách phanh |
| "Encoder CRC fail" HOẶC "Timeout" | Mất bộ mã hóa | Ngay lập tức ức chế hoặc vào chế độ bị suy giảm đã được xác nhận |
| `Signature check fail` | Cập nhật lỗi | Vẫn ở trạng thái phục hồi bị vô hiệu hóa mô-men xoắn |
| `Watchdog timeout` | Software lockup | Trigger hardware reset; gate outputs default off |

# 11.3. Tư thế an ninh

Phần mềm cơ sở sẽ xác thực hình ảnh cập nhật sản xuất. Hệ thống sẽ xác thực tất cả các độ dài và loại gói bên ngoài. Hệ thống sẽ yêu cầu trạng thái vô hiệu hóa mô-men xoắn trước khi chấp nhận lệnh dịch vụ hoặc gỡ lỗi. Phần mềm bán lẻ sẽ vô hiệu hóa các giao diện gỡ lỗi sản xuất.

** Nền tảng được cấp phép và nhận dạng vành: ** Hướng dẫn Fanatec công khai xác nhận các vị trí cấp phép cấp sản phẩm được mô tả ở trên, không phải thuật toán xác thực nội bộ. Trình giả lập vành cộng đồng thể hiện các quan sát cho các liên kết cơ sở / vành được chọn; chúng không phải là bằng chứng về một cái bắt tay mật mã phổ quát, hợp đồng ClubSport DD / DD + hiện tại hoặc đường dẫn xác thực bảng điều khiển được phê duyệt. Xử lý nhận dạng vành, bật mô-men xoắn và cấp phép nền tảng theo các yêu cầu riêng biệt cho đến khi đặc tả giao diện được phê duyệt chứng minh khác.

## 12. Chế độ xem kỹ thuật firmware

Phần này phác thảo các thực tiễn kỹ thuật, chiến lược thử nghiệm và các bước xác nhận cần thiết để xây dựng firmware.

### 12.1. Yêu cầu kỹ thuật hệ thống con

Kỹ sư phải kiểm tra hợp đồng của từng hệ thống con, chuyển trạng thái và kiểm tra trước khi tích hợp.

| Hệ thống con | API và dòng chảy trạng thái | Mục tiêu thử nghiệm chính |
|---|---|---|
| Boot/update | `reset` → `verify` → `boot/recovery` | Phần cứng không khớp, ảnh hỏng, mất điện khi flash |
| USB/PID | "detached" → "configured" → "suspended" | Xác nhận mô tả, báo cáo mờ, đo độ trễ |
| FFB | "idle" → "allocated" → "playing" → "stopped" | Trạng thái vòng đời, tràn số học, quấn thời gian |
| Trọng tài mô-men xoắn | "vô hiệu hóa" → "sẵn sàng" → "bật" → "lỗi" | Giới hạn ưu tiên, logic lỗi nhiệt, phản ứng vật chủ cũ |
| Điều khiển động cơ | "init" → "offset cal" → "ready" → "run" → "fault" | Điều khiển toán học, độ bão hòa, phần cứng trong vòng lặp (HIL) |
| Cài đặt | "valid" → "dirty" → "commit/error" | Torn ghi, cân bằng hao mòn, dịch lược đồ |
| An toàn | "safe" → "ready" → "enabled" → "fault" | Lỗi tiêm, kiểm tra ưu tiên nhập ngắt |

### 12.2. Trình tự xác minh

Sự phát triển sẽ theo một tiến trình từ các thử nghiệm cách ly đến vận hành toàn bộ công suất. Công việc mô-men xoắn đầy đủ sẽ chỉ bắt đầu sau khi bằng chứng năng lượng thấp chứng minh hệ thống có thể thực thi các giới hạn và tắt độc lập.

**Hình 12-1: Kiểm tra tiến triển **

```nàng tiên cá
Sơ đồ TD
Đơn vị [Kiểm tra đơn vị lưu trữ] -> Thành phần [Kiểm tra trình điều khiển / thời gian mục tiêu]
Thành phần -> HIL [Hardware-in-the-Loop]
HIL -> Power [Kiểm tra năng lượng thấp được bảo vệ]
Nguồn --> Hệ thống[System/Game/Peripheral/Fault Tests]
Hệ thống --> Ngâm [Soak / EMC / Nhiệt / Power-Cycle Tests]
```

Các kỹ sư phải xác minh thực tế quy mô và hướng mã hóa, thời gian ADC và cổng độc lập vô hiệu hóa trước khi kết nối nguồn điện đầy đủ.

## 13. Câu hỏi chưa được giải quyết

Phần này liệt kê các yêu cầu kỹ thuật và sản phẩm phải được xác nhận trước khi thực hiện.

- Nền tảng chuyển động, đầu dò xúc giác, buồng lái và phần mềm đo từ xa có nên được mở rộng thành các tài liệu riêng biệt không?
- Mô-men xoắn, tốc độ, quán tính, quay, âm thanh và yêu cầu môi trường của sản phẩm?
- Nền tảng PC / console được hỗ trợ và kiến trúc cấp phép được phê duyệt?
- MCU / ASIC chính xác, bộ mã hóa, trình điều khiển cổng, cấu trúc liên kết cảm biến và xếp hạng công suất?
- Cấu trúc liên kết điện / giao thức ngoại vi và quyền sở hữu?
- Mô tả USB, báo cáo nhịp, khả năng hiệu ứng và giao diện nhà cung cấp?
- Đường dẫn mô-men xoắn phần cứng và mục tiêu an toàn / quy định?
- Cập nhật chính sách ký, rollback, cung cấp và phục hồi?
- Khả năng tương thích hiệu chuẩn / phiên bản trên cơ sở, động cơ, vành, bàn đạp và bộ điều hợp?
- Ngân sách và phương pháp chấp nhận độ trễ / jitter từ đầu đến cuối?
- Giữ chẩn đoán và bán lẻ quy tắc gỡ lỗi truy cập?

## 14. Tài liệu tham khảo

Phần này cung cấp trích dẫn đến các tiêu chuẩn công cộng, thiết kế tham chiếu và tài liệu của nhà sản xuất.

- [Kiến trúc vành lái] (./wheel_rim.md)
- [Kiến trúc cơ sở bánh xe](./wheel_base.md)
- [Công cụ và thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid) - HID 1.11, bảng sử dụng, điểm vào PID.
- [USB-IF PID Class 1.0] (https://www.usb.org/sites/default/files/documents/pid1_01.pdf) - báo cáo / mô hình HID phản hồi lực.
- [OpenFFBoard wiki công cộng] (https://github.com/Ultrawipf/OpenFFBoard/wiki/) - mô-đun công cộng HID PID, trình điều khiển động cơ, bộ mã hóa và kiến trúc I / O.
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) - các cổng tiếp xúc, phiên bản cơ sở / động cơ, cập nhật và hiệu chuẩn.
- [Fanatec Ecosystem Diagram] (https://help.fanatec.com/hc/de/articles/43786297099281-Fanatec-Ecosystem-Diagramm) - điểm vào trực quan hệ sinh thái chính thức; không có chi tiết sơ đồ không nhìn thấy được suy ra.
- [Fanatec Wheel Bases FAQ] (https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) - các tầng hiện tại, tập hợp ngoại vi bảng điều khiển và bối cảnh thiết bị độc lập PC.
- [Tương thích nền tảng Fanatec] (https://www.fanatec.com/us-en/platforms) - Quy tắc cấp phép cơ sở Xbox wheel/hub và PlayStation.
- [Câu hỏi thường gặp về tay lái Fanatec] (https://help.fanatec.com/hc/en-us/articles/43802514108433-Steering-Wheel-FAQ) - Ngày ngừng sử dụng QR2 và QR1.
- [Đăng ký nguồn hệ sinh thái Fanatec] (./references.md) - ngày xem xét, sử dụng hướng dẫn cộng đồng và các tuyên bố cũ đã biết.
- [Hướng dẫn cập nhật Fanatec] (https://www.fanatec.com/eu/en/explorer/products/racing-wheels-wheel-bases/update-fanatec-firmware-and-drivers/) - cập nhật cơ sở, bánh xe đã chọn, bàn đạp USB và bộ điều hợp.
- [lshachar/Arduino_Fanatec_Wheel] (https://github.com/lshachar/Arduino_Fanatec_Wheel) - trình giả lập SPI vô lăng tùy chỉnh.
- [StuyoP/Fanatec-Wheel-Barebone-Emulator] (https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator) - Trình giả lập chiều dài cơ sở xương sống ATmega328p.
- [Alexbox364/F_Interface_AL] (https://github.com/Alexbox364/F_Interface_AL) - Tay lái tùy chỉnh DIY thông qua SPI.
- [jssting/ArduinoTec-Pedals] (https://github.com/jssting/ArduinoTec-Pedals) - Bộ điều khiển thay thế bàn đạp Fanatec ClubSport.
- [GeekyDeaks/fanatec-pedal-emulator] (https://github.com/GeekyDeaks/fanatec-pedal-emulator) - bàn đạp USB của bên thứ ba proxy qua RJ12.
- [StuyoP/Universal-Shifter-Interface-for-Fanatec] (https://github.com/StuyoP/Universal-Shifter-Interface-for-Fanatec) - giao diện shifter dựa trên chuyển mạch thông qua RJ12.
- [vnmsimulation/VNM_MOTION_CONTROLLER](https://github.com/vnmsimulation/VNM_MOTION_CONTROLLER) - Bộ điều khiển phần cứng dựa trên DIY STM32.
- [FendtXerion3800/Fanatec-Pinout] (https://github.com/FendtXerion3800/Fanatec-Pinout) - Tham chiếu pinout ổ cắm RJ12.
- [gotzl/hid-fanatecff] (https://github.com/gotzl/hid-fanatecff) - Mô-đun trình điều khiển hạt nhân Linux để hỗ trợ Fanatec FFB.
- [Hướng dẫn sử dụng Simucube 2] (https://simucube.com/app/uploads/2022/11/Simucube_2_User_Guide.pdf) - Điều khiển không dây, rơle USB, ghép nối, mô-men xoắn an toàn.
- [Hướng dẫn Simucube 3] (https://docs.simucube.com/Simucube3/index.html) - dữ liệu QR không dây / tuyên bố nguồn.
- [Hỗ trợ cơ sở bánh xe MOZA] (https://support.mozaracing.com/en/support/solutions/articles/70000627811-wheel-base-faqs) - khả năng tương thích, phục hồi, hiệu chuẩn và phát hiện.
- [Simagic Alpha hướng dẫn sử dụng] (https://image.simagic.com/profile/upload/2022/08/16/41a7d396-805a-439e-b0da-0b81632e2511.pdf) - thiết lập công cộng / cập nhật / bối cảnh an toàn.
- [Logitech TRUEFORCE] (https://www.logitechg.com/en-za/innovation/trueforce.html) - tuyên bố vật lý công cộng / âm thanh và xử lý.
- [Tham khảo Infineon PMSM FOC] (https://documentation.infineon.com/aurixtc3xx/docs/kbv1711616051757) - lấy mẫu hiện tại, kích hoạt PWM, hiệu chuẩn bù đắp.
- [TI TIDA-01599] (https://www.ti.com/tool/TIDA-01599) - kiến trúc tham chiếu STO công nghiệp được đánh giá.
- [EP1501004A3] (https://patents.google.com/patent/EP1501004A3/en) - ngữ cảnh kiến trúc public motor/encoder/controller.
- [Hiệu ứng Simucube FFB] (https://docs.simucube.com/TunerSoftware/wheelbases/wheelbaseeffects.html)
- [TI cảm biến FOC] (https://software-dl.ti.com/msp430/esd/MSPM0-SDK/2_04_00_06/docs/english/middleware/motor_control_pmsm_sensored_foc/doc_guide/doc_guide-srcs/Sensored_FOC_Motor_Control_Library.html)
