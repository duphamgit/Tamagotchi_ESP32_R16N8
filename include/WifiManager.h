#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ButtonManager.h" // Cần để sử dụng ButtonIndex

// --- CẤU HÌNH HIỂN THỊ WIFI SCANNER ---
#define DISPLAY_W 240 // Chiều rộng màn hình (chế độ dọc)
#define DISPLAY_H 280 // Chiều cao màn hình (chế độ dọc)
#define HEADER_H 25   // Chiều cao tiêu đề
#define FOOTER_H 20   // Chiều cao chân trang
#define START_Y (HEADER_H + 5) // Tọa độ Y bắt đầu của danh sách
#define END_Y (DISPLAY_H - FOOTER_H) // Tọa độ Y kết thúc của danh sách
#define ITEM_H 22 // Chiều cao mỗi mục (tối ưu hóa cho 12 mục)
#define MAX_AP_DISPLAY ((END_Y - START_Y) / ITEM_H) // Số lượng mục tối đa có thể hiển thị (sẽ là 12)

// Cấu trúc để lưu thông tin mạng Wi-Fi
struct AccessPoint {
    String ssid;
    int rssi;
    int channel;
};


class WifiManager {
private:
    TFT_eSPI& tft;
    AccessPoint aps[60]; // Mảng lớn để lưu trữ kết quả quét
    int apCount = 0;
    int scrollIndex = 0; // Index của mục đầu tiên được hiển thị trên màn hình
    int selectedIndex = 0; // Index của mục hiện đang được chọn

    void drawHeader();
    void drawFooter();
    void drawAPList();
    void drawAPItem(int apIndex, int screenY, bool isSelected);

public:
    WifiManager(TFT_eSPI& displayRef);
    void begin();

    /**
     * @brief Vẽ khung màn hình Wi-Fi, không thực hiện quét mạng.
     */
    void drawScreen();

    /**
     * @brief Thực hiện quét mạng Wi-Fi (Blocking call) và cập nhật danh sách.
     */
    void scanNetworks();

    /**
     * @brief Xử lý input từ các nút bấm (UP, DOWN, SELECT)
     * @param pressedIndex Index của nút đã được nhấn (từ ButtonManager).
     */
    void handleInput(ButtonIndex pressedIndex);
};

#endif // WIFI_MANAGER_H
