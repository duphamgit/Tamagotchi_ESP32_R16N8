#include <Arduino.h> 
#include <TFT_eSPI.h> 
#include "SystemMonitor.h" 
#include "InputManager.h"  
#include "ButtonManager.h" 
#include "LEDManager.h" 
#include "MenuManager.h" 
#include "WifiManager.h" 


// --- KHAI BÁO CÁC HẰNG SỐ HIỂN THỊ TOÀN CỤC ---
const int MSG_X = 5;
const int MSG_Y = 165; 
const int MSG_WIDTH = 230; 
const int MSG_HEIGHT = 20; 

// --- KHAI BÁO THÔNG SỐ PHẦN CỨNG TOÀN CỤC ---
const int TOUCH_BUTTON_PIN = 13; 

// --- KHAI BÁO ENUM CHO TRẠNG THÁI HIỂN THỊ ---
enum DisplayState {
    STATE_MENU,       // Trạng thái hiện tại: Đang ở Menu chính
    STATE_MONITOR,    // Màn hình System Monitor (sử dụng tạm cho mục MISC)
    STATE_WIFI        // Trạng thái cho màn hình cấu hình WIFI
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
// SystemMonitor cần được giả định tồn tại, nếu không cần phải comment/xóa
// SystemMonitor monitor(tft); 
InputManager touchButton(TOUCH_BUTTON_PIN); 
ButtonManager physicalButtons;              
LEDManager ledControl;
MenuManager menuManager(tft); 
WifiManager wifiManager(tft);

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
DisplayState currentState = STATE_MENU; 

// 🌟 LOGIC ONE-SHOT BẰM HÀM TRỢ GIÚP 🌟
// Mảng lưu trữ trạng thái nhấn cuối cùng (last state) của các nút vật lý
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
    
    // Vừa nhấn = Đang nhấn VÀ Lần trước KHÔNG nhấn
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
    
    // 2. Khởi tạo các Manager
    wifiManager.begin(); // Khởi tạo Wi-Fi và scan lần đầu
    menuManager.drawMenu(); // Vẽ Menu chính ban đầu
    
    ledControl.begin(); 
}

// --- LOOP (Người Điều Phối) ---
void loop() {
    // 1. CẬP NHẬT TRẠNG THÁI NÚT VÀ TOUCH
    physicalButtons.update(); 
    touchButton.checkButton(); 
    
    // 2. XỬ LÝ QUAY LẠI MENU BẰNG BTN_MENU (Từ bất kỳ màn hình con nào)
    // Nếu KHÔNG ở Menu và BTN_MENU được nhấn, chuyển về Menu
    if (currentState != STATE_MENU) {
        if (checkPhysicalButtonOneShot(BTN_MENU)) {
            currentState = STATE_MENU;
            menuManager.drawMenu(); 
            Serial.println("State Changed: Sub-Screen -> MENU (via BTN_MENU)");
            return; 
        }
    }
    
    // 3. XỬ LÝ LOGIC DỰA TRÊN TRẠNG THÁI HIỆN TẠI
    
    if (currentState == STATE_MENU) {
        // --- Xử lý điều hướng UP/DOWN (Di chuyển trong Menu) ---
         if (checkPhysicalButtonOneShot(BTN_UP)) { 
            // Thử gán logic đi XUỐNG cho nút vật lý BTN_UP (ID 0)
            menuManager.handleInput(BTN_UP); 
        } 
        
        if (checkPhysicalButtonOneShot(BTN_DOWN)) { 
            // Thử gán logic đi LÊN cho nút vật lý BTN_DOWN (ID 1)
            menuManager.handleInput(BTN_DOWN); 
        }
        
        // --- Xử lý NÚT CHỌN (BTN_SELECT) hoặc Touch ---
        if (checkPhysicalButtonOneShot(BTN_SELECT) || touchButton.checkButton()) { 
            // Lấy ID mục được chọn để chuyển trạng thái
            int selectedID = menuManager.getSelectedItem();
            const char* selectedLabel = menuManager.getItemLabel(selectedID); 
            
            Serial.printf("ACTION: Selected Item (ID %d): %s\n", selectedID, selectedLabel);
            
            // 🌟 LOGIC CHUYỂN TRẠNG THÁI 🌟
            if (selectedID == MENU_WIFI) {
                currentState = STATE_WIFI;
                wifiManager.drawScreen(); // Vẽ giao diện Wi-Fi
                Serial.println("State Changed: MENU -> WIFI");
            } 
            else if (selectedID == MENU_MISC) { 
                currentState = STATE_MONITOR;
                tft.fillScreen(TFT_BLACK); 
                tft.setTextSize(2); 
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.setCursor(10, 5); 
                tft.println("MISCELLANEOUS (TBD)"); 
                // monitor.displayMemoryStatus(); // Uncomment nếu SystemMonitor được sử dụng
                Serial.println("State Changed: MENU -> MONITOR (MISC)");
            }
            // Thêm logic cho các mục MenuID khác nếu cần...
            
            else {
                // Xử lý chung cho các mục Menu chưa được triển khai
                tft.fillRect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, TFT_BLACK); 
                tft.setCursor(MSG_X, MSG_Y); 
                tft.setTextSize(2);
                tft.setTextColor(TFT_GREEN, TFT_BLACK);
                tft.printf("SELECTED: %s", selectedLabel); 
                delay(1000); 
                menuManager.drawMenu(); // Quay lại Menu
            }
        } 
    } 
    else if (currentState == STATE_WIFI) {
        // Logic màn hình WIFI
        wifiManager.updateScreen();
        
        // 🌟 LOGIC QUÉT LẠI (BTN_SELECT) 🌟
        if (checkPhysicalButtonOneShot(BTN_SELECT)) { 
            Serial.println("WIFI: Re-scanning networks triggered by BTN_SELECT...");
            wifiManager.scanNetworks();
        }
    }
    else if (currentState == STATE_MONITOR) {
        // Logic màn hình MONITOR
        // (Hiện tại chỉ cần chờ BTN_MENU để thoát)
    }
    
    // 4. CHẠY CÁC CHỨC NĂNG NỀN VÀ HIỆU ỨNG
    delay(10); 
    ledControl.runGreenFade(); 
}
