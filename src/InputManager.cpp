#include "InputManager.h"

/**
 * @brief Constructor.
 * @param pin GPIO được kết nối với cảm biến TP223.
 */
InputManager::InputManager(int pin) : TOUCH_PIN(pin) {}

/**
 * @brief Khởi tạo chân GPIO.
 */
void InputManager::begin() {
    // Thiết lập chân là INPUT (TP223 không cần PULLUP)
    pinMode(TOUCH_PIN, INPUT); 
    Serial.printf("InputManager: Touch sensor initialized on GPIO %d\n", TOUCH_PIN);
}

/**
 * @brief Kiểm tra trạng thái nút bấm và xử lý chống rung (debounce).
 * @return true nếu nút VỪA được nhấn (chỉ 1 lần cho mỗi lần nhấn).
 */
bool InputManager::checkButton() {
    // Đọc trạng thái hiện tại của chân GPIO
    int reading = digitalRead(TOUCH_PIN);
    bool pressedNow = (reading == HIGH); // TP223 trả về HIGH (1) khi chạm
    
    // Lưu lại trạng thái nút bấm đã được lọc trước đó
    bool buttonWasPressed = isPressed;

    // --- LOGIC CHỐNG RUNG (DEBOUNCING) ĐÃ SỬA ---

    // 🌟 KHẮC PHỤC: Chỉ reset timer khi tín hiệu RAW thay đổi 🌟
    if (reading != lastReading) {
        lastDebounceTime = millis();
    }

    // Nếu thời gian chống rung đã trôi qua (100ms)
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // Nếu trạng thái đã lọc khác với trạng thái thô ổn định
        if (pressedNow != isPressed) {
            isPressed = pressedNow; // Cập nhật trạng thái đã lọc
        }
    }
    
    // Cập nhật trạng thái RAW cuối cùng cho lần lặp tiếp theo
    lastReading = reading; 
    
    // --- LOGIC ONE-SHOT ---

    // Trả về TRUE chỉ khi nút vừa chuyển từ trạng thái Released sang Pressed
    if (isPressed && !buttonWasPressed) {
        return true;
    }

    return false;
}