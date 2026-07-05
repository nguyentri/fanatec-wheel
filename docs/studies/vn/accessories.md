# Sim Racing Phụ kiện Kiến trúc

> Ngày nghiên cứu: 2026-07-02
> Mô hình bằng chứng: tiêu chuẩn công cộng, hướng dẫn sử dụng / hỗ trợ của nhà sản xuất và các dự án cộng đồng. Các dự án cộng đồng là bằng chứng thực hiện, không phải thông số kỹ thuật của nhà cung cấp chính thức.
> Bắt đầu ở đây sau [sim_racing_research.md](./sim_racing_research.md), [wheel_base.md](./wheel_base.md), và [wheel_rim.md](./wheel_rim.md).

Tài liệu này phác thảo kiến trúc phần cứng và phần mềm của các thiết bị ngoại vi đua xe mô phỏng phổ biến, cụ thể là hệ thống Quick Release (QR), bảng điều khiển và hộp nút. Nó nhằm cung cấp sự hiểu biết nền tảng cho các kỹ sư tham gia vào lĩnh vực đua xe mô phỏng, tập trung vào thiết kế hệ thống nhúng, giao thức truyền thông và tích hợp giao diện người-máy (HMI).

## 1. Hệ thống phát hành nhanh (QR)

> ** Lưu ý: ** Phần này bao gồm các tiêu chuẩn khớp nối vật lý và cơ học chung của các phụ kiện độc lập. Để biết chi tiết về cách tay lái PCB và firmware xử lý cụ thể tải trọng dữ liệu QR và muxing nguồn, hãy tham khảo [Kiến trúc vành lái] (./wheel_rim.md).

Hệ thống Quick Release (QR) tạo thành cầu nối vật lý và điện quan trọng giữa chiều dài cơ sở cố định (hoặc quay) và vành vô lăng có thể hoán đổi cho nhau. Phần này nêu chi tiết các ràng buộc cơ học và kiến trúc truyền dữ liệu của nó.

### 1.1 Giao diện cơ điện

> ** Tin tức: ** Một hệ thống QR phải xử lý mô-men xoắn đáng kể từ chiều dài cơ sở Direct Drive (DD) trong khi duy trì tiếp xúc điện liên tục cho các nút và màn hình gắn trên bánh xe.

QR và trung tâm là ranh giới tương thích riêng biệt. Một trung tâm phổ quát có thể hỗ trợ các mẫu bu lông vành phổ biến như 6x70 mm hoặc 3x50 mm, trong khi bản thân QR phải phù hợp với thế hệ trục cơ sở bánh xe, xếp hạng mô-men xoắn, khóa cơ và giao diện điện. Một mẫu bu lông chung không làm cho vành an toàn hoặc tương thích điện với đế.

Đối với các sản phẩm Fanatec hiện tại, QR2 có các thành phần ** Base-Side ** và ** Wheel-Side ** riêng biệt. Cả hai bên phải sử dụng QR2. QR1 và QR2 không giao phối, QR1 bị ngưng và hỗ trợ chuyển đổi là dành riêng cho mô hình. Các biến thể Wheel-Side QR2 Lite, QR2 và QR2 Pro cũng có các phê duyệt sản phẩm và mô-men xoắn cao khác nhau; kiểm tra danh sách tương thích hiện tại thay vì suy ra hỗ trợ từ tài liệu một mình.

![Phát hành nhanh: khớp nối cơ khí và cầu điện / dữ liệu] (./quick_release_exploded.svg)

Khớp nối thực hiện hai công việc cùng một lúc. Về mặt cơ học, nó khóa trung tâm Wheel-Side vào trục Base-Side để truyền mô-men xoắn mà không cần chơi. Về mặt điện, đối với vành có nút và màn hình, chốt lò xo đáp ứng các miếng đệm tiếp xúc trên khớp để mang điện và dữ liệu. Bởi vì cả hai nửa phải là cùng một thế hệ để giao phối, một mẫu bu lông vành được chia sẻ trên hub không * chứng minh khả năng tương thích QR hoặc điện - thế hệ QR, xếp hạng mô-men xoắn và giao diện tiếp xúc là các kiểm tra riêng biệt.

**Hình 1-1: Kiến trúc điện phát hành nhanh **

```nàng tiên cá
đồ thị LR
PC [Host PC] -> |USB| Base [MCU cơ sở bánh xe]
Cơ sở -->|Giao thức độc quyền| SlipRing[Slip Ring / Pins]
SlipRing -->|Nguồn & Dữ liệu| WheelMCU[Bánh lái MCU]
WheelMCU -> Nút [Chuyển đổi đầu vào]
WheelMCU -> SPI [Màn hình SPI]
```

### 1.2 Giao thức truyền dữ liệu

> ** Tin tức: ** Các nhà sản xuất sử dụng các giao thức khác nhau để truyền dữ liệu qua QR. Một số sử dụng thông qua USB tiêu chuẩn, trong khi những người khác dựa vào các bus nối tiếp độc quyền hoặc các liên kết không dây (Bluetooth / 2.4GHz) kết hợp với công suất cảm ứng.

Bộ vi điều khiển vô lăng ** sẽ ** xử lý các đầu vào ngoại vi thô và đóng gói chúng thành một tải trọng có cấu trúc. Liên kết dữ liệu QR ** sẽ ** duy trì tỷ lệ bỏ phiếu ít nhất 100 Hz để tránh độ trễ đầu vào đáng chú ý.

| Yếu tố | Hướng | Kiểu | Mô tả |
|---------|-----------|------|-------------|
| `VCC` | Đầu vào | Nguồn | 5V hoặc 12V cung cấp từ chiều dài cơ sở đến vành |
| `GND` | Chung | Mặt đất | Tham chiếu mặt đất hệ thống |
| `DATA_TX` | Đầu ra | Nối tiếp | Tải trọng trạng thái nút đến chiều dài cơ sở |
| `DATA_RX` | Đầu vào | Nối tiếp | Buộc phản hồi hoặc hiển thị dữ liệu từ chiều dài cơ sở |

## 2. Bảng điều khiển và hiển thị từ xa

> ** Lưu ý: ** Phần này bao gồm bảng điều khiển từ xa * độc lập * USB hoặc Wi-Fi. Để biết thông tin về màn hình * tích hợp * được tích hợp vào vô lăng và được điều khiển trực tiếp bởi liên kết cơ sở bánh xe, hãy tham khảo [Kiến trúc vành lái] (./wheel_rim.md).

Bảng điều khiển cung cấp từ xa thời gian thực, chẳng hạn như RPM, bánh răng và nhiệt độ lốp cho người lái xe. Họ yêu cầu một cầu nối phần mềm đáng tin cậy để trích xuất dữ liệu trò chơi và bộ điều khiển nhúng để điều khiển màn hình vật lý.

### 2.1 Kiến trúc phần cứng

> **Informative:** DIY và prosumer bảng điều khiển thường dựa trên vi điều khiển (ví dụ, Arduino, ESP32) cầu nối dữ liệu nối tiếp USB để hiển thị thiết bị ngoại vi thông qua I2C hoặc SPI.

Bộ điều khiển bảng điều khiển ** sẽ ** giao diện với màn hình ký tự thông qua bus I2C và màn hình TFT / OLED thông qua bus SPI. Hệ thống ** nên ** giảm thiểu sự xáo trộn thiết bị I2C để ngăn chặn độ bão hòa của bus trong quá trình cập nhật từ xa tốc độ làm mới cao.

**Hình 2-1: Kiến trúc điều khiển bảng điều khiển **

```nàng tiên cá
đồ thị TD
SimHub [PC: Phần mềm SimHub] -> |Virtual COM / USB| MCU [ESP32 / Arduino]
MCU -->|I2C| CharDisplay [1602 Ký tự LCD]
MCU -->|SPI| TFTDisplay[Nextion / USBD480 TFT] Name
MCU -> |GPIO| WS2812 [Dải LED RPM]
```

### 2.2 Tích hợp phần mềm từ xa

> ** Tin tức: ** SimHub là phần mềm tiêu chuẩn công nghiệp để trích xuất từ xa trò chơi và gửi nó đến các thiết bị bên ngoài.

Phần mềm máy chủ ** sẽ ** truyền các chuỗi từ xa được mã hóa qua cổng nối tiếp ảo đến bộ điều khiển bảng điều khiển. Phần sụn bảng điều khiển ** sẽ ** phân tích cú pháp các chuỗi này và cập nhật bộ đệm hiển thị tương ứng.

| Điều kiện | Kích hoạt | Hành động |
|-----------|---------|--------|
| `RPM >= SHIFT_POINT` | RPM Threshold | Nhấp nháy tất cả đèn LED WS2812 |
| `Rx_Timeout > 2000ms` | Mất kết nối | Xóa màn hình và hiển thị "KHÔNG TÍN HIỆU" |

## 3. Hộp nút và ma trận đầu vào

Các hộp nút mở rộng khả năng đầu vào của người lái, xử lý các công tắc đánh lửa, quay thiên vị phanh và điều hướng menu. Chúng hoạt động như các thiết bị giao diện người dùng USB độc lập (HID).

# 3.1 Phần cứng Ma trận Đầu vào

> ** Tin tức: ** Để hỗ trợ hàng chục thiết bị chuyển mạch mà không làm cạn kiệt chân GPIO vi điều khiển, các nút được nối dây trong ma trận cột hàng.

Phần cứng hộp nút ** sẽ ** sử dụng cấu trúc liên kết ma trận chuyển đổi cho tất cả các nút nhấn và chuyển đổi. Một diode (ví dụ: 1N4148) ** sẽ ** được đặt theo chuỗi với mỗi công tắc để ngăn chặn các phím ảo (lờ mờ) khi nhiều đầu vào đang hoạt động. Bộ mã hóa quay ** không ** được nối dây vào ma trận; chúng ** sẽ ** được kết nối với các chân GPIO chuyên dụng với phần cứng hoặc phần mềm debouncing.

![Ma trận nút và điốt chống ma] (./button_matrix_ghosting.svg)

Yêu cầu diode không phải là mỹ phẩm tùy chọn. Trong một ma trận trần, giữ ba nút chia sẻ một hàng và cột cho phép lén lút hiện tại thông qua một đường dẫn ngược, vì vậy việc quét sẽ đọc một phím thứ tư mà không ai nhấn - một báo chí ảo. Một diode nối tiếp với mỗi công tắc chỉ cho phép một cách hiện tại, chặn đường dẫn lén lút đó để cùng ba lần nhấn đọc chính xác. Đây là lý do tại sao diode được chỉ định cho mỗi công tắc thay vì mỗi hàng hoặc cột.

**Hình 3-1: Cấu trúc liên kết ma trận **

```nàng tiên cá
đồ thị TD
MCU [ví dụ STM32 MCU] -> |Row Outputs | Rows [Matrix Rows]
MCU -->|Đầu vào cột|Cột[Cột ma trận]
Hàng -> Công tắc [Nút nhấn]
Cols -> Công tắc
Công tắc -> Điốt [Điốt chống ma]
```

### 3.2 Firmware và USB HID Enumeration

> ** Informatitive: ** Đối với khả năng tương thích plug-and-play bản địa với các hệ điều hành và mô phỏng đua xe, thiết bị phải mô phỏng một bộ điều khiển trò chơi tiêu chuẩn.

Bộ vi điều khiển hộp nút ** sẽ ** có hỗ trợ USB HID gốc (ví dụ: ATmega32U4 hoặc RP2040). Phần sụn ** sẽ ** giải mã tất cả các chuyển đổi trạng thái chuyển đổi vật lý.

| Bước | Hành động | Ghi chú / ràng buộc |
|------|--------|--------------------|
| 1 | Firmware ** sẽ ** cấu hình các hàng ma trận làm đầu ra và các cột làm đầu vào với các pull-up bên trong. | Khởi tạo trạng thái phần cứng. |
| 2 | Firmware ** sẽ ** tuần tự kéo từng hàng LOW và lấy mẫu trạng thái cột. | Ma trận quét vòng lặp. |
Phần sụn ** sẽ ** xây dựng một báo cáo cần điều khiển HID tiêu chuẩn. Định dạng dữ liệu cho PC chủ.
Phần sụn ** sẽ ** truyền báo cáo HID qua USB. Xảy ra khi thay đổi trạng thái hoặc khoảng thời gian bỏ phiếu.

## 4. Câu hỏi chưa được giải quyết

* Những trao đổi danh tính, khả năng và quyền mô-men xoắn nào được chỉ định công khai cho mỗi thế hệ QR? Không giả định một cơ chế mật mã hoặc DRM mà không có bằng chứng được phê duyệt.
* Chi phí độ trễ cụ thể khi bắc cầu từ xa thông qua phần mềm trung gian như SimHub so với đầu ra từ xa động cơ trò chơi gốc là gì?
* Kiến trúc bus CAN có thể thay thế các kết nối nối tiếp hoặc SPI đơn giản trong vô lăng đua sim prosumer cho độ tin cậy cao hơn và mở rộng nút không?

## 5. Tài liệu tham khảo

### 5.1 Nguồn chính thức và tiêu chuẩn

- [Công cụ và thông số kỹ thuật USB-IF HID] (https://www.usb.org/hid) - Mô tả HID, cách sử dụng và công cụ cho hộp nút và giao diện điều khiển bảng điều khiển.
- [USB-IF PID Class 1.0] (https://www.usb.org/sites/default/files/documents/pid1_01.pdf) - mô hình thiết bị haptic /force-feedback; ngữ cảnh ranh giới hữu ích để tách FFB khỏi đo từ xa bảng điều khiển.
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) - phát hành nhanh, khởi động, cập nhật, hiệu chuẩn và bối cảnh phát hiện tay lái.
- [Hướng dẫn chuyển đổi Fanatec QR2] (https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) - Ranh giới thế hệ QR1 / QR2, các biến thể Base-Side / Wheel-Side và nâng cấp theo mô hình cụ thể.
- [Câu hỏi thường gặp về tay lái Fanatec] (https://help.fanatec.com/hc/en-us/articles/43802514108433-Steering-Wheel-FAQ) - Ngày ngừng sử dụng QR2 và QR1.

# 5.2 Công cụ công cộng và nguồn cộng đồng

- [SimHub wiki] (https://github.com/SHWotever/SimHub/wiki) - bảng điều khiển, màn hình Arduino, đèn LED, nút, thiết bị nối tiếp tùy chỉnh và công cụ đo từ xa.
- [OpenFFBoard wiki] (https://github.com/Ultrawipf/OpenFFBoard/wiki/) - kiến trúc thiết bị phản hồi lực mở; hữu ích để tách các mối quan tâm về điều khiển động cơ, HID / PID và I / O.
- [gotzl/hid-fanatecff] (https://github.com/gotzl/hid-fanatecff) - Linux Fanatec LED / hiển thị, HIDRAW và các mẫu tích hợp phản hồi lực.
- [FendtXerion3800/Fanatec-Pinout] (https://github.com/FendtXerion3800/Fanatec-Pinout) - quan sát kết nối cộng đồng; xác minh bằng điện trước khi sử dụng.
- [Đăng ký nguồn hệ sinh thái Fanatec] (./references.md) - phân loại nguồn chính thức / cộng đồng và ghi chú tiền tệ.
