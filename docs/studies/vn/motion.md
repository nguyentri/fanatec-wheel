# Kiến trúc nền tảng chuyển động

> Phiên bản: 1.0
> Đánh giá: 2026-07-02
> Mục đích: giới thiệu nền tảng chuyển động (giàn cueing chuyển động) như một hệ thống con, ở cấp độ kiến trúc. Trả lời một phần của câu hỏi mở rộng trong [sim_racing_research.md] (./sim_racing_research.md) §13.

## Nhật ký thay đổi tài liệu

| Phiên bản | Ngày | Thay đổi |
|---|---|---|
| 1.0 | 2026-07-02 | Tài liệu mới. Xử lý cấp kiến trúc dựa trên dự án `VNM_MOTION_CONTROLLER` từ [repos.md](./repos.md) và mô hình an toàn hệ sinh thái trong [sim_racing_research.md](./sim_racing_research.md). |

## 1. Mục đích

Một nền tảng chuyển động vật lý di chuyển buồng lái để báo hiệu hệ thống tiền đình của người lái - truyền đạt tăng tốc, phanh và kết cấu đường mà một giàn khoan tĩnh không thể. Tài liệu này xác định ranh giới kiến trúc và trên hết là các yêu cầu an toàn; nó không chỉ định nội bộ của một sản phẩm cụ thể.

> [ QUAN TRỌNG]
> Nền tảng chuyển động di chuyển một người dưới quyền lực. Các yêu cầu an toàn trong §6 không phải là tùy chọn và được ưu tiên hơn sự trung thực.

## 2. Trách nhiệm

- Nhận được một nguồn chuyển động (gia tốc từ xa có nguồn gốc, hoặc đầu ra chuyển động của trò chơi).
- Áp dụng tín hiệu chuyển động (tỷ lệ, lọc và rửa) để phù hợp với hành trình truyền động thực.
- Thiết bị truyền động chỉ huy trong phạm vi di chuyển, vận tốc và giới hạn lực.
- Phát hiện các lỗi và đưa nền tảng đến trạng thái an toàn trên bất kỳ sự bất thường nào.

## 3. Mức độ tự do (Chung)

Như ** đã được xác minh công khai ** kiến thức chung, sở thích và nền tảng prosumer thường được mô tả bởi mức độ tự do được kích hoạt của chúng: heave (dọc), pitch, roll, surge, sway và yaw. Các nền tảng nhỏ thường kích hoạt một tập hợp con (ví dụ: pitch and roll); giàn khoan lớn hơn thêm nhiều. DOF chính xác và hình học là nền tảng cụ thể.

![Sáu mức độ tự do của một nền tảng chuyển động](../assets/motion_dof.svg)

Mỗi trục cho thấy một cảm giác khác nhau: tăng tốc truyền tải gia tốc và phanh, lắc lư truyền tải góc, nặng truyền tải va chạm và đỉnh, trong khi pitch (mũi lên / xuống), cuộn (nghiêng vào góc) và yaw (xoay, ví dụ như sự khởi đầu của một spin) là ba vòng quay. Một chiến lược cueing chọn cái nào trong số này một nền tảng nhất định có thể hiển thị và mạnh mẽ như thế nào.

## 4. Kiến trúc điều khiển

**Hình 4-1: Đường dẫn điều khiển chuyển động **

```nàng tiên cá
Lưu đồ LR
Tele ["Telemetry / Motion Source"] -> Cue ["Motion Cueing (quy mô, bộ lọc, rửa trôi)"]
Cue -> Ctl ["Bộ điều khiển chuyển động (MCU)"]
Ctl -> Drv ["Trình điều khiển thiết bị truyền động"]
Drv -> Act ["Thiết bị truyền động"]
Hành động --> Cảm giác ["Vị trí / Cảm giác giới hạn"]
Cảm giác -> Ctl
Estop ["E-Stop"] -> Ctl
```

Các bộ điều khiển DIY cộng đồng tồn tại cho lớp phần cứng này: `VNM_MOTION_CONTROLLER` được ghi lại dưới dạng phần sụn dựa trên STM32F401RCT và bộ cấu hình để xây dựng phần cứng DIY bao gồm các giàn chuyển động (xem [repos.md] (./repos.md)). Đây là ** bằng chứng thực hiện cộng đồng **, không phải là thiết kế tham chiếu.

## 5. Chuyển động Cueing (Chung)

Chuyển động cueing bản đồ tăng tốc ảo lớn vào du lịch vật lý nhỏ. Như ** kỹ thuật suy luận ** từ thực hành mô phỏng chuyển động tiêu chuẩn, một giai đoạn cueing quy mô đầu vào, áp dụng lọc, và sử dụng một ** washout ** bộ lọc trả về thiết bị truyền động về phía trung tâm sau khi một đầu vào duy trì để nền tảng không đạt giới hạn du lịch của nó. Điều chỉnh ngành nghề sức mạnh cue chống lại du lịch có sẵn.

## 6. Yêu cầu an toàn

Đây là bắt buộc và phù hợp với tư thế an toàn trong [sim_racing_research.md](./sim_racing_research.md) và [tools.md](./tools.md) (E-stop / fault-injection).

- Một phần cứng ** E-stop sẽ ** có mặt và ** sẽ ** loại bỏ sức mạnh thiết bị truyền động độc lập với trạng thái firmware.
- Các giới hạn cứng về di chuyển, vận tốc và lực ** phải ** được thực thi và ** không ** có thể ghi đè lên người dùng vào một phạm vi không an toàn.
- Cảm biến vị trí / giới hạn ** sẽ ** được xác nhận; mất cảm biến ** sẽ ** dừng lại an toàn.
- Trên bất kỳ lỗi nào (mất cảm biến, thời gian chờ lệnh, ngoài phạm vi), bộ điều khiển ** sẽ ** đưa nền tảng đến trạng thái an toàn được xác định và chốt cho đến khi thiết lập lại được xác thực.
- Hệ thống ** sẽ không ** thực hiện bỏ qua các khóa liên động này. Kiểm tra chuyển động toàn năng ** sẽ ** theo gating HIL / lỗi tiêm trong [tools.md] (./tools.md) §5.

## 7. Giao diện truyền thông

Bộ điều khiển nhận dữ liệu chuyển động từ đường ống từ xa (xem [telemetry.md] (./telemetry.md)) qua USB / nối tiếp hoặc mạng và ra lệnh cho trình điều khiển bộ truyền động qua giao diện dành riêng cho trình điều khiển (PWM, bước / hướng hoặc bus điều khiển động cơ). Giao diện ** sẽ ** sử dụng các tin nhắn bị chặn, được kiểm tra độ dài với cơ quan giám sát thời gian chờ lệnh.

## 8. Chiến lược gỡ lỗi

Đưa lên chống lại nguồn cung cấp giới hạn hiện tại và, nếu có thể, không tải; xác minh E-stop và giới hạn xử lý * trước * gắn vào buồng lái; đo độ trễ lệnh chuyển động; và xác nhận rửa trôi giữ cho bộ truyền động tắt các điểm dừng cuối của chúng dưới đầu vào duy trì.

## 9. Quan điểm Firmware

Bộ điều khiển chuyển động là một hệ thống truyền động thời gian thực với một người trong vòng lặp. Nó ** sẽ ** coi nguồn chuyển động là không đáng tin cậy, thực thi các giới hạn trong phần sụn độc lập với nguồn và không an toàn. Chất lượng cueing là thứ yếu để không bao giờ vượt quá một phong bì an toàn.

## 10. Key Takeaways

- Nền tảng chuyển động báo hiệu tăng tốc thông qua du lịch vật lý bị chặn; báo hiệu cộng với rửa làm cho điều này có thể.
- An toàn (E-stop, giới hạn cứng, dừng an toàn do lỗi) là bắt buộc và không thể bỏ qua.
- Bộ điều khiển DIY tồn tại (`VNM_MOTION_CONTROLLER`) làm bằng chứng cộng đồng, không phải thiết kế tham chiếu.
- Nguồn chuyển động đến từ đường ống đo từ xa; coi đó là không đáng tin cậy.

## Tài liệu tham khảo

- [vnmsimulation/VNM_MOTION_CONTROLLER](https://github.com/vnmsimulation/VNM_MOTION_CONTROLLER) - Bộ điều khiển chuyển động / phần cứng dựa trên DIY STM32.
- [telemetry.md] (./telemetry.md) - đường ống nguồn chuyển động.
- [tools.md] (./tools.md) - Đồ đạc HIL và gating tiêm lỗi.
- [cockpits.md](./cockpits.md) - cấu trúc một nền tảng di chuyển.

## Câu hỏi chưa được giải quyết

- Giới hạn DOF, lớp thiết bị truyền động và giới hạn di chuyển sẽ nằm trong phạm vi nào và các tiêu chí chấp nhận cho độ trễ và thời gian dừng an toàn trên phần cứng mục tiêu là gì?
