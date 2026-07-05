# Kiến trúc đầu dò xúc giác

> Phiên bản: 1.0
> Đánh giá: 2026-07-02
> Mục đích: xử lý các đầu dò xúc giác ("bass shakers") như một hệ thống rung động riêng biệt và giải quyết các câu hỏi cô lập và cộng hưởng được nêu trong [cockpits.md] (./cockpits.md) §6. Trả lời một phần của câu hỏi mở rộng trong [sim_racing_research.md] (./sim_racing_research.md) §13.

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Thay đổi |
|---|---|---|
| 1.0 | 2026-07-02 | Tài liệu mới. Trực tiếp giải quyết các câu hỏi mở về cách ly và cộng hưởng của bộ chuyển đổi xúc giác trong [cockpits.md](./cockpits.md) §6 và cổng kiểm tra cách ly trong [tools.md](./tools.md) §5. |

## 1. Mục đích

Đầu dò xúc giác chuyển đổi tín hiệu âm thanh / từ xa thành rung động tần số thấp qua ghế và khung gầm (động cơ ầm ầm, lề đường, khóa bánh xe, kết cấu đường). Tài liệu này định nghĩa chúng là ** hệ thống rung riêng ** không được làm hỏng phản hồi lực truyền động trực tiếp hoặc đọc cảm biến bàn đạp.

> [ QUAN TRỌNG]
> Các ràng buộc hướng dẫn từ [cockpits.md] (./cockpits.md): đầu dò xúc giác ** sẽ ** được coi là một hệ thống rung động riêng biệt và được cách ly và thử nghiệm để chúng không che giấu FFB hoặc chẩn đoán cảm biến.

## 2. Trách nhiệm

- Tái tạo hiệu ứng tần số thấp từ nguồn từ xa hoặc nguồn âm thanh.
- Cung cấp rung động cho người lái xe mà không cần bơm năng lượng phá hủy vào FFB hoặc đường dẫn cảm biến.
- Cho phép điều chỉnh trên mỗi hiệu ứng (kênh, dải tần số, mức).

## 3. Các loại đầu dò (Chung)

Như kiến thức chung ** đã được xác minh **, các đầu dò xúc giác dao động từ các bộ kích thích "puck" nhỏ được gắn vào ghế hoặc bảng điều khiển, đến các máy lắc công suất cao lớn hơn được gắn vào khung giàn khoan. Các đơn vị lớn hơn cung cấp nhiều năng lượng hơn cho cấu trúc - tăng cả cường độ hiệu ứng và nguy cơ can thiệp vào FFB và cảm biến.

## 4. Nguồn tín hiệu và Crossover

**Hình 4-1: Đường dẫn tín hiệu xúc giác **

```nàng tiên cá
Lưu đồ LR
Src ["Nguồn từ xa / âm thanh (ví dụ SimHub)"] -> Xover ["Low-Pass / Crossover"]
Xover -> Amp ["Bộ khuếch đại"]
Amp -> Trans ["Đầu dò xúc giác"]
Trans -> Ghế ["Ghế / Bảng / Khung"]
```

Nguồn thường là đường ống từ xa (xem [telemetry.md] (./telemetry.md)) hoặc kênh âm thanh tần số thấp chuyên dụng. Giai đoạn chéo / thông thấp ** sẽ ** hạn chế năng lượng cho dải tần số thấp dự định của đầu dò để nội dung tần số cao hơn không bị đổ vào cấu trúc.

! [Đầu dò xúc giác: giữ năng lượng trong dải tần số thấp] (./tactile_crossover.svg)

Giữ máy lắc bên trong dải thấp dự định của nó (màu xanh lá cây) là điều ngăn năng lượng của nó tổng hợp vào dải chi tiết FFB của bánh xe (màu tím) hoặc điều khiển cộng hưởng cấu trúc của giàn khoan (màu đỏ). Các tần số cộng hưởng chính xác là cụ thể của giàn khoan và phải được đo thay vì giả định - xem §6.

## 5. Cách ly cơ học (Buồng lái hạ thấp.md §6)

Câu hỏi mở trong [cockpits.md] (./cockpits.md) §6 hỏi làm thế nào các đầu dò nên được cách ly khỏi các cấu hình cấu trúc chính để tránh nhiễu phá hủy với FFB tần số cao DD. Như ** suy luận kỹ thuật ** phù hợp với hướng dẫn của tài liệu đó:

- Đầu dò ** nên ** được gắn vào ghế hoặc bảng điều khiển chuyên dụng thay vì cứng nhắc với đường dẫn tải FFB chính khi thực tế, do đó năng lượng của chúng không tổng hợp với mô-men xoắn FFB tại bánh xe.
- Trường hợp đầu dò phải gắn vào khung, các giá đỡ tuân thủ / cách ly ** nên ** được sử dụng để tách rung động của nó khỏi các cấu hình cấu trúc.
- Hệ thống xúc giác ** nên ** được vận hành và đo độc lập (xem §7) trước khi chạy cùng với FFB mô-men xoắn cao.

## 6. Tương tác cộng hưởng (Answering cockpits.md §6)

Câu hỏi mở thứ hai đặt ra là liệu các cấu trúc nhôm 40x80 mm tiêu chuẩn có tần số cộng hưởng phù hợp với tần số tín hiệu FFB phổ biến hay không và làm thế nào để làm ẩm chúng. Cơ sở nghiên cứu này không khẳng định tần số cộng hưởng đo cụ thể cho một giàn khoan nhất định - đó là ** Không rõ ** mà không cần đo trên cấu trúc thực tế. Phương pháp chính xác:

- Đo phản ứng cấu trúc của giàn khoan (ví dụ: bằng gia tốc kế và kích thích quét) để xác định vị trí cộng hưởng, theo kiểm tra cộng hưởng / cách ly xúc giác trong [tools.md] (./tools.md) §5.
- Tránh điều khiển năng lượng đầu dò vào một dải trùng với cộng hưởng cấu trúc hoặc dải hiệu ứng FFB chiếm ưu thế.
- Thêm giảm xóc (khối lượng, giằng hoặc gắn kết cách ly) trong đó cộng hưởng chồng lên dải hoạt động.

## 7. Gỡ lỗi và Vận hành thử

Ủy quyền cho hệ thống xúc giác một mình trước: quét tần số và mức độ, và xác nhận với gia tốc kế rằng năng lượng vẫn ở trong băng tần và không kích thích cộng hưởng cấu trúc. Sau đó bật FFB và xác nhận các đầu dò không che giấu chi tiết FFB hoặc làm nhiễu các chỉ số cảm biến bàn đạp. Xử lý bất kỳ thay đổi nào trong hiệu chuẩn bàn đạp hoặc sàn tiếng ồn FFB khi máy lắc hoạt động như một vấn đề khớp nối để khắc phục cơ học.

## 8. Quan điểm Firmware

Đầu ra xúc giác thường được điều khiển bởi phần mềm máy chủ cộng với bộ khuếch đại, do đó sự tham gia của phần sụn thiết bị là tối thiểu; khi sử dụng bộ điều khiển, nó ** sẽ ** giữ các hiệu ứng trong băng tần được định cấu hình và ** sẽ ** có trạng thái yên tĩnh được xác định khi nguồn của nó bị mất (xem [telemetry.md] (./telemetry.md) §9).

# 9. Key Takeaways

- Đầu dò xúc giác là một hệ thống rung riêng biệt, không phải là một phần của đường dẫn FFB.
- Cô lập chúng (gắn ghế / bảng điều khiển, gắn kết tuân thủ) để chúng không tổng hợp với FFB hoặc làm phiền cảm biến.
- Cộng hưởng cấu trúc là cụ thể cho giàn khoan và ** Không rõ ** cho đến khi được đo; xác định vị trí chúng, sau đó giữ năng lượng ra khỏi dải đó.
- Vận hành xúc giác một mình, sau đó xác minh không có nhiễu với FFB và bàn đạp.

## Tài liệu tham khảo

- [cockpits.md](./cockpits.md) - cấu trúc và các câu hỏi cách ly / cộng hưởng ban đầu.
- [telemetry.md](./telemetry.md) - nguồn từ xa / âm thanh và hành vi an toàn yên tĩnh.
- [tools.md](./tools.md) - kiểm tra cách ly cộng hưởng / đầu dò xúc giác.

## Câu hỏi chưa được giải quyết

- Các tần số cộng hưởng đo được của cấu trúc giàn khoan mục tiêu là gì và kết quả là phải tránh hoặc giảm các dải hiệu ứng nào?
