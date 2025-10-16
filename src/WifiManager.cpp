#include "WifiManager.h"
#include <WiFi.h>

// --- CONSTRUCTOR ---
WifiManager::WifiManager(TFT_eSPI& displayRef) : tft(displayRef) {
    apCount = 0;
    scrollIndex = 0;
    selectedIndex = -1; // -1 cho trạng thái chưa chọn
}

void WifiManager::begin() {
    WiFi.mode(WIFI_STA); // Đảm bảo ở chế độ Station
    Serial.println("WifiManager initialized. WiFi set to STA mode.");
}

// --- HÀM VẼ TỪNG MỤC MẠNG ---
/**
 * @brief Vẽ một mục mạng Wi-Fi (Access Point) tại vị trí Y trên màn hình.
 * @param apIndex Index của AP trong mảng aps[] (0 đến apCount-1).
 * @param screenY Tọa độ Y trên màn hình để vẽ mục này.
 * @param isSelected Trạng thái chọn (true/false).
 */
void WifiManager::drawAPItem(int apIndex, int screenY, bool isSelected) {
    if (apIndex >= apCount || apIndex < 0) return;

    // Chiều rộng và chiều cao được định nghĩa trong WifiManager.h
    int w = DISPLAY_W;
    int h = ITEM_H;

    // Màu sắc
    uint16_t bgColor = isSelected ? TFT_DARKGREEN : TFT_BLACK;
    uint16_t textColor = isSelected ? TFT_DARKGREEN : TFT_DARKGREEN;
    
    // Xóa vùng vẽ cũ
    tft.fillRect(0, screenY, w, h, bgColor); 

    // 1. Số thứ tự (01.)
    tft.setCursor(5, screenY + 3);
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN);
    // Sử dụng apIndex + 1 để hiển thị số thứ tự từ 1
    tft.printf("%02d.", apIndex + 1);

    // 2. Tên SSID
    tft.setCursor(45, screenY + 3);
    // Cắt bớt tên SSID nếu quá dài
    String ssid = aps[apIndex].ssid;
    if (ssid.length() > 18) {
        ssid = ssid.substring(0, 15) + "...";
    }
    tft.print(ssid);

    // 3. Cường độ sóng (RSSI)
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(w - 45, screenY + 3);
    tft.printf("%d dBm", aps[apIndex].rssi);

    // 4. Mã hóa 
    tft.setCursor(w - 45, screenY + 12);
    
    // Thay thế bằng một giả định đơn giản dựa trên tên:
    if (aps[apIndex].rssi < -80) {
        tft.print("Weak");
    } else if (aps[apIndex].rssi > -50) {
        tft.print("Strong");
    } else {
        tft.print("Normal");
    }

    // Vẽ đường chia nếu không phải là mục cuối cùng của màn hình
    if (screenY + h < END_Y) {
        tft.drawFastHLine(0, screenY + h - 1, w, TFT_DARKGREY);
    }
}


// --- HÀM VẼ KHUNG TIÊU ĐỀ ---
void WifiManager::drawHeader() {
    tft.fillRect(0, 0, DISPLAY_W, HEADER_H, TFT_BLACK);
    
    // Căn giữa tiêu đề
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    tft.setTextDatum(MC_DATUM); 
    tft.drawString("WIFI SCANNER", DISPLAY_W / 2, HEADER_H / 2);

    // Thiết lập lại điểm neo về góc trên bên trái
    tft.setTextDatum(TL_DATUM); 
}

// --- HÀM VẼ KHUNG CHÂN TRANG ---
void WifiManager::drawFooter() {
    tft.fillRect(0, END_Y, DISPLAY_W, FOOTER_H, TFT_BLACK);
    tft.setCursor(17, END_Y + 3);
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN, TFT_GREEN);
    
    // Hiển thị thông tin tổng quan
    int maxScroll = apCount > MAX_AP_DISPLAY ? apCount - MAX_AP_DISPLAY : 0;
    tft.printf("Total: %d | Scroll: %d/%d | Sel: %d", apCount, scrollIndex, maxScroll, selectedIndex + 1);
}

// --- HÀM VẼ TOÀN BỘ DANH SÁCH AP ---
void WifiManager::drawAPList() {
    // Xóa vùng danh sách (CẦN thiết khi có SCROLL hoặc lần đầu vẽ)
    tft.fillRect(0, START_Y, DISPLAY_W, END_Y - START_Y, TFT_BLACK);

    if (apCount <= 0) {
        tft.setCursor(5, START_Y + 10);
        tft.setTextSize(1);
        tft.setTextColor(TFT_RED);
        tft.print("No Networks Found!");
        return;
    }

    // Vòng lặp chỉ vẽ các mục hiển thị được trên màn hình
    int visibleCount = min(apCount - scrollIndex, (int)MAX_AP_DISPLAY);

    for (int i = 0; i < visibleCount; i++) {
        // apIndex: Index thực tế trong mảng aps[]
        int apIndex = scrollIndex + i; 
        // screenY: Tọa độ Y trên màn hình
        int screenY = START_Y + (i * ITEM_H); 
        
        // Vẽ mục
        drawAPItem(apIndex, screenY, (apIndex == selectedIndex));
    }
}

// --- HÀM VẼ TOÀN BỘ MÀN HÌNH (KHÔNG QUÉT) ---
void WifiManager::drawScreen() {
    tft.fillScreen(TFT_BLACK);
    drawHeader();
    drawAPList(); // Vẽ danh sách hiện tại (có thể rỗng)
    drawFooter();
}


// --- HÀM THỰC HIỆN QUÉT MẠNG ---
void WifiManager::scanNetworks() {
    // Hiển thị trạng thái quét
    tft.fillRect(0, START_Y, DISPLAY_W, END_Y - START_Y, TFT_BLACK);
    tft.setCursor(10, START_Y + 50);
    tft.setTextSize(2);
    tft.setTextColor(TFT_ORANGE);
    tft.print("Scanning Networks...");
    
    // Thực hiện quét và lấy số lượng mạng
    int newApCount = WiFi.scanNetworks(false, true); // (async=false, show_hidden=true)

    // Cập nhật lại thông tin
    apCount = 0;
    scrollIndex = 0;
    selectedIndex = -1;

    if (newApCount > 0) {
        for (int i = 0; i < newApCount && i < 60; i++) {
            aps[i] = {
                WiFi.SSID(i),
                WiFi.RSSI(i),
                WiFi.channel(i)
            };
            apCount++;
        }
        selectedIndex = 0; // Chọn mục đầu tiên nếu có mạng
    }

    // Vẽ lại toàn bộ màn hình với danh sách mới
    drawScreen();
    
    Serial.printf("Scan completed. Found %d networks.\n", apCount);
}

// --- HÀM XỬ LÝ INPUT (CUỘN) ---
void WifiManager::handleInput(ButtonIndex pressedIndex) {
    if (apCount <= 0) return;

    int oldSelectedIndex = selectedIndex;
    int oldScrollIndex = scrollIndex;
    bool needsUpdate = false;

    // 1. Cập nhật selectedIndex
    if (pressedIndex == BTN_UP) {
        selectedIndex = max(0, selectedIndex - 1);
        needsUpdate = true;
    } else if (pressedIndex == BTN_DOWN) {
        selectedIndex = min(apCount - 1, selectedIndex + 1);
        needsUpdate = true;
    }

    if (needsUpdate) {
        // 2. Cập nhật scrollIndex để mục được chọn nằm trong khung nhìn
        
        // Nếu mục được chọn nằm ngoài giới hạn dưới cùng của khung nhìn (cần cuộn xuống)
        if (selectedIndex >= scrollIndex + MAX_AP_DISPLAY) {
            scrollIndex = selectedIndex - MAX_AP_DISPLAY + 1;
        } 
        // Nếu mục được chọn nằm ngoài giới hạn trên cùng của khung nhìn (cần cuộn lên)
        else if (selectedIndex < scrollIndex) {
            scrollIndex = selectedIndex;
        }

        // Đảm bảo scrollIndex không vượt quá giới hạn trên cùng (0)
        scrollIndex = max(0, scrollIndex);

        // 3. Nếu có thay đổi, vẽ lại danh sách
        if (oldSelectedIndex != selectedIndex || oldScrollIndex != scrollIndex) {
            
            // 🌟 LOGIC KHẮC PHỤC FLICKER 🌟
            if (oldScrollIndex == scrollIndex) {
                // Trường hợp 1: KHÔNG cuộn trang (chỉ thay đổi highlight)
                // Ta chỉ cần vẽ lại hai mục thay vì toàn bộ 12 mục
                
                // Vị trí trên màn hình của mục cũ và mục mới
                int oldScreenIndex = oldSelectedIndex - scrollIndex;
                int newScreenIndex = selectedIndex - scrollIndex;
                
                // Redraw mục cũ (xóa highlight)
                drawAPItem(oldSelectedIndex, START_Y + (oldScreenIndex * ITEM_H), false);
                
                // Redraw mục mới (thêm highlight)
                drawAPItem(selectedIndex, START_Y + (newScreenIndex * ITEM_H), true);
                
                drawFooter(); // Vẫn cần cập nhật footer (chỉ thay đổi số Sel:)
            } else {
                // Trường hợp 2: Có CUỘN trang (scrollIndex thay đổi)
                // Cần redraw toàn bộ danh sách hiển thị
                drawAPList();
                drawFooter();
            }
        }
    }
}
void WifiManager::drawInputPasswordScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 100);
    tft.setTextSize(2); 
    tft.printf("Input Wi-Fi passwords page."); 
}

void WifiManager::drawConnectingScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 100);
    tft.setTextSize(2); 
    tft.setTextColor(TFT_YELLOW);
    tft.printf("Connecting to Wi-Fi...");
}
