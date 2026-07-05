# Kiến trúc phần cứng và phần mềm cơ sở bánh xe

> Phiên bản: 1.1
> Ngày nghiên cứu: 2026-07-01
> Khán giả: Fresher / junior trong lĩnh vực đua xe sim, trung cấp trong hệ thống nhúng.
> Phạm vi: cơ sở bánh xe đua sim trực tiếp hiện đại. Nội bộ vành: [wheel_rim.md] (./wheel_rim.md).
> Bằng chứng: tiêu chuẩn công cộng, hướng dẫn sử dụng / hỗ trợ nhà sản xuất, dự án nguồn mở công cộng và [wheel_rim.md] (./wheel_rim.md). Không có kỹ thuật đảo ngược firmware độc quyền.

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Mô tả |
|---|---|---|
| 1.0 | 2026-07-01 | Tài liệu nghiên cứu ban đầu. |
| 1.1 | 2026-07-01 | Sắp xếp lại các phần (cơ bản đến nâng cao), áp dụng ngôn ngữ quy phạm, thay thế mã giả bằng bảng giao diện, thêm chú thích hình và chèn đoạn giới thiệu. |

## 1. Tóm tắt điều hành

Phần này cung cấp một cái nhìn tổng quan cấp cao về vai trò của đế bánh xe trong hệ sinh thái đua xe mô phỏng. Nó thiết lập các mô hình kiến trúc và an toàn nền tảng.

Cơ sở bánh xe là trung tâm hệ sinh thái quan trọng về an toàn. Nó đồng thời là một thiết bị giao diện người USB, ổ đĩa servo thời gian thực và trung tâm ngoại vi. Nó báo cáo chỉ đạo và điều khiển, chấp nhận hiệu ứng phản hồi lực, chuyển đổi nhu cầu mô-men xoắn giới hạn thành dòng điện pha động cơ / PWM và tổng hợp dữ liệu vành, bàn đạp, bộ chuyển đổi và phanh tay.

Kiến trúc nên sử dụng MCU chính cho USB, FFB, cấu hình, thiết bị ngoại vi, cập nhật và chính sách hệ thống, cùng với MCU động cơ riêng biệt hoặc ASIC điều khiển để thu nhận bộ mã hóa / dòng điện và biến tần PWM. Một trọng tài mô-men xoắn sẽ hoạt động như cầu nối phần mềm duy nhất giữa các miền này. Phần cứng quá dòng, lỗi cổng, E-stop, cơ quan giám sát và đường dẫn ngắt hẹn giờ sẽ vẫn có thẩm quyền nếu phần mềm không thành công.

Cơ sở sẽ không hoạt động ở trạng thái tắt mô-men xoắn. Hệ thống sẽ không được cấp nguồn trong quá trình thiết lập lại, thực thi bộ nạp khởi động, cập nhật, liệt kê USB, phát hiện vành không tương thích, phản hồi bộ mã hóa / dòng không hợp lệ, điều kiện lệnh mô-men xoắn cũ, mất điện và khôi phục cơ quan giám sát. Kích hoạt mô-men xoắn cao sẽ yêu cầu hình ảnh đã được xác minh, tự kiểm tra thành công, cảm biến được hiệu chỉnh, giai đoạn nguồn khỏe mạnh, chính sách phần mềm rõ ràng và không có lỗi chốt.

## 2. Phạm vi và bằng chứng

Phần này xác định ranh giới của phân tích và làm rõ mức độ tin cậy của thông tin được trình bày. Nó là cần thiết để hiểu bối cảnh của các tuyên bố được thực hiện.

| Nhãn | Ý nghĩa |
|---|---|
| Hành vi công cộng đã được xác minh | Được hỗ trợ bởi một tiêu chuẩn công cộng, hướng dẫn sử dụng hoặc tài liệu dự án |
| Mô hình công nghiệp | Kiến trúc chung, không phổ quát |
| Thiết kế tham chiếu | Cấu trúc dự án được đề xuất, không phải yêu cầu nội bộ của nhà cung cấp |
| Không rõ | Cần sơ đồ khách hàng, BOM, nguồn, mô tả hoặc yêu cầu |

Các mặt hàng bao gồm là thiết bị điện tử cơ bản, bộ xử lý, biến tần, cảm biến, USB, thiết bị ngoại vi, phần sụn, thời gian, an toàn, cập nhật, chẩn đoán và xác minh. Các mặt hàng không bao gồm là các định dạng độc quyền, phần sụn được trích xuất, bỏ qua xác thực giao diện điều khiển, phương trình / lợi nhuận kiểm soát chi tiết và nội bộ vành.

`repos.md` là đầu vào khám phá, không phải là cơ quan kỹ thuật. Cộng đồng Fanatec dự án thiết lập quan sát, không phải thông số kỹ thuật chính thức.

## 3. Bối cảnh hệ thống

Phần này minh họa cách đế bánh xe tương tác với các tác nhân bên ngoài, bao gồm PC / bảng điều khiển máy chủ, thiết bị ngoại vi của người dùng và nguồn điện.

**Hình 3-1: Sơ đồ ngữ cảnh hệ thống **

```nàng tiên cá
Lưu đồ LR
Trò chơi -->|Hiệu ứng FFB| Máy chủ [Trình điều khiển / Nhà cung cấp dịch vụ]
Máy chủ <-->|Giao diện nhà cung cấp USB HID/PID +| Base
Bảng điều khiển[Bảng điều khiển được cấp phép] <-->|đường dẫn phụ kiện được phê duyệt| Base
PSU [Cung cấp DC cách ly] -> Cơ sở
cơ sở tiểu đồ thị [Cơ sở bánh xe]
Chính [MCU chính] -> MotorDomain [Miền điều khiển động cơ]
An toàn -->|enable / inhibit| MotorDomain
Chính <--> Cổng [Giao diện ngoại vi]
kết thúc
Cơ sở <--> |QR điện / dữ liệu + mô-men xoắn cơ khí | Rim
Bàn đạp -> Cổng
Shifter -> Cổng
Phanh tay -> Cổng
Công cụ[Cấu hình / Cập nhật] <-->|USB| Base
```

| Trách nhiệm | Cơ sở | Vành | Máy chủ |
|---|---|---|---|
| Góc trục | Chính | Không | Tiêu thụ |
| Dòng động cơ/mô-men xoắn | Chính | Không | Hiệu ứng yêu cầu |
| Mô-men xoắn phần cứng ức chế | Chính | Không | Không |
| Kiểm soát vành | Tổng hợp | Quét | Tiêu thụ |
| Đèn LED/hiển thị | Tuyến đường | Ổ đĩa | Sản xuất giá trị |
| USB enumeration/update | Primary | Thường gián tiếp | Điều khiển bus/tool |

### 3.1 Ranh giới hệ sinh thái Fanatec công cộng

Hệ sinh thái công cộng của Fanatec thể hiện mô hình cơ sở như trung tâm nhưng không xuất bản hợp đồng kiến trúc nội bộ. Các sản phẩm hiện tại sử dụng rộng rãi các cấp CSL, ClubSport và Podium. Tên cấp là bối cảnh thương mại / sản phẩm; khả năng tương thích firmware vẫn phụ thuộc vào mô hình chính xác, thế hệ, QR, cổng ngoại vi và phiên bản phần mềm.

Đối với các hệ thống console, cấp phép nền tảng và tập hợp ngoại vi là những mối quan tâm riêng biệt:

| Quan tâm | Quy tắc Fanatec công cộng |
|---|---|
| Khả năng tương thích với Xbox | Xuất phát từ vô lăng hoặc hub được cấp phép của Xbox. |
| Tương thích với PlayStation | Xuất phát từ cơ sở bánh xe được PlayStation cấp phép. |
| Bàn đạp / cần số / phanh tay | Kết nối thông qua đế bánh xe tương thích. |
Thiết bị ngoại vi độc lập PC Các sản phẩm được hỗ trợ có thể kết nối riêng bằng USB hoặc ClubSport USB Adapter.

Đây là những sự kiện cấp sản phẩm. Chúng không thiết lập giao thức vành công cộng, thuật toán xác thực, bộ mô tả USB hoặc phân vùng điều khiển động cơ.

## 4. Kiến trúc phần cứng

Phần này nêu chi tiết các thành phần điện tử vật lý và các lĩnh vực trong cơ sở bánh xe. Quen thuộc với thiết kế PCB tín hiệu hỗn hợp và điện tử công suất là cần thiết để hiểu ranh giới.

** Hình 4-1: Sơ đồ khối kiến trúc phần cứng **

```nàng tiên cá
Sơ đồ TD
USB [USB + ESD + VBUS Sense] -> TẠI SAO -> Chính [MCU chính]
Chính <--> Flash [Flash / EEPROM]
Chính <--> RimIF [Giao diện QR]
Chính <--> Phụ kiện [Pedal / Shifter / Handbrake]
Chính -> TorqueLink [Liên kết mô-men xoắn giới hạn]

tiểu đồ thị MotorDomain [Miền động cơ thời gian thực cứng]
Liên kết mô-men xoắn -> MotorMCU [Motor MCU / ASIC]
Bộ mã hóa -> Bộ mã hóaIF -> MotorMCU
Shunts -> CSA [Bộ khuếch đại hiện tại] -> ADC [Kích hoạt ADC + PWM] -> MotorMCU
MotorMCU -> Hẹn giờ [PWM bổ sung] -> Cổng [Trình điều khiển cổng]
Cổng -> Cầu [Cầu MOSFET 3 pha] -> Động cơ [PMSM / BLDC]
kết thúc

DC [Đầu vào DC] -> Bảo vệ [Fuse / Reverse / TVS / Inrush] -> Xe buýt [Liên kết DC]
Xe buýt -> Cầu
Xe buýt -> Rails [Logic Rails] -> Chính
Xe buýt -> Sense [Điện áp / Sense hiện tại] -> Chính
Nhiệt [Nhiệt độ động cơ / FET / PCB] -> Chính
Cổng -->|FAULT| Latch[Hardware Fault Latch]
OCP [Bộ so sánh quá dòng] -> Chốt
EStop [E-stop / Torque-Off] -> Chốt
Chốt -> ức chế không đồng bộ | Cổng
```

| Tên miền | Nội dung | Mục tiêu thiết kế |
|---|---|---|
| Logic | Main MCU, USB, NVM, bộ thu phát phụ kiện | Bảo vệ USB / logic khỏi chuyển đổi tiếng ồn |
| Điều khiển động cơ | Motor MCU/ASIC, bộ thu mã hóa, đường dẫn ADC | Đường phản hồi ngắn xác định |
Giai đoạn nguồn Trình điều khiển cổng, cầu MOSFET, shunt, liên kết DC Điện cảm thấp, làm mát, ngăn chặn lỗi
| Đầu vào nguồn | Bảo vệ, xâm nhập, đường ray, chiến lược regen | Chứa lỗi nguồn / thoáng qua |
| Kết nối | USB, QR, phụ kiện, E-stop | ESD / cách ly lỗi cáp |

## 5. Nguồn điện và động cơ

Phần này giải thích các thiết bị điện tử công suất cần thiết để chuyển đổi nguồn DC đến thành dòng điện xoay chiều ba pha điều khiển động cơ servo.

**Hình 5-1: Dòng điện và động cơ **

```nàng tiên cá
Lưu đồ LR
AC -> PSU [AC / DC cách ly]
PSU -> Cầu chì -> Đảo ngược [Bảo vệ ngược] -> Inrush -> Bus [DC Bus]
Xe buýt -> Biến tần -> Động cơ
Xe buýt -> Buck [Logic Rails]
Xe buýt -> Regen [Chiến lược kẹp / phanh / Regen]
Xe buýt -> Đo lường [Điện áp / Hiện tại / Nhiệt độ]
```

| Chặn | Trách nhiệm phần cứng | Trách nhiệm phần cứng |
|---|---|---|
| PSU | Isolated rated DC | Phạm vi phát hiện; không giả định hấp thụ regen |
Bảo vệ đầu vào Cầu chì / eFuse, đảo ngược, TVS, xâm nhập Trình tự / báo cáo lỗi có thể kiểm soát
| Liên kết DC | Năng lượng xung | Điện áp giám sát và lề regen |
| Trình điều khiển cổng | Drive, UVLO, lỗi | Cấu hình; chỉ bật sau khi kiểm tra |
| Biến tần | Chuyển đổi DC sang ba pha | Chỉ PWM miền động cơ |
| Shunts/CSA | Phản hồi hiện tại | Offset/gain/saturation/plausibility |
| Regen phần cứng | Xử lý năng lượng trở lại | Phối hợp giảm mô-men xoắn / kẹp |
| Đường ray | MCU/cảm biến/phụ kiện cung cấp | Power-good/reset sequencing |

Chuyển động nhanh của người dùng có thể tái tạo năng lượng vào bus DC. Phần sụn sẽ tính toán năng lượng trả lại để xác định giới hạn hấp thụ nguồn cung cấp, điều phối điện trở / kẹp phanh và quản lý giới hạn điện áp.
Đối với thế hệ PWM, bộ vi điều khiển sẽ sử dụng đầu ra hẹn giờ bổ sung. Phần cứng sẽ thực thi thời gian chết. Bộ kích hoạt ADC phải được đồng bộ hóa với chu kỳ PWM. Phần cứng sẽ cung cấp đầu vào ngắt không đồng bộ để dừng ngay việc chuyển đổi. Tín hiệu bật cổng sẽ mặc định ở trạng thái tắt thông qua cài đặt lại và thực thi bộ nạp khởi động. Phần sụn sẽ thực hiện tất cả các cập nhật tham số PWM trên ranh giới nguyên tử.

# 5.1 Làm thế nào biến tần tạo ra dòng điện ba pha

Biến tần là trái tim sức mạnh của cơ sở bánh xe. Một động cơ PMSM / BLDC không thể được điều khiển từ DC thô; nó cần ba dòng pha hình sin, bù 120 ° với nhau, có từ trường quay mà rôto theo sau. Biến tần tổng hợp các pha đó từ bus DC cố định bằng cách sử dụng sáu MOSFET công suất được sắp xếp thành ba ** nửa cầu ** (một trên mỗi pha).

![Biến tần ba pha điều khiển động cơ](../assets/three_phase_inverter.svg)

Mỗi pha có một công tắc phía cao (đến đường ray DC +) và công tắc phía thấp (đến DC−). Nhanh chóng bật và tắt công tắc thích hợp với PWM đặt điện áp trung bình trên pha đó; làm điều này trên cả ba chân với đúng thời gian tạo ra trường quay. Hai sự thật từ bản đồ vẽ này trực tiếp lên các yêu cầu quy phạm ở trên:

- ** Dead-time là bắt buộc. ** Hai công tắc trong một chân không bao giờ được bật cùng nhau, hoặc chúng sẽ tạo ra một đoạn ngắn trực tiếp trên bus DC (được gọi là * shoot-through*) và phá hủy các MOSFET. Phần cứng thực thi một khoảng cách thời gian chết ngắn trong đó cả hai đều tắt trong mỗi lần chuyển đổi.
- ** Các shunt phía thấp cung cấp cho vòng lặp hiện tại. ** Các điện trở shunt nhỏ trong mỗi chân phía thấp là cách đo dòng pha, đó là phản hồi mà vòng lặp dòng FOC (§ 5.2 và § 6) cần để điều khiển mô-men xoắn.

### 5.2 Thời gian PWM và lấy mẫu hiện tại

Điều khiển hướng trường phụ thuộc vào việc đo dòng điện pha một cách chính xác, và *khi * dòng điện được lấy mẫu cũng quan trọng như chính mẫu. Các cạnh chuyển đổi của MOSFET tạo ra nhiễu điện, do đó ADC được kích hoạt tại điểm yên tĩnh ở giữa giai đoạn PWM chứ không phải gần một cạnh.

! [PWM tàu sân bay, chu kỳ làm việc, và điểm mẫu ADC] (./foc_pwm_timing.svg)

Một sóng mang hình tam giác được so sánh với lệnh nhiệm vụ của mỗi pha để tạo tín hiệu cổng: trong đó sóng mang nằm dưới lệnh, công tắc phía cao được bật. Dòng lấy mẫu ở đỉnh sóng mang (giữa thời gian bật) thu được giá trị trung bình sạch khỏi các chuyển đổi tạm thời. Đây chính xác là yêu cầu "kích hoạt giữa PWM hợp lệ" trong §6 và zoom thời gian chết cho thấy khoảng cách cả hai nhỏ ngăn chặn bắn xuyên qua trên mọi cạnh.

## 6. Cảm biến

Phần này nêu chi tiết các cơ chế phản hồi được sử dụng để đo vị trí trục và dòng động cơ, rất quan trọng cho điều khiển hướng trường (FOC) và phản hồi lực.

Bản thân động cơ là PMSM ba pha: một stato thép bị thương xung quanh rôto nam châm vĩnh cửu kết hợp với trục lái. Bộ mã hóa đọc góc rôto / trục mà FOC cần giao hoán chính xác và cảm biến hiện tại đọc dòng pha FOC điều chỉnh. Mặt cắt ngang dưới đây cho thấy các phần liên quan như thế nào.

! [Phần cắt ngang động cơ servo trực tiếp] (./servo_motor_cross_section.svg)

| Bộ mã hóa | Sức mạnh | Quan tâm |
|---|---|---|
| SPI/SSI/BiSS-C absolute | Angle at boot, CRC/status | Timing, receiver, wrap |
| ABZ | Độ trễ đơn giản/thấp | Tham chiếu/chỉ mục và các cạnh bị thiếu |
| Sin/Cos | Nội suy tốt | Analog offset/gain/phase |
| Khu vực hội trường | Chuyển đổi thô mạnh mẽ | Không đủ một mình cho tay lái cao cấp |

**Hình 6-1: Đường ống xử lý cảm biến **

```nàng tiên cá
Lưu đồ LR
Bộ mã hóa -> Mua [SPI / Hẹn giờ / ADC]
Mua -> Xác nhận [CRC / Trạng thái / Phạm vi / Nhảy]
Xác thực --> Cal [Direction / Offset / Scale]
Cal --> Unwrap [Góc / Tốc độ]
Unwrap -> Kiểm soát
Unwrap -> USBReport [Ảnh chụp trục USB]
```

| Tín hiệu | Thu nhận | Kiểm tra |
|---|---|---|
| Dòng pha | Shunt / CSA / ADC đồng bộ | Offset, độ lợi, độ bão hòa, tính nhất quán |
| Dòng điện DC | Shunt / Hall ADC | Quá dòng, độ tin cậy của điện |
| Điện áp DC | Bộ chia / cách ly ADC | Brownout, danh nghĩa, quá áp regen |
| FET / nhiệt độ động cơ | NTC / IC / model | Mở / ngắn, tốc độ, giảm tốc |
| Sức khỏe đường sắt | Supervisor/ADC/GPIO | Nguồn điện tốt và nguyên nhân thiết lập lại |

Kiến trúc nên hỗ trợ lấy mẫu hiện tại trong mỗi chu kỳ PWM, sử dụng các trình kích hoạt giữa PWM hợp lệ và phải thực hiện hiệu chuẩn bù đắp khởi động. Các cửa sổ lấy mẫu chính xác phụ thuộc vào cấu trúc liên kết biến tần và sơ đồ điều chế đã chọn.

## 7. Giao diện bên ngoài

Phần này liệt kê các ranh giới vật lý và logic nơi cơ sở bánh xe giao tiếp với các thiết bị ngoại vi phần cứng bên ngoài.

| Giao diện | Chủ | Mục đích |
|---|---|---|
| USB FS / HS | MCU chính | HID đầu vào / PID FFB, cấu hình, chẩn đoán, cập nhật
| QR / liên kết rim | MCU chính | Nhận dạng, điều khiển, đầu ra LED / hiển thị |
| Pedal/shifter/handbrake | Main MCU | Analog/digital controls (e.g., via RJ12, subject to hardware emulation proxying) |
| Bộ mã hóa động cơ | MCU động cơ | Phản hồi rôto / trục |
| Main↔motor | Cả hai | Mô-men xoắn, góc, sức khỏe, lỗi |
| SWD/JTAG/UART/service USB | Dịch vụ điều khiển | Sản xuất/phục hồi |
| Khóa E-stop / mô-men xoắn | Logic an toàn phần cứng | Quyền / ức chế mô-men xoắn |

Các dự án cộng đồng kế thừa báo cáo SPI cơ sở 3.3 V / rim-slave cho các sản phẩm cũ hơn, cùng với ranh giới tương thích mới hơn cho các cơ sở truyền động trực tiếp hiện đại. Liên kết vành nên được thiết kế như một bộ điều hợp giao thức có thể thay thế thay vì một giả định phổ quát. Đường ray điện phụ kiện phải được bảo vệ bằng điện để các lỗi vành hoặc cáp không thể làm sụp đổ logic điều khiển động cơ chính.

## 8. Phân vùng bộ xử lý

Phần này thảo luận về việc phân phối trách nhiệm tính toán. Tách ngăn xếp USB khỏi điều khiển động cơ thời gian thực cứng là một quyết định kiến trúc cốt lõi.

| Tùy chọn | Sức mạnh | Yếu đuối | Sử dụng |
|---|---|---|---|
| MCU đơn | BOM thấp / cập nhật đơn giản | Miền thời gian / lỗi được chia sẻ | Chỉ với lề đã được chứng minh |
| Main + motor MCU | Phân tách thời gian / lỗi | Yêu cầu IPC phiên bản | Cơ sở mô-men xoắn cao được đề xuất |
| Main + control ASIC | Điều khiển xác định chuyên biệt | Giới hạn của nhà cung cấp / khóa trong | Nếu cảm biến / điều khiển phù hợp được chứng minh |
| Main MPU + motor MCU | Mạng / UI phong phú | Hệ điều hành khởi động / bảo mật phức tạp | Sản phẩm kết nối cao cấp |

**Hình 8-1: Tương tác miền bộ xử lý **

```nàng tiên cá
Lưu đồ LR
đồ thị con Chính
USB[USB/PID]
FFB [Động cơ FFB]
Đầu vào [Tổng hợp đầu vào]
Hồ sơ[Profiles]
Supervisor[Giám sát an toàn]
kết thúc
biểu đồ phụ MotorMCU
Enc[Bộ mã hóa]
Curr [Hiện tại]
Điều khiển [Điều khiển động cơ]
PWM[PWM] Name
FastFault [Lỗi nhanh]
kết thúc
FFB -> Arbiter [Arbiter mô-men xoắn]
Giám sát -> Trọng tài
Trọng tài -->|lệnh ràng buộc + độ tươi| Kiểm soát
Kiểm soát -->|angle/current/health| Supervisor
Enc -> Kiểm soát
Curr -> Kiểm soát
Điều khiển -> PWM
FastFault -> PWM
```

| Dữ liệu | Hướng | Yêu cầu |
|---|---|---|
| Yêu cầu mô-men xoắn | Chính → động cơ | Đơn vị vật lý, ràng buộc, trình tự, dấu thời gian |
| Bật/giới hạn | Chính → động cơ | Rõ ràng; tắt mặc định; động cơ áp dụng lại giới hạn cục bộ |
| Góc / tốc độ / dòng điện | Động cơ → chính | Giá trị, hiệu lực, dấu thời gian, ngữ nghĩa gói |
| Sức khỏe / lỗi | Cả hai | Phiên bản, đặt lại lý do, thời hạn và trạng thái lỗi |
Nhịp tim Cả hai Bộ đếm độc lập và phản ứng hết giờ

Các lệnh chính của Stale sẽ đạt đến mô-men xoắn bằng không hoặc kích hoạt bộ ức chế phần cứng trong thời gian giới hạn.

## 9. Kiến trúc phần mềm

Phần này phác thảo các mô-đun logic bao gồm phần sụn cơ sở bánh xe và cách ly cần thiết của chúng để đảm bảo an toàn và xác định.

**Hình 9-1: Kiến trúc thành phần phần mềm**

```nàng tiên cá
Sơ đồ TD
Boot [Bộ nạp khởi động được bảo vệ] -> BSP [BSP / Chân an toàn / Tự kiểm tra]
subgraph Dịch vụ
USB [USB / HID / PID] Name
FFB[Trình quản lý hiệu ứng]
Trọng tài[Trọng tài mô-men xoắn]
Đầu vào [Tập hợp đầu vào]
Rim [Quản lý Rim]
Acc [Quản lý phụ kiện]
Cài đặt[Cài đặt / Hiệu chuẩn]
Cập nhật[Update]
Diag [Chẩn đoán]
An toàn[Giám sát an toàn]
kết thúc
động cơ subgraph
Bộ mã hóa[Dịch vụ mã hóa]
Hiện tại [Mua lại hiện tại]
Bộ điều khiển [Điều khiển động cơ]
PWM[Trình điều khiển PWM]
Lỗi [Xử lý lỗi nhanh]
kết thúc
BSP --> Dịch vụ
USB -> FFB -> Arbiter -> Bộ điều khiển
Rim -> Đầu vào -> USB
Acc -> Đầu vào
Cài đặt -> FFB
An toàn -> Trọng tài
Bộ mã hóa -> Bộ điều khiển
Hiện tại -> Bộ điều khiển
Bộ điều khiển -> PWM
Lỗi -> PWM
```

| Mô-đun | Sở hữu | Không được sở hữu |
|---|---|---|
| USB | Mô tả, điểm cuối, báo cáo | PWM/cổng ghi |
| FFB | Trạng thái hiệu ứng/trộn | Bật an toàn |
| Torque arbiter | Final enable/torque/slew/thermal/power/freshness limits | USB transport |
| Điều khiển động cơ | Phản hồi đến PWM | Phân tích cú pháp máy chủ |
| An toàn | Chính sách trạng thái / lỗi | Bảo vệ điện duy nhất |
| Thiết bị ngoại vi | Khám phá, lập bản đồ, sức khỏe | Kích hoạt động cơ |
| Cài đặt | Sơ đồ/nguyên tử bền bỉ | Flash ghi trong ISR điều khiển |
| Chẩn đoán | Sự kiện/dấu vết bị chặn | Ngăn chặn đầu ra |
| Cập nhật / khởi động | Xác minh / giai đoạn / phục hồi | Kích hoạt động cơ |

## 10. Đường dẫn phản hồi lực

Phần này theo dõi vòng đời của hiệu ứng phản hồi lực từ yêu cầu của máy chủ xuống dòng động cơ. Nó tập trung vào cách các hiệu ứng trừu tượng trở thành mô-men xoắn vật lý.

**Hình 10-1: Dòng dữ liệu Force-Feedback **

```nàng tiên cá
Lưu đồ LR
Trò chơi --> API [DirectInput / HID PID / Nhà cung cấp SDK]
API -> USB [Báo cáo USB] -> Phân tích cú pháp [Phân tích cú pháp được xác thực]
Phân tích cú pháp -> Pool [Effect Pool] -> Mixer [Effect Mixer]
Máy trộn -> Arbiter [Arbiter mô-men xoắn]
Giới hạn [Mô-men xoắn / Dòng điện / Xoay / Nhiệt / Điện áp / Độ tươi] -> Trọng tài
Arbiter -> Điều khiển [Điều khiển dòng động cơ] -> PWM -> Biến tần -> Động cơ
Bộ mã hóa -> Kiểm soát
Hiện tại -> Kiểm soát
```

| Chặn | Trách nhiệm |
|---|---|
| Phân tích cú pháp | Xác thực ID, kích thước, chỉ mục, đơn vị, thời lượng |
| Hồ hiệu ứng | Phân bổ/cập nhật/bắt đầu/dừng hiệu ứng tất định |
| Mixer | Kết hợp hiệu ứng mà không tràn |
| Bộ lọc thiết bị | Cấu hình giảm xóc/ma sát/ quán tính/làm mịn |
| Trọng tài mô-men xoắn | Áp dụng tất cả các giới hạn an toàn và sản phẩm cuối cùng |
| Điều khiển động cơ | Chuyển đổi yêu cầu mô-men xoắn sang hiện tại / PWM; không có ngữ nghĩa hiệu ứng |
Lỗi phần cứng Loại bỏ ổ đĩa cổng một cách độc lập

Theo dõi độ tươi của máy chủ phải rõ ràng. Mất máy chủ bình thường sẽ kích hoạt chính sách phân rã bị chặn và vô hiệu hóa. Cảm biến quan trọng hoặc lỗi điện sẽ kích hoạt ức chế phần cứng ngay lập tức.

# 10.1. Bất biến tên miền-động cơ

1. Đầu ra PWM sẽ không hoạt động sau khi đặt lại cho đến khi nhận được lệnh bật hợp lệ.
2. Bộ mã hóa không hợp lệ hoặc phản hồi hiện tại sẽ không cùng tồn tại với mô-men xoắn hoạt động vượt quá giới hạn thời gian phát hiện được ghi lại.
3. Cả MCU chính và miền động cơ sẽ thực thi giới hạn mô-men xoắn và tốc độ.
4. Tín hiệu ngắt phần cứng sẽ ghi đè bất kỳ lệnh phần mềm nào.
5. Giao tiếp giữa các bộ xử lý (IPC) sẽ đạt mô-men xoắn bằng không hoặc ức chế phần cứng trong một thời gian giới hạn.
6. Dữ liệu hiệu chuẩn phải được phiên bản và kiểm tra phạm vi toán học trước khi sử dụng.
7. NaN, tràn số, liệt kê không hợp lệ hoặc khung truyền thông bị hỏng sẽ không bao giờ đến được mô-đun tạo PWM.

# 10.2. Tổng hợp đầu vào

** Hình 10-2: Dòng dữ liệu tổng hợp đầu vào **

```nàng tiên cá
Lưu đồ LR
Rim [Rim Snapshot] -> Xác thực
Bàn đạp [Bàn đạp] -> Xác thực
Shifter [Shifter] -> Xác thực
Phanh tay [Phanh tay] -> Xác nhận
Góc [Góc lái] -> Xác nhận
Xác nhận [Hiệu lực / Tươi / Hiệu chuẩn] -> Tổng hợp [Tổng hợp nguyên tử]
Tổng hợp -> HID [Báo cáo đầu vào USB]
```

Mỗi giá trị tổng hợp sẽ mang dấu thời gian, trạng thái và mã nhận dạng nguồn. Báo cáo đầu vào USB sẽ sử dụng ảnh chụp nhanh trạng thái hệ thống mạch lạc thay vì đọc trực tiếp từ các thói quen dịch vụ ngắt khác nhau. Việc đọc bộ mã hóa cũ sẽ được coi là lỗi động cơ quan trọng. Liên kết vành cũ sẽ tự động xóa các điều khiển nút tạm thời để ngăn chặn các đầu vào bị kẹt. Dữ liệu bàn đạp, bộ chuyển đổi hoặc phanh tay không hợp lệ sẽ quay trở lại các giá trị báo cáo an toàn được ghi lại rõ ràng.

## 11. Phần mềm máy chủ

Phần này mô tả các trình điều khiển và dịch vụ phía máy chủ tương tác với cơ sở bánh xe. Trong khi các nhà phát triển nhúng không viết trình điều khiển hệ điều hành, họ phải hiểu các yêu cầu của nó.

**Hình 11-1: Mô hình tương tác phần mềm máy chủ **

```nàng tiên cá
Sơ đồ TD
Trò chơi -->|DirectInput / HID PID / SDK| Trình điều khiển [HĐH / Trình điều khiển nhà cung cấp]
Trình điều khiển <-->|USB HID/PID| Base
Config[Control Panel] <-->|vendor config| Base
Updater[Update Manager] <-->|giao diện phục hồi| Base
Trò chơi -->|telemetry| Service[Telemetry Service]
Dịch vụ -->|LED/display| Base
```

Trình điều khiển `hid-fanatecff` công khai thể hiện cách các hiệu ứng đầu vào và phản hồi lực của Linux chuyển thành các báo cáo HID tùy chỉnh trên bộ hẹn giờ mặc định 2 ms không đồng bộ. Nó ánh xạ các đường dẫn sysfs / HIDRAW cho phạm vi điều chỉnh, ID bánh xe, đèn LED, màn hình, phản hồi tế bào tải và đạp rumble khi được hỗ trợ. Các công cụ như UDP cầu nối `hid-fanatecff-tools` hoặc từ xa trò chơi chia sẻ bộ nhớ vào các đầu ra này.

Căn cứ quản lý một số mặt phẳng logic thông tin liên lạc riêng biệt:

| Máy bay | Sự quan trọng | Dữ liệu |
|---|---|---|
| Đầu vào | Cao | Tay lái, bàn đạp, nút |
| FFB | Cao | Hiệu ứng/trạng thái liên quan đến mô-men xoắn |
| An toàn | Chính quyền địa phương cao nhất | Kích hoạt, lỗi, điện, nhiệt, độ tươi |
| Cấu hình | Trung bình | Hồ sơ/phạm vi/điều chỉnh |
Hiển thị từ xa Nỗ lực tốt nhất RPM, bánh răng, cờ, tốc độ
| Cập nhật/chẩn đoán | Dịch vụ vô hiệu hóa mô-men xoắn | Hình ảnh, dấu vết, hiệu chuẩn |

## 12. Máy nhà nước

Phần này xác định các trạng thái hoạt động của đế bánh xe và miền động cơ. Nó mô tả các điều kiện chính xác cần thiết để chuyển từ trạng thái nhàn rỗi an toàn sang tạo mô-men xoắn hoạt động.

### 12.1. Vòng đời cơ sở

** Hình 12-1: Máy trạng thái vòng đời cơ bản **

```nàng tiên cá
stateSơ đồ-v2
[*] -> Đặt lại
Đặt lại -> Khôi phục: yêu cầu ứng dụng / khôi phục không hợp lệ
Đặt lại -> Tự kiểm tra: ứng dụng đã xác minh
Tự kiểm tra -> SafeIdle: vượt qua
Tự kiểm tra -> LatchedFault: thất bại
SafeIdle -> Hiệu chỉnh
Hiệu chuẩn -> Sẵn sàng: cảm biến / hiệu chuẩn hợp lệ
Sẵn sàng -> TorqueEnabled: máy chủ + người dùng + khóa liên động hợp lệ
TorqueEnabled -> Derated: giới hạn nhiệt / điện áp
Derated -> TorqueEnabled: phục hồi với trễ
TorqueEnabled -> SafeIdle: vô hiệu hóa bình thường / mất máy chủ
TorqueEnabled -> LatchedFault: lỗi nghiêm trọng
Derated -> LatchedFault: lỗi nghiêm trọng
LatchedFault -> SafeIdle: ủy quyền rõ ràng + điều kiện an toàn
Phục hồi -> Đặt lại: hình ảnh đã xác minh
```

### 12.2. Tên miền động cơ

**Hình 12-2: Máy trạng thái miền động cơ **

```nàng tiên cá
stateSơ đồ-v2
[*] --> Vô hiệu hóa
Vô hiệu hóa -> OffsetCalibration: điện hợp lệ
OffsetCalibration -> EncoderCheck: offsets hợp lệ
EncoderCheck -> Sẵn sàng: bộ mã hóa hợp lệ
Sẵn sàng -> Chạy: bật lệnh + tươi
Chạy -> Sẵn sàng: zero / vô hiệu hóa
Chạy -> Lỗi: lỗi ngắt / phản hồi / thời hạn / lỗi liên kết
Sẵn sàng -> Lỗi: lỗi nghiêm trọng
Lỗi -> Vô hiệu hóa: thiết lập lại / ủy quyền phục hồi
```

Việc liệt kê và cấu hình USB sẽ không ngụ ý rằng mô-men xoắn được bật.

## 13. Thực hiện theo thời gian thực

Phần này xác định thời hạn quan trọng cho phần sụn. Thời hạn bị bỏ lỡ trong vòng điều khiển động cơ có thể dẫn đến biến dạng mô-men xoắn hoặc các nguy cơ an toàn.

| Hoạt động | Phạm vi chung | Bối cảnh | Hụt tác động |
|---|---|---|---|
| Vòng lặp hiện tại / FOC | 10-40 kHz | PWM / ADC ISR hoặc lõi động cơ | Biến dạng mô-men xoắn / lỗi |
| Bộ mã hóa | Mỗi chu kỳ điều khiển / bộ nhân | Hẹn giờ / SPI DMA ISR | Góc ổn định |
Mô-men xoắn / FFB 0,5-2 kHz Nhiệm vụ ưu tiên cao Jitter / độ trễ pha
| USB | Hợp đồng mô tả / điều khiển sự kiện | USB ISR + task | Báo cáo giai đoạn |
| Rim link | 100–1000 Hz hoặc giao thức được xác định | DMA + task | Stale controls/display |
| Bàn đạp | 250-1000 Hz | ADC DMA / tác vụ | Độ trễ / tiếng ồn |
| An toàn | Chuyến đi phần cứng + kiểm tra định kỳ | Phần cứng / ISR / tác vụ | Tắt máy chậm |
| Nhiệt | 10–100 Hz điển hình | Nhiệm vụ | Delayed derating |
| Diagnostics/NVM | Bounded background/on demand | Task/bootloader | Không được chặn điều khiển |

**Hình 13-1: Ưu tiên thực hiện theo thời gian thực**

```nàng tiên cá
Sơ đồ TD
HW [Comparator / Gate Fault / E-stop] -> P0 [Ức chế phần cứng]
ADCIRQ [ADC / PWM IRQ] -> P1 [Điều khiển động cơ]
EncIRQ [Bộ mã hóa hoàn chỉnh / Lỗi] -> P1
IPC [Main-Motor IRQ] -> P2 [Mô-men xoắn / Sức khỏe]
USBIRQ [USB IRQ] -> P3 [USB giới hạn]
BusIRQ [Rim / Phụ kiện IRQ] -> P4 [Hoàn thành bộ đệm]
Nhiệm vụ [Nhiệm vụ RTOS] -> P5 [FFB / Đầu vào / An toàn]
BG [Nền] -> P6 [Logs / NVM / UI]
```

Nhóm phát triển sẽ đo thời gian thực hiện trường hợp xấu nhất mục tiêu (WCET) dưới lưu lượng xe buýt tối đa. Hệ thống sẽ ràng buộc nghiêm ngặt tất cả thời gian thực hiện ISR. Các đường dẫn điều khiển động cơ sẽ không thực hiện phân bổ bộ nhớ, ghi flash, chặn thu nhận khóa hoặc chặn I / O. Kiến trúc sẽ xác định và ghi lại thời gian che mặt nạ gián đoạn tối đa. Phần sụn sẽ hiển thị bộ đếm thời gian phần cứng vượt quá để chẩn đoán. Phần sụn sẽ chỉ phục vụ bộ đếm thời gian giám sát phần cứng từ một đường dẫn quan trọng đã được xác minh, lành mạnh.

## 14. Khởi động, Cập nhật và Cấu hình

Phần này nêu chi tiết trình tự khởi động, quy trình cập nhật firmware an toàn và quản lý dữ liệu cấu hình không bay hơi.

### 14.1. Chuỗi khởi động

1. Phần cứng sẽ giữ cổng biến tần bị vô hiệu hóa khi bật nguồn.
2. Bộ nạp khởi động phải kiểm tra ảnh ứng dụng đã chọn và khả năng tương thích phần cứng của nó.
3. Ứng dụng phải định cấu hình trạng thái pin và đồng hồ an toàn, sau đó ghi lại lý do đặt lại phần cứng.
4. Bộ xử lý chính và bộ xử lý động cơ trao đổi phiên bản phần mềm và khả năng tính năng.
5. Firmware sẽ xác minh độ lệch dòng điện, trạng thái bộ mã hóa, điện áp bus, nhiệt độ và khóa liên động vật lý.
6. Cơ sở sẽ nhập trạng thái `SafeIdle`; chuyển sang mô-men xoắn hoạt động sẽ yêu cầu điều kiện máy chủ hoặc người dùng rõ ràng.

**Hình 14-1: Trình tự cập nhật firmware **

```nàng tiên cá
trình tựSơ đồ
Công cụ tham gia với tư cách là Công cụ chủ nhà
Ứng dụng người tham gia là ứng dụng cơ sở
người tham gia Khởi động như bộ nạp khởi động cơ sở
người tham gia Động cơ như Motor Bootloader
Công cụ >> Ứng dụng: truy vấn danh tính / phiên bản
App->>App: nhập trạng thái cập nhật bị vô hiệu hóa mô-men xoắn
Công cụ->>App: manifest đã ký
App->>Boot: xác thực thiết lập lại
Công cụ->> Khởi động: giai đoạn và xác minh hình ảnh cơ sở
Boot->>Motor: giai đoạn hình ảnh động cơ tương thích nếu cần thiết
Motor->>Motor: xác minh với PWM bị vô hiệu hóa
Khởi động->>Ứng dụng: đặt lại
Ứng dụng->>Công cụ: tự kiểm tra / di chuyển / kết quả
```

Cập nhật firmware sẽ yêu cầu hình ảnh được xác thực được bảo vệ bởi băm hoặc CRC. Bộ nạp khởi động sẽ thực thi khả năng tương thích phần cứng và gating phiên bản. Phân đoạn flash phải an toàn khi mất điện (ví dụ: A / B ngân hàng kép). Chế độ khôi phục bộ nạp khởi động phải hoàn toàn độc lập với ứng dụng chính. Hệ thống sẽ buộc mô-men xoắn bị vô hiệu hóa trong toàn bộ quá trình cập nhật. Bất kỳ hiệu chuẩn hoặc di chuyển cài đặt sẽ là nguyên tử.

| Dữ liệu | Quy tắc lưu trữ |
|---|---|
Hiệu chuẩn nhà máy Bảo vệ, phiên bản, CRC, xuất xứ
| Trung tâm/phạm vi người dùng | Nguyên tử và có thể đặt lại |
| Hồ sơ | Phiên bản lược đồ, giới hạn, mặc định |
| Giới hạn an toàn | Bất biến/được ủy quyền dịch vụ |
| Hồ sơ lỗi | Vòng tới hạn giới hạn mòn |
| Cập nhật siêu dữ liệu | Ảnh đã chọn, các lần thử, trạng thái phục hồi |

## 15. Kiến trúc An toàn

Phần này củng cố các chiến lược phát hiện và giảm thiểu lỗi. Nó lập bản đồ các mối nguy hiểm điện và phần mềm cụ thể cho các phản ứng an toàn tương ứng của chúng.

**Hình 15-1: Kiến trúc xử lý lỗi và an toàn **

```nàng tiên cá
Sơ đồ TD
Host [Host Command] -> Validate [Parser Validation] -> Arbiter [Software Arbiter]
Arbiter -> Địa phương [Giới hạn cục bộ MCU động cơ] -> PWM -> Cổng
OCP [Bộ so sánh quá dòng] -> Chốt [Chốt lỗi phần cứng]
GF [Lỗi trình điều khiển cổng] -> Chốt
EStop [E-stop] -> Chốt
WDG [Cơ quan giám sát độc lập / Bật thời gian chờ] -> Chốt
Chốt -->|tắt không đồng bộ| Cổng
```

| Nguy hiểm | Phát hiện / kiểm soát | Phản ứng |
|---|---|---|
| Mô-men xoắn bất ngờ | Khởi động ức chế, xác nhận, giới hạn mô-men xoắn / xoay / tươi | Kiểm soát không hoặc ức chế ngay lập tức |
| Sai hướng | Đang chạy thử tính nhất quán | Bật từ chối; lỗi chốt |
| Quá dòng / ngắn | Bộ so sánh, bảo vệ cổng, độ tin cậy ADC | Tắt phần cứng |
| Mất bộ mã hóa | CRC/status/timeout/jump | Ức chế ngay lập tức trừ khi dự phòng được xác nhận |
| Quá nhiệt | Chẩn đoán cảm biến / mô hình | Derate sau đó vô hiệu hóa |
| Regen quá áp | Chiến lược năng lượng và cảm biến xe buýt | Giảm mô-men xoắn / kẹp / ức chế |
| Khóa chính / động cơ | Thời gian chờ IPC, cơ quan giám sát, thời gian chờ cổng | Zero / ức chế / đặt lại |
| Mất USB | SOF / báo cáo độ tươi | Phân rã FFB bị chặn / vô hiệu hóa |
| Phụ kiện ngắn | Đường ray / cổng được bảo vệ | Cô lập phụ kiện; bảo vệ an toàn động cơ |
| Cập nhật tham nhũng | Xác thực / toàn vẹn / phục hồi | Ở lại mô-men xoắn vô hiệu hóa |

Phần sụn phải phân biệt được các loại lỗi: thông tin, hoạt động xuống cấp, mô-men xoắn có thể phục hồi, lỗi chốt và lỗi chủ yếu là phần cứng. Phần mềm không được phép xóa lỗi chủ yếu là phần cứng trong khi tình trạng phần cứng cơ bản vẫn được khẳng định.

# 15.1 Giảm nhiệt trong thực tế

Quá nhiệt được xử lý bằng cách * derating * - giảm nhẹ trần mô-men xoắn khi động cơ và biến tần nóng lên - thay vì cắt đột ngột ở giới hạn. Bởi vì mô-men xoắn cần dòng điện và dòng điện làm cho nhiệt (`T ≈ Kt × Iq`), một cơ sở chạy cứng sẽ ấm lên; derating giữ cho nó có thể sử dụng và an toàn thay vì thất bại đột ngột ở góc giữa.

![Đường cong giảm nhiệt](../assets/thermal_derating_curve.svg)

Dưới nhiệt độ khởi động giảm tốc, trần mô-men xoắn đầy đủ có sẵn. Giữa khởi động giảm tốc và nhiệt độ tắt máy, trần nhà rơi ra; trên tắt máy, mô-men xoắn được loại bỏ. Phục hồi sử dụng trễ: mô-men xoắn chỉ được phục hồi khi nhiệt độ giảm xuống dưới điểm khởi động giảm tốc, do đó hệ thống không dao động vào và ra khỏi giảm tốc ngay tại ngưỡng. Đây là phản ứng "derate sau đó vô hiệu hóa" được liệt kê cho quá nhiệt trong bảng nguy hiểm ở trên.

## 16. Chẩn đoán

Phần này liệt kê các yêu cầu khai thác gỗ và từ xa cần thiết để chẩn đoán các vấn đề trong lĩnh vực mà không ảnh hưởng đến hiệu suất thời gian thực.

**Hình 16-1: Dòng dữ liệu chẩn đoán **

```nàng tiên cá
Lưu đồ LR
Sự kiện [Sự kiện đã nhập] -> Vòng [Vòng RAM bị ràng buộc] -> USB [USB chẩn đoán]
Nhanh [Mẫu kích hoạt] -> Dấu vết [Bộ đệm dấu vết cố định] -> USB
Critical [Critical Snapshot] -> NVM [NVM có giới hạn mặc]
```

Phần sụn sẽ cung cấp lý do đặt lại, số phiên bản, lề WCET, lỗi bộ mã hóa, sự kiện bão hòa ADC, lỗi cổng, tăng điện áp bus và lịch sử nhiệt. Phần sụn sẽ không thực hiện các chức năng ghi nhật ký được định dạng trong ISR động cơ. Từ xa sẽ sử dụng các bản ghi nhị phân cố định, có gắn nhãn thời gian. Dấu vết kiểm soát tốc độ cao sẽ được kích hoạt và giới hạn thời gian. Các bản ghi liên tục không biến động sẽ chỉ lưu trữ các ảnh chụp nhanh lỗi quan trọng để ngăn chặn hao mòn flash. Việc thực hiện các lệnh dịch vụ nguy hiểm sẽ yêu cầu hệ thống ở trạng thái vô hiệu hóa mô-men xoắn. Hệ thống sẽ không bao giờ đăng nhập thông tin bảo mật hoặc ký bí mật.

## 17. Thiết kế tham khảo

Phần này cung cấp một điểm khởi đầu cụ thể, được đề xuất cho thiết kế phần cứng và phần mềm của một cơ sở bánh xe trực tiếp mới.

| Khu vực | Khuyến nghị |
|---|---|
| Main MCU | Cortex-M7/M33-class với USB và bộ nhớ xác định đầy đủ |
| Bộ điều khiển động cơ | MCU / DSP chuyên dụng hoặc động cơ ASIC được xác nhận
| IPC | CRC/sequence/freshness-protected SPI hoặc CAN-FD cộng với bật timeout |
| Động cơ | PMSM / BLDC có kích thước từ mô-men xoắn liên tục / đỉnh và nhiệm vụ nhiệt |
| Bộ mã hóa | Cảm biến CRC/trạng thái tuyệt đối cộng với độ tin cậy |
| Cảm giác hiện tại | Hai / ba shunt hoặc cảm biến nội tuyến dựa trên kiểm soát / chẩn đoán |
Biến tần | Trình điều khiển cổng mạnh mẽ, UVLO, lỗi và ngắt hẹn giờ |
Đầu vào DC Cầu chì / eFuse, đảo ngược, TVS, xâm nhập, điện dung bus, chiến lược regen
| USB | FS cho HID trừ khi băng thông HS là hợp lý |
| Phụ kiện | Bảo vệ cá nhân / công suất giới hạn dòng điện |
| An toàn | Comparator/break, chốt lỗi, watchdog, E-stop/torque-off |
| NVM/debug | Bảo vệ hiệu chuẩn, cấu hình nguyên tử, kiểm soát vòng đời debug |

Đường cơ sở phần mềm nên bao gồm bộ nạp khởi động được bảo vệ, sử dụng RTOS bị hạn chế đối với các tác vụ phi động cơ, bộ lập lịch trình động cơ kim loại trần chuyên dụng và API được phiên bản.

** Bảng 17-1: Giao diện yêu cầu mô-men xoắn (Chính đến động cơ) **

| Yếu tố | Hướng | Kiểu | Mô tả |
|---------|-----------|------|-------------|
| `torque_mNm` | Đầu vào | int32 | Mô-men xoắn yêu cầu tính bằng mili-Newton-mét |
| `sequence` | Đầu vào | uint32 | Số thứ tự tăng đơn điệu |
| `timestamp_us` | Đầu vào | uint32 | Dấu thời gian microsecond của yêu cầu |
| `validity` | Input | uint32 | Magic word hoặc CRC xác nhận yêu cầu |

** Bảng 17-2: Giao diện chụp nhanh động cơ (Động cơ đến chính) **

| Yếu tố | Hướng | Kiểu | Mô tả |
|---------|-----------|------|-------------|
| `angle_urad` | Đầu ra | int32 | Góc trục động cơ tính bằng microradian |
| `speed_urad_s` | Đầu ra | int32 | Tốc độ trục động cơ tính bằng microradian trên giây |
| `phase_current_mA_u` | Đầu ra | int32 | Giai đoạn U hiện tại trong milliamperes |
| `phase_current_mA_v` | Đầu ra | int32 | Giai đoạn V hiện tại tính bằng miliamperes |
| `phase_current_mA_w` | Đầu ra | int32 | Giai đoạn W hiện tại tính bằng miliamperes |
| `timestamp_us` | Đầu ra | uint32 | Dấu thời gian microsecond của ảnh chụp |
| `status` | Output | uint32 | Trường bit chỉ trạng thái lành và trạng thái lỗi |

Các giao diện sẽ sử dụng các đơn vị vật lý rõ ràng, các trường xác nhận và các loại nguyên thủy bị chặn. Việc thực hiện sẽ tránh các chuyển tiếp cho phép ẩn và sẽ ghi rõ quyền sở hữu ISR so với quyền sở hữu bộ đệm tác vụ.

## 18. Xác minh

Phần này phác thảo kim tự tháp thử nghiệm cần thiết để đảm bảo độ tin cậy và an toàn của đế bánh xe.

**Hình 18-1: Đường ống xác minh **

```nàng tiên cá
Sơ đồ TD
Đơn vị [Kiểm tra đơn vị lưu trữ] -> Tĩnh [Phân tích tĩnh / Đánh giá]
Tĩnh -> Target [Target Driver / Kiểm tra thời gian]
Mục tiêu --> HIL [Hardware-in-the-Loop]
HIL -> Thấp [Kiểm tra động cơ năng lượng thấp được bảo vệ]
Thấp -> Đầy đủ [Kiểm tra toàn bộ công suất]
Đầy đủ --> Hệ thống [Trò chơi / Thiết bị ngoại vi / Cập nhật thử nghiệm]
Hệ thống --> Qual [Nhiệt / EMC / Rung / Ngâm]
```

| Diện tích | Kiểm tra tối thiểu |
|---|---|
| USB/FFB | Mô tả, báo cáo sai dạng, vòng đời hiệu ứng, reset/suspend, máy cũ |
| Trọng tài | Hạn chế ưu tiên, xoay, giảm tỷ lệ, bật/lỗi chủng tộc |
| Encoder/current | CRC, timeout, wrap, direction, offset, saturation, ADC timing |
| PWM/cổng | Thời gian chết, trạng thái reset, độ trễ ngắt, lỗi cổng |
| IPC | Mất, hỏng, sao chép, gói, phiên bản không tương thích |
| Power | Brownout, overvoltage, regeneration, inrush, sequencing |
| Thiết bị ngoại vi | Hot-plug, dữ liệu ngắn, cũ, nhận dạng không tương thích |
| Update/NVM | Sai HW/image, gián đoạn, rollback, rách ghi, di chuyển |
| Watchdogs | Trì hoãn công việc/vòng lặp, bế tắc xe buýt, gián đoạn bão

### 18.1. Cổng vào mô-men xoắn đầy đủ

Các tiêu chí sau phải được đáp ứng trước khi cho phép mô-men xoắn động cơ đầy đủ trong quá trình phát triển:

- Rà soát sơ đồ, BOM, thiết kế điện, thiết kế nhiệt.
- Kiểm tra quy mô bộ mã hóa, ký hiệu, trạng thái và logic thời gian chờ.
- Đo tỷ lệ hiện tại, thời gian kích hoạt ADC, độ bù và giới hạn bão hòa.
- PWM và trạng thái trình điều khiển cổng phải được chứng minh an toàn thông qua thiết lập lại, khởi động và cập nhật.
- Bộ so sánh, lỗi cổng và đầu vào E-stop sẽ vô hiệu hóa độc lập PWM trong phần cứng.
- Ký hiệu mô-men xoắn và chuyển đổi dòng điện phải được kiểm tra dưới tải trọng hạn chế, giới hạn dòng điện.
- Mất máy chủ, mất IPC, mất điện, giảm nhiệt, lỗi bộ mã hóa và kiểm tra giám sát phải vượt qua.
- Cơ chế cập nhật phải đảm bảo duy trì được tình trạng mất điện tại các thời điểm chuyển tiếp liên tục.
- Biên độ WCET và jitter phải được chứng minh trong điều kiện giao thông hệ thống tối đa.
- Phê duyệt quy trình bảo vệ cơ khí, quy trình dừng điện tử, quy trình loại trừ người vận hành.

## 19. Lộ trình thực hiện

Phần này cung cấp kế hoạch dự án từng bước để phát triển firmware từ phần cứng ban đầu đến xác nhận cuối cùng.

**Bảng 19-1: Trình tự thực hiện **

| Bước | Hành động | Ghi chú / ràng buộc |
|------|--------|--------------------|
| 1 | Có được mô-men xoắn, tốc độ, quán tính, góc, độ trễ, âm thanh, nhiệt, nền tảng và các yêu cầu an toàn. | Điều kiện tiên quyết cho tất cả các nhiệm vụ |
| 2 | Có được sơ đồ, BOM, và kết nối; ánh xạ điện, đồng hồ, thiết lập lại, gián đoạn, DMA. | Cần thiết cho thiết lập BSP |
| 3 | Hoàn thành phân tích nguy hiểm và phản ứng lỗi trước khi bật động cơ. | Cổng an toàn bắt buộc |
| 4 | Chọn phân vùng bộ xử lý và phiên bản IPC. | Xác định kiến trúc phần mềm |
5 Đưa ra đường ray, thiết lập lại, giám sát, ức chế và chẩn đoán với biến tần bị vô hiệu hóa. Xác nhận phần cứng cơ sở
| 6 | Đưa lên bộ mã hóa và cảm biến dòng điện; xác minh thời gian bằng điện. | Xác minh đồng bộ ADC / Timer |
| 7 | Đưa PWM và trình điều khiển cổng vào giả an toàn hoặc tải điện áp thấp. | Xác minh điều khiển biến tần |
| 8 | Thực hiện các trạng thái động cơ, giới hạn cục bộ và thực hiện các thử nghiệm năng lượng thấp. | Điều chỉnh FOC ban đầu |
| 9 | Triển khai USB, PID, FFB và trọng tài chống lại điểm cuối mô phỏng động cơ. | Tích hợp phần mềm máy chủ |
| 10 | Tích hợp miền động cơ và xác minh các đường dẫn cũ và lỗi trước. | Xác thực kiến trúc an toàn |
| 11 | Thêm vành và phụ kiện phía sau bộ điều hợp bị cô lập. | Tích hợp ngoại vi |
| 12 | Thêm hồ sơ, hiệu chuẩn, chẩn đoán và cơ chế cập nhật / phục hồi. | Tính năng hệ thống |
| 13 | Thực hiện kiểm tra HIL, hệ thống, nhiệt, EMC, độ rung, chu kỳ năng lượng và ngâm. | Xác minh chính thức |
| 14 | Xuất bản tài liệu tương thích, hiệu chuẩn, cập nhật, lỗi và dịch vụ. | Phát hành điều kiện tiên quyết |

## 20. Tài liệu tham khảo

Phần này chứa các liên kết đến tài liệu bên ngoài và các tệp nghiên cứu nội bộ liên quan được sử dụng để biên dịch kiến trúc này.

### 20.1. Nghiên cứu hiện tại

- [`sim_racing_research.md`] (./sim_racing_research.md) - hệ sinh thái, cơ sở / động cơ, truyền thông, thời gian và an toàn.
- [`wheel_rim.md`](./wheel_rim.md) - kiến trúc vành và ranh giới QR.
- [`pedals.md`] (./pedals.md) - cảm biến bàn đạp và ngữ cảnh proxy cổng cơ sở.
- [`tools.md`] (./tools.md) - công cụ xác nhận và điểm nhập tiêu chuẩn.
- [`repos.md`](./repos.md) — danh sách phát hiện kho công cộng.

### 20.2. Nguồn công cộng hợp nhất

- [Thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid)
- [USB-IF PID Lớp 1.0] (https://www.usb.org/sites/default/files/documents/pid1_01.pdf)
- [OpenFFBoard wiki] (https://github.com/Ultrawipf/OpenFFBoard/wiki/)
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf)
- [Câu hỏi thường gặp về cơ sở bánh xe Fanatec] (https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ)
- [Tương thích nền tảng Fanatec] (https://www.fanatec.com/us-en/platforms)
- [Đăng ký nguồn hệ sinh thái Fanatec] (./references.md)
- [Hướng dẫn cập nhật Fanatec] (https://www.fanatec.com/eu/en/explorer/products/racing-wheels-wheel-bases/update-fanatec-firmware-and-drivers/)
- [Hướng dẫn sử dụng Simucube 2] (https://simucube.com/app/uploads/2022/11/Simucube_2_User_Guide.pdf)
- [Tham khảo Infineon PMSM FOC] (https://documentation.infineon.com/aurixtc3xx/docs/kbv1711616051757)
- [TI TIDA-01599] (https://www.ti.com/tool/TIDA-01599)
- [gotzl/hid-fanatecff] (https://github.com/gotzl/hid-fanatecff)
- [gotzl/hid-fanatecff-tools] (https://github.com/gotzl/hid-fanatecff-tools)
- [Fanatec-Pinout] (https://github.com/FendtXerion3800/Fanatec-Pinout) - cộng đồng, không chính thức.

### Trình giả lập phần cứng mã nguồn mở 20.3

- [lshachar/Arduino_Fanatec_Wheel] (https://github.com/lshachar/Arduino_Fanatec_Wheel) - trình giả lập SPI vô lăng tùy chỉnh.
- [StuyoP/Fanatec-Wheel-Barebone-Emulator] (https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator) - trình giả lập chiều dài cơ sở xương sống.
- [Alexbox364/F_Interface_AL] (https://github.com/Alexbox364/F_Interface_AL) - Tay lái tùy chỉnh DIY thông qua SPI.
- [jssting/ArduinoTec-Pedals] (https://github.com/jssting/ArduinoTec-Pedals) - Bộ điều khiển thay thế bàn đạp Fanatec.
- [GeekyDeaks/fanatec-pedal-emulator] (https://github.com/GeekyDeaks/fanatec-pedal-emulator) - bàn đạp USB của bên thứ ba proxy qua RJ12.
- [StuyoP/Universal-Shifter-Interface-for-Fanatec] (https://github.com/StuyoP/Universal-Shifter-Interface-for-Fanatec) - proxy chuyển đổi dựa trên chuyển đổi.
- [vnmsimulation/VNM_MOTION_CONTROLLER](https://github.com/vnmsimulation/VNM_MOTION_CONTROLLER) - Bộ điều khiển phần cứng dựa trên DIY STM32.

# 21. Câu hỏi chưa được giải quyết

Phần này liệt kê các quyết định thiết kế mở và thông tin còn thiếu phải được giải quyết để hoàn thành kiến trúc hệ thống.

- Mô-men xoắn cực đại / liên tục, tốc độ, quán tính, phạm vi và chu kỳ làm việc?
- Dữ liệu điện / nhiệt động cơ và phong bì năng lượng tái tạo?
- Phân vùng bộ xử lý được chọn, bộ mã hóa, cấu trúc liên kết hiện tại, trình điều khiển cổng và MOSFET?
- Yêu cầu nền tảng PC / console và kiến trúc xác thực được cấp phép?
- Mô tả USB, nhịp, khả năng hiệu ứng và giao diện nhà cung cấp?
- Giao thức QR / rim cho mỗi chính sách tương thích và thế hệ được hỗ trợ?
- Định nghĩa kết nối Accessory/E-stop/torque-key/CAN?
- Mục tiêu an toàn/điều tiết và đánh giá độc lập?
- Ký, rollback, cung cấp, chống hạ cấp, và chính sách gỡ lỗi?
- Quyền sở hữu hiệu chuẩn / di chuyển trên cơ sở, động cơ, vành và thiết bị ngoại vi?
- Ngân sách chấp nhận trễ / jitter và thiết bị?
- Yêu cầu về môi trường, EMC, độ rung, âm thanh và tuổi thọ?
