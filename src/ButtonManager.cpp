#include "ButtonManager.h"

// 🌟 Đã sửa lỗi LINKER: Định nghĩa mảng PIN tĩnh cho class ButtonManager 🌟
// KHÔNG dùng 'static' ở đây. Phải dùng `ButtonManager::` để định nghĩa member.
const int ButtonManager::BUTTON_PINS[BTN_COUNT] = {12, 11, 10, 9}; 
// Thứ tự: BTN_UP(0), BTN_DOWN(1), BTN_SELECT(2), BTN_MENU(3)

ButtonManager::ButtonManager() {
    // Khởi tạo các phần tử trong mảng
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = PhysicalButton(BUTTON_PINS[i]); 
    }
}

void ButtonManager::begin() {
    // 🌟 Đã sửa: Cập nhật Serial output cho các nút mới 🌟
    Serial.println("ButtonManager: Initializing 4 physical buttons (UP, DOWN, SELECT, MENU)..."); 
    for (int i = 0; i < NUM_BUTTONS; i++) {
        // Sử dụng INPUT_PULLUP, trạng thái LOW = PRESSED
        pinMode(buttons[i].pin, INPUT_PULLUP); 
        Serial.printf("  - Button %d (GPIO %d) initialized.\n", i, buttons[i].pin); 
    }
}

void ButtonManager::update() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        PhysicalButton& btn = buttons[i]; 

        int reading = digitalRead(btn.pin);
        
        // Logic chống rung (Giữ nguyên)
        if (reading != btn.lastReading) {
            btn.lastDebounceTime = millis();
        }

        if ((millis() - btn.lastDebounceTime) > btn.debounceDelay) {
            
            bool stablePressed = (reading == LOW); 

            if (stablePressed != btn.isPressed) {
                btn.isPressed = stablePressed; 
            }
        }
        
        // CẬP NHẬT trạng thái RAW cuối cùng
        btn.lastReading = reading;
    }
}

bool ButtonManager::isPressed(ButtonIndex index) {
    if (index >= 0 && index < BTN_COUNT) {
        return buttons[index].isPressed; 
    }
    return false;
}