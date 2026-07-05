# Sim Racing Chỉ số nghiên cứu

> Phiên bản: 1.2
> Đánh giá: 2026-07-05

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Thay đổi |
|---|---|---|
| 1.2 | 2026-07-05 | Giải thích có hệ thống + thông qua minh họa. Đã thêm hình minh họa SVG gốc cho các khái niệm vật lý / điện tử trên các tài liệu hệ thống con (biến tần ba pha, thời gian PWM / ADC, giảm nhiệt, tiết diện servo, cảm biến tế bào tải / Hall / chiết áp, độ phân giải ADC, bóng ma trận nút, cổng mẫu H, khớp nối phóng nhanh, uốn cong buồng lái, chuyển động 6-DOF, ngăn xếp liên lạc, độ trễ từ xa, giao thoa xúc giác), mỗi hình minh họa đi kèm với giải thích bằng ngôn ngữ đơn giản. Hình minh họa là sơ đồ gốc, không phải tác phẩm nghệ thuật của nhà sản xuất. |
| 1.1 | 2026-07-02 | Đã thêm tiêu đề phiên bản và nhật ký thay đổi; thêm năm tài liệu hệ thống con mới hơn (đo từ xa, xúc giác, chuyển động, ma trận tương thích, giao thức truyền thông) vào đường dẫn đọc và bản đồ phụ thuộc; chú thích sơ đồ bản đồ phụ thuộc. |

Thư mục này là một bản đồ nghiên cứu định hướng nhà phát triển cho phần cứng và phần sụn mô phỏng đua xe. Nó phân tách các sự kiện công khai, bằng chứng cộng đồng và các khuyến nghị kỹ thuật để công việc triển khai có thể tiến hành mà không giả định nội bộ Fanatec độc quyền.

## Đường dẫn đọc được đề xuất

| Bước | Đọc | Kết quả |
|---|---|---|
| 1 | [glossary.md](./glossary.md) | Tìm hiểu thuật ngữ sản phẩm, từ viết tắt, nhãn tương thích và từ ngữ an toàn với khách hàng. |
| 2 | [sim_racing_research.md](./sim_racing_research.md) | Tìm hiểu hệ sinh thái, mô hình an toàn, đường dẫn FFB và quyền sở hữu hệ thống con. |
| 3 | [wheel_base.md](./wheel_base.md) | Hiểu được trung tâm an toàn quan trọng: USB / PID, điều khiển động cơ, trọng tài mô-men xoắn, cập nhật, chẩn đoán. |
| 4 | [wheel_rim.md](./wheel_rim.md) | Hiểu các nút I / O quay, liên kết QR, nhận dạng vành, quét đầu vào, hiển thị và ranh giới tạo. |
| 5 | [pedals.md](./pedals.md) | Hiểu chuỗi cảm biến, hiệu chuẩn, USB HID và ủy nhiệm cổng cơ sở. |
| 6 | [add_ons.md](./add_ons.md) | Hiểu shifters và handbrakes như thiết bị đầu vào rời rạc hoặc analog. |
| 7 | [accessories.md](./accessories.md) | Hiểu các bản phát hành nhanh, bảng điều khiển, hiển thị từ xa và hộp nút. |
| 8 | [cockpits.md](./cockpits.md) | Hiểu khung gầm cơ khí bảo tồn FFB và độ trung thực tín hiệu đạp. |
| 9 | [tools.md](./tools.md) | Chọn tiêu chuẩn, phần mềm, công cụ đo lường và tài liệu tham khảo xác nhận. |
| 10 | [repos.md](./repos.md) | Kiểm tra các dự án công cộng; coi chúng là bằng chứng cộng đồng, không phải thông số kỹ thuật chính thức. |
| 11 | [telemetry.md](./telemetry.md) | Hiểu các trò chơi -> cầu -> thiết bị đo từ xa đường ống. |
| 12 | [tactile.md](./tactile.md) | Hiểu đầu dò xúc giác là một hệ thống rung riêng biệt được phân lập từ FFB. |
| 13 | [motion.md](./motion.md) | Hiểu nền tảng chuyển động, cueing chuyển động, và phong bì an toàn bắt buộc. |
| 14 | [communication-protocols.md](./communication-protocols.md) | Hiểu ngăn xếp giao thức lớp và cách các công cụ phần mềm tiếp cận thiết bị. |
| 15 | [compatibility-matrix.md](./compatibility-matrix.md) | Phân tách các đường dẫn USB-direct với base-proxy, QR generation và platform, với trạng thái xác minh. |

## Bản đồ phụ thuộc hệ thống con

**Hình 1-1: Bản đồ phụ thuộc hệ thống con **

```nàng tiên cá
Sơ đồ TD
Glossary[glossary.md] --> Hệ sinh thái[sim_racing_research.md]
Hệ sinh thái --> Base[wheel_base.md]
Hệ sinh thái --> Rim[wheel_rim.md]
Hệ sinh thái --> Bàn đạp[pedals.md]
Hệ sinh thái --> AddOns[add_ons.md]
Hệ sinh thái --> Phụ kiện[accessories.md]
Hệ sinh thái --> Buồng lái[cockpits.md]
Cơ sở -> Rim
Cơ sở -> Bàn đạp
Cơ sở -> AddOns
Rim -> Phụ kiện
Công cụ [tools.md] -> Cơ sở
Công cụ --> Rim
Công cụ --> Bàn đạp
Repos[repos.md] --> Cơ sở
Repos -> vành
Repos -> Bàn đạp
Repos -> AddOns
Phụ kiện -> Telemetry [telemetry.md]
Buồng lái --> Tactile[tactile.md]
Từ xa -> Tactile
Từ xa -> Chuyển động [motion.md]
Hệ sinh thái --> Chuyển động
Cơ sở --> Comms[communication-protocols.md]
Comms -> Vành
Bàn đạp --> Compat[compatibility-matrix.md]
Repos -> Compat
```

## Mô hình bằng chứng

| Nhãn | Ý nghĩa | Nguồn ưa thích |
|---|---|---|
| Hành vi công khai đã được xác minh | Sản phẩm hoặc hành vi tiêu chuẩn được ghi lại công khai | Thông số kỹ thuật USB-IF, hướng dẫn sử dụng nhà sản xuất, trang hỗ trợ |
| Thực hiện cộng đồng | Thực hiện công khai hoặc tài liệu làm việc | Kho GitHub, wiki dự án |
| Suy luận kỹ thuật | Kết luận thiết kế hợp lý từ bằng chứng công cộng | Nhiều nguồn cộng với thực hành hệ thống nhúng / điều khiển |
| Không rõ | Không đủ công khai để yêu cầu | Yêu cầu sơ đồ, BOM, dấu vết, mô tả hoặc đặc tả nhà cung cấp được phê duyệt |

> ** Trên các hình minh họa (đã thêm v1.2). ** Các sơ đồ SVG được thêm vào trong đèo này là bản gốc, minh họa giảng dạy sơ đồ về các nguyên tắc kỹ thuật chung (xây dựng động cơ, đảo ngược ba pha, thời gian PWM, cầu đo căng thẳng, mã hóa tứ giác, ma trận nút, v.v.). Chúng là ** không ** bản sao của bất kỳ sơ đồ hoặc tác phẩm nghệ thuật sản phẩm nào của nhà sản xuất và chúng mô tả các khái niệm chung hơn là nội bộ của bất kỳ sản phẩm cụ thể nào. Trong trường hợp một giá trị sẽ là sản phẩm cụ thể (số cực, tốc độ PWM, độ phân giải, tần số cộng hưởng), hình minh họa được dán nhãn là minh họa và văn bản trì hoãn đo lường hoặc thông số kỹ thuật được phê duyệt. Họ ngồi ở mức độ tin cậy "suy luận kỹ thuật / xác minh kiến thức chung công cộng", không phải "hành vi sản phẩm đã được xác minh".

## Quy tắc an toàn và phạm vi

- Không bao gồm firmware bị rò rỉ, sơ đồ bí mật, nhị phân độc quyền, thông tin đăng nhập hoặc tài liệu hỗ trợ riêng.
- Không trình bày các dự án cộng đồng tương thích Fanatec như các thông số kỹ thuật giao thức Fanatec chính thức.
- Không bỏ qua xác thực bảng điều khiển, giới hạn mô-men xoắn, phím an toàn hoặc bảo vệ phần sụn.
- Xử lý thử nghiệm động cơ mô-men xoắn cao là nguy hiểm cho đến khi vô hiệu hóa cổng độc lập và xử lý lỗi được xác minh.

## Hub tham chiếu

- [Thông số kỹ thuật và công cụ USB-IF HID] (https://www.usb.org/hid)
- [USB-IF PID Lớp 1.0] (https://www.usb.org/sites/default/files/documents/pid1_01.pdf)
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf)
- [OpenFFBoard wiki] (https://github.com/Ultrawipf/OpenFFBoard/wiki/)
- [hid-fanatecff trình điều khiển Linux] (https://github.com/gotzl/hid-fanatecff)
- [SimHub wiki] (https://github.com/SHWotever/SimHub/wiki)

## Câu hỏi chưa được giải quyết

- Không.
