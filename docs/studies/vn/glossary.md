# Thuật ngữ Khách hàng Fanatec Sim-Racing

> Ngày nghiên cứu: 2026-07-02
> Đối tượng: hỗ trợ khách hàng, bán hàng, nhà phát triển, người viết tài liệu kỹ thuật, và những người mới chơi sim-racing
> Phạm vi: sản phẩm Fanatec hiện tại và thường gặp, khả năng tương thích, thiết lập, điều chỉnh, thiết bị ngoại vi, và ngôn ngữ khắc phục sự cố

Bảng thuật ngữ này chuẩn hóa các từ được sử dụng khi thảo luận về các sản phẩm Fanatec với khách hàng. Nó không phải là tài liệu chính thức và không thay thế cho trang sản phẩm, Hướng dẫn nhanh (Quick Guide), sách hướng dẫn (manual), hoặc các hướng dẫn Hỗ trợ Fanatec cho một sản phẩm và phiên bản firmware cụ thể.

## Nội dung

- [Cách sử dụng Bảng thuật ngữ này](#cach-su-dung-bang-thuat-ngu-nay)
- [Từ vựng Tiếp nhận Khách hàng](#tu-vung-tiep-nhan-khach-hang)
- [Thuật ngữ Thương hiệu, Sản phẩm và Bundle](#thuat-ngu-thuong-hieu-san-pham-va-bundle)
- [Các Dòng Wheel Base Fanatec DD](#cac-dong-wheel-base-fanatec-dd)
- [Thuật ngữ Nền tảng và Tương thích](#thuat-ngu-nen-tang-va-tuong-thich)
- [Thuật ngữ Wheel Base, Mô-men xoắn, và Lắp đặt](#thuat-ngu-wheel-base-mo-men-xoan-va-lap-dat)
- [Thuật ngữ Vô lăng và Điều khiển](#thuat-ngu-vo-lang-va-dieu-khien)
- [Thuật ngữ Bàn đạp, Cần số, và Phanh tay](#thuat-ngu-ban-dap-can-so-va-phanh-tay)
- [Thuật ngữ Phần mềm, Firmware, và Thiết lập](#thuat-ngu-phan-mem-firmware-va-thiet-lap)
- [Các từ viết tắt trong Tuning Menu](#cac-tu-viet-tat-trong-tuning-menu)
- [Thuật ngữ FFB và Sim-Racing chung](#thuat-ngu-ffb-va-sim-racing-chung)
- [Thuật ngữ Khắc phục sự cố](#thuat-ngu-khac-phuc-su-co)
- [Bảng tra cứu nhanh Từ viết tắt](#bang-tra-cuu-nhanh-tu-viet-tat)
- [Tài liệu tham khảo](#tai-lieu-tham-khao)

## Cách sử dụng Bảng thuật ngữ này

### Nhãn bằng chứng

| Nhãn | Ý nghĩa |
|---|---|
| Official | Ngôn ngữ từ sản phẩm Fanatec, hỗ trợ, Explorer, hoặc sách hướng dẫn. |
| Standard | Cách sử dụng chung trong USB, hệ thống điều khiển, điện, hoặc motorsport. |
| Common | Ngôn ngữ sim-racing được sử dụng rộng rãi; việc triển khai chính xác có thể khác nhau tùy theo sản phẩm hoặc trò chơi. |
| Legacy | Ngôn ngữ phần mềm/sản phẩm Fanatec cũ mà khách hàng vẫn có thể sử dụng. |

Các tuyên bố về sản phẩm hiện tại rất nhanh bị lỗi thời. [Sổ đăng ký nguồn hệ sinh thái](./references.md) ghi lại ngày đánh giá và các xung đột đã biết. Đặc biệt, một hướng dẫn cộng đồng vào tháng 1 năm 2026 có trước bản cập nhật mô-men xoắn của ClubSport DD/DD+ hiện tại và định vị flagship Podium DD.

### Quy tắc giao tiếp

- Gọi là **Fanatec**, không phải "Fantec".
- Yêu cầu cung cấp tên sản phẩm chính xác, nền tảng, đường dẫn kết nối, phiên bản Fanatec App, và phiên bản firmware trước khi chẩn đoán.
- Phân biệt rõ **steering wheel** (vô lăng) với **wheel base** (đế bánh xe). Khách hàng thường gọi chung cả hai là "the wheel".
- Dùng **cường độ FFB** cho một cài đặt và **mô-men xoắn tính bằng Nm** cho lực đầu ra vật lý. Chúng có liên quan nhưng không thể thay thế cho nhau.
- Không hứa hẹn khả năng tương thích chỉ dựa vào hình dạng cổng kết nối. Sản phẩm, nền tảng, firmware, thế hệ quick-release, và đường dẫn kết nối đều rất quan trọng.
- Xử lý "Xbox Ready" và "PlayStation Ready" như là các nhãn có điều kiện, không giống như khả năng tương thích được cấp phép đầy đủ.
- Xác minh hỗ trợ mô-men xoắn QR2 cho từng sản phẩm hiện tại. Hành vi của QR2 Lite đã thay đổi đối với một số wheel thông qua các bản cập nhật firmware. [F4]

## Từ vựng Tiếp nhận Khách hàng

Hãy thu thập những thông tin này trước. Điều này ngăn chặn hầu hết các lỗi hỗ trợ do thuật ngữ gây ra.

| Hỏi Khách hàng Về | Ý nghĩa | Ví dụ Hữu ích |
|---|---|---|
| Platform | Máy chủ (host) đang chạy trò chơi | Windows PC, Xbox Series X\|S, PS5 |
| Wheel base | Bộ phận động cơ và hub USB | CSL DD, ClubSport DD+, Podium DD1 |
| Steering wheel hoặc hub | Cụm điều khiển có thể tháo rời | Formula V2.5X, CSL Universal Hub V2 |
| Quick release | Khớp nối cơ/điện giữa wheel và base | QR1, QR2 Lite, QR2 Pro |
| Peripherals | Bàn đạp, cần số, phanh tay, hoặc ngàm chuyển số tĩnh | CSL Pedals, ClubSport Shifter SQ V1.5 |
| Connection path | Cách mỗi thiết bị ngoại vi kết nối với máy chủ | Bàn đạp nối với base bằng RJ12; base nối với PC bằng USB |
| Software versions | Gói phần mềm trên máy chủ và firmware thiết bị | Fanatec App và phiên bản firmware của base/wheel |
| Mode | Nền tảng hoặc chế độ tương thích hiện tại | PC mode, Xbox mode, compatibility mode |
| Symptom | Lỗi có thể quan sát được, không phải nguyên nhân giả định | "Base được nhận diện nhưng không có FFB" |
| Trigger | Hành động sau đó sự cố bắt đầu | Cập nhật firmware, thay QR, cập nhật trò chơi |

## Thuật ngữ Thương hiệu, Sản phẩm và Bundle

| Thuật ngữ | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| Fanatec | Thương hiệu phần cứng sim-racing. Sử dụng chữ viết hoa chính xác. | Official |
| Ecosystem | Tập hợp các base, wheel, bàn đạp, cần số, phanh tay, quick release, phần mềm, và phụ kiện lắp đặt có thể hoạt động cùng nhau. Nó không có nghĩa là mọi kết hợp đều hỗ trợ mọi nền tảng hoặc tính năng. | Official [F1] |
| CSL | CSL là viết tắt của ClubSport Light. Cấp độ sản phẩm Fanatec được phân loại là dòng entry-level (cơ bản). Đây là tên của một dòng sản phẩm, không phải là chuẩn cổng kết nối hay tính tương thích. | Official [F19] |
| CSL Elite | Tên dòng sản phẩm được sử dụng cho một số wheel và bàn đạp. "Elite" không xác định tính tương thích nền tảng. | Official |
| ClubSport / CS | Cấp độ sản phẩm từ trung bình đến cao và là tiền tố viết tắt phổ biến. Các sản phẩm như ClubSport DD và ClubSport Shifter là hoàn toàn khác nhau. | Official |
| Podium / P | Cấp độ sản phẩm cao cấp được sử dụng cho các base, hub, module, và phụ kiện hiệu suất cao. | Official |
| Ready2Race / R2R | Bundle được thiết kế để chứa các thành phần chính cần thiết để bắt đầu đua. Cần xác nhận hỗ trợ nền tảng, thiết bị lắp đặt, và các nâng cấp tùy chọn trên trang của bundle đó. | Official |
| Racing Wheel | Có thể có nghĩa là một gói hoàn chỉnh gồm cả base và wheel. Hãy hỏi rõ xem khách hàng đang nói đến toàn bộ hệ thống hay chỉ riêng vô lăng có thể tháo rời. | Official/Common |
| Wheel Base | Đơn vị cố định chứa động cơ lái, trục, mạch điều khiển, giao diện USB, và các cổng cho thiết bị ngoại vi. Nó tạo ra FFB. | Official |
| Steering Wheel | Cụm điều khiển của người lái có thể tháo rời với tay cầm/vành, các nút, paddle, và thường có đèn LED/màn hình. Nó không tạo ra mô-men xoắn lái chính. | Official |
| Wheel Rim / Rim | Phần được người lái cầm nắm. Trong các hệ thống module, nó được gắn vít vào một hub. Khách hàng cũng thường dùng từ "rim" để chỉ toàn bộ steering wheel; hãy làm rõ. | Official/Common |
| Hub | Giao diện giữa rim và base, thường mang các nút điều khiển, paddle, mạch điện tử, và ngàm quick-release. | Official [F3] |
| Universal Hub | Hub dạng module của Fanatec chấp nhận các rim tương thích. Vẫn cần kiểm tra kiểu bắt vít, đường kính, trọng lượng, nút điều khiển, QR, và hỗ trợ nền tảng. | Official [F3] |
| Button Module | Cụm điều khiển/màn hình bổ sung được gắn vào một hub hoặc rim tương thích. | Official |
| Paddle Module | Cụm điều khiển phía sau bổ sung chứa các paddle sang số và, tùy thuộc vào mẫu mã, các paddle ly hợp analog. | Official |
| V1 / V2 / V2.5 | Phiên bản phần cứng trong tên sản phẩm. Đừng mặc định rằng các phụ kiện hoặc firmware có thể hoán đổi giữa các phiên bản. | Official |
| Discontinued / Legacy Product | Sản phẩm nằm ngoài danh mục hiện tại nhưng có thể vẫn được hỗ trợ bởi các driver, firmware, tài liệu, hoặc phụ tùng cũ. | Legacy |

## Các Dòng Wheel Base Fanatec DD

`DD` có nghĩa là **Direct Drive**, nhưng khách hàng có thể dùng "DD" để chỉ cấu trúc truyền động, ClubSport DD, hoặc sản phẩm Podium DD hiện tại. Luôn luôn hỏi tên mẫu mã (model) đầy đủ.

| Model | Định vị Sản phẩm | Mô-men xoắn Quảng cáo | Tóm tắt Nền tảng | Điểm khác biệt chính |
|---|---|---:|---|---|
| CSL DD | Direct drive entry-level | 5 Nm; 8 Nm với Boost Kit hỗ trợ | Windows PC; Xbox với wheel/hub được cấp phép Xbox; không cấp phép PlayStation | Sự lựa chọn nhỏ gọn, chi phí thấp để bước vào hệ sinh thái DD của Fanatec. [F13] |
| Podium Wheel Base DD1 | Flagship Podium thế hệ trước | Đỉnh (peak) lên tới 20 Nm | Mẫu tiêu chuẩn: Windows PC; Xbox với wheel/hub được cấp phép Xbox | Base Podium mô-men xoắn cao thế hệ trước. Từng có một mẫu DD1/bundle cấp phép PlayStation lịch sử, nên hãy kiểm tra SKU chính xác. [F14] |
| Podium Wheel Base DD2 | Flagship Podium thế hệ trước | Đỉnh (peak) lên tới 25 Nm | Windows PC; Xbox với wheel/hub được cấp phép Xbox | Mẫu mạnh hơn trong thời kỳ DD1; đi kèm Podium Kill Switch theo chuẩn. [F14] |
| ClubSport DD | Direct drive ClubSport hiện tại | 15 Nm holding torque với firmware hiện tại | Windows PC; Xbox với wheel/hub được cấp phép Xbox; không cấp phép PlayStation | Nền tảng QR2 và FullForce thế hệ hiện tại không có cấp phép PlayStation. [F15] |
| ClubSport DD+ | Direct drive ClubSport hiện tại | 18 Nm holding torque với firmware hiện tại | Windows PC và PlayStation; Xbox với wheel/hub được cấp phép Xbox | Mẫu ClubSport mạnh hơn có cấp phép PlayStation. [F15] |
| Podium DD | Flagship Podium hiện tại ra mắt năm 2026 | 25 Nm holding torque; đỉnh lên tới 33 Nm | Kiểm tra trang sản phẩm theo khu vực hiện tại và wheel/hub gắn kèm để biết hỗ trợ nền tảng | Hậu duệ của DD1/DD2, dựa trên kiến trúc ClubSport DD mới hơn với FullForce. [F16] |

### Cảnh báo khi gọi tên và so sánh

- **CSL DD** là tên mô hình hoàn chỉnh: `CSL` là cấp độ sản phẩm và `DD` có nghĩa là Direct Drive.
- **DD1** và **DD2** là mã mô hình trong thế hệ Podium Wheel Base trước đó; số "2" xác định biến thể có công suất cao hơn, không phải "Direct Drive phiên bản 2".
- **DD / DD+** trong cách gọi tắt của khách hàng thường có nghĩa là **ClubSport DD / ClubSport DD+**. Dấu `+` xác định mẫu ClubSport mạnh hơn, được cấp phép PlayStation.
- **Podium DD** không có số là một sản phẩm năm 2026 hoàn toàn riêng biệt. Đừng nhầm lẫn với DD1, DD2, hoặc bất kỳ direct drive chung chung nào.
- Số liệu của DD1/DD2 được công bố là **peak torque** (mô-men xoắn đỉnh). ClubSport DD/DD+ và Podium DD sử dụng **holding torque** (mô-men xoắn duy trì) trong các thông số sản phẩm hiện tại. Các con số này không tương đương trực tiếp.
- ClubSport DD và DD+ ban đầu được bán với holding torque quảng cáo là 12 Nm và 15 Nm. Firmware Fanatec V1.4.2.3 hoặc mới hơn đã tăng lên 15 Nm và 18 Nm vào tháng 5 năm 2026 mà không thay đổi phần cứng. Hãy cập nhật qua Fanatec App mới nhất. [F15]
- Lượng mô-men xoắn khả dụng vẫn có thể bị giới hạn bởi steering wheel gắn kèm, quick release, firmware, hoặc Low Torque Mode. [F4]

## Thuật ngữ Nền tảng và Tương thích

| Thuật ngữ | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| Platform | PC hoặc môi trường console mà trò chơi đang chạy trên đó. Hỗ trợ có thể phụ thuộc vào phần cứng cấp phép và đường dẫn kết nối thiết bị ngoại vi. | Official [F2] |
| PC Compatible | Sản phẩm hoạt động trên các Windows PC được hỗ trợ qua kết nối được phê duyệt. Nó không hứa hẹn sẽ hỗ trợ trong mọi trò chơi hoặc các hệ điều hành không phải Windows. | Official [F2] |
| Xbox Compatible / Xbox Licensed | Steering wheel hoặc hub có chứa phần cứng cấp phép Xbox. Khi được gắn vào một base Fanatec tương thích, nó kích hoạt hệ thống đó trên Xbox. | Official [F2] |
| Xbox Ready | Nhãn có điều kiện: sản phẩm sẽ sử dụng được trên Xbox khi kết hợp với một steering wheel hoặc hub Fanatec được cấp phép Xbox. | Official [F2] |
| PlayStation Compatible / PS Licensed | Việc cấp phép PlayStation của Fanatec nằm ở wheel base. Một base được cấp phép sẽ kích hoạt các thành phần gắn kèm được hỗ trợ trên PlayStation. | Official [F2] |
| PlayStation Ready / PS Ready | Nhãn có điều kiện: thiết bị có thể hoạt động trên PlayStation khi được kết nối qua một base Fanatec được cấp phép PlayStation. | Official [F2] |
| Cross-Platform Setup | Wheel/hub cấp phép Xbox cộng với base cấp phép PlayStation. Thiết lập này có thể hỗ trợ PC và cả hai dòng console; hãy xác minh từng trang sản phẩm. | Official [F2] |
| Console Compatibility | Yêu cầu phần cứng được cấp phép và trò chơi hỗ trợ. Các thiết bị ngoại vi thường kết nối qua base thay vì các cổng USB riêng biệt của console. | Official [F2][F8] |
| Game Support | Trò chơi có triển khai các trục, nút, FFB, màn hình, đèn LED, hoặc telemetry liên quan. Tính tương thích phần cứng không đảm bảo mọi tính năng đều có. | Official [F7] |
| Native Mode | Thiết bị nhận diện ở chế độ hiện tại theo đúng dự định của nó. Tên gọi và hành vi chính xác khác nhau tùy base và nền tảng. | Official/Common |
| Compatibility Mode / CSW Mode | Base mô phỏng hoặc nhận diện tương tự như một base cũ hơn để các trò chơi không có native support có thể hoạt động. Tính năng có thể khác biệt. | Official/Legacy |
| PC Mode | Chế độ wheel-base dành cho Windows PC. Màu đèn LED và quy trình chuyển chế độ tùy thuộc vào sản phẩm. | Official |
| Xbox Mode | Trạng thái được dùng với các phần cứng được hỗ trợ trên Xbox. Việc cấp phép Xbox vẫn nằm trong wheel hoặc hub được cấp phép. | Official |
| PlayStation Mode | Trạng thái của một base cấp phép PlayStation trên console PlayStation được hỗ trợ. Chỉ báo chính xác có thể khác nhau. | Official |
| Standalone | Thiết bị ngoại vi kết nối trực tiếp với PC, thường qua USB hoặc ClubSport USB Adapter. USB standalone thường không hoạt động trên console. | Official [F2][F8] |
| Base-Connected | Thiết bị ngoại vi kết nối với base, base sẽ tổng hợp và truyền tín hiệu qua kết nối host/console của base. | Official [F2] |

## Thuật ngữ Wheel Base, Mô-men xoắn, và Lắp đặt

| Thuật ngữ / Viết tắt | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| DD — Direct Drive | Trục động cơ truyền động trực tiếp trục lái, không qua dây đai hoặc bánh răng giảm tốc. DD mô tả cấu trúc, không tự động mang ý nghĩa mô-men xoắn hoặc chất lượng. | Official/Common |
| Belt Drive | Động cơ truyền lực qua dây đai và puli. Thường thấy trên các wheel base đời cũ. | Common/Legacy |
| FFB — Force Feedback | Lực lái do động cơ tạo ra dựa trên các lệnh từ trò chơi và cài đặt của base. | Official [F5] |
| Torque | Lực xoay tại trục lái, thường được biểu thị bằng newton-mét (Nm). | Standard |
| Nm — Newton-metre | Đơn vị SI của mô-men xoắn. Nhiều Nm hơn có nghĩa là mô-men xoắn khả thi cao hơn, không tự động có nghĩa là chi tiết hoặc chân thực hơn. | Standard |
| Peak Torque | Mô-men xoắn cao nhất trong thời gian ngắn dưới các điều kiện quy định. Không so sánh trực tiếp nó với holding torque của một sản phẩm khác. | Common |
| Holding / Sustained Torque | Mô-men xoắn duy trì được lâu hơn dưới các giới hạn về nhiệt và điện. Định nghĩa và điều kiện thử nghiệm của mỗi nhà cung cấp có thể khác nhau. | Common |
| High Torque Mode | Trạng thái cho phép sử dụng quá mức giới hạn low-torque khi bộ đôi wheel/QR được phê duyệt. Không bao giờ bỏ qua cơ chế phát hiện hoặc các cảnh báo. | Official [F4] |
| Low Torque Mode | Chế độ giới hạn an toàn đối với wheel hoặc QR không được phê duyệt cho lực kéo tối đa của base. Hướng dẫn hiện tại quy định là 8 Nm trên các base đầu ra cao được liệt kê. | Official [F4] |
| QR — Quick Release | Khớp nối cơ/điện kết nối steering wheel với base. Cả hai phía và thế hệ QR phải khớp nhau. | Official [F4] |
| QR1 | Hệ QR Fanatec thế hệ thứ nhất. QR1 và QR2 không kết nối được với nhau. Fanatec tuyên bố QR1 đã bị ngừng sản xuất, dù các phần cứng cũ và cách chuyển đổi tùy model vẫn tồn tại. | Official [F4][F17] |
| QR1 Lite | QR1 wheel-side bằng vật liệu composite có giới hạn mô-men xoắn theo sản phẩm. Nó không tương đương với QR2 Lite. | Official [F4] |
| QR2 | Hệ QR hiện tại với các thành phần Base-Side và Wheel-Side tách biệt. | Official [F4] |
| QR2 Base-Side | Thành phần QR2 được lắp trên trục của base. Type-C, Type-F, hoặc Type-M tùy thuộc vào từng base. | Official [F4] |
| QR2 Wheel-Side | Thành phần được lắp trên wheel/hub. Nó kết nối với các biến thể QR2 Base-Side chính thức, tùy thuộc vào sản phẩm và sự phê duyệt mô-men xoắn. | Official [F4] |
| QR2 Lite Wheel-Side | QR2 wheel-side bằng composite. Hỗ trợ mô-men xoắn cao tùy thuộc vào wheel và firmware; các sản phẩm nhất định đã nhận được phê duyệt full-torque. | Official [F4] |
| QR2 Pro Wheel-Side | Phiên bản kim loại cao cấp hướng tới motorsport. "Pro" không ngụ ý là tương thích hoàn toàn với mọi nền tảng. | Official [F4] |
| Type-C / Type-F / Type-M | Các biến thể QR2 Base-Side tùy theo base: Type-C cho CSL DD/GT DD Pro, Type-F cho ClubSport DD, Type-M cho Podium trong hướng dẫn hiện tại. | Official [F4] |
| Shaft | Trục xoay đầu ra của wheel base. QR Base-Side được lắp vào đây. | Standard |
| Flex | Chuyển động đàn hồi không mong muốn ở buồng lái (cockpit), ngàm lắp, rim, hoặc QR. Nó có thể làm giảm chi tiết cảm nhận mà không phải do lỗi điện tử. | Common |
| Play / Backlash | Chuyển động tự do trước khi tải được truyền đi. Hãy hỏi ở vị trí nào: QR, shaft, hub, rim, bàn đạp, hay cockpit. | Common |
| Hard Mount | Bắt chặt thiết bị trực tiếp vào cockpit/bản kim loại bằng các điểm được chỉ định. Sử dụng kích thước và độ sâu bu lông được phê duyệt. | Official |
| Table Clamp | Phụ kiện dùng để kẹp chặt base hoặc shifter vào bàn. Độ cứng của bàn và giới hạn mô-men xoắn là yếu tố quan trọng. | Official |
| Cockpit / Rig | Khung kết cấu hỗ trợ ghế, base, bàn đạp, và phụ kiện. Độ cứng vững và sự điều chỉnh ảnh hưởng đến sự thoải mái và cảm nhận FFB. | Common |
| Boost Kit | Bộ nguồn công suất cao hơn cho các base được hỗ trợ giúp kích hoạt cấu hình mô-men xoắn cao hơn đã quảng cáo. Đây không phải là overclock chung chung. | Official |
| FullForce | Lớp hiệu ứng/giao thức FFB tần số cao của Fanatec trên các phần cứng và trò chơi được hỗ trợ. Tính đến tháng 6 năm 2026, hướng dẫn chính thức hiện tại bao gồm CSL DD và GT DD Pro cùng với các dòng ClubSport và Podium DD hiện tại; vẫn yêu cầu hỗ trợ từ phía trò chơi. | Official [F18] |

## Thuật ngữ Vô lăng và Điều khiển

| Thuật ngữ / Viết tắt | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| D-pad | Điều khiển kỹ thuật số bốn hướng, thường có bấm ở giữa, dùng cho các menu hoặc điều hướng Tuning Menu. | Common |
| FunkySwitch | Điều khiển đa chức năng của Fanatec kết hợp di chuyển theo hướng, xoay, và bấm trên các wheel được hỗ trợ. | Official [F5] |
| Rotary Encoder | Núm vặn tạo ra tín hiệu tăng/giảm theo từng bước khi xoay. Thường không báo cáo một vị trí tuyệt đối cố định. | Standard |
| Thumb Encoder | Rotary encoder được đặt ở vị trí ngón cái để dễ thao tác khi cầm vô lăng. | Common |
| MPS — Multi-Position Switch | Điều khiển đa vị trí có thể xuất ra dưới dạng encoder, pulse, constant, hoặc hành vi do game tùy chọn. | Official [F5] |
| Button Cluster / Island | Nhóm các nút được bố trí cùng nhau, thường có thể điều chỉnh trên một Universal Hub. | Official [F3] |
| Button Caps | Các nắp phím có thể thay thế phù hợp với các chức năng của nền tảng/game. Nhãn hiệu không làm thay đổi tính tương thích điện tử. | Official |
| Magnetic Shifter Paddle | Paddle với hành động trả về/chốt bằng từ tính, thường báo cáo tín hiệu sang số kỹ thuật số. | Common |
| Analogue Paddle | Paddle sử dụng trục biến thiên có thể cấu hình cho ly hợp, phanh tay, phanh/ga, hoặc các trục bổ sung trên các wheel được hỗ trợ. | Official [F5] |
| Dual Clutch | Hai paddle analog được sử dụng cùng lúc cho các pha xuất phát cuộc đua; một bên có thể giữ điểm cắt (bite point) đã thiết lập. | Official [F5] |
| CbP — Clutch Bite Point | Tỷ lệ phần trăm bám ly hợp được cấu hình cho việc xuất phát lặp lại với các paddle analog kép. | Official [F5] |
| RevLEDs | Đèn báo RPM/vòng tua do telemetry của game hoặc phần mềm hỗ trợ điều khiển. Chúng không chứng tỏ FFB đang hoạt động. | Official [F7] |
| FlagLEDs | Đèn LED nhiều màu dùng cho các cờ đua, pit/giới hạn, và các trạng thái khác được hỗ trợ. | Official [F7] |
| RevStripe | Dải đèn tua máy sáng đặc thù cho từng sản phẩm. Hành vi phụ thuộc vào wheel, game, nền tảng, và phần mềm. | Official [F7] |
| OLED / LCD | Công nghệ hiển thị tốc độ, số, tuning, telemetry, hoặc trạng thái. Hỗ trợ hiển thị nội dung khác nhau. | Standard/Official |
| Telemetry | Dữ liệu xuất từ game như RPM, tốc độ, số, cờ, hoặc nhiên liệu. Nó điều khiển màn hình/đèn LED và độc lập với FFB. | Common |
| Static Shifter Paddles | Các paddle cố định được gắn vào base/cockpit thay vì quay cùng với wheel. Hoạt động của cổng tùy thuộc vào từng base. | Official [F8] |

## Thuật ngữ Bàn đạp, Cần số, và Phanh tay

| Thuật ngữ / Viết tắt | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| Pedal Set | Cụm chân ga, chân phanh, và ly hợp tùy chọn. Kết nối và khả năng USB khác nhau tùy thuộc vào từng model và kit đã lắp. | Official |
| Throttle / Accelerator | Trục bàn đạp ra lệnh công suất động cơ, thường được cảm biến bằng vị trí. | Common |
| Brake Pedal | Trục bàn đạp ra lệnh phanh. Nó có thể đo hành trình, lực, hoặc kết hợp cả hai. | Common |
| Clutch Pedal | Trục bàn đạp dùng để điều khiển ly hợp thủ công. | Common |
| Load Cell / LC | Cảm biến lực thường được dùng trong các bàn đạp phanh; đo lực tác động thay vì vị trí. | Official [F6] |
| Hall Sensor | Cảm biến từ không tiếp xúc thường đo vị trí bàn đạp hoặc đòn bẩy. | Official [F6] |
| Potentiometer / Pot | Biến trở đo vị trí có tiếp xúc. Hao mòn hoặc bụi bẩn có thể gây ra tín hiệu nhiễu. | Standard/Common |
| Hydraulic-Equipped Pedal | Sử dụng điện trở/áp suất dựa trên chất lỏng. Hãy xác nhận xem mẫu cụ thể đó cảm biến áp suất, lực hay hành trình. | Official [F6] |
| Elastomer | Phần tử polymer chịu nén để chỉnh độ cứng/hành trình của bàn đạp, đặc biệt là quanh phanh load-cell. | Official [F6] |
| Preload | Lực nén ban đầu trước khi bàn đạp dịch chuyển bình thường. Nó không phải là lực phanh tối đa. | Common |
| Pedal Travel | Khoảng cách hoặc góc vật lý mà qua đó bàn đạp dịch chuyển. | Standard |
| BRF — Brake Force | Cài đặt Fanatec quy định mức lực load-cell vật lý tương ứng với tín hiệu phanh tối đa. Nó không phải là phân bổ lực phanh (brake bias). | Official [F5] |
| Brake Bias | Cài đặt trong game phân bổ lực phanh giữa trục trước/sau; không liên quan đến hiệu chỉnh BRF. | Common |
| Dead Zone | Vùng đầu vào bị bỏ qua ở phần đầu/cuối của một trục. Cài đặt quá nhiều sẽ giảm độ phân giải khả dụng. | Common |
| Calibration | Ánh xạ các vị trí vật lý tối thiểu, tối đa, vị trí giữa, hoặc số sang các giá trị logic. | Official [F8] |
| H-Pattern | Kiểu cần số thủ công với vị trí cửa vào (gate) vật lý cho từng số. | Official [F8] |
| SQ — Sequential | Thao tác cần số tiến/lùi để lên/xuống số. "SQ" nhận diện khả năng tuần tự. | Official [F8] |
| Reverse Inhibitor | Cơ cấu ngăn việc lùi số vô tình, ví dụ như phải ấn đòn bẩy xuống trước. | Official [F8] |
| Shifter 1 Port | Hỗ trợ các chế độ H-pattern và sequential cho ClubSport Shifter SQ V1.5 trong hướng dẫn hiện tại. | Official [F8] |
| Shifter 2 Port | Hỗ trợ đầu vào sequential; hướng dẫn hiện tại cũng xác nhận sử dụng Static Shifter Paddles. | Official [F8] |
| Handbrake | Thường là trục đòn bẩy analog dùng cho drift/rally. Không phải là nút kỹ thuật số trừ khi được gán (mapped) như vậy. | Official/Common |
| ClubSport USB Adapter | Adapter cho phép các thiết bị ngoại vi được chọn hoạt động standalone trên PC. Adapter mode phải khớp với thiết bị ngoại vi. | Official [F9] |
| RJ12 | Cổng kết nối dạng module dùng cho các thiết bị ngoại vi Fanatec. Tên cổng không xác định giao thức hay đảm bảo tương thích. | Official/Common [F9] |
| PS/2 Connector | Mini-DIN được sử dụng trên một số kết nối base/shifter đời cũ. Không ngụ ý tương thích với bàn phím/chuột PC. | Official/Legacy [F8] |
| Simultaneous USB and Base Connection | Việc cắm cùng lúc USB và Base có thể không an toàn/không được hỗ trợ cho một số bàn đạp. Làm theo tài liệu hướng dẫn chính xác; không dùng cả hai trừ khi được cho phép rõ ràng. | Hướng dẫn official theo từng sản phẩm |

## Thuật ngữ Phần mềm, Firmware, và Thiết lập

| Thuật ngữ | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| Fanatec App | Phần mềm Windows hiện tại dùng cho việc thiết lập, kiểm tra, điều chỉnh, cấu hình LED/hiển thị, và quản lý firmware được hỗ trợ. | Official [F4][F8] |
| Fanatec Control Panel / Wheel Property Page | Giao diện cấu hình Windows cũ trong các driver/tài liệu hỗ trợ cũ. Hãy hỏi khách hàng đang xem gói nào. | Legacy [F5] |
| FanaLab | Công cụ PC tinh chỉnh profile telemetry/vệ tinh cũ trong các thiết lập cũ. Xác minh phiên bản và profile đang hoạt động trước khi chẩn đoán. | Legacy |
| Driver | Phần mềm host giúp Windows giao tiếp với phần cứng. Phiên bản Driver và firmware là các giá trị khác nhau. | Official [F10] |
| Firmware / FW | Phần mềm được lưu trữ bên trong base, mạch điều khiển động cơ, wheel, bàn đạp, hoặc adapter. Một hệ thống có thể có nhiều thành phần firmware. | Official [F10] |
| Firmware Manager | Chức năng quy trình cập nhật/khôi phục dùng để kiểm tra và nạp (flash) firmware được hỗ trợ. | Official [F8][F10] |
| Bootloader | Đoạn mã thiết bị tối thiểu được sử dụng để khởi động/khôi phục các bản cập nhật. Chế độ cập nhật không nhất thiết có nghĩa là hư hỏng vĩnh viễn. | Standard |
| Firmware Update | Thay thế firmware của thiết bị. Giữ nguồn/cáp ổn định và làm theo lời nhắc. | Official [F10] |
| Manual Firmware Update | Quy trình nâng cao để chọn cấu hình firmware/thành phần thủ công. Chỉ sử dụng khi hướng dẫn chính thức yêu cầu. | Official [F8] |
| Adapter Mode | Firmware/định dạng của ClubSport USB Adapter được chọn cho thiết bị ngoại vi gắn kèm. | Official [F8][F9] |
| Device Detection | Phần mềm host nhận diện được base/thành phần. Việc nhận diện là riêng biệt với gán nút trong game và FFB. | Common |
| Center Calibration | Lưu trữ vị trí đi thẳng vật lý làm tâm (center) logic. Nó không sửa chữa việc bị lệch tâm cơ học. | Official |
| Shifter Calibration | Dạy (teach) các vị trí số H-pattern; có thể bị yêu cầu làm lại sau các bản cập nhật firmware. | Official [F8] |
| Pedal Calibration | Lưu trữ dải tối thiểu/tối đa hoặc lực của bàn đạp. Các chế độ phụ thuộc vào sản phẩm/phần mềm. | Official |
| Tuning Menu | Các cài đặt của base có thể truy cập qua điều khiển wheel và phần mềm PC. Nó không thay thế thiết lập (setup) trong game. | Official [F5] |
| Standard Tuning Menu | Chế độ xem đơn giản hóa bộc lộ các tham số cốt lõi và ít profile hơn. | Official [F5] |
| Advanced Tuning Menu | Chế độ xem đầy đủ bộc lộ thêm các tham số và nhiều slot tùy chỉnh. | Official [F5] |
| A SET — Auto Setup | Cho phép một game được hỗ trợ kiểm soát các giá trị điều chỉnh; các giá trị mặc định được dùng khi tính năng này không kích hoạt. | Official [F5] |
| C SET — Custom Setup | Profile có thể do người dùng tùy chỉnh trong Standard Tuning Menu trên các base hiện tại hỗ trợ tính năng này. | Official [F5] |
| S_1–S_5 / SET 1–5 | Profile Advanced Tuning Menu do người dùng lưu. Tính khả dụng phụ thuộc vào base/firmware. | Official [F5] |
| Profile | Nhóm các giá trị đã được lưu. Hãy nói rõ ai sở hữu nó: base, Fanatec App, FanaLab, hay game. | Official/Common |
| Factory Settings | Cài đặt gốc của nhà sản xuất. Reset profile không phải là hạ cấp (downgrade) firmware hay gỡ cài đặt driver. | Official |

## Các từ viết tắt trong Tuning Menu

Dải đo chính xác và sự khả dụng khác nhau theo từng base, wheel, bàn đạp, firmware, và chế độ menu. Kiểm tra trang hướng dẫn sử dụng/hỗ trợ hiện hành của sản phẩm trước khi khuyến nghị các giá trị. [F5]

| Viết tắt | Ý nghĩa (Mở rộng) | Giải thích thực tế |
|---|---|---|
| SEN | Sensitivity | Góc quay của tay lái (độ), hoặc được điều khiển tự động bởi game/driver. Không phải là polling sensitivity (độ nhạy cập nhật USB). |
| FF / FFB | Force Feedback | Cường độ FFB tối đa của base. Lực xoắn thực tế cuối cùng còn phụ thuộc vào game xuất ra và các modifiers. |
| FFS | Force Feedback Scaling | Chọn hành vi dạng tuyến tính (`LIN`) hoặc đỉnh (`PEA`). Không giống với cường độ FF. |
| LIN | Linear | Duy trì mối quan hệ yêu cầu-lực (request-to-torque) tuyến tính hơn, nhưng lực tối đa có thể bị giảm lại. |
| PEA | Peak | Cho phép hành vi đầu ra chạm đỉnh (peak) trên các base hỗ trợ. |
| NDP | Natural Damper | Thêm kháng lực phụ thuộc vào tốc độ, kiểm soát chuyển động và dao động (oscillation). Thiết lập quá cao sẽ cảm thấy tay lái bị chậm/nặng. |
| NFR | Natural Friction | Thêm kháng lực tương đối không đổi bất kể chi tiết trong game. Thiết lập quá cao làm mất chi tiết và gây mỏi tay. [F11] |
| NIN | Natural Inertia | Mô phỏng khối lượng lái/quán tính bổ sung, thường rất hữu ích với những wheel có trọng lượng nhẹ. |
| INT | FFB Interpolation | Làm mịn các tín hiệu FFB giật/nhiễu từ game. Giá trị cao làm giảm sự thô ráp nhưng có thể làm giảm tính tức thời. |
| FEI | Force Effect Intensity | Làm thay đổi độ gắt/sắc nét (intensity/sharpness) của hiệu ứng lực trong game. Không phải là bộ giới hạn lực chính. |
| FOR | Force | Nhân bản (scale) hiệu ứng constant-force từ game. Giữ nguyên giá trị trừ khi hướng dẫn của game yêu cầu thay đổi. |
| SPR | Spring | Nhân bản (scale) hiệu ứng spring (lò xo) từ game. Không phải lúc nào cũng tạo ra hiệu ứng tự động trả thẳng tâm (auto-centering). |
| DPR | Damper | Nhân bản (scale) hiệu ứng damper từ game. Khác biệt với hiệu ứng NDP được tạo bởi base. |
| BRF | Brake Force | Thiết lập mức lực vật lý lên load-cell cần thiết để đạt tín hiệu phanh tối đa trên bàn đạp hỗ trợ. |
| BLI | Brake Level Indicator | Ngưỡng (threshold) cho cơ chế rung bàn đạp/wheel được hỗ trợ; hành vi do game điều khiển có thể khác. |
| SHO | Shock / Vibration Strength | Điều khiển mô-tơ rung tích hợp (nếu có hỗ trợ), không phải điều khiển mô-tơ FFB chính của base. |
| MPS | Multi-Position Switch Function | Lựa chọn cách công tắc MPS (hỗ trợ) sẽ gửi tín hiệu đầu vào. |
| AUTO | Automatic / Game Specific | Để game/phần mềm chọn cách hoạt động. Ý nghĩa phụ thuộc vào thiết lập cha (parent setting). |
| ENC | Encoder | Công tắc MPS xuất một input khi quay theo chiều kim đồng hồ, và một input khác khi quay ngược chiều. |
| CONST | Constant | Công tắc MPS gửi duy trì (hold) một trạng thái nút khác nhau cho từng vị trí. |
| PULSE | Pulse | Công tắc MPS gửi một input cụ thể cực nhanh (pulse) cho từng vị trí khi di chuyển. |
| AP | Analogue Paddles | Chọn các chức năng hoạt động của analogue-paddle có hỗ trợ. |
| CbP | Clutch Bite Point | Cả 2 paddle phối hợp làm điều khiển ly hợp cho khả năng xuất phát ổn định, lặp lại cao. |
| CH | Clutch / Handbrake | Một paddle analog đóng vai trò ly hợp; paddle kia đóng vai trò phanh tay. |
| bt / BT | Brake / Throttle | Analogue paddles đóng vai trò chân phanh và chân ga. |
| AnA | Mappable Analogue Axes | Mở rộng paddle như là các trục có thể gán phím riêng lẻ. |

## Thuật ngữ FFB và Sim-Racing chung

| Thuật ngữ / Viết tắt | Định nghĩa và Giải thích cho Khách hàng | Bằng chứng |
|---|---|---|
| Axis | Tín hiệu đầu vào liên tục như tay lái, bướm ga, chân phanh, ly hợp, hoặc phanh tay. | USB/Common |
| Button Mapping | Việc gán chức năng từ nút bấm vật lý thành lệnh trong game. Nó không thay đổi tính cấp phép hay firmware. | Common |
| Clipping | FFB đạt đến mức tối đa được cấu hình, vì vậy các lực mạnh hơn bị mất đi điểm nhấn. Hạ/Cân bằng lại gain trong game để giữ lại các chi tiết. | Common |
| Damping | Kháng lực chủ yếu liên quan tới tốc độ chuyển động. Giúp làm mượt sự di chuyển; đặt quá mức che lấp các chi tiết xử lý nhanh. | Control-system/Common |
| Friction | Kháng lực chống lại sự di chuyển, bao gồm di chuyển chậm. Trong tinh chỉnh Fanatec, xem mục NFR. | Control-system/Common |
| Inertia | Sự phản kháng tới sự thay đổi tốc độ quay vì khối lượng hiệu dụng. Trong tinh chỉnh, xem mục NIN. | Control-system/Common |
| Oscillation | Dao động qua lại không mong muốn ở tay lái. FFB quá mạnh, có độ trễ, damping quá thấp, hay do cơ chế game đều có thể gây ra hiện tượng này. | Official/Common [F12] |
| Gain | Bộ nhân cường độ FFB trong game/phần mềm. Mức Gain quá cao có thể gây clipping. | Common |
| Minimum Force | Cài đặt trong game giúp tăng FFB ở vùng giữa/tâm (center). Mức cao quá có thể gây oscillation trên các mẫu DD. | Common [F12] |
| Linearity | Cách mô-men xoắn phản ứng theo tỉ lệ tuyến tính của lực yêu cầu. Lực chạy tuyến tính hơn không có nghĩa là mạnh hơn. | Common |
| Latency | Độ trễ giữa game, tín hiệu USB, firmware, mô-tơ, và driver hồi đáp. | Standard |
| Polling / Report Rate | Tần suất mà các báo cáo dữ liệu đầu vào được trao đổi. Nó không phải là toàn bộ độ trễ end-to-end. | USB/Common |
| USB HID | Universal Serial Bus Human Interface Device class dành cho điều khiển như các trục (axes)/nút bấm (buttons). | Standard [S1] |
| USB PID | USB Physical Interface Device model dành cho các hiệu ứng FFB; không liên quan đến Product ID (PID). | Standard [S2] |
| RPM | Số vòng quay mỗi phút (Revolutions per minute). Telemetry RPM của động cơ thường dùng để điểu khiển RevLEDs. | Standard |
| Understeer | Lốp trước bị thiếu độ bám so với lượt đánh lái yêu cầu. Sự thể hiện qua FFB sẽ dựa trên thiết kế/hệ vật lý của game. | Motorsport/Common |
| Oversteer | Lốp sau trượt (mất độ bám) nhanh hơn so với lốp trước, khiến xe bị quay (rotate) gắt hơn mức yêu cầu. | Motorsport/Common |
| Road Effects | Bề mặt, ổ gà, lề đường (kerb) hoặc các hiệu ứng rung lắc; có thể được tính bằng vật lý hay từ các file vạch sẵn (canned effect) tùy vào game. | Common |
| Soft Lock | Lực ngáng ảo được thiết lập gần giới hạn lái góc quay thực tế của xe trong game, đây không phải điểm kết thúc vòng quay vật lý (physical shaft stop). | Common |
| Steering Lock / Rotation | Tổng góc lái tối đa tính bằng độ. Hãy điều chỉnh Fanatec SEN và thiết lập steering lock của game để có thông số trùng khớp. | Common |
| Sim Rig | Thiết lập đầy đủ: khung đua (cockpit), ghế, điều khiển, màn hình/VR, thiết bị (PC/Console), và các phụ kiện khác. | Common |
| Telemetry Dashboard | Bảng hiển thị điều khiển từ dữ liệu game, thông thường qua Fanatec App hoặc SimHub. | Common |

## Thuật ngữ Khắc phục sự cố

| Thuật ngữ | Định nghĩa và Diễn đạt An toàn với Khách hàng |
|---|---|
| Not Detected | Máy chủ không nhận dạng được thiết bị. Kiểm tra lại nguồn điện, cáp USB, chế độ, cổng kết nối, và Device Manager (trên Windows) trước khi nghi ngờ hỏng firmware. |
| Detected but Not Working in Game | Trình điều khiển (driver) nhận thiết bị, nhưng các bản map/cài đặt tương thích của game có thể đã bị thiếu. Tách biệt riêng các khâu test input, FFB, LED, và telemetry. |
| No FFB | Tín hiệu vào (Input) có thể vẫn hoạt động dù mô-tơ mất FFB. Kiểm tra trong game, các cài đặt FFB, FF của thiết bị, tình trạng firmware, trạng thái low-torque/an toàn, và xem màn hình game có được focus không. |
| FFB Loss | FFB hoạt động được rồi ngừng. Lưu lại thao tác nào làm lỗi xảy ra (trigger), nhiệt độ, cổng kết nối USB, tình trạng phiên đua, và xác nhận coi Input (trục tay lái) còn tác dụng không. |
| Wheel Disconnect | Wheel tháo rời liên tục mất tín hiệu dù base vẫn có điện. Xác nhận khả năng khớp nối, nhận diện QR, kiểm tra cổng chân tiếp xúc (contact pin), firmware, và mức độ lỏng (play). |
| Mis-shift | Số dạng cổng chữ H (H-pattern) đọc sai. Xác nhận cắm vào cổng Shifter 1, chế độ hộp số H mode, kiểm tra dây cắm, và căn chỉnh lại. |
| Input Jitter / Spiking | Tín hiệu Trục (axis) nhảy loạn xạ dù không tác động vật lý. Các nguyên nhân có thể do kẹt/rơ, nhiễu/kéo dãn cáp cảm biến, đường nối mass, hay vấn đề bộ phận cơ học. |
| EMI — Electromagnetic Interference | Nhiễu tín hiệu điện cộng hưởng vào USB/cảm biến/dây điện. Việc chẩn đoán sẽ dựa vào đường đi của dây, cách nối mass, công suất tải, và thử đổi các cổng khác. |
| Ground Loop | Đường dây rò điện không mong muốn chạy xuyên qua nối đất làm ngắt kết nối. Tuyệt đối không gỡ dây nối đất bảo vệ đi vì nghĩ đây là cách giải quyết. |
| Power Cycle | Đóng tắt hoàn toàn một cách có kiểm soát, chờ một chút rồi bật lại. Không phải là rút giắc nguồn trong khi đang nâng cấp Firmware. |
| Re-enumeration | Gỡ kết nối thiết bị USB rồi nhận lại, có thể bằng mode/ID nhận dạng khác. |
| Firmware Mismatch | Các thành phần đang dùng lẫn lộn bản firmware không tương thích. Hãy dùng theo hướng dẫn hiện tại của các bản cập nhật chính thức. |
| Recovery Mode | Trạng thái bootloader/update dùng để khôi phục lại (restore) phần sụn. Thao tác đúng theo bước chính thức từ từng bản phần cứng đó. |
| Bricked | Một thuật ngữ không chính thức diễn đạt việc thiết bị sau khi update bị hư hỏng không dùng được nữa. Phải kiểm tra tính năng khôi phục (recovery), mức độ nhận dạng, và điện đóm (power) trước khi chẩn đoán hỏng vĩnh viễn. |
| RMA — Return Merchandise Authorization | Quy trình trả lại/sửa chữa thiết bị được hãng phê duyệt. Cung cấp rõ Serial, biên lai mua bán (proof), số series thiết bị, thông tin logs, bằng chứng, và các bước mô tả tái hiện sự cố. |
| DOA — Dead on Arrival | Một nhãn đánh giá không chính thức dùng khi thiết bị gặp lỗi hỏng hóc ngay lập tức khi khui thùng setup (first-setup failure). Vẫn cần rà soát qua các bước kiểm tra an toàn tối thiểu (minimum safe checks) trước khi dùng thuật ngữ này lên ticket hỗ trợ. |
| Reproduction Steps | Trình tự ngắn và đáng tin cậy mô tả cách khiến lỗi xảy ra, kể ra trạng thái, thiết bị, dây cắm kết nối, mode (chế độ), thông số firmware, phần mềm game, và kết quả lỗi. |
| Expected Result | Cách hoạt động phải tuân thủ đúng như phần Sách hướng dẫn / Tài liệu mô tả của sản phẩm hiện tại. |
| Actual Result | Hiện tượng được khách hàng báo lại đúng như lúc nhìn thấy trên mặt báo, không dựa theo kết luận bị ngộ nhận (assumed cause). |
| Workaround | Biện pháp bảo đảm tính hoạt động an toàn tạm thời nhưng bản chất chưa loại trừ hoàn toàn nguyên nhân gốc (root cause). |
| Root Cause | Nguyên do gây hỏng hóc đã được xác nhận. Triệu chứng, các mối liên kết, hoặc khởi động lại (reboot) thành công đơn lẻ không dùng làm bằng chứng kết luận. |

## Bảng tra cứu nhanh Từ viết tắt

| Viết tắt | Mở rộng | Viết tắt | Mở rộng |
|---|---|---|---|
| AnA | Mappable Analogue Axes | AP | Analogue Paddles |
| BLI | Brake Level Indicator | BRF | Brake Force |
| BT / bt | Brake / Throttle | CbP | Clutch Bite Point |
| CH | Clutch / Handbrake | CONST | Constant |
| CS | ClubSport | DD | Direct Drive |
| DOA | Dead on Arrival | DPR | Damper |
| EMI | Electromagnetic Interference | ENC | Encoder |
| FEI | Force Effect Intensity | FF / FFB | Force Feedback |
| FFS | Force Feedback Scaling | FW | Firmware |
| HID | Human Interface Device | INT | FFB Interpolation |
| LC | Load Cell | LIN | Linear |
| MPS | Multi-Position Switch | NDP | Natural Damper |
| NFR | Natural Friction | NIN | Natural Inertia |
| Nm | Newton-metre | OLED | Organic Light-Emitting Diode |
| PEA | Peak | PID | Physical Interface Device |
| PS | PlayStation | QR | Quick Release |
| R2R | Ready2Race | RMA | Return Merchandise Authorization |
| RPM | Revolutions per minute | SEN | Sensitivity |
| SHO | Shock / Vibration Strength | SPR | Spring |
| SQ | Sequential | USB | Universal Serial Bus |

## Ngôn ngữ Khách hàng được Khuyến nghị

| Tránh | Ưu tiên | Lý do |
|---|---|---|
| "Wheel của bạn không tương thích." | "Vui lòng xác nhận chính xác base, wheel/hub, QR, nền tảng (platform), và kết nối (connection path)." | "Wheel" là từ dễ gây hiểu lầm; mức tương thích cụ thể cho từng loại. |
| "Vặn lực xoắn tới 8 Nm đi." | "Set thông số FFB theo hướng dẫn; base/QR sẽ tự điều chỉnh giới hạn mô-men xoắn cho bạn." | Các chỉ số thông số phần trăm (%) và lực kéo vật lý tính bằng mô men xoắn sẽ khác nhau. |
| "Tính năng tương thích Xbox nằm ở base." | "Hệ phần cứng cấp phép cho nền tảng Xbox nằm trong Fanatec steering wheel hoặc bộ hub có bản quyền (licensed hub)." | Đề phòng mua sai thông tin dẫn tới cắm sai. [F2] |
| "Tính năng tương thích PlayStation nằm ở wheel." | "Bản quyền cấp phép PlayStation nằm trong Fanatec wheel base." | Đề phòng mua sai thiết bị dẫn tới cắm nhầm. [F2] |
| "Loại ngàm QR2 Lite lúc nào cũng giảm/giới hạn lực kéo." | "Mức hỗ trợ của QR2 Lite sẽ bị phục thuộc vô sự phê duyệt thông qua thông số bánh lái (wheel) cùng phiên bản firmware hiện tại." | Các sản phẩm nhất định sẽ có sự thay đổi khác biệt. [F4] |
| "Bóng đèn LEDs bị hư rồi." | "Các dải LED có sáng lên không khi Test (thử) bằng Fanatec App, và tựa game/nền tảng này có hỗ trợ cấp tín hiệu telemetry không?" | Phân chia khu vực giới hạn khả năng hoạt động trên phần cứng từ thiết bị (hardware) tránh liên lụy sang game. [F7] |
| "Cập nhật khiến máy thành cục gạch (bricked)." | "Thiết bị không được phát hiện sau khi hoàn tất update; hãy rà soát tính khôi phục (recovery)/chế độ (bootloader)." | Tránh đưa ra kết luận nhanh dẫn tới hiểu lầm rủi ro hư hỏng (permanent damage) khó gỡ. |

## Tài liệu tham khảo

### Nguồn chính thức từ Fanatec

- **[F1]** [Sơ đồ Hệ sinh thái Fanatec](https://help.fanatec.com/hc/de/articles/43786297099281-Fanatec-Ecosystem-Diagramm) — các mối liên hệ danh mục.
- **[F2]** [Giải thích tính tương thích nền tảng](https://www.fanatec.com/us-en/platforms) — nhãn tính tương thích và vị trí cấp phép.
- **[F3]** [So sánh các hub steering wheel của Fanatec](https://www.fanatec.com/jp/en/explorer/products/steering-wheel/fanatec-hubs-a-comparison/) — công dụng hub và điểm khác biệt.
- **[F4]** [Cập nhật mô-men xoắn của QR2 Lite](https://www.fanatec.com/eu/en/explorer/products/steering-wheel/qr2-lite-torque-limit-lifted/) và [Hướng dẫn chuyển đổi QR2](https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) — Các biến thể QR và những cảnh báo hiện tại.
- **[F5]** [Tham số Tuning Menu](https://help.fanatec.com/hc/en-us/articles/43901256649233-In-the-Tuning-Menu-of-your-wheel-base-you-can-adjust-a-variety-of-parameters) — tên gọi cài đặt và chế độ.
- **[F6]** [Bàn đạp trang bị load cell, Hall, và thủy lực](https://www.fanatec.com/us/en/explorer/products/pedals/difference-load-cell-hall-sensor-and-hydraulic-pedals/) — các loại cảm biến bàn đạp.
- **[F7]** [Kích hoạt RevLED và FlagLED](https://help.fanatec.com/hc/en-us/articles/30312122625553-How-do-I-activate-the-RevLEDs-or-flag-LEDs-on-my-wheel) và [Hướng dẫn FlagLED](https://www.fanatec.com/au/en/explorer/products/steering-wheel/understanding-fanatec-steering-wheel-flagleds/) — các sự phụ thuộc của game/nền tảng.
- **[F8]** [Hướng dẫn ClubSport Shifter SQ V1.5](https://www.fanatec.com/us/en/explorer/products/shifters/guide-to-fanatecs-clubsport-shifter-sq-v15/) và [hướng dẫn cổng shifter-port](https://help.fanatec.com/hc/en-us/articles/45597346898449-Which-shifter-port-should-I-use-on-my-Fanatec-wheel-base) — các chế độ, căn chỉnh, cổng.
- **[F9]** [Hướng dẫn ClubSport USB Adapter](https://www.fanatec.com/ca/en/explorer/products/handbrakes/guide-to-fanatecs-clubsport-usb-adapter/) — cách sử dụng standalone (riêng biệt) và chế độ adapter.
- **[F10]** [Sách Hướng dẫn Driver và Firmware](https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/Driver-Firmware-Instructions-Manual-EN_Web_02_MO.pdf) — thuật ngữ về update.
- **[F11]** [Giải thích Natural Friction](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/nfr-natural-friction-tuning-menu/) — cơ chế của NFR.
- **[F12]** [Hướng dẫn về hiện tượng dao động vô lăng (oscillation)](https://help.fanatec.com/hc/en-us/articles/30312108300177-Why-is-my-steering-wheel-oscillating-or-shaking) — hiện tượng dao động và các thiết lập.
- **[F13]** [Hướng dẫn CSL DD Wheel Base](https://www.fanatec.com/au/en/explorer/products/racing-wheels-wheel-bases/fanatec-csl-dd-wheel-base/) — các cấu hình 5 Nm và 8 Nm.
- **[F14]** [So sánh Podium DD1 và DD2](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/podium-dd1-vs-dd2-differences/) — Podium thế hệ trước, mô-men xoắn đỉnh (peak torque), tính năng, và tương thích nền tảng.
- **[F15]** [Cập nhật mô-men xoắn ClubSport DD và DD+](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/more-torque-same-hardware/) và [Trang sản phẩm ClubSport DD+](https://www.fanatec.com/us/en/p/wheel-bases/cs_dd%2B_us/clubsport-dd-plus-eu) — thông số mô-men xoắn duy trì (holding-torque) 15 Nm và 18 Nm hiện hành cùng yêu cầu về firmware.
- **[F16]** [So sánh Podium DD, DD1, và DD2](https://www.fanatec.com/ca/en/explorer/products/racing-wheels-wheel-bases/fanatec-podium-dd-vs-dd1-vs-dd2-key-differences/) — kiến trúc Podium DD hiện tại, mô-men xoắn duy trì (holding torque), và mức vọt qua đỉnh (peak overshoot).
- **[F17]** [Fanatec Steering Wheel FAQ](https://help.fanatec.com/hc/en-us/articles/43802514108433-Steering-Wheel-FAQ) — mặc định QR2 và ngày ngưng sản xuất QR1.
- **[F18]** [FullForce chính thức ra mắt trên CSL DD và Gran Turismo DD Pro](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/fullforce-arrives-on-csl-dd-and-gran-turismo-dd-pro/) — đợt triển khai FullForce vào tháng 6 năm 2026 và kiến trúc thế hệ mới nhất.
- **[F19]** [CSL có nghĩa là gì?](https://help.fanatec.com/hc/de/articles/30312787274641-What-does-CSL-mean) — Định nghĩa chính thức của từ viết tắt CSL là ClubSport Light cùng cách xếp loại dòng cơ bản.

### Nguồn tiêu chuẩn

- **[S1]** [Công cụ và đặc tả USB-IF HID](https://www.usb.org/hid) — các thuật ngữ USB input.
- **[S2]** [USB PID Class 1.0](https://www.usb.org/sites/default/files/documents/pid1_01.pdf) — mô hình USB FFB.

### Tài liệu bổ sung từ cộng đồng

Những tài liệu này hỗ trợ cho việc làm quen nhưng không thể dùng làm nguồn xác thực cho các quyết định về sự tương thích hay độ an toàn hiện hành.

- [OC Racing: Giải thích về Hệ sinh thái Fanatec](https://ocracing.com/guides/fanatec-ecosystem-explained-for-dummies/)
- [Sim Racing Setups: Giải thích Hệ sinh thái Fanatec](https://simracingsetup.com/product-guides/fanatec-ecosystem-explained/)
- [Chỉ mục nghiên cứu dành cho nhà phát triển nội bộ](./README.md)
- [Sổ đăng ký nguồn hệ sinh thái và ghi chú tính hiện hành](./references.md)

## Các Bước Hành động Tiếp theo

1. Sử dụng bộ từ vựng này vào trong các kịch bản trả lời (support scripts), các mẫu biểu mẫu ticket (ticket templates), FAQs, và chi tiết đặc tả kĩ thuật (specifications).
2. Hãy tách riêng các ma trận về tính tương thích SKU; bảng thuật ngữ không nên sao chép các số liệu tương thích thay đổi liên tục.
3. Kiểm tra lại thông tin trên trang chính thức về cấu hình độ tương thích, QR, ứng dụng Fanatec App, và Tuning Menu trước những đợt xuất bản lớn.
4. Ghi lại các cách dùng từ mà khách hàng hay xài dưới dạng bí danh (alias) chỉ sau khi bạn đã quy chiếu nó đúng vào một thuật ngữ cấu thành xác thực (exact component term).
