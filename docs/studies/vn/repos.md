# Fanatec & Sim Racing Kho mã nguồn mở (Open-Source Repositories)

Tài liệu này là một bản đồ khám phá được chọn lọc về các dự án công cộng có liên quan đến nghiên cứu phần cứng tương thích Fanatec và sim racing. Các repository này rất hữu ích cho việc tìm hiểu kiến trúc và đưa ra giả thuyết về khả năng tương tác. Đây không phải là các thông số kỹ thuật chính thức của Fanatec trừ khi repository thuộc sở hữu của nhà sản xuất.

## Cách Sử dụng Các Kho chứa (Repositories) này

| Loại Bằng chứng | Có thể Hỗ trợ gì | Lưu ý Cần thiết |
|---|---|---|
| README/tài liệu dự án | Mục tiêu dự án, phần cứng hỗ trợ, phương pháp build | Có thể đã cũ hoặc không đầy đủ |
| Mã nguồn (Source code) | Định dạng frame, timing, descriptors, và giả định phần cứng được quan sát | Có thể chỉ nhắm tới một thế hệ thiết bị |
| Sơ đồ (Schematics)/pinout | Giả thuyết điện từ cộng đồng | Cần được xác minh dựa trên tài liệu được phê duyệt hoặc đo đạc an toàn |
| Issues/discussions | Các báo cáo lỗi và độ tương thích | Chỉ xem là thông tin tham khảo cho đến khi tái hiện được lỗi |
| Releases/binaries | Đường dẫn build/deploy có thể tái hiện | Không phân phối lại firmware độc quyền hoặc binary không rõ nguồn gốc |

## Trình tự Đọc cho Developer

1. Đọc [sim_racing_research.md](./sim_racing_research.md) để hiểu ranh giới của các hệ thống con.
2. Đọc [wheel_rim.md](./wheel_rim.md) trước khi tìm hiểu các dự án wheel-emulator.
3. Đọc [pedals.md](./pedals.md) trước khi tìm hiểu các dự án pedal-controller hoặc pedal-proxy.
4. Đọc [add_ons.md](./add_ons.md) trước khi tìm hiểu các dự án shifter/handbrake.
5. Sử dụng [tools.md](./tools.md) để xác thực các giả định về USB, HID, timing và mạch điện.

## Trình giả lập Vô lăng (Wheel Emulators) & Vô lăng DIY

* **[lshachar/Arduino_Fanatec_Wheel](https://github.com/lshachar/Arduino_Fanatec_Wheel)**
  Một dự án do-it-yourself cho phép bạn tạo một steering wheel tùy chỉnh, giao tiếp với các Fanatec wheel base qua SPI. Nó hỗ trợ buttons, D-pad, và alphanumeric display, đánh lừa wheel base để kích hoạt Force Feedback.

* **[StuyoP/Fanatec-Wheel-Barebone-Emulator](https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator)**
  Một barebone emulator cho Fanatec wheel base sử dụng chip ATmega328p chạy native ở mức 3.3V. Nó cho phép bạn tạo DIY steering wheel hỗ trợ đầy đủ button, display, và LED mà không cần bộ chuyển đổi mức logic (logic level shifters).

* **[Alexbox364/F_Interface_AL](https://github.com/Alexbox364/F_Interface_AL)**
  Một nền tảng phần cứng và phần mềm đa năng để build vô lăng DIY tùy chỉnh. Nó giao tiếp trực tiếp với Fanatec wheel base qua giao thức SPI và có thể hỗ trợ lên đến 24 push button, rotary encoder, màn hình TM1637 và màn hình OLED.

## Trình giả lập Pedal & Thay thế Bộ điều khiển (Controller Replacements)

* **[jssting/ArduinoTec-Pedals](https://github.com/jssting/ArduinoTec-Pedals)**
  Một dự án Arduino Leonardo/Pro Micro dùng để thay thế board điều khiển OEM của Fanatec ClubSport Pedals (CSP) V1/V2. Hoàn hảo để sửa chữa các bộ pedal cũ khi PCB chính bị hỏng, cho phép bạn giao tiếp trực tiếp với các load cell và Hall effect sensor nguyên bản qua chuẩn USB.

* **[GeekyDeaks/fanatec-pedal-emulator](https://github.com/GeekyDeaks/fanatec-pedal-emulator)**
  Một công cụ cho phép bạn dùng pedal USB của bên thứ ba (như Heusinkveld) và chuyển tiếp (proxy) chúng qua Fanatec wheel base bằng cổng RJ12, giúp chúng có thể dùng được trên các console như PS4/PS5 hoặc Xbox.

* **[adamcrawley/fanatec-pedal-emulator-vrs](https://github.com/adamcrawley/fanatec-pedal-emulator-vrs)**
  Một phiên bản được sửa đổi từ emulator phía trên, thiết kế riêng để làm cho VRS DirectForce Pro Pedals tương thích với một Fanatec wheel base.

## Shifters, Motion & Force Feedback

* **[StuyoP/Universal-Shifter-Interface-for-Fanatec](https://github.com/StuyoP/Universal-Shifter-Interface-for-Fanatec)**
  Một giao diện cho phép bạn kết nối bất kỳ H-pattern shifter dùng công tắc hoặc sequential shifter nào trực tiếp với một Fanatec wheel base thông qua giao thức RJ12 bên trong nó.

* **[Ultrawipf/OpenFFBoard](https://github.com/Ultrawipf/OpenFFBoard)**
  Một nền tảng giao diện force feedback mã nguồn mở phổ quát mang tính bao quát cho các thiết bị mô phỏng DIY có độ tương thích cao, đặc biệt là direct-drive steering wheel. Nó hỗ trợ các trình điều khiển motor khác nhau (như TMC4671, ODrive, VESC) và encoders.

* **[vnmsimulation/VNM_MOTION_CONTROLLER](https://github.com/vnmsimulation/VNM_MOTION_CONTROLLER)**
  Firmware và ứng dụng cấu hình dựa trên STM32F401RCT từ VNM Simulation để build phần cứng DIY bao gồm wheel base, pedal và motion rig.

## Tài liệu Tham khảo Phần cứng & Sơ đồ chân (Pinouts)

* **[FendtXerion3800/Fanatec-Pinout](https://github.com/FendtXerion3800/Fanatec-Pinout)**
  Một repository tham khảo phần cứng hữu ích ghi chép pinout cho thiết bị Fanatec (chẳng hạn như cổng RJ12 cho pedal, shifter, và wheel quick release). Đây là điều cực kỳ quan trọng đối với bất kỳ ai muốn chế tạo cáp chuyển đổi DIY hoặc custom adapter.

## Trình điều khiển (Drivers) & Giao diện Phần mềm

* **[gotzl/hid-fanatecff](https://github.com/gotzl/hid-fanatecff)**
  Một Linux kernel driver module được đánh giá cao cung cấp hỗ trợ đầy đủ force feedback (FFB) cho Fanatec wheel base. Cần thiết cho bất cứ ai muốn chơi sim racing trên nền tảng Linux hoặc SteamOS.

## Tìm kiếm Repository Chung

Nếu bạn muốn tiếp tục khám phá những sáng tạo mới nhất của cộng đồng trên GitHub và GitLab, dưới đây là một số truy vấn tìm kiếm trực tiếp:
* [GitHub Search: "fanatec"](https://github.com/search?q=%20fanatec&type=repositories)
* [GitHub Search: "racing wheel base"](https://github.com/search?q=racing+wheel+base+&type=repositories)
* [GitLab Search: "fanatec"](https://gitlab.com/search?search=fanatec)
* [GitLab Search: "sim racing"](https://gitlab.com/search?search=sim+racing)

## Tài liệu Neo Tham chiếu (Reference Anchors)

- [USB-IF HID specifications and tools](https://www.usb.org/hid) — baseline cho USB HID reports và usages.
- [USB-IF PID Class 1.0](https://www.usb.org/sites/default/files/documents/pid1_01.pdf) — baseline cho mô hình HID force-feedback/PID.
- [Fanatec Podium DD1 manual](https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) — hành vi nhà sản xuất công khai cho một dòng base cao cấp.
- [SimHub wiki](https://github.com/SHWotever/SimHub/wiki) — hệ sinh thái công khai cho telemetry/dashboard/button-box.
- [OpenFFBoard wiki](https://github.com/Ultrawipf/OpenFFBoard/wiki/) — hệ sinh thái công khai cho wheel base FFB dạng module.

## Các Câu hỏi Mở để Developer Tự Tìm hiểu

Reviewed 2026-07-05. Câu hỏi định hướng quá trình — câu trả lời được tạo ra dựa trên nỗ lực kiểm tra của người đọc, chứ không phải do tra cứu.

- **Những repositories nào nên được thăng cấp vào một ma trận tính tương thích chính thức sau khi xác minh trên bench (bench verification)?**
  *Cách điều tra:* xem mọi repository ở đây chỉ như **thông tin để tham khảo / bằng chứng cộng đồng (discovery input / community evidence)**, tuyệt đối không xem là spec chính thức. Chỉ đưa một dự án vào file [`compatibility-matrix.md`](./compatibility-matrix.md) sau khi (a) xác nhận rõ nó được validate chạy trên phần cứng/firmware cụ thể nào, (b) tái hiện được đúng hành vi của nó trên test bench, và (c) ghi nhận kết quả kèm ngày tháng và phiên bản. Ưu tiên các dự án có chia sẻ thông tin cụ thể, có thể kiểm tra được — ví dụ `gotzl/hid-fanatecff` (USB IDs của thiết bị, các extended control) và `GeekyDeaks/fanatec-pedal-emulator` (RJ12/UART pinout) — do chúng dễ xác minh nhất. Cần re-verify mỗi khi firmware hoặc sản phẩm thay đổi.
