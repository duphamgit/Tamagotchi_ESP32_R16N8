#include <Arduino.h> 
#include <TFT_eSPI.h> 
#include "SystemMonitor.h" 
#include "InputManager.h"  
#include "ButtonManager.h" 
#include "LEDManager.h" 
#include "MenuManager.h" 

// --- KHAI BÁO CÁC HẰNG SỐ HIỂN THỊ TOÀN CỤC ---
const int MSG_X = 5;
const int MSG_Y = 165; 
const int MSG_WIDTH = 230; 
const int MSG_HEIGHT = 20; 

// --- KHAI BÁO THÔNG SỐ PHẦN CỨNG TOÀN CỤC ---
const int TOUCH_BUTTON_PIN = 13; 

// --- KHAI BÁO ENUM CHO TRẠNG THÁI HIỂN THỊ ---
enum DisplayState {
    STATE_MENU,       
    STATE_MONITOR,    // Màn hình System Monitor
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
SystemMonitor monitor(tft);
InputManager touchButton(TOUCH_BUTTON_PIN); 
ButtonManager physicalButtons;              
LEDManager ledControl;
MenuManager menuManager(tft); 

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
DisplayState currentState = STATE_MENU; // Bắt đầu ở trang Menu

// ❌ ĐÃ LOẠI BỎ LOGIC HẸN GIỜ CẬP NHẬT (lastMonitorUpdateTime và monitorUpdateInterval) ❌

// 🌟 LOGIC ONE-SHOT BẰNG HÀM TRỢ GIÚP 🌟
bool physicalButtonLastState[BTN_COUNT] = {false, false, false, false}; 

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
    
    touchButton.begin();
    physicalButtons.begin(); 
    
    // 1. Khởi tạo màn hình
    tft.init();
    tft.setRotation(0); 
    
    // 2. Vẽ Menu lần đầu tiên
    menuManager.drawMenu(); 
    
    ledControl.begin(); 
}

// --- LOOP (Người Điều Phối) ---
void loop() {
    // 1. CẬP NHẬT TRẠNG THÁI NÚT VÀ TOUCH
    physicalButtons.update(); 
    touchButton.checkButton(); 
    
    // 2. XỬ LÝ NÚT MENU (CHUYỂN TRANG)
    if (checkPhysicalButtonOneShot(BTN_MENU)) {
        if (currentState == STATE_MENU) {
            // 🌟 CHỈ CẬP NHẬT 1 LẦN KHI CHUYỂN TRANG 🌟
            currentState = STATE_MONITOR;
            tft.fillScreen(TFT_BLACK); // Xóa màn hình khi chuyển trang
            
            // 🌟 VẼ TIÊU ĐỀ MONITOR 🌟
            tft.setTextSize(2); 
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.setCursor(10, 5); 
            tft.println("SYSTEM MONITOR");
            
            monitor.displayMemoryStatus(); // Vẽ dữ liệu hệ thống LẦN DUY NHẤT
            Serial.println("State Changed: MENU -> MONITOR (One-time update)");
        } else if (currentState == STATE_MONITOR) {
            // Đang ở MONITOR -> Chuyển về MENU
            currentState = STATE_MENU;
            menuManager.drawMenu(); // Vẽ lại Menu
            Serial.println("State Changed: MONITOR -> MENU");
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
        
        // --- Xử lý NÚT CHỌN (SELECT) ---
        if (checkPhysicalButtonOneShot(BTN_SELECT)) { 
            int selectedID = menuManager.getSelectedItem();
            const char* selectedLabel = menuManager.getItemLabel(selectedID); 
            
            Serial.printf("ACTION: Selected Item (ID %d): %s\n", selectedID, selectedLabel);
            
            // --- HIỂN THỊ HÀNH ĐỘNG CHỌN ---
            tft.fillRect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, TFT_BLACK); 
            tft.setCursor(MSG_X, MSG_Y); 
            tft.setTextSize(2);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.printf("SELECTED: %s", selectedLabel); 

            delay(1000); 
            menuManager.drawMenu(); 
        } 
    } 
    // ❌ KHÔNG CÓ ELSE IF (currentState == STATE_MONITOR) NỮA ❌
    
    // 4. CHẠY CÁC CHỨC NĂNG NỀN VÀ HIỆU ỨNG
    delay(10); 
    ledControl.runGreenFade(); 
}