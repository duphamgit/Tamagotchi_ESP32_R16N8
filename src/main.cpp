#include <Arduino.h> 
#include <TFT_eSPI.h> 
#include "SystemMonitor.h" 
#include "InputManager.h"  
#include "ButtonManager.h" 
#include "LEDManager.h" 

// --- KHAI BÁO CÁC HẰNG SỐ HIỂN THỊ TOÀN CỤC ---
const int MSG_X = 5;
const int MSG_Y = 70;
const int MSG_WIDTH = 230; 
const int MSG_HEIGHT = 20; 

// --- KHAI BÁO THÔNG SỐ PHẦN CỨNG TOÀN CỤC ---
const int TOUCH_BUTTON_PIN = 13; 

// --- KHAI BÁO ENUM CHO TRẠNG THÁI HIỂN THỊ ---
enum DisplayState {
    STATE_NONE,       // Không có nút nào được nhấn
    STATE_UP,
    STATE_DOWN,
    STATE_LEFT,
    STATE_RIGHT,
    STATE_TOUCH       // Đang giữ nút chạm
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
SystemMonitor monitor(tft);
InputManager touchButton(TOUCH_BUTTON_PIN); 
ButtonManager physicalButtons;              
LEDManager ledControl;

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
// Dùng để theo dõi trạng thái hiển thị hiện tại, chỉ cập nhật khi trạng thái thay đổi.
DisplayState previousState = STATE_NONE; 


// --- SETUP ---
void setup() {
    Serial.begin(115200);
    delay(1000); 
    
    monitor.begin(); 
    touchButton.begin();
    physicalButtons.begin(); 
    
    monitor.displayMemoryStatus(); 
    
    delay(5000);
    tft.fillScreen(TFT_BLACK); 
    
    // Hướng dẫn ban đầu
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("READY!");
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.println("Test 4 buttons and Touch.");
    
    ledControl.begin(); 
}

// --- LOOP (Người Điều Phối) ---
void loop() {
    // 1. CẬP NHẬT TRẠNG THÁI NÚT VÀ TOUCH
    physicalButtons.update(); 
    touchButton.checkButton(); // Cần gọi để logic chống rung được thực thi và cập nhật trạng thái isPressed

    bool isTouching = touchButton.isTouched(); 

    // 2. XÁC ĐỊNH TRẠNG THÁI HIỂN THỊ MỚI
    DisplayState newState = STATE_NONE;
    
    // Ưu tiên nút vật lý
    if (physicalButtons.isPressed(BTN_UP)) { 
        newState = STATE_UP;
    } else if (physicalButtons.isPressed(BTN_DOWN)) { 
        newState = STATE_DOWN;
    } else if (physicalButtons.isPressed(BTN_LEFT)) { 
        newState = STATE_LEFT;
    } else if (physicalButtons.isPressed(BTN_RIGHT)) { 
        newState = STATE_RIGHT;
    } else if (isTouching) { 
        newState = STATE_TOUCH;
    } 

    // 3. 🌟 LOGIC CHỐNG NHÒE: CHỈ CẬP NHẬT MÀN HÌNH KHI TRẠNG THÁI THAY ĐỔI 🌟
    if (newState != previousState) {
        
        // 3a. XÓA KHU VỰC CŨ TRƯỚC KHI VẼ MỚI
        tft.fillRect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, TFT_BLACK); 
        tft.setCursor(MSG_X, MSG_Y); 
        tft.setTextSize(2);

        // 3b. VẼ NỘI DUNG MỚI DỰA TRÊN TRẠNG THÁI
        switch (newState) {
            case STATE_UP:
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.println("UP Pressed!");
                break;
            case STATE_DOWN:
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.println("DOWN Pressed!");
                break;
            case STATE_LEFT:
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.println("LEFT Pressed!");
                break;
            case STATE_RIGHT:
                tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                tft.println("RIGHT Pressed!");
                break;
            case STATE_TOUCH:
                tft.setTextColor(TFT_GREEN, TFT_BLACK);
                tft.println("TOUCH Action!");
                break;
            case STATE_NONE:
            default:
                // Trạng thái NONE: khu vực đã được xóa ở 3a, không in gì
                break;
        }

        // 3c. LƯU TRẠNG THÁI MỚI để so sánh ở lần lặp tiếp theo
        previousState = newState;
    }
    
    // DEBUG RAW STATE (Giữ nguyên)
    int rawState = digitalRead(TOUCH_BUTTON_PIN);
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 150);
    tft.printf("RAW State (Pin %d): %d", TOUCH_BUTTON_PIN, rawState);
    
    // Chạy hiệu ứng LED
    delay(10); 
    ledControl.runGreenFade(); 
}