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

// --- KHAI BÁO ENUM CHO TRẠNG THÁI HIỂN THỊ ---(Giữ nguyên)
enum DisplayState {
    STATE_MENU,       
    STATE_FEED,
    STATE_PLAY,
    STATE_CLEAN,
    STATE_STATUS      
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
SystemMonitor monitor(tft);
InputManager touchButton(TOUCH_BUTTON_PIN); // Giữ lại cho mục đích debug/dùng Touch cho việc khác
ButtonManager physicalButtons;              
LEDManager ledControl;
MenuManager menuManager(tft); 

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
DisplayState previousState = STATE_MENU; 

// 🌟 LOGIC ONE-SHOT BẰNG HÀM TRỢ GIÚP 🌟
bool physicalButtonLastState[BTN_COUNT] = {false, false, false, false}; // Sử dụng BTN_COUNT

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

    // 2. XỬ LÝ INPUT ĐIỀU HƯỚNG (NAVIGATION) - Chỉ dùng UP và DOWN
    
    // BTN_UP: Di chuyển Lùi
    if (checkPhysicalButtonOneShot(BTN_UP)) { 
        menuManager.handleInput(BTN_UP);
    } 
    // BTN_DOWN: Di chuyển Tiến
    else if (checkPhysicalButtonOneShot(BTN_DOWN)) { 
        menuManager.handleInput(BTN_DOWN);
    } 
    
    // 3. NÚT CHỌN (SELECT/ENTER) - Dùng Nút vật lý BTN_SELECT
    // 🌟 ĐÃ SỬA: Thay Touch Button bằng BTN_SELECT 🌟
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