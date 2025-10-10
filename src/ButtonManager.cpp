#include "ButtonManager.h"

// Dữ liệu cấu hình: Dùng mảng pin toàn cục
static const int BUTTON_PINS[BTN_COUNT] = {12, 11, 10, 9}; 

ButtonManager::ButtonManager() {
    // Khởi tạo các phần tử trong mảng
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = PhysicalButton(BUTTON_PINS[i]); 
    }
}

void ButtonManager::begin() {
    Serial.println("ButtonManager: Initializing 4 physical buttons...");
    for (int i = 0; i < NUM_BUTTONS; i++) {
        // Sử dụng INPUT_PULLUP, trạng thái LOW = PRESSED
        pinMode(buttons[i].pin, INPUT_PULLUP); 
        Serial.printf("  - Physical Button on GPIO %d initialized.\n", buttons[i].pin);
    }
}

void ButtonManager::update() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        PhysicalButton& btn = buttons[i]; 

        int reading = digitalRead(btn.pin);
        
        // 🌟 KHẮC PHỤC LỖI LOGIC: Chỉ reset timer khi tín hiệu RAW thay đổi 🌟
        if (reading != btn.lastReading) {
            btn.lastDebounceTime = millis();
        }

        // Nếu thời gian chống rung đã trôi qua (100ms)
        if ((millis() - btn.lastDebounceTime) > btn.debounceDelay) {
            
            // Xác định trạng thái ổn định hiện tại
            bool stablePressed = (reading == LOW); 

            // Cập nhật trạng thái đã lọc (isPressed)
            if (stablePressed != btn.isPressed) {
                btn.isPressed = stablePressed; 
            }
        }
        
        // CẬP NHẬT trạng thái RAW cuối cùng cho lần lặp tiếp theo
        btn.lastReading = reading;
    }
}

bool ButtonManager::isPressed(ButtonIndex index) {
    if (index >= 0 && index < BTN_COUNT) {
        return buttons[index].isPressed; 
    }
    return false;
}