#include "WifiManager.h"
#include "ButtonManager.h" // Cần để truy cập ButtonManager::BUTTON_PINS

// Khởi tạo các màu sắc cần dùng (đã sửa TFT_DARKGRAY -> TFT_DARKGREY)
#define COLOR_WIFI_BG TFT_BLACK
#define COLOR_WIFI_TITLE TFT_DARKGREEN
#define COLOR_WIFI_TEXT TFT_DARKGREEN
#define COLOR_WIFI_EXIT TFT_CYAN

WifiManager::WifiManager(TFT_eSPI& displayRef) : tft(displayRef) {
    // Constructor
}

void WifiManager::begin() {
    Serial.println("WifiManager: Initialized. Ready to connect or scan.");
    // Thực hiện bất kỳ logic khởi tạo Wi-Fi nào ở đây, ví dụ:
    // WiFi.mode(WIFI_STA); 
}

void WifiManager::drawScreen() {
    // 1. Xóa màn hình với màu nền
    tft.fillScreen(COLOR_WIFI_BG); 
    
    // 2. Vẽ tiêu đề
    tft.setCursor(15, 10);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_WIFI_TITLE);
    tft.println("--- WIFI SETUP ---");

    // 3. Vẽ trạng thái
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WIFI_TEXT);
    tft.setCursor(20, 40);
    tft.println("\n\nDang Quet Mang...");
    
    // 4. Hướng dẫn thoát (Dùng hằng số từ ButtonManager)
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WIFI_EXIT);
    tft.setCursor(25, tft.height() - 15);
    // Sử dụng ButtonManager::BUTTON_PINS để lấy GPIO của BTN_MENU
    tft.printf("Nhan BTN_MENU (GPIO %d) de Thoat", ButtonManager::BUTTON_PINS[BTN_MENU]);
}

void WifiManager::updateScreen() {
    // Logic cập nhật màn hình Wi-Fi theo thời gian thực (ví dụ: cuộn danh sách mạng, cập nhật IP)
    // Hiện tại để trống, sẽ được phát triển sau.
}
