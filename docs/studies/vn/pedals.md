# Sim Racing Bàn đạp Kiến trúc

> Ngày nghiên cứu: 2026-07-02
> Mô hình bằng chứng: tiêu chuẩn công cộng, hướng dẫn sử dụng / hỗ trợ của nhà sản xuất và các dự án cộng đồng. Các dự án cộng đồng là bằng chứng thực hiện, không phải thông số kỹ thuật của nhà cung cấp chính thức.
> Tài liệu liên quan: [sim_racing_research.md](./sim_racing_research.md), [wheel_base.md](./wheel_base.md), [add_ons.md](./add_ons.md), [repos.md](./repos.md).

## 1. Giới thiệu và Phạm vi

Tài liệu này cung cấp một cái nhìn tổng quan kỹ thuật về kiến trúc phần cứng và phần mềm của bàn đạp đua xe sim hiện đại. Nó được thiết kế để giới thiệu các khái niệm cơ bản về thiết bị ngoại vi đua xe sim cho các kỹ sư trong khi chi tiết các hệ thống nhúng điều khiển chúng. Người đọc nên có một sự hiểu biết cơ bản về vi điều khiển và xử lý tín hiệu tương tự trước khi đọc đặc điểm kỹ thuật này.

Tài liệu này mô tả các công nghệ cảm biến vật lý, chuỗi tín hiệu analog-to-digital, các giao thức truyền thông được sử dụng để giao tiếp với hệ thống máy chủ và lớp phần mềm chịu trách nhiệm hiệu chuẩn và đo từ xa.

---

## 2. Công nghệ cảm biến

Phần này nêu chi tiết các cảm biến vật lý được sử dụng để chuyển động bàn đạp cơ học thành tín hiệu điện. Hiểu các cảm biến này là rất quan trọng để đánh giá độ bền, độ chính xác và cảm giác thực tế của bộ bàn đạp.

### 2.1 Potentiometers

Potentiometers là điện trở quay dựa trên tiếp xúc đo khoảng cách di chuyển (vị trí) của bàn đạp. Khi bàn đạp được nhấn, một cần gạt nước cơ học di chuyển trên một rãnh điện trở, tạo ra đầu ra điện áp thay đổi.

![Potentiometer vs. bộ mã hóa quay](../assets/potentiometer_and_encoder.svg)

Đọc như một bộ chia điện áp, đầu ra chỉ đơn giản là 'V_out = V + × (vị trí gạt nước / rãnh đầy đủ)'. Điều này rẻ và tầm thường đối với ADC để đọc, nhưng tiếp điểm trượt cũng là điểm yếu: nó mòn và các rãnh mòn phát triển tiếng ồn và các điểm chết. (Bảng điều khiển bên phải hiển thị bộ mã hóa quay để tương phản - một giải pháp thay thế không tiếp xúc được sử dụng để cảm biến góc lái và rôto thay vì di chuyển bằng bàn đạp.)

* Hệ thống ** sẽ ** cung cấp điện áp tham chiếu ổn định cho chiết áp.
* Potentiometers dễ bị hao mòn cơ học theo thời gian, trong đó giới thiệu tiếng ồn hoặc "điểm chết" vào tín hiệu.

### 2.2 Cảm biến hiệu ứng Hall

Cảm biến Hall Effect là thiết bị không tiếp xúc đo sự thay đổi trong từ trường để xác định vị trí bàn đạp. Một nam châm gắn vào cánh tay bàn đạp di chuyển tương đối so với cảm biến, làm thay đổi từ thông.

![Hoạt động cảm biến Hall-effect](../assets/hall_effect_sensor.svg)

Bởi vì không có tiếp xúc vật lý giữa nam châm và cảm biến, không có gì để hao mòn. Sự đánh đổi là độ nhạy vị trí: kích thước nam châm, khoảng cách không khí và căn chỉnh phải đúng và hiệu chuẩn phải giữ cho bàn đạp di chuyển bên trong phạm vi tuyến tính của cảm biến (đầu ra bão hòa bên ngoài nó, như đường cong ở trên cho thấy).

* Triển khai Hiệu ứng Hall ** sẽ ** sử dụng tham chiếu từ cố định để đảm bảo theo dõi vị trí lặp lại.
* Bởi vì chúng thiếu tiếp xúc vật lý, cảm biến Hall Effect có độ bền cao và miễn dịch với sự mài mòn ma sát liên quan đến chiết áp.

### 2.3 Tải ô

Các tế bào tải đo lực vật lý (áp suất) được áp dụng cho bàn đạp thay vì vị trí của nó. Họ sử dụng đồng hồ đo biến dạng dưới áp suất, thay đổi điện trở của chúng và tạo ra tín hiệu chênh lệch mức microvolt. Điều này bắt chước các hệ thống phanh thủy lực trong thế giới thực, nơi áp suất bàn đạp ra lệnh cho lực phanh.

! [Tải đồng hồ đo biến dạng tế bào trong cầu Wheatstone] (./load_cell_wheatstone_bridge.svg)

Lý do tín hiệu thô quá nhỏ - và tại sao bộ khuếch đại không thể thương lượng - có thể nhìn thấy trong cây cầu ở trên. Bốn đồng hồ đo căng thẳng được nối vào cầu Wheatstone, hai trong căng thẳng và hai trong nén, do đó, áp dụng lực không cân bằng cầu và tạo ra điện áp chênh lệch. Sự sắp xếp này tăng gấp đôi độ nhạy và hủy bỏ sự trôi dạt nhiệt độ, nhưng đầu ra vẫn chỉ là microvolts đến millivolts, vì vậy một bộ khuếch đại thiết bị nằm giữa cầu và ADC.

* Bàn đạp phanh dựa trên tế bào tải ** sẽ ** được ghép nối với môi trường kháng cơ học thích hợp (ví dụ: chất đàn hồi hoặc lò xo) để cung cấp phản hồi vật lý.
* Tín hiệu thô từ một ô tải ** sẽ ** được khuếch đại bằng bộ khuếch đại thiết bị trước khi chuyển đổi tương tự sang kỹ thuật số.

**Bảng 2-1: So sánh công nghệ cảm biến **

| Loại cảm biến | Đo lường | Loại tiếp xúc | Độ bền | Ứng dụng tiêu biểu |
|-------------|-------------|--------------|------------|---------------------|
| Potentiometer | Vị trí | Liên hệ | Thấp | Bàn đạp đầu vào |
Hiệu ứng Hall | Vị trí | Không tiếp xúc | Cao | Throttle, Clutch |
| Load Cell | Force | Không tiếp xúc | Cao | Phanh |

---

## 3. Kiến trúc phần cứng và chuỗi tín hiệu

Phần này bao gồm việc định tuyến và xử lý tín hiệu điện tử cần thiết để số hóa các đầu vào cảm biến analog.

Chuỗi tín hiệu phần cứng chuyển đổi đầu vào vật lý thành giá trị kỹ thuật số có thể được xử lý bởi vi điều khiển. Bàn đạp cao cấp yêu cầu Bộ chuyển đổi Analog-to-Digital (ADC) chính xác để đảm bảo độ ồn tối thiểu và độ nhạy cao.

**Hình 3-1: Chuỗi tín hiệu phần cứng bàn đạp **

```nàng tiên cá
đồ thị LR
A [Đầu vào cơ khí] -> B [Cảm biến]
B -> C [Tín hiệu tương tự]
C -> D [ADC / Amplifier]
D -> E [Microcontroller]
E -> F [Đầu ra kỹ thuật số]
```

### 3.1 Chuyển đổi Analog-to-Digital

Các tín hiệu analog thô từ các cảm biến phải được số hóa. Độ phân giải của ADC ảnh hưởng trực tiếp đến độ chính xác của đầu vào bàn đạp.

![Độ phân giải ADC](../assets/adc_sampling_resolution.svg)

Một ADC ghi lại tín hiệu tương tự liên tục dưới dạng một loạt các bước rời rạc. Nhiều bit hơn có nghĩa là nhiều bước hơn, vì vậy cầu thang được ghi lại ôm tín hiệu thực sự chặt chẽ hơn và bàn đạp di chuyển trơn tru hơn là trong các bước nhảy có thể nhìn thấy. Đây là lý do tại sao một tế bào tải - có tín hiệu có thể sử dụng là nhỏ ngay cả sau khi khuếch đại - được hưởng lợi từ chuyển đổi 16-bit hoặc cao hơn, trong khi cảm biến vị trí là đủ ở 12-bit. Cảnh báo quan trọng là các bit bổ sung chỉ giúp ích nếu tín hiệu tương tự sạch; không có tham chiếu ổn định và lọc thích hợp, các bit bổ sung chỉ đơn giản là số hóa nhiễu.

* Hệ thống ** sẽ ** số hóa tín hiệu tế bào tải khuếch đại bằng cách sử dụng ADC có độ phân giải cao (ví dụ: HX711 hoặc ADS1115).
* Độ phân giải ADC ** nên ** ít nhất 12-bit cho chiết áp và cảm biến Hiệu ứng Hall, và 16-bit hoặc cao hơn cho các tế bào tải.
* Phần cứng ** sẽ ** thực hiện lọc thông thấp để giảm thiểu nhiễu điện tần số cao trước pha lấy mẫu ADC.

---

## 4. Giao diện truyền thông

Phần này giải thích hai phương pháp chính để kết nối bàn đạp đua sim với hệ thống máy chủ: kết nối USB trực tiếp và ủy nhiệm RJ12 thông qua chân đế.

Bàn đạp phải truyền trạng thái kỹ thuật số của họ đến phần mềm mô phỏng. Sự lựa chọn giao diện ảnh hưởng đến sự tiện lợi, khả năng tương thích hệ sinh thái và đôi khi là tỷ lệ bỏ phiếu.

**Hình 4-1: Cấu trúc liên kết truyền thông **

```nàng tiên cá
đồ thị TD
P[Pedals] -->|USB| PC[Host PC]
P -->|RJ12| W[Chân đế]
W -> |USB| PC
W -->|Proprietary| C[Bảng điều khiển]
```

### 4.1 USB (Trực tiếp đến máy chủ)

Kết nối bàn đạp trực tiếp với PC chủ thông qua USB cho phép bàn đạp hoạt động như một Thiết bị giao diện con người độc lập (HID).

* Tỷ lệ bỏ phiếu / báo cáo, đường dẫn cập nhật firmware và các điều khiển hiệu chuẩn có sẵn là sản phẩm cụ thể.
* USB trực tiếp là đường dẫn kết nối PC. Phụ kiện Fanatec phải kết nối thông qua đế bánh xe Fanatec để sử dụng bảng điều khiển.
* Bàn đạp CSL cơ bản không bao gồm USB độc lập. Đường dẫn USB được hỗ trợ yêu cầu Bộ tải di động CSL Pedals hoặc Bộ chuyển đổi USB ClubSport.
* Bàn đạp USB của bên thứ ba có thể hoạt động độc lập trên PC nhưng không thể kết nối trực tiếp với cổng bàn đạp cơ sở bánh xe Fanatec.

### 4.2 Ủy nhiệm RJ12 (thông qua Chân đế)

Kết nối bàn đạp với chân đế thông qua cáp RJ12 cho phép chân đế hoạt động như một proxy, dịch tín hiệu bàn đạp thành giao thức truyền thông của riêng nó.

* Các cơ sở tổng hợp các trục bàn đạp vào báo cáo máy chủ của nó và cung cấp đường dẫn giao diện điều khiển cần thiết.
* Bàn đạp tải được hỗ trợ có thể hiển thị điều chỉnh Lực phanh (BRF) thông qua Menu Điều chỉnh cơ sở.
* Thực hiện theo hướng dẫn đạp chính xác để lựa chọn USB / RJ12. Không giả định kết nối đồng thời được phép trên tất cả các mô hình.

**Bảng 4-1: So sánh giao diện**

| Tính năng | USB | RJ12 (Wheelbase Proxy) |
|---------|-----|------------------------|
| Độ phân giải/tỷ lệ báo cáo | Sản phẩm cụ thể | Sản phẩm/cơ sở cụ thể |
| Nền tảng | Đường dẫn PC được hỗ trợ | PC và các bảng điều khiển được hỗ trợ thông qua một cơ sở tương thích |
| Điều chỉnh | Sản phẩm phần mềm/Ứng dụng | Cơ sở/Ứng dụng hành vi phụ thuộc vào mô hình bàn đạp |
| Cập nhật firmware | Sản phẩm cụ thể | Thực hiện theo hướng dẫn hiện tại / App workflow |

---

## 5. Kiến trúc phần mềm

Phần này mô tả logic phần mềm chịu trách nhiệm xử lý các giá trị kỹ thuật số thô và ánh xạ chúng đến các đầu vào trò chơi được chuẩn hóa.

Phần sụn nhúng và phần mềm máy tính chủ hoạt động song song để hiệu chỉnh bàn đạp, áp dụng các đường cong tùy chỉnh và cung cấp phản hồi xúc giác.

# 5.1 Hiệu chuẩn và lọc

* Firmware bàn đạp ** sẽ ** cho phép người dùng xác định các giới hạn tối thiểu (deadzone) và tối đa (bão hòa) hợp lý cho mỗi trục.
* Phần mềm ** nên ** thực hiện lọc động (ví dụ: bộ lọc trung bình động hoặc bộ lọc Kalman) để làm mịn jitter tế bào tải mà không giới thiệu độ trễ quá mức.
* Các thông số hiệu chuẩn ** sẽ ** được lưu trữ trong bộ nhớ không bay hơi trên bộ điều khiển bàn đạp để đảm bảo sự bền bỉ trong các chu kỳ năng lượng.

### 5.2 Lập bản đồ và từ xa HID

* Bộ điều khiển ** sẽ ** báo cáo các giá trị bàn đạp được hiệu chỉnh theo trục cần điều khiển HID tiêu chuẩn.
* Nếu được trang bị bộ truyền động haptic (ví dụ: động cơ rung), phần mềm ** có thể ** đọc từ xa trò chơi (như kích hoạt ABS hoặc trượt bánh xe) để kích hoạt phản hồi vật lý thông qua mặt bàn đạp.

---

## 6. Phân tích kho

Phần này khám phá cách các dự án nguồn mở cộng đồng tiếp cận mô phỏng bàn đạp và chuyển đổi giao diện.

# 6.1 `jssting/ArduinoTec-Pedals`

| Khía cạnh | Tìm kiếm |
|---|---|
| Goal | Thay bộ điều khiển bàn đạp CSP V1 bằng USB MCU độc lập |
Leonardo, Pro Micro, hoặc Teensy sử dụng ArduinoJoystickLibrary
| Cảm biến | Cảm biến Hall tuyến tính hiện có; tế bào tải thông qua bộ khuếch đại hiện có hoặc bên ngoài |
| Đầu ra | trục cần điều khiển USB; hiệu chuẩn máy chủ |
| Đầu ra thêm | Động cơ rung bàn đạp qua bóng bán dẫn / PWM |
Bàn đạp sẽ hoạt động như một nút cảm biến / USB riêng biệt; AFE tế bào tải, hành vi Hall tương tự và ổ đĩa truyền động yêu cầu quyền sở hữu riêng biệt.

### 6.2 `GeekyDeaks/fanatec-pedal-emulator`

| Khía cạnh | Tìm kiếm |
|---|---|
Proxy bàn đạp USB của bên thứ ba vào chiều dài cơ sở Fanatec thông qua RJ12
Raspberry Pi Pico (RP2040) hoạt động như máy chủ USB và trình giả lập tương tự RJ12
| Động lực | Cho phép sử dụng bàn đạp không phải Fanatec trên bàn điều khiển, bỏ qua bảo mật bàn điều khiển |
| Bài học sản phẩm | Cổng bàn đạp chân đế là một giao diện analog/digital (không phải USB), có thể được giả mạo bởi DACs/PWM bên ngoài để proxy các thiết bị khác. |

## 7. Câu hỏi chưa được giải quyết

* Ngân sách độ trễ chấp nhận được (tính bằng mili giây) từ khởi động bàn đạp vật lý đến truyền gói USB HID cho thể thao điện tử cạnh tranh là gì?
* Phần sụn có nên hỗ trợ các đường cong gamma tùy chỉnh, phi tuyến tính cho các ứng dụng bướm ga trực tiếp trên vi điều khiển không hoặc điều này có nên được ủy quyền cho phần mềm PC chủ không?
* Có các giao thức chiều dài cơ sở độc quyền trên RJ12 yêu cầu cấp phép hoặc kỹ thuật đảo ngược để tương thích chéo không?

## 8. Tài liệu tham khảo

# 8.1 Nguồn chính thức và tiêu chuẩn

- [Công cụ và thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid) - Mô tả HID và mô hình báo cáo cho bàn đạp USB độc lập.
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) - bối cảnh cập nhật / hiệu chuẩn phía cơ sở công cộng và các ràng buộc tích hợp phụ kiện.
- [Fanatec Wheel Bases FAQ] (https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) - tập hợp bảng điều khiển, thiết bị ngoại vi độc lập PC và ranh giới bàn đạp của bên thứ ba.
- [Fanatec CSL Pedals hướng dẫn USB trực tiếp] (https://help.fanatec.com/hc/en-us/articles/30312127196945-How-can-I-connect-CSL-Pedals-directly-to-a-PC-via-USB) - Load Cell Kit và ClubSport USB Adapter yêu cầu.

# # 8.2 Nguồn thực hiện cộng đồng

- [jssting/ArduinoTec-Pedals] (https://github.com/jssting/ArduinoTec-Pedals) - Bộ điều khiển thay thế Arduino Leonardo / Pro Micro / Teensy cho bàn đạp Fanatec ClubSport cũ hơn.
- [GeekyDeaks/fanatec-pedal-emulator] (https://github.com/GeekyDeaks/fanatec-pedal-emulator) - Bàn đạp USB ủy quyền cho chiều dài cơ sở Fanatec thông qua mô phỏng cổng bàn đạp.
- [adamcrawley/fanatec-pedal-emulator-vrs] (https://github.com/adamcrawley/fanatec-pedal-emulator-vrs) - Ngã ba tập trung vào VRS của mẫu giả lập bàn đạp.
- [FendtXerion3800/Fanatec-Pinout] (https://github.com/FendtXerion3800/Fanatec-Pinout) - quan sát kết nối bàn đạp cộng đồng /shifter/handbrake; xác minh trước khi sử dụng phần cứng.
- [Fanatec đăng ký nguồn hệ sinh thái] (./references.md) - niềm tin nguồn và ghi chú tiền tệ.

### 8.3 Thiết kế theo dõi

- Xây dựng ma trận tương thích bàn đạp ngăn cách hành vi USB HID trực tiếp với hành vi proxy cổng chân đế.
- Nắm bắt các giới hạn điện đã biết từ hướng dẫn sử dụng được phê duyệt hoặc đo băng ghế dự bị trước khi kết nối phần cứng của bên thứ ba.
