#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// Số lượng mạng tối đa hiển thị trên màn hình
#define MAX_APS_DISPLAY 6 

// Chiều cao cố định của thanh biểu đồ RSSI
#define RSSI_BAR_HEIGHT 12

// Cấu trúc để lưu trữ thông tin mạng Wi-Fi
struct ScannedAP {
    String ssid;
    int32_t rssi;
    int encryptionType; // Loại mã hóa
    bool isHidden;
};

/**
 * @brief Class quản lý tất cả các chức năng liên quan đến Wi-Fi và hiển thị màn hình Wi-Fi.
 */
class WifiManager {
private:
    TFT_eSPI& tft; // Tham chiếu đến đối tượng TFT để vẽ
    
    // State variables
    bool isScanning = false;
    int numNetworks = 0;
    // Mảng lưu trữ các mạng đã quét (chỉ lưu MAX_APS_DISPLAY mạng đầu tiên)
    ScannedAP networks[MAX_APS_DISPLAY];

    // Cấu hình vẽ
    const int START_Y = 40;     // Tọa độ Y bắt đầu của danh sách AP
    const int ROW_HEIGHT = 30;  // Chiều cao mỗi dòng AP
    const int RSSI_BAR_WIDTH = 60; // Chiều rộng tối đa của thanh tín hiệu

    /**
     * @brief Vẽ biểu đồ cường độ tín hiệu (Bar Graph) dựa trên giá trị RSSI.
     * @param rssi Giá trị RSSI (dBm).
     * @param x, y Vị trí.
     */
    void drawRssiBar(int32_t rssi, int x, int y);

    /**
     * @brief Vẽ từng mục AP (SSID, RSSI bar, Icon khóa).
     * @param index Vị trí trong mảng networks.
     * @param y Tọa độ Y để vẽ.
     */
    void drawAPItem(int index, int y);

public:
    WifiManager(TFT_eSPI& displayRef);

    /**
     * @brief Khởi tạo Wi-Fi và bắt đầu scan lần đầu.
     */
    void begin();

    /**
     * @brief Quét Wi-Fi và cập nhật mảng networks.
     * @return Số lượng mạng đã tìm thấy.
     */
    int scanNetworks();

    /**
     * @brief Vẽ toàn bộ giao diện màn hình Wi-Fi.
     */
    void drawScreen();

    /**
     * @brief Cập nhật logic trên màn hình Wi-Fi (ví dụ: kết nối, trạng thái).
     */
    void updateScreen();
};

#endif // WIFI_MANAGER_H
