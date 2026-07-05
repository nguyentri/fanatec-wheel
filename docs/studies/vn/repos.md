# Fanatec & Sim Racing Kho mã nguồn mở

Tài liệu này là một bản đồ khám phá được quản lý cho các dự án công cộng có liên quan đến nghiên cứu phần cứng tương thích Fanatec và đua xe mô phỏng. Các kho lưu trữ này rất hữu ích cho các bài học kiến trúc và giả thuyết khả năng tương tác. Chúng không phải là thông số kỹ thuật chính thức của Fanatec trừ khi kho lưu trữ thuộc sở hữu của nhà cung cấp.

## Làm thế nào để sử dụng các kho

| Loại bằng chứng | Những gì nó có thể hỗ trợ | Cần thận trọng |
|---|---|---|
| README/tài liệu dự án | Mục tiêu dự án, phần cứng hỗ trợ, phương thức xây dựng | Có thể cũ hoặc không đầy đủ |
| Mã nguồn | Quan sát khung hình, thời gian, mô tả và giả định phần cứng | Có thể chỉ nhắm mục tiêu một thế hệ thiết bị |
| Sơ đồ /pinouts | Giả thuyết điện cộng đồng | Phải được xác minh chống lại các tài liệu được phê duyệt hoặc được đo một cách an toàn |
| Các vấn đề / thảo luận | Chế độ thất bại và báo cáo khả năng tương thích | Điều trị như giai thoại cho đến khi sao chép |
| Phát hành / nhị phân | Đường dẫn xây dựng / triển khai có thể tái tạo | Không phân phối lại firmware độc quyền hoặc nhị phân không xác định |

## Trình tự đọc của nhà phát triển

1. Đọc [sim_racing_research.md] (./sim_racing_research.md) cho ranh giới hệ thống con.
2. Đọc [wheel_rim.md] (./wheel_rim.md) trước các dự án giả lập bánh xe.
3. Đọc [pedals.md] (./pedals.md) trước các dự án bộ điều khiển bàn đạp hoặc bàn đạp.
4. Đọc [add_ons.md] (./add_ons.md) trước các dự án phanh tay / phanh tay.
5. Sử dụng [tools.md] (./tools.md) để xác thực các giả định USB, HID, thời gian và điện.

## Trình giả lập bánh xe & Tay lái DIY

* **[lshachar/Arduino_Fanatec_Wheel](https://github.com/lshachar/Arduino_Fanatec_Wheel)**
Một dự án do-it-yourself cho phép bạn xây dựng một tay lái tùy chỉnh giao tiếp với các cơ sở bánh xe Fanatec qua SPI. Nó hỗ trợ các nút, D-pad và hiển thị chữ và số, giả mạo chiều dài cơ sở để cho phép Phản hồi lực.

* **[StuyoP/Fanatec-Wheel-Barebone-Emulator](https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator)**
Trình giả lập xương sống cho cơ sở bánh xe Fanatec sử dụng chip ATmega328p chạy ở mức 3.3V. Nó cho phép bạn tạo vô lăng DIY với nút đầy đủ, màn hình và hỗ trợ LED mà không cần bộ chuyển đổi mức logic.

* **[Alexbox364/F_Interface_AL](https://github.com/Alexbox364/F_Interface_AL)**
Một nền tảng phần cứng và phần mềm linh hoạt để xây dựng vô lăng tùy chỉnh DIY. Nó giao tiếp trực tiếp với chiều dài cơ sở Fanatec thông qua giao thức SPI và có thể hỗ trợ tối đa 24 nút ấn, bộ mã hóa quay, màn hình TM1637 và màn hình OLED.

## Trình giả lập bàn đạp & Bộ điều khiển thay thế

* **[jssting/ArduinoTec-Pedals](https://github.com/jssting/ArduinoTec-Pedals)**
Một dự án Arduino Leonardo / Pro Micro có nghĩa là thay thế bảng điều khiển OEM của Fanatec ClubSport Pedals (CSP) V1 / V2. Hoàn hảo để sửa chữa bàn đạp cũ khi PCB chính bị hỏng, cho phép bạn giao tiếp các tế bào tải ban đầu và cảm biến hiệu ứng Hall thông qua USB tiêu chuẩn.

* **[GeekyDeaks/fanatec-pedal-emulator](https://github.com/GeekyDeaks/fanatec-pedal-emulator)**
Một công cụ cho phép bạn lấy bàn đạp USB của bên thứ ba (như Heusinkveld) và ủy quyền chúng thông qua chân đế Fanatec thông qua cổng RJ12, giúp chúng có thể sử dụng được trên các máy chơi game như PS4 / PS5 hoặc Xbox.

* **[adamcrawley/fanatec-pedal-emulator-vrs](https://github.com/adamcrawley/fanatec-pedal-emulator-vrs)**
Một phiên bản sửa đổi của trình giả lập ở trên, đặc biệt hướng tới việc làm cho VRS DirectForce Pro Pedals tương thích với chiều dài cơ sở Fanatec.

## Shifters, phản hồi chuyển động và lực

* **[StuyoP/Universal-Shifter-Interface-for-Fanatec](https://github.com/StuyoP/Universal-Shifter-Interface-for-Fanatec)**
Một giao diện cho phép bạn kết nối bất kỳ mô hình H dựa trên công tắc hoặc bộ chuyển mạch tuần tự nào trực tiếp với chiều dài cơ sở Fanatec bằng giao thức RJ12 bên trong của nó.

* ** [Ultrawipf/OpenFFBoard] (https://github.com/Ultrawipf/OpenFFBoard) **
Một nền tảng giao diện phản hồi lực nguồn mở phổ quát bao quát cho các thiết bị mô phỏng DIY tương thích cao, đặc biệt là vô lăng lái trực tiếp. Nó hỗ trợ các trình điều khiển động cơ khác nhau (như TMC4671, ODrive, VESC) và bộ mã hóa.

* **[vnmsimulation/VNM_MOTION_CONTROLLER](https://github.com/vnmsimulation/VNM_MOTION_CONTROLLER)**
Ứng dụng cấu hình và phần sụn dựa trên STM32F401RCT từ VNM Simulation để xây dựng phần cứng DIY bao gồm chiều dài cơ sở, bàn đạp và giàn chuyển động.

## Tham khảo phần cứng & Pinouts

* **[FendtXerion3800/Fanatec-Pinout](https://github.com/FendtXerion3800/Fanatec-Pinout)**
Một kho lưu trữ tham khảo phần cứng hữu ích ghi lại các chân cắm cho thiết bị Fanatec (chẳng hạn như ổ cắm RJ12 cho bàn đạp, cần gạt và phát hành nhanh bánh xe). Điều này rất quan trọng đối với bất kỳ ai xây dựng cáp chuyển đổi DIY hoặc bộ điều hợp tùy chỉnh.

## Trình điều khiển & Giao diện phần mềm

* **[gotzl/hid-fanatecff](https://github.com/gotzl/hid-fanatecff)**
Một mô-đun trình điều khiển hạt nhân Linux được đánh giá cao cung cấp hỗ trợ phản hồi toàn lực (FFB) cho các cơ sở bánh xe Fanatec. Cần thiết cho bất cứ ai muốn đua mô phỏng trên nền tảng Linux hoặc SteamOS.

## Tìm kiếm kho chung

Nếu bạn muốn tiếp tục khám phá những sáng tạo cộng đồng mới nhất trên GitHub và GitLab, đây là một vài truy vấn tìm kiếm trực tiếp:
* [Tìm kiếm trên GitHub: "fanatec"](https://github.com/search?q=%20fanatec&type=repositories)
* [GitHub Tìm kiếm: "cơ sở bánh xe đua"](https://github.com/search?q=racing+wheel+base+&type=repositories)
* [GitLab Search: "fanatec"](https://gitlab.com/search?search=fanatec)
* [GitLab Search: "sim đua"](https://gitlab.com/search?search=sim+racing)

## Neo tham chiếu

- [Công cụ và thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid) - cơ sở cho các báo cáo và sử dụng USB HID.
- [USB-IF PID Class 1.0] (https://www.usb.org/sites/default/files/documents/pid1_01.pdf) - Đường cơ sở cho mô hình HID force-feedback / PID.
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) - hành vi nhà sản xuất công cộng cho một gia đình cơ sở cao cấp.
- [SimHub wiki] (https://github.com/SHWotever/SimHub/wiki) - hệ sinh thái từ xa công cộng / bảng điều khiển / nút-box.
- [OpenFFBoard wiki] (https://github.com/Ultrawipf/OpenFFBoard/wiki/) - hệ sinh thái cơ sở bánh xe FFB mô-đun công cộng.

## Câu hỏi chưa được giải quyết

- Những kho nào nên được thúc đẩy thành ma trận tương thích chính thức sau khi xác minh băng ghế dự bị?
