# Sim Racing Công cụ phát triển và tài liệu tham khảo

Tài liệu này liệt kê các công cụ thực tế và các nguồn tham khảo cho các nhà phát triển nghiên cứu hoặc tạo mẫu các thiết bị ngoại vi đua xe mô phỏng. Đây là điểm khởi đầu, không phải là danh sách phê duyệt sản phẩm.

## 1. Tiêu chuẩn và tham chiếu giao diện

| Cần | Tham khảo | Sử dụng |
|---|---|---|
| Thiết bị đầu vào USB | [Công cụ và thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid) | Mô tả HID, cách sử dụng, mô hình báo cáo, công cụ tác giả mô tả. |
| Thiết bị phản hồi lực [USB-IF PID Class 1.0] (https://www.usb.org/sites/default/files/documents/pid1_01.pdf) | Báo cáo thiết bị giao diện vật lý cho bánh xe phản hồi lực và thiết bị haptic. |
| Linux input và FF API | [Linux force-feedback documentation](https://www.kernel.org/doc/html/latest/input/ff.html) | Host-side effect upload/playback concepts. |
| Truy cập HIDRAW | [Linux HIDRAW documentation](https://docs.kernel.org/hid/hidraw.html) | Direct HID descriptor/report access for tools and compatibility layers. |

## 2. Hệ sinh thái và tài liệu tham khảo tương thích

| Cần | Tham khảo | Sử dụng |
|---|---|---|
| Nguồn tin cậy và ngày tháng | [Sổ đăng ký nguồn hệ sinh thái Fanatec](./references.md) | Kiểm tra xem yêu cầu bồi thường là chính thức, bối cảnh cộng đồng hiện tại hay tài liệu hướng dẫn người mua cũ. |
| Thuật ngữ khách hàng | [Fanatec customer glossary](./glossary.md) | Sử dụng thành phần nhất quán, nền tảng, QR, điều chỉnh và ngôn ngữ khắc phục sự cố. |
| Cấp độ cơ sở bánh xe hiện tại [Fanatec Wheel Bases FAQ](https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) | Xác minh các hạn chế về định vị và kết nối CSL, ClubSport và Podium hiện tại. |
| Cấp phép nền tảng [Tương thích nền tảng Fanatec] (https://www.fanatec.com/us-en/platforms) | Xác minh quyền sở hữu cơ sở Xbox wheel/hub và PlayStation. |
| Các thế hệ QR | [Hướng dẫn chuyển đổi Fanatec QR2](https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) | Kiểm tra thế hệ Base-Side / Wheel-Side, đường dẫn nâng cấp và các hạn chế dành riêng cho mô hình. |

## 3. Phần mềm Sim-Racing công cộng

| Công cụ | Nguồn | Sử dụng cho nhà phát triển |
|---|---|---|
| OpenFFBoard [OpenFFBoard wiki] (https://github.com/Ultrawipf/OpenFFBoard/wiki/) | Nghiên cứu các khái niệm firmware FFB mô-đun, trình điều khiển động cơ, bộ mã hóa và tích hợp HID / PID. |
| hid-fanatecff | [gotzl/hid-fanatecff](https://github.com/gotzl/hid-fanatecff) | Nghiên cứu bản dịch Fanatec HID/FFB phía Linux, ID thiết bị, tách sysfs LED / hiển thị và hành vi HIDRAW. |
| hid-fanatecff-tools | [gotzl/hid-fanatecff-tools](https://github.com/gotzl/hid-fanatecff-tools) | Nghiên cứu các mẫu cầu trò chơi từ xa cho đèn LED, màn hình và giá trị điều chỉnh. |
SimHub [SimHub wiki] (https://github.com/SHWotever/SimHub/wiki) | Nghiên cứu bảng điều khiển từ xa, màn hình Arduino, đèn LED, hộp nút và tích hợp thiết bị nối tiếp. |

## 4. Công cụ khởi động phần cứng và phần cứng

| Lớp công cụ | Ví dụ | Dùng |
|---|---|---|
| Logic analyzer | Saleae-class analyzer, sigrok/PulseView | Validate SPI/UART/CAN timing, ranh giới giao dịch QR, và thời hạn boot-to-response. |
| Máy hiện sóng | 2 + kênh DSO | Xác minh đường ray, thiết lập lại, thời gian PWM, tín hiệu mã hóa, thời gian hiện tại và nguồn cấp dữ liệu QR. |
| Trình phân tích/phần mềm USB | Wireshark USBPcap, Linux `usbmon`, hid-tools | Kiểm tra liệt kê, mô tả, báo cáo, và thời gian máy/thiết bị. |
| Firmware debug | SWD/JTAG, RTT, semihosting bị vô hiệu hóa trong các đường dẫn thời gian thực | Debug startup, state machines, and diagnostics without disturbing critical link timing. |
Thiết bị HIL Trình mô phỏng giao thức, thiết bị nguồn giới hạn hiện tại, động cơ giả / tải Xác minh xử lý lỗi trước khi vận hành phần cứng thương mại hoặc toàn năng.

## 5. Danh sách kiểm tra xác thực theo hệ thống con

| Hệ thống con | Dụng cụ tối thiểu |
|---|---|
Cơ sở bánh xe Máy hiện sóng, máy phân tích logic, nguồn cung cấp giới hạn hiện tại, dấu vết USB, thiết bị phun E-stop / lỗi.
| Vành lái | Phân tích logic, vật cố định pinout QR, kiểm tra đường ray / nguồn cấp dữ liệu ngược, kiểm tra trả lại đầu vào, kiểm tra ứng suất hiển thị / LED.
Bàn đạp DMM, chụp ADC, vật cố định trọng lượng / lực đã biết, dấu vết USB HID, kiểm tra độ bền hiệu chuẩn.
| Shifter/handbrake | GPIO/ADC capture, debounce timing trace, impossible-state injection.
Bảng điều khiển / hộp nút USB / dấu vết nối tiếp, hồ sơ SimHub, hiển thị kiểm tra căng thẳng làm mới.
Buồng lái Đo độ lệch dưới mô-men xoắn bánh xe và tải trọng phanh, kiểm tra mô-men xoắn fastener, kiểm tra cách ly cộng hưởng / đầu dò xúc giác.

## 6. Quy tắc xử lý nguồn

- Sử dụng các tiêu chuẩn chính thức cho tuyên bố giao thức.
- Sử dụng hướng dẫn sử dụng của nhà sản xuất để thiết lập công khai, cập nhật, an toàn và hành vi kết nối.
- Chỉ sử dụng các dự án cộng đồng/GitHub cho các triển khai công khai đã được chứng minh.
- Biên giới thế hệ kỷ lục. Trình giả lập vành Fanatec SPI kế thừa không phải là bằng chứng cho hành vi ClubSport DD / DD +.
- Liên kết nguồn chính xác được sử dụng; không trích dẫn kết quả tìm kiếm kho lưu trữ làm bằng chứng cho một yêu cầu cụ thể.
- Ngày hướng dẫn người mua cộng đồng. Kiểm tra lại mô-men xoắn, tính khả dụng, QR, nền tảng và phần sụn tuyên bố chống lại sự hỗ trợ của nhà sản xuất hiện tại trước khi tái sử dụng.

## Câu hỏi chưa được giải quyết

- Công cụ kiểm tra mô tả USB / HID nào nên được chuẩn hóa khi mã nguồn tồn tại?
