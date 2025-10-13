#include "WifiManager.h"
#include "ButtonManager.h" // Cần để truy cập các hằng số pin
#include <algorithm>     // Cần cho hàm std::min() và std::max()
#include <WiFi.h>        // 🌟 THƯ VIỆN WI-FI THỰC TẾ 🌟

// --- MÀU SẮC & CẤU HÌNH ---
#define COLOR_WIFI_BG TFT_BLACK
#define COLOR_WIFI_TITLE TFT_CYAN
#define COLOR_WIFI_TEXT TFT_WHITE
#define COLOR_WIFI_EXIT TFT_YELLOW
#define COLOR_WIFI_GOOD TFT_GREEN     // RSSI tốt
#define COLOR_WIFI_MEDIUM 0xFEE0      // Màu Cam/Vàng (Trung bình)
#define COLOR_WIFI_WEAK TFT_RED       // RSSI yếu

// Cấu hình RSSI cho Bar Graph
#define RSSI_MAX_DBM -30 // Mốc tín hiệu mạnh nhất (100% Bar)
#define RSSI_MIN_DBM -90 // Mốc tín hiệu yếu nhất (0% Bar)

// --- KHÔNG CÒN SỬ DỤNG MOCK DATA NỮA ---


WifiManager::WifiManager(TFT_eSPI& displayRef) : tft(displayRef) {
    // Không cần logic khởi tạo ở đây.
}

/**
 * @brief Chuyển đổi RSSI (-30 đến -90 dBm) thành màu sắc dựa trên cường độ.
 */
uint16_t getRssiColor(int32_t rssi) {
    if (rssi >= -50) return COLOR_WIFI_GOOD;      // Rất tốt
    if (rssi >= -70) return COLOR_WIFI_MEDIUM;     // Trung bình
    return COLOR_WIFI_WEAK;                       // Yếu
}

void WifiManager::begin() {
    // Khởi tạo Wi-Fi ở chế độ Station (STA) để quét mạng
    WiFi.mode(WIFI_STA);
    Serial.println("WifiManager: Initialized. Starting initial Wi-Fi scan...");
    scanNetworks();
}

/**
 * @brief Vẽ biểu đồ cường độ tín hiệu (Bar Graph).
 * @param rssi Giá trị RSSI.
 * @param x, y Vị trí.
 */
void WifiManager::drawRssiBar(int32_t rssi, int x, int y) {
    // Giới hạn RSSI trong khoảng quan tâm (từ -90 đến -30)
    int32_t limitedRssi = std::max(std::min(rssi, RSSI_MAX_DBM), RSSI_MIN_DBM);

    // Tính toán tỷ lệ phần trăm cường độ (0-100%)
    float rssiRange = (float)(RSSI_MAX_DBM - RSSI_MIN_DBM); // 60
    // Lượng RSSI đã vượt qua mốc yếu nhất
    float currentRssi = (float)(limitedRssi - RSSI_MIN_DBM); 
    
    // Chiều rộng thanh tín hiệu
    int barWidth = (int)((currentRssi / rssiRange) * RSSI_BAR_WIDTH);

    // 1. Vẽ nền thanh (màu xám tối) và khung
    tft.fillRect(x, y, RSSI_BAR_WIDTH, RSSI_BAR_HEIGHT, TFT_DARKGREY);
    tft.drawRect(x, y, RSSI_BAR_WIDTH, RSSI_BAR_HEIGHT, TFT_LIGHTGREY); 

    // 2. Vẽ thanh tín hiệu
    uint16_t barColor = getRssiColor(limitedRssi);
    if (barWidth > 0) {
        // Đảm bảo không vượt quá chiều rộng tối đa và vẽ thanh tín hiệu
        tft.fillRect(x, y, std::min(barWidth, RSSI_BAR_WIDTH), RSSI_BAR_HEIGHT, barColor);
    }
}

/**
 * @brief Vẽ một mục AP (SSID, Bar Graph, Icon).
 */
void WifiManager::drawAPItem(int index, int y) {
    if (index >= numNetworks) return;

    // 1. Xóa dòng
    tft.fillRect(0, y, tft.width(), ROW_HEIGHT, COLOR_WIFI_BG); 
    
    // 2. Vẽ SSID (Tên mạng)
    tft.setCursor(5, y + 8);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WIFI_TEXT);
    tft.print(networks[index].ssid.substring(0, 15)); 

    // 3. Tính toán vị trí cho Graph và RSSI text
    // -30: khoảng trống giữa Bar và RSSI text
    int textX = tft.width() - 5 - 30; // Vị trí cho giá trị dBm (ví dụ: -75)
    int graphX = tft.width() - 5 - RSSI_BAR_WIDTH - 35; // Vị trí cho thanh graph
    int iconX = tft.width() - 20; // Vị trí cho icon khóa/mở

    // 4. Vẽ biểu đồ tín hiệu (Graph)
    drawRssiBar(networks[index].rssi, graphX, y + 9); 

    // 5. Vẽ giá trị RSSI text
    tft.setCursor(textX, y + 8);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WIFI_TEXT);
    tft.printf("%d", networks[index].rssi);

    // 6. Vẽ Icon Khóa/Mở (sử dụng ký tự)
    tft.setCursor(iconX, y + 8);
    tft.setTextSize(2); 
    tft.setTextColor(COLOR_WIFI_TEXT);
    
    // Kiểm tra loại mã hóa. Nếu là WIFI_AUTH_OPEN (0) thì là mạng mở.
    if (networks[index].encryptionType != WIFI_AUTH_OPEN) {
        // Ký tự khóa (Lock Icon)
        tft.print('L'); 
    } else {
        // Ký tự mở (Unlock Icon)
        tft.print('U'); 
    }
}

int WifiManager::scanNetworks() {
    // Sử dụng biến cờ để tránh gọi scanNetworks quá nhiều lần cùng lúc
    if (isScanning) return numNetworks;
    
    isScanning = true;
    
    // Xóa danh sách cũ và hiển thị thông báo "Scanning..."
    tft.fillRect(0, START_Y, tft.width(), tft.height() - START_Y - 40, COLOR_WIFI_BG); 
    tft.setCursor(5, START_Y + 10);
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WIFI_EXIT);
    tft.println("Dang Quet... Vui long doi.");
    
    // 🌟 THỰC HIỆN SCAN WI-FI THỰC TẾ 🌟
    // n = Số lượng mạng tìm thấy
    int n = WiFi.scanNetworks(false, false); 

    numNetworks = 0;
    
    if (n > 0) {
        Serial.printf("Found %d networks.\n", n);
        // Chỉ lưu tối đa MAX_APS_DISPLAY mạng
        int maxIndex = std::min(n, MAX_APS_DISPLAY);
        
        for (int i = 0; i < maxIndex; ++i) {
            // Lấy thông tin mạng bằng các hàm WiFi.
            networks[i].ssid = WiFi.SSID(i);
            networks[i].rssi = WiFi.RSSI(i);
            networks[i].encryptionType = WiFi.encryptionType(i); // Sử dụng enum của WiFi.h
            numNetworks++;
        }
        // Sau khi lấy dữ liệu, giải phóng bộ nhớ của quá trình scan
        WiFi.scanDelete(); 
    } else {
        Serial.println("No networks found.");
    }
    
    isScanning = false;
    
    // Sau khi scan xong, vẽ lại toàn bộ màn hình với kết quả
    drawScreen();

    return numNetworks;
}

void WifiManager::drawScreen() {
    tft.fillScreen(COLOR_WIFI_BG); 
    
    // 1. Vẽ tiêu đề
    tft.setCursor(15, 5);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_WIFI_TITLE);
    tft.println("--- WIFI SCAN ---");
    tft.drawFastHLine(0, 35, tft.width(), COLOR_WIFI_TITLE);

    // 2. Vẽ Danh sách AP
    if (numNetworks > 0) {
        for (int i = 0; i < numNetworks; i++) {
            drawAPItem(i, START_Y + i * ROW_HEIGHT);
        }
    } else {
        // Thông báo nếu không có mạng
        tft.setCursor(5, START_Y + 10);
        tft.setTextSize(2);
        tft.setTextColor(COLOR_WIFI_TEXT);
        if (isScanning) {
             tft.println("Dang Quet... (Vui long doi)");
        } else {
             tft.println("Khong tim thay mang nao.");
        }
    }

    // 3. Hướng dẫn thoát và quét lại
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WIFI_EXIT);
    tft.setCursor(5, tft.height() - 20);
    // Sử dụng ButtonManager::BUTTON_PINS để hiển thị GPIO cho người dùng
    tft.printf("BTN_MENU (GPIO %d) THOAT | BTN_SELECT (GPIO %d) QUET LAI", 
               ButtonManager::BUTTON_PINS[BTN_MENU], ButtonManager::BUTTON_PINS[BTN_SELECT]);
}

void WifiManager::updateScreen() {
    // Đây là nơi có thể thêm logic cập nhật liên tục (ví dụ: trạng thái kết nối)
}
