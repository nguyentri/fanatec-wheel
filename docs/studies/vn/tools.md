# Công cụ Phát triển và Tài liệu Tham khảo Sim Racing

Tài liệu này liệt kê các công cụ thực tế và các nguồn tham khảo cho các nhà phát triển đang nghiên cứu hoặc tạo mẫu các thiết bị ngoại vi sim racing. Đây là một điểm khởi đầu, không phải là danh sách phê duyệt sản phẩm.

## 1. Tiêu chuẩn và Tham chiếu Giao diện

| Nhu cầu | Tham chiếu | Sử dụng |
|---|---|---|
| Thiết bị đầu vào USB | [USB-IF HID specifications and tools](https://www.usb.org/hid) | Mô tả HID (HID descriptors), cách sử dụng, mô hình báo cáo, công cụ tạo mô tả. |
| Thiết bị phản hồi lực (Force feedback) | [USB-IF PID Class 1.0](https://www.usb.org/sites/default/files/documents/pid1_01.pdf) | Báo cáo Thiết bị Giao diện Vật lý (PID) cho vô lăng phản hồi lực và thiết bị haptic. |
| API đầu vào và FF của Linux | [Linux force-feedback documentation](https://www.kernel.org/doc/html/latest/input/ff.html) | Các khái niệm về tải lên/phát lại hiệu ứng từ phía máy chủ (host). |
| Truy cập HIDRAW | [Linux HIDRAW documentation](https://docs.kernel.org/hid/hidraw.html) | Truy cập trực tiếp vào mô tả/báo cáo HID cho các công cụ và lớp tương thích. |

## 2. Hệ sinh thái và Tham chiếu Tương thích

| Nhu cầu | Tham chiếu | Sử dụng |
|---|---|---|
| Độ tin cậy của nguồn và ngày tháng | [Fanatec ecosystem source register](./references.md) | Kiểm tra xem một tuyên bố là chính thức, bối cảnh cộng đồng hiện tại hay tài liệu hướng dẫn người mua đã cũ. |
| Thuật ngữ khách hàng | [Fanatec customer glossary](./glossary.md) | Sử dụng ngôn ngữ nhất quán về thành phần, nền tảng, QR, tuning và khắc phục sự cố. |
| Phân khúc wheel-base hiện tại | [Fanatec Wheel Bases FAQ](https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) | Xác minh các hạn chế về định vị và kết nối của CSL, ClubSport và Podium hiện tại. |
| Cấp phép nền tảng | [Fanatec platform compatibility](https://www.fanatec.com/us-en/platforms) | Xác minh quyền sở hữu cơ sở vô lăng/hub Xbox và PlayStation. |
| Các thế hệ QR | [Fanatec QR2 conversion guidance](https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) | Kiểm tra thế hệ Base-Side/Wheel-Side, lộ trình nâng cấp và các hạn chế theo từng mẫu cụ thể. |

## 3. Phần mềm Sim-Racing Công cộng

| Công cụ | Nguồn | Sử dụng cho Nhà phát triển |
|---|---|---|
| OpenFFBoard | [OpenFFBoard wiki](https://github.com/Ultrawipf/OpenFFBoard/wiki/) | Nghiên cứu các khái niệm firmware FFB mô-đun, trình điều khiển động cơ, bộ mã hóa (encoders) và tích hợp HID/PID. |
| hid-fanatecff | [gotzl/hid-fanatecff](https://github.com/gotzl/hid-fanatecff) | Nghiên cứu bản dịch HID/FFB của Fanatec phía Linux, ID thiết bị, phân tách sysfs LED/màn hình và hành vi HIDRAW. |
| hid-fanatecff-tools | [gotzl/hid-fanatecff-tools](https://github.com/gotzl/hid-fanatecff-tools) | Nghiên cứu các mẫu cầu nối telemetry của trò chơi cho đèn LED, màn hình và các giá trị tuning. |
| SimHub | [SimHub wiki](https://github.com/SHWotever/SimHub/wiki) | Nghiên cứu bảng điều khiển telemetry, màn hình Arduino, đèn LED, hộp nút (button boxes) và tích hợp thiết bị nối tiếp. |

## 4. Công cụ Kiểm tra Phần mềm và Phần cứng

| Lớp Công cụ | Ví dụ | Sử dụng |
|---|---|---|
| Máy phân tích logic | Máy phân tích lớp Saleae, sigrok/PulseView | Xác thực thời gian SPI/UART/CAN, ranh giới giao dịch QR và thời hạn khởi động-đến-phản hồi (boot-to-response). |
| Máy hiện sóng (Oscilloscope) | DSO 2+ kênh | Xác minh các đường điện (rails), reset, thời gian PWM, tín hiệu mã hóa, thời gian cảm biến dòng điện và dòng điện ngược từ QR. |
| Phần mềm/máy phân tích USB | Wireshark USBPcap, Linux `usbmon`, hid-tools | Kiểm tra quá trình liệt kê (enumeration), mô tả, báo cáo và thời gian host/device. |
| Gỡ lỗi Firmware | SWD/JTAG, RTT, vô hiệu hóa semihosting trong các luồng thời gian thực | Gỡ lỗi quá trình khởi động, máy trạng thái và chẩn đoán mà không làm xáo trộn thời gian liên kết quan trọng. |
| Thiết bị thử nghiệm HIL | Trình mô phỏng giao thức, bộ nguồn giới hạn dòng điện, động cơ/tải giả | Xác minh khả năng xử lý lỗi trước khi vận hành phần cứng thương mại hoặc sử dụng toàn bộ năng lượng. |

## 5. Danh sách Kiểm tra Xác thực theo Hệ thống con

| Hệ thống con | Công cụ Tối thiểu |
|---|---|
| Wheel base | Máy hiện sóng, máy phân tích logic, bộ nguồn giới hạn dòng điện, theo dõi USB, thiết bị thử nghiệm E-stop/tiêm lỗi. |
| Steering rim | Máy phân tích logic, thiết bị ghim QR, kiểm tra đường điện/dòng điện ngược, kiểm tra dội đầu vào (bounce), kiểm tra độ bền màn hình/LED. |
| Bàn đạp | DMM, chụp ADC, thiết bị thử nghiệm lực/trọng lượng đã biết, theo dõi USB HID, kiểm tra độ bền hiệu chuẩn. |
| Shifter/handbrake | Chụp GPIO/ADC, theo dõi thời gian chống dội (debounce), tiêm trạng thái không hợp lệ. |
| Dashboard/button box | Theo dõi USB/serial, cấu hình SimHub, kiểm tra độ bền làm mới màn hình. |
| Buồng lái (Cockpit) | Đo độ võng dưới mô-men xoắn vô lăng và tải trọng phanh, kiểm tra mô-men xoắn ốc vít, kiểm tra cách ly cảm biến rung/chạm. |

## 6. Quy tắc Xử lý Nguồn

- Sử dụng các tiêu chuẩn chính thức cho các tuyên bố về giao thức.
- Sử dụng hướng dẫn của nhà sản xuất về thiết lập công khai, cập nhật, an toàn và hành vi kết nối.
- Chỉ sử dụng các dự án GitHub/cộng đồng cho các triển khai công khai đã được chứng minh.
- Ghi chép ranh giới thế hệ. Một trình giả lập SPI rim cũ của Fanatec không phải là bằng chứng cho hành vi của ClubSport DD/DD+.
- Liên kết đến nguồn chính xác được sử dụng; không trích dẫn kết quả tìm kiếm repo như là bằng chứng cho một tuyên bố cụ thể.
- Ghi ngày cho các hướng dẫn mua hàng từ cộng đồng. Kiểm tra lại các tuyên bố về mô-men xoắn, tính khả dụng, QR, nền tảng và firmware so với hỗ trợ hiện tại của nhà sản xuất trước khi tái sử dụng.

## Sổ Đăng ký Câu hỏi (Đã giải quyết và Mở)

Được review vào ngày 2026-07-05.

### Đã giải quyết

- **Công cụ kiểm tra mô tả USB/HID nào nên được tiêu chuẩn hóa sau khi có mã nguồn?**
  **Khuyến nghị kỹ thuật (các công cụ công khai đã được xác minh).** Tiêu chuẩn hóa một bộ nhỏ, đa nền tảng thay vì một công cụ: để giải mã descriptor, hãy sử dụng **USB-IF HID Descriptor Tool** hoặc bộ giải mã report-descriptor HID trực tuyến; để bắt/giải mã trực tiếp, sử dụng **Wireshark với USBPcap** (Windows) hoặc **usbmon** (Linux); đối với đọc/ghi HID thô và kiểm tra báo cáo FFB nhanh, sử dụng **hidapitester** hoặc `usbhid-dump` + `evtest` của Linux; và xác thực hành vi của trục/nút/FFB bằng bảng điều khiển kiểm tra joystick/HID của hệ điều hành. Đặc biệt trên Linux, trình điều khiển `hid-fanatecff` cùng với `evtest`/`fftest` là đường dẫn xác minh FFB thực tế. Chọn bộ giải mã descriptor làm tiêu chuẩn chính và giữ các công cụ chụp/HID thô làm chuỗi công cụ hỗ trợ.
