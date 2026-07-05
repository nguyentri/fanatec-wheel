# Sim Racing Buồng lái: Kiến trúc cơ khí & Độ cứng

> Ngày nghiên cứu: 2026-07-02
> Mô hình bằng chứng: sản phẩm công cộng / thông tin hướng dẫn sử dụng cộng với suy luận kỹ thuật. Khuyến nghị độ cứng buồng lái là hướng dẫn thiết kế, không phải là yêu cầu của nhà cung cấp phổ quát.
> Tài liệu liên quan: [sim_racing_research.md](./sim_racing_research.md), [wheel_base.md](./wheel_base.md), [pedals.md](./pedals.md).

## 1. Giới thiệu

Buồng lái đua xe mô phỏng đóng vai trò là mặt phẳng nối đất cơ học cho tất cả các đầu vào và đầu ra hệ thống của người dùng. Đối với một kỹ sư hệ thống nhúng, buồng lái có thể được xem như khung gầm cấu trúc chứa các bộ truyền động chính (chiều dài cơ sở truyền động trực tiếp) và cảm biến (bàn đạp Load Cell). Bất kỳ độ cong cơ học nào trong khung gầm này hoạt động như một bộ lọc thông thấp ngoài ý muốn cho tín hiệu Phản hồi lực (FFB) và đưa tiếng ồn vào các chỉ số áp suất phanh. Tài liệu này nêu chi tiết các tiêu chuẩn kiến trúc, yêu cầu độ cứng và phương pháp tích hợp cho buồng lái đua xe bằng nhôm ép đùn (8020).

## 2. Tổng quan về kiến trúc hệ thống

Các thiết bị đua xe mô phỏng độ trung thực cao hiện đại dựa trên các cấu hình nhôm ép đùn khe chữ T mô-đun. Cách tiếp cận này cung cấp khả năng điều chỉnh vô hạn và tỷ lệ độ cứng trên trọng lượng to lớn. Kiến trúc cô lập các điểm căng thẳng khác nhau bằng cách phân phối chúng trên một khung cơ sở cứng nhắc.

**Hình 2-1: Hệ thống phân cấp thành phần buồng lái tiêu chuẩn **

```nàng tiên cá
đồ thị TD
RigBase [Khung cơ sở chính] -> WheelUprights [Wheelbase Uprights]
RigBase -> PedalDeck [Bàn đạp]
RigBase -> SeatCrossmembers [Thành viên chéo ghế]
    
WheelUprights -> DDMount [Ổ đĩa trực tiếp gắn kết]
PedalDeck -> LCPedals [Tải bàn đạp di động]
SeatCrossmembers -> Chân đế ghế [Giá đỡ ghế / Thanh trượt]
Chân đế -> DriverSeat [Driver Seat]
```

Khung cơ sở là vòng lặp cấu trúc nền tảng. Tất cả các cấu trúc thứ cấp (thẳng đứng, sàn và thành viên chéo) phân nhánh khỏi cơ sở này để hỗ trợ trình điều khiển và phần cứng.

## 3. Độ cứng kết cấu và độ trung thực tín hiệu

Cấu trúc flex giới thiệu tổn thất ký sinh trùng cho hệ thống. Hiểu cách flex ảnh hưởng đến cả độ trung thực đầu ra và tính nhất quán đầu vào là rất quan trọng để thiết kế một giàn khoan hiệu suất cao.

! [Cockpit rigidity: nơi lực đi trong một giàn khoan cứng so với linh hoạt] (./cockpit_flex_forces.svg)

Hình minh họa làm cho ý tưởng cốt lõi cụ thể: trong một giàn khoan cứng, mô-men xoắn FFB của chiều dài cơ sở và lực phanh của người lái chuyển gần như hoàn toàn sang tay và chân. Trong một giàn khoan linh hoạt, một phần năng lượng đó uốn cong khung thay vào đó - người đứng thẳng nghiêng dưới mô-men xoắn và bàn đạp di chuyển trở lại dưới phanh. Năng lượng hấp thụ đó chính xác là chi tiết mà người lái xe mất: bánh xe cảm thấy mềm hoặc bị trì hoãn, và phanh trở nên khó lặp lại.

3.1. Động lực mô-men xoắn trực tiếp

Chiều dài cơ sở truyền động trực tiếp (DD) kết hợp một động cơ servo lớn trực tiếp với vô lăng, có khả năng tạo ra các gai mô-men xoắn thoáng qua vượt quá 20Nm. Những động cơ này hoạt động với băng thông cao để cung cấp kết cấu đường chi tiết và phản hồi góc trượt.

Nếu chiều dài cơ sở thẳng đứng uốn cong, cấu trúc cơ học hấp thụ các FFB tần số cao thoáng qua.

* Chiều cao cơ sở ** sẽ ** được xây dựng từ cấu hình không nhỏ hơn 40x80mm để chống lại uốn cong xoắn.
* Các khung gắn kết nối các uprights với khung cơ sở ** nên ** sử dụng miếng đệm góc hạng nặng và tấm bánh sandwich để loại bỏ chơi.
* Gắn cột lái ** sẽ ** đảm bảo độ lệch ngang hoặc dọc bằng mô-men xoắn vận hành cực đại.

### 3.2. Lực lệch phanh tế bào tải

Không giống như các chiết áp tiêu chuẩn đo chuyển vị góc, bàn đạp tế bào tải đo lực vật lý thực tế áp dụng cho mặt phanh (thường lên đến hoặc vượt quá 100kg lực). Điều này bắt chước áp suất thủy lực trong một chiếc xe đua thực sự, dựa vào bộ nhớ cơ bắp của con người rất nhạy cảm với áp suất nhưng kém ở khoảng cách đo.

Khi người lái xe tác dụng 100kg lực, bất kỳ lực uốn cong ngược nào trong bàn đạp hoặc ghế của người lái sẽ tạo ra "mất năng lượng" và thay đổi vị trí không gian của bàn đạp so với người lái. Điều này làm thay đổi động tỷ lệ áp suất trên chuyển vị, phá hủy tính nhất quán đầu vào trong các giai đoạn phanh đường mòn quan trọng.

* Bàn đạp và hệ thống lắp ghế ** sẽ ** vẫn hoàn toàn cứng nhắc dưới lực dọc tối thiểu 100kg.
* Độ cứng bàn đạp tế bào tải ** nên ** được điều chỉnh thông qua chất đàn hồi hoặc lò xo để cho phép di chuyển tối thiểu, có thể dự đoán mà không gây ra uốn cong khung gầm.

## 4. Thành phần Kích thước và Thông số kỹ thuật

Cấu hình nhôm được phân loại theo kích thước mặt cắt ngang của chúng. Lựa chọn đúng là rất quan trọng để đáp ứng các yêu cầu về độ cứng mà không có chi phí không cần thiết.

| Kích thước hồ sơ | Ứng dụng chính | Xếp hạng độ cứng kết cấu | Yêu cầu sử dụng |
|--------------|---------------------|----------------------------|-------------------|
** 40x40mm ** | Phụ kiện, giá đỡ màn hình | Thấp ** Không ** được sử dụng cho các đường dẫn tải cấu trúc chính. |
** 40x80mm ** | Khung cơ sở, Uprights, Pedal Deck | Cao ** Sẽ ** là tiêu chuẩn tối thiểu cho khung gầm chính và uprights. |
| ** 40x120mm + ** | Uprights hạng nặng, xây dựng thẩm mỹ | Cực | ** Có thể ** được sử dụng cho các cơ sở ổ đĩa trực tiếp mô-men xoắn cực cao (20Nm +). |

## 5. Tích hợp ghế và công thái học

Kiến trúc lắp ghế phải thu hẹp khoảng cách bên giữa các đường ray cơ sở chính trong khi chứa các trình điều khiển có kích cỡ khác nhau. Tính mô đun đạt được thông qua cách tiếp cận nhiều lớp của các thành viên chéo và đường ray trượt.

** Hình 5-1: Kiến trúc lắp đặt ghế và đường dẫn tải **

```nàng tiên cá
đồ thị LR
MainRails [40x80mm Base Rails] -> Crossmembers [40x40mm Cross-thành viên]
Crossmembers -> Thanh trượt [Ghế trượt]
Thanh trượt -> Chân đế [Chân đế bên FIA]
Chân đế -> Ghế [Racing Bucket Seat]
```

* Hệ thống ** sẽ ** cung cấp một giao diện lắp đặt an toàn, phẳng để ngăn chặn ràng buộc trong đường ray trượt.
* Ghế ngồi chéo thành viên ** nên ** kéo dài chiều rộng chính xác giữa các kênh bên trong của đường ray cơ sở.
* Nếu ghế ô tô được sử dụng, miếng đệm hoặc tấm bộ chuyển đổi ** có thể ** được yêu cầu để tuôn ra gắn đường ray ghế nhà máy không đồng đều với các cấu hình nhôm phẳng.

# 5.1 Ví dụ lắp đặt hệ sinh thái

Hướng dẫn buồng lái ClubSport GT công cộng của Fanatec tuyên bố rằng tấm đáy tiêu chuẩn của nó hỗ trợ các cơ sở Fanatec hiện tại bao gồm CSL DD, Gran Turismo DD Pro, ClubSport DD / DD + và Podium DD2 QR2. Núi trước ổ đĩa trực tiếp tùy chọn là một đường dẫn lắp riêng. Đây là bằng chứng cụ thể về sản phẩm, không phải là bằng chứng cho thấy mọi buồng lái hoặc mẫu lỗ đều hỗ trợ mọi cơ sở.

Để liên lạc với khách hàng, tách ba câu hỏi:

1. Tấm lắp có mẫu lỗ và độ sâu dây buộc được phê duyệt cho đế chính xác không?
2. Cấu trúc có được đánh giá và đủ cứng cho mô-men xoắn và lực đạp có sẵn của đế không?
3. Ghế, bánh xe, bàn đạp, cần số và phanh tay có thể điều chỉnh công thái học cùng nhau không?

## 6. Câu hỏi chưa được giải quyết

- Dung sai có thể đo lường, chấp nhận được đối với micro-flex (tính bằng milimét) dưới tải trọng phanh 100kg trước khi nó tác động rõ rệt đến tính nhất quán của cảm biến tế bào tải là gì?
- Đầu dò rung (ví dụ: đầu dò xúc giác hoặc "máy lắc bass") nên được cách ly cơ học với các cấu hình cấu trúc chính để tránh nhiễu phá hủy với FFB tần số cao Direct Drive như thế nào?
- Có tần số cộng hưởng cụ thể trong các cấu trúc nhôm 40x80mm tiêu chuẩn phù hợp với tần số tín hiệu FFB phổ biến không, và nếu có, làm thế nào chúng có thể được làm ẩm?

## 7. Tài liệu tham khảo

# 7.1 Nhà sản xuất và bối cảnh sản phẩm

- [Sim-Lab P1X Pro cockpit] (https://sim-lab.eu/products/p1x-pro-sim-racing-cockpit) - ví dụ công khai về hệ sinh thái buồng lái bằng nhôm với các phụ kiện lắp bánh xe, bàn đạp, ghế ngồi, màn hình, cần số và phanh tay.
- [Hướng dẫn sử dụng Fanatec Podium DD1] (https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) - thiết lập cơ sở mô-men xoắn cao công cộng, hiệu chuẩn và bối cảnh an toàn.
- [Fanatec ClubSport GT Tương thích cơ sở bánh xe buồng lái] (https://www.fanatec.com/us/en/explorer/products/cockpit/wheel-bases-fit-on-the-fanatec-clubsport-gt-cockpit/) - ví dụ sản phẩm gắn phía dưới và tùy chọn phía trước.
- [Đăng ký nguồn hệ sinh thái Fanatec] (./references.md) - ngày nguồn và giới hạn hướng dẫn người mua cộng đồng.

### 7.2 Tham chiếu nội bộ liên quan

- [Kiến trúc cơ sở bánh xe] (./wheel_base.md) - tạo mô-men xoắn, an toàn động cơ và các ràng buộc FFB.
- [Kiến trúc bàn đạp] (./pedals.md) - đường dẫn lực tải và độ nhạy hiệu chuẩn.

## 8. Ghi chú thực hiện

- Đo độ lệch của buồng lái theo lực phanh đỉnh dự kiến và mô-men xoắn bánh xe trước khi coi giàn khoan như một khung tham chiếu ổn định.
- Xử lý các đầu dò xúc giác như một hệ thống rung riêng biệt; cách ly và thử nghiệm chúng để chúng không che lấp FFB hoặc chẩn đoán cảm biến.
- Ghi mô-men xoắn dây buộc, loại khung và kích thước hồ sơ trong báo cáo băng ghế dự bị để kết quả cơ học có thể tái tạo.
