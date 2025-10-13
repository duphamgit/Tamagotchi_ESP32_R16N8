#include <Arduino.h> 
#include <TFT_eSPI.h> 
#include "SystemMonitor.h" 
#include "InputManager.h"  
#include "ButtonManager.h" 
#include "LEDManager.h" 
#include "MenuManager.h" 
// 🌟 THÊM WIFI MANAGER MỚI 🌟
#include "WifiManager.h" 


// --- KHAI BÁO CÁC HẰNG SỐ HIỂN THỊ TOÀN CỤC ---
const int MSG_X = 5;
const int MSG_Y = 165; 
const int MSG_WIDTH = 230; 
const int MSG_HEIGHT = 20; 

// --- KHAI BÁO THÔNG SỐ PHẦN CỨNG TOÀN CỤC ---
const int TOUCH_BUTTON_PIN = 13; 

// --- KHAI BÁO ENUM CHO TRẠNG THÁI HIỂN THỊ ---
// Thêm STATE_WIFI để quản lý màn hình Wi-Fi
enum DisplayState {
    STATE_MENU,       // Trạng thái hiện tại: Đang ở Menu chính
    STATE_MONITOR,    // Màn hình System Monitor
    STATE_WIFI        // Trạng thái cho màn hình cấu hình WIFI
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
SystemMonitor monitor(tft);
InputManager touchButton(TOUCH_BUTTON_PIN); 
ButtonManager physicalButtons;              
LEDManager ledControl;
MenuManager menuManager(tft); 
// 🌟 KHAI BÁO ĐỐI TƯỢNG WIFI MANAGER 🌟
WifiManager wifiManager(tft);

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
DisplayState currentState = STATE_MENU; // Bắt đầu ở trang Menu

// 🌟 LOGIC ONE-SHOT BẰNG HÀM TRỢ GIÚP 🌟
bool physicalButtonLastState[BTN_COUNT] = {false, false, false, false}; 

/**
 * @brief Kiểm tra xem một nút vật lý có vừa được nhấn trong vòng lặp này hay không (one-shot logic).
 * @param buttonIndex Index của nút (BTN_UP, BTN_DOWN, BTN_SELECT, BTN_MENU).
 * @return true nếu nút vừa được nhấn, false nếu ngược lại.
 */
bool checkPhysicalButtonOneShot(ButtonIndex buttonIndex) {
    int index = (int)buttonIndex;
    if (index < 0 || index >= BTN_COUNT) return false; 

    bool currentState = physicalButtons.isPressed(buttonIndex);
    
    // So sánh: Vừa nhấn = Đang nhấn VÀ Lần trước KHÔNG nhấn
    bool justPressed = (currentState == true && physicalButtonLastState[index] == false);
    
    // Cập nhật trạng thái cũ cho lần lặp tiếp theo
    physicalButtonLastState[index] = currentState; 
    
    return justPressed;
}


// --- SETUP ---
void setup() {
    Serial.begin(115200);
    delay(1000); 
    
    physicalButtons.begin(); 
    
    // 1. Khởi tạo màn hình
    tft.init();
    tft.setRotation(0); 
    
    // 2. Khởi tạo các Manager khác
    wifiManager.begin(); // Khởi tạo WifiManager
    menuManager.drawMenu(); 
    
    ledControl.begin(); 
}

// --- LOOP (Người Điều Phối) ---
void loop() {
    // 1. CẬP NHẬT TRẠNG THÁI NÚT VÀ TOUCH
    physicalButtons.update(); 
    touchButton.checkButton(); 
    
    // 2. XỬ LÝ QUAY LẠI MENU BẰNG BTN_MENU (Từ bất kỳ màn hình con nào)
    if (currentState != STATE_MENU) {
        if (checkPhysicalButtonOneShot(BTN_MENU)) {
            currentState = STATE_MENU;
            menuManager.drawMenu(); // Vẽ lại Menu
            Serial.println("State Changed: Sub-Screen -> MENU");
            return; // Thoát khỏi loop hiện tại để tránh xử lý tiếp
        }
    }
    
    // 3. XỬ LÝ LOGIC DỰA TRÊN TRẠNG THÁI HIỆN TẠI
    
    if (currentState == STATE_MENU) {
        // --- Xử lý điều hướng UP/DOWN ---
        if (checkPhysicalButtonOneShot(BTN_UP)) { 
            menuManager.handleInput(BTN_UP);
        } else if (checkPhysicalButtonOneShot(BTN_DOWN)) { 
            menuManager.handleInput(BTN_DOWN);
        } 
        
        // --- Xử lý NÚT CHỌN (BTN_SELECT) ---
        if (checkPhysicalButtonOneShot(BTN_SELECT) || touchButton.checkButton()) { 
            int selectedID = menuManager.getSelectedItem();
            const char* selectedLabel = menuManager.getItemLabel(selectedID); 
            
            Serial.printf("ACTION: Selected Item (ID %d): %s\n", selectedID, selectedLabel);
            
            // 🌟 LOGIC CHUYỂN TRẠNG THÁI MỚI 🌟
            if (selectedID == MENU_WIFI) {
                currentState = STATE_WIFI;
                wifiManager.drawScreen(); // Gọi hàm vẽ màn hình từ WifiManager
                Serial.println("State Changed: MENU -> WIFI");
            } 
            else if (selectedID == MENU_MISC) { 
                currentState = STATE_MONITOR;
                tft.fillScreen(TFT_BLACK); 
                tft.setTextSize(2); 
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.setCursor(10, 5); 
                tft.println("SYSTEM MONITOR (MISC)"); 
                monitor.displayMemoryStatus();
                Serial.println("State Changed: MENU -> MONITOR (via MISC)");
            }
            // Thêm logic cho các mục khác...
            
            else {
                // Xử lý chung cho các mục Menu chưa được triển khai
                tft.fillRect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, TFT_BLACK); 
                tft.setCursor(MSG_X, MSG_Y); 
                tft.setTextSize(2);
                tft.setTextColor(TFT_GREEN, TFT_BLACK);
                tft.printf("SELECTED: %s", selectedLabel); 
                delay(1000); 
                menuManager.drawMenu(); 
            }
        } 
    } 
    else if (currentState == STATE_WIFI) {
        // Logic màn hình WIFI: chạy cập nhật nền (ví dụ: quét mạng, hiển thị IP)
        wifiManager.updateScreen();
    }
    else if (currentState == STATE_MONITOR) {
        // Logic màn hình MONITOR
    }
    
    // 4. CHẠY CÁC CHỨC NĂNG NỀN VÀ HIỆU ỨNG
    delay(10); 
    ledControl.runGreenFade(); 
}
