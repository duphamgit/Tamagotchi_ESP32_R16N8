#include <Arduino.h> 
#include <TFT_eSPI.h> 
#include "SystemMonitor.h" 
#include "InputManager.h"  
#include "ButtonManager.h" // Chứa enum ButtonIndex
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
    STATE_MENU,       // Trạng thái hiện tại: Đang ở Menu
    STATE_FEED,
    STATE_PLAY,
    STATE_CLEAN,
    STATE_STATUS      
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
SystemMonitor monitor(tft);
InputManager touchButton(TOUCH_BUTTON_PIN); 
ButtonManager physicalButtons;              
LEDManager ledControl;
MenuManager menuManager(tft); 

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
DisplayState previousState = STATE_MENU; 

// TRACKING TRẠNG THÁI CHO ONE-SHOT (Tạm thời)
// Kích thước mảng phải đủ lớn cho 4 nút (BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT)
bool physicalButtonLastState[4] = {false, false, false, false};


/**
 * @brief Hàm trợ giúp để mô phỏng checkButton (One-Shot) cho ButtonManager.
 * @param buttonIndex Index của nút cần kiểm tra (BTN_UP, BTN_DOWN...).
 * @return true nếu nút VỪA được nhấn (rising edge), false nếu không.
 */
// 🌟 ĐÃ SỬA: Thay int bằng ButtonIndex để khớp với ButtonManager::isPressed() 🌟
bool checkPhysicalButtonOneShot(ButtonIndex buttonIndex) {
    // Ép kiểu ButtonIndex thành int để truy cập mảng tracking
    int index = (int)buttonIndex;

    // 1. Lấy trạng thái hiện tại (giữ) từ ButtonManager
    bool currentState = physicalButtons.isPressed(buttonIndex);
    
    // 2. So sánh: Vừa nhấn = Đang nhấn VÀ Lần trước KHÔNG nhấn
    bool justPressed = (currentState == true && physicalButtonLastState[index] == false);
    
    // 3. Cập nhật trạng thái cũ cho lần lặp tiếp theo
    physicalButtonLastState[index] = currentState; 
    
    return justPressed;
}

// --- SETUP ---
void setup() {
    Serial.begin(115200);
    delay(1000); 
    
    monitor.begin(); 
    touchButton.begin();
    physicalButtons.begin(); 
    
    // KHỞI TẠO VÀ VẼ MENU 
    menuManager.drawMenu(); 
    
    ledControl.begin(); 
}

// --- LOOP (Người Điều Phối) ---
void loop() {
    // 1. CẬP NHẬT TRẠNG THÁI NÚT VÀ TOUCH
    physicalButtons.update(); 
    touchButton.checkButton(); 

    // 2. XỬ LÝ INPUT ĐIỀU HƯỚNG (NAVIGATION) - Dùng hàm trợ giúp One-Shot
    
    if (checkPhysicalButtonOneShot(BTN_UP)) { 
        menuManager.handleInput(BTN_UP);
    } else if (checkPhysicalButtonOneShot(BTN_DOWN)) { 
        menuManager.handleInput(BTN_DOWN);
    } else if (checkPhysicalButtonOneShot(BTN_LEFT)) { 
        menuManager.handleInput(BTN_LEFT);
    } else if (checkPhysicalButtonOneShot(BTN_RIGHT)) { 
        menuManager.handleInput(BTN_RIGHT);
    } 
    
    // 3. NÚT CHỌN (SELECT/ENTER) - Dùng Touch Button
    if (touchButton.checkButton()) { 
        int selectedID = menuManager.getSelectedItem();
        const char* selectedLabel = menuManager.getItemLabel(selectedID); 
        
        Serial.printf("ACTION: Selected Item (ID %d): %s\n", selectedID, selectedLabel);
        
        // --- HIỂN THỊ HÀNH ĐỘNG CHỌN ---
        tft.fillRect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, TFT_BLACK); 
        tft.setCursor(MSG_X, MSG_Y); 
        tft.setTextSize(2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.printf("SELECTED: %s", selectedLabel); 

        // Giữ thông báo trong thời gian ngắn và vẽ lại Menu
        delay(1000); 
        menuManager.drawMenu(); 
    } 

    // 4. DEBUG RAW STATE (Giữ nguyên)
    int rawState = digitalRead(TOUCH_BUTTON_PIN);
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 200); 
    tft.printf("RAW State (Pin %d): %d", TOUCH_BUTTON_PIN, rawState);
    
    // 5. Chạy hiệu ứng LED
    delay(10); 
    ledControl.runGreenFade(); 
}