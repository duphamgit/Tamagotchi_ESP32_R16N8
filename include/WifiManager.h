#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>

/**
 * @brief Class quản lý tất cả các chức năng liên quan đến Wi-Fi và hiển thị màn hình Wi-Fi.
 */
class WifiManager {
private:
    TFT_eSPI& tft; // Tham chiếu đến đối tượng TFT để vẽ

public:
    /**
     * @brief Constructor.
     * @param displayRef Tham chiếu đến đối tượng TFT_eSPI đã được khởi tạo.
     */
    WifiManager(TFT_eSPI& displayRef);

    /**
     * @brief Hàm bắt đầu kết nối/quét Wi-Fi (nếu cần).
     * Đây là nơi bạn có thể đặt logic WiFi.begin() hoặc WiFi.scan().
     */
    void begin();

    /**
     * @brief Vẽ toàn bộ giao diện màn hình Wi-Fi.
     */
    void drawScreen();

    /**
     * @brief Cập nhật logic trên màn hình Wi-Fi (ví dụ: hiển thị tiến trình quét mạng).
     */
    void updateScreen();
};

#endif // WIFI_MANAGER_H
