# Ma trận tương thích

> Phiên bản: 1.0
> Đánh giá: 2026-07-02
> Mục đích: cung cấp cấu trúc của ma trận tương thích bàn đạp / bánh xe / QR, theo yêu cầu trong [pedals.md] (./pedals.md) §8.3 và [repos.md] (./repos.md). Nó tách các trục tương thích và ghi lại trạng thái xác minh trên mỗi ô; nó ** không ** khẳng định tuyên bố sản phẩm phụ chưa được xác minh.

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Thay đổi |
|---|---|---|
| 1.0 | 2026-07-02 | Tài liệu mới. Khung được tạo cho mỗi lần theo dõi ma trận bàn đạp trong [pedals.md] (./pedals.md) §8.3 và câu hỏi "thúc đẩy repos thành ma trận tương thích chính thức sau khi xác minh băng ghế dự bị" trong [repos.md] (./repos.md). Các tế bào chỉ cư trú ở nơi cơ sở nghiên cứu này đã thiết lập một thực tế; tất cả những người khác được đánh dấu để xác minh. |

## 1. Mục đích và phương pháp

> [ QUAN TRỌNG]
> Tuyên bố tương thích chính xác là loại thực tế mà cơ sở nghiên cứu này từ chối khẳng định mà không có bằng chứng. Tài liệu này là ** khung **: mọi mục nhập đều mang trạng thái ** Đã xác minh công khai **, ** Báo cáo cộng đồng ** hoặc ** Xác minh ** (chưa được thiết lập ở đây). Dân số "Xác minh" các tế bào chỉ sau khi xác nhận chống lại một nguồn chính thức hoặc một thử nghiệm băng ghế dự bị.

Giá trị của một ma trận là tách các trục thường được conflated. Tài liệu này xác định các trục đó và chỉ điền vào những gì tập hợp đã hỗ trợ.

## 2. Trục tương thích

| Trục | Câu hỏi nó trả lời |
|---|---|
| Đường dẫn kết nối | USB HID trực tiếp, hoặc thông qua cổng cơ sở bánh xe (proxy)? |
| Thế hệ QR | QR1 vs QR2; Biến thể Base-Side vs Wheel-Side. |
| Nền tảng/bảng điều khiển | PC, Xbox, PlayStation — và nơi cấp phép/sở hữu. |
| Tạo ra thiết bị | Thiết bị ngoại vi hay giao thức phụ thuộc vào một thế hệ cơ sở? |

## 3. Đường dẫn kết nối (USB-direct vs Base-Proxy)

Đây là tách [pedals.md](./pedals.md) §8.3 rõ ràng yêu cầu.

| Đường dẫn | Hành vi | Trạng thái |
|---|---|---|
| Trực tiếp USB HID | Pedals / thiết bị ngoại vi liệt kê như là thiết bị HID riêng của họ cho máy chủ. | Đã xác minh công khai (xem [pedals.md](./pedals.md)) |
| Proxy cổng cơ sở bánh xe | Pedals / ngoại vi kết nối với một cổng cơ sở (ví dụ: RJ12); cơ sở tổng hợp và báo cáo chúng. | Đã xác minh công khai (xem [pedals.md](./pedals.md)) |
| Console via base | Console support is generally reached *through* a compatible base rather than direct USB. | Community-reported / Verify against official platform docs |

ID nhà cung cấp USB Fanatec được quan sát bởi trình điều khiển Linux `hid-fanatecff` là `0x0EB7` (xem [repos.md](./repos.md)) - hữu ích khi xác định thiết bị USB trực tiếp và được gắn nhãn quan sát trình điều khiển thay vì tuyên bố chính thức.

## 4. Tạo QR

Dựa trên hướng dẫn chuyển đổi QR2 được trích dẫn trong [accessories.md](./accessories.md) và điều trị QR trong [wheel_rim.md](./wheel_rim.md).

| Mặt | Ghi chú | Trạng thái |
|---|---|---|
| QR1 vs QR2 | Các thế hệ khác biệt; không phải tất cả các sản phẩm chuyển đổi và chuyển đổi có thể là mô hình cụ thể. | Đã xác minh công khai (Hướng dẫn chuyển đổi Fanatec QR2) |
| Base-Side vs Wheel-Side | QR2 có các biến thể Base-Side và Wheel-Side phải được khớp. | Đã xác minh công khai (cùng nguồn) |
| Mô-men xoắn phù hợp | Mô-men xoắn DD cao hơn làm tăng nhu cầu cơ học đối với khớp nối. | Suy luận kỹ thuật (xem [phụ kiện.md] (./phụ kiện.md)) |
Danh sách chuyển đổi mô hình cụ thể Sản phẩm chính xác nào chuyển đổi sang QR2. Xác minh với bài viết chuyển đổi Fanatec trực tiếp

## 5. Thiết bị / Giao thức thế hệ ranh giới

Ranh giới được thiết lập rõ ràng nhất trong cơ sở nghiên cứu này, từ [wheel_rim.md](./wheel_rim.md) và README `Fanatec-Wheel-Barebone-Emulator` (đã xác minh):

| Ca bệnh | Kết quả được báo cáo | Tình trạng |
|---|---|---|
| Trình giả lập vành AVR SPI cộng đồng trên các cơ sở cũ hơn (thông qua CSL DD, DD1 / DD2) | Báo cáo hoạt động | Báo cáo cộng đồng |
| Trình giả lập rim AVR SPI cộng đồng trên ClubSport DD / DD+ | Đã báo cáo ** không ** hoạt động | Đã báo cáo cộng đồng (cảnh báo giả lập ngược dòng) |

> [! Chú ý]
> Đây là một cảnh báo quan trọng về tính chính xác: đó là ranh giới thế hệ, không phải là tinh chỉnh thời gian. Bất kỳ kế hoạch tích hợp vành / cơ sở ** sẽ ** tính đến nó và ** sẽ ** xác minh trên cơ sở mục tiêu thực tế.

## 6. Cấp phép Nền tảng / Bảng điều khiển

Dựa trên các mục cấp phép nền tảng trong [tools.md](./tools.md); tất cả các yêu cầu quyền sở hữu cụ thể được đánh dấu để xác minh vì các trang của nhà cung cấp không thể truy cập được trong môi trường đánh giá này (xem lưu ý về khả năng truy cập của báo cáo đánh giá).

| Mặt | Ghi chú | Trạng thái |
|---|---|---|
| PC | Đường dẫn tương thích rộng nhất. | Verified public |
| Xbox | Hỗ trợ Xbox thường được gắn với một bánh xe / trung tâm được cấp phép. | Xác minh với trang nền tảng Fanatec |
| PlayStation | Hỗ trợ PlayStation thường được gắn với một cơ sở được cấp phép. | Xác minh với trang nền tảng Fanatec |

## 7. Thúc đẩy Repos cộng đồng vào Ma trận này

[repos.md](./repos.md) yêu cầu kho nào nên được quảng bá thành ma trận tương thích chính thức sau khi xác minh băng ghế dự bị. Quy tắc: hỗ trợ thiết bị được yêu cầu của kho lưu trữ chuyển từ ** Báo cáo cộng đồng ** sang ** Đã xác minh ** trong ma trận này chỉ sau khi nó được sao chép trên băng ghế dự bị cho mỗi [tools.md](./tools.md) §5. Cho đến lúc đó, nó vẫn được dán nhãn báo cáo cộng đồng, với một liên kết đến nguồn.

## 8. Làm thế nào để mở rộng

Thêm một hàng chỉ với trạng thái rõ ràng. Các hàng "Xác minh" mới được chào đón dưới dạng danh sách việc cần làm, nhưng một ô ** sẽ không ** được đánh dấu Xác minh mà không có nguồn chính thức hoặc kết quả băng ghế dự bị được sao chép được ghi lại cùng với nó.

## Câu hỏi chưa được giải quyết

- Những sản phẩm cụ thể, chuyển đổi QR và dự án cộng đồng nào có thể được chuyển từ "Xác minh" / "Báo cáo cộng đồng" sang "Xác minh" sau khi thử nghiệm băng ghế dự bị và xác nhận với các nguồn chính thức trực tiếp?
