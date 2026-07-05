# Ma trận tương thích

> Phiên bản: 1.0
> Đánh giá: 2026-07-02
> Mục đích: cung cấp cấu trúc của ma trận tương thích bàn đạp / bánh xe / QR, theo yêu cầu trong [pedals.md](./pedals.md) §8.3 và [repos.md](./repos.md). Nó tách các trục tương thích và ghi lại trạng thái xác minh trên mỗi ô; nó **không** khẳng định các tuyên bố về từng sản phẩm chưa được xác minh.

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Thay đổi |
|---|---|---|
| 1.0 | 2026-07-02 | Tài liệu mới. Khung được tạo theo phần tiếp theo về ma trận bàn đạp trong [pedals.md](./pedals.md) §8.3 và câu hỏi "đưa các repos thành ma trận tương thích chính thức sau khi xác minh trên băng thử nghiệm" trong [repos.md](./repos.md). Các ô chỉ được điền khi cơ sở nghiên cứu này đã thiết lập một thực tế; tất cả những ô khác được đánh dấu để xác minh. |

## 1. Mục đích và phương pháp

> [!IMPORTANT]
> Các tuyên bố về khả năng tương thích chính xác là loại thực tế mà cơ sở nghiên cứu này từ chối khẳng định nếu không có bằng chứng. Tài liệu này là một **khung**: mọi mục nhập đều mang trạng thái **Đã xác minh công khai**, **Báo cáo từ cộng đồng** hoặc **Xác minh** (chưa được thiết lập ở đây). Chỉ điền vào các ô "Xác minh" sau khi xác nhận với một nguồn chính thức hoặc một thử nghiệm trên băng thử nghiệm.

Giá trị của một ma trận là tách các trục thường bị gộp chung. Tài liệu này xác định các trục đó và chỉ điền vào những gì đã được hỗ trợ.

## 2. Các trục tương thích

| Trục | Câu hỏi cần trả lời |
|---|---|
| Đường dẫn kết nối | USB HID trực tiếp, hay thông qua cổng của wheel base (proxy)? |
| Thế hệ QR | QR1 vs QR2; Biến thể Base-Side vs Wheel-Side. |
| Nền tảng/bảng điều khiển | PC, Xbox, PlayStation — và nơi đặt giấy phép/quyền sở hữu. |
| Thế hệ thiết bị | Thiết bị ngoại vi hoặc giao thức có phụ thuộc vào một thế hệ base nào không? |

## 3. Đường dẫn kết nối (USB trực tiếp vs Base-Proxy)

Đây là sự phân tách mà [pedals.md](./pedals.md) §8.3 yêu cầu rõ ràng.

| Đường dẫn | Hành vi | Trạng thái |
|---|---|---|
| Trực tiếp qua USB HID | Bàn đạp/thiết bị ngoại vi liệt kê như là thiết bị HID riêng của chúng cho máy chủ. | Đã xác minh công khai (xem [pedals.md](./pedals.md)) |
| Proxy cổng wheel base | Bàn đạp/thiết bị ngoại vi kết nối với một cổng trên base (ví dụ: RJ12); base tổng hợp và báo cáo chúng. | Đã xác minh công khai (xem [pedals.md](./pedals.md)) |
| Console thông qua base | Hỗ trợ Console thường đạt được *thông qua* một base tương thích thay vì cổng USB trực tiếp. | Báo cáo từ cộng đồng / Xác minh với tài liệu nền tảng chính thức |

ID nhà cung cấp USB Fanatec được quan sát bởi driver Linux `hid-fanatecff` là `0x0EB7` (xem [repos.md](./repos.md)) — hữu ích khi xác định một thiết bị USB trực tiếp, và được dán nhãn là một quan sát từ driver thay vì một tuyên bố chính thức.

## 4. Thế hệ QR

Dựa trên hướng dẫn chuyển đổi QR2 được trích dẫn trong [accessories.md](./accessories.md) và cách xử lý QR trong [wheel_rim.md](./wheel_rim.md).

| Khía cạnh | Ghi chú | Trạng thái |
|---|---|---|
| QR1 vs QR2 | Các thế hệ khác biệt; không phải tất cả các sản phẩm đều chuyển đổi được và việc chuyển đổi có thể phụ thuộc vào từng mô hình. | Đã xác minh công khai (Hướng dẫn chuyển đổi Fanatec QR2) |
| Base-Side vs Wheel-Side | QR2 có các biến thể Base-Side và Wheel-Side phải được khớp với nhau. | Đã xác minh công khai (cùng nguồn) |
| Phù hợp mô-men xoắn | Mô-men xoắn DD cao hơn làm tăng nhu cầu cơ học đối với khớp nối. | Suy luận kỹ thuật (xem [accessories.md](./accessories.md)) |
| Danh sách chuyển đổi mô hình cụ thể | Những sản phẩm cụ thể nào có thể chuyển đổi sang QR2. | Xác minh với bài viết chuyển đổi Fanatec đang hiển thị |

## 5. Ranh giới Thế hệ Thiết bị / Giao thức

Ranh giới được thiết lập rõ ràng nhất trong cơ sở nghiên cứu này, từ [wheel_rim.md](./wheel_rim.md) và README `Fanatec-Wheel-Barebone-Emulator` (đã xác minh):

| Trường hợp | Kết quả được báo cáo | Trạng thái |
|---|---|---|
| Các emulator vành AVR SPI của cộng đồng trên các base cũ (qua CSL DD, DD1/DD2) | Báo cáo là hoạt động | Báo cáo từ cộng đồng |
| Các emulator vành AVR SPI của cộng đồng trên ClubSport DD / DD+ | Báo cáo là **không** hoạt động | Báo cáo từ cộng đồng (cảnh báo từ upstream emulator) |

> [!NOTE]
> Đây là một cảnh báo quan trọng về tính đúng đắn: nó là ranh giới thế hệ, không phải là sự tinh chỉnh thời gian. Bất kỳ kế hoạch tích hợp vành/base nào cũng **phải** tính đến điều này và **phải** xác minh trên base mục tiêu thực tế.

## 6. Cấp phép Nền tảng / Console

Dựa trên các mục cấp phép nền tảng trong [tools.md](./tools.md); tất cả các xác nhận quyền sở hữu cụ thể được đánh dấu để xác minh vì các trang của nhà cung cấp không thể truy cập được trong môi trường đánh giá này (xem ghi chú về khả năng truy cập trong báo cáo đánh giá).

| Khía cạnh | Ghi chú | Trạng thái |
|---|---|---|
| PC | Đường dẫn tương thích rộng nhất. | Đã xác minh công khai |
| Xbox | Hỗ trợ Xbox thường được gắn với một wheel/hub được cấp phép. | Xác minh với trang nền tảng Fanatec |
| PlayStation | Hỗ trợ PlayStation thường được gắn với một base được cấp phép. | Xác minh với trang nền tảng Fanatec |

## 7. Đưa các Repos của Cộng đồng vào Ma trận này

[repos.md](./repos.md) hỏi những kho lưu trữ nào nên được đưa vào ma trận tương thích chính thức sau khi xác minh trên băng thử nghiệm. Quy tắc: hỗ trợ thiết bị được tuyên bố của một repo chuyển từ **Báo cáo từ cộng đồng** sang **Đã xác minh** trong ma trận này chỉ sau khi nó được tái tạo trên băng thử nghiệm theo [tools.md](./tools.md) §5. Cho đến lúc đó, nó vẫn được dán nhãn là báo cáo từ cộng đồng, với một liên kết đến nguồn.

## 8. Cách mở rộng

Chỉ thêm một hàng có trạng thái rõ ràng. Các hàng "Xác minh" mới được hoan nghênh dưới dạng danh sách việc cần làm, nhưng một ô **không được** đánh dấu là Đã xác minh nếu không có nguồn chính thức hoặc kết quả tái tạo trên băng thử nghiệm được ghi lại cùng với nó.

## Các câu hỏi mở để nhà phát triển tự điều tra

Đã đánh giá 2026-07-05. Đây là một câu hỏi về quy trình/xác minh không có câu trả lời tĩnh — việc thăng cấp phụ thuộc vào công việc thử nghiệm mà người đọc thực hiện.

- **Những sản phẩm cụ thể, các chuyển đổi QR và dự án cộng đồng nào có thể chuyển từ "Xác minh" / "Báo cáo từ cộng đồng" sang "Đã xác minh" sau khi thử nghiệm trên băng thử nghiệm?**
  *Cách điều tra:* áp dụng mô hình bằng chứng trong [`README.md`](./README.md). Thăng cấp một mục lên **Hành vi công khai đã được xác minh** chỉ khi nó (a) được xác nhận với một nguồn chính thức đang hoạt động (hướng dẫn sử dụng của nhà sản xuất/trang hỗ trợ/trang sản phẩm) *và* được kiểm tra lại về tính hiện hành, hoặc (b) được tái tạo trên băng thử nghiệm với phần cứng thực tế và được ghi lại (thiết lập, phiên bản firmware, ngày tháng, phép đo). Các mục được cộng đồng báo cáo (ví dụ: USB IDs `0EB7:xxxx` từ `hid-fanatecff`, sơ đồ chân RJ12, các tuyên bố chuyển đổi QR) vẫn là "Báo cáo từ cộng đồng" cho đến khi một trong hai điều kiện đó được đáp ứng. Giữ một nhật ký có ghi ngày tháng để các mục có thể được xác minh lại khi firmware hoặc dòng sản phẩm thay đổi.
