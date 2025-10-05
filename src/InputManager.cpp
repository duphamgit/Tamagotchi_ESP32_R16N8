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
    // Thiết lập chân là INPUT
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
    bool pressedNow = (reading == HIGH); // TP223 trả về HIGH khi chạm
    
    // Lưu lại trạng thái nút bấm đã được lọc trước đó
    bool buttonWasPressed = isPressed;

    // --- LOGIC CHỐNG RUNG (DEBOUNCING) ---

    // Nếu trạng thái thô (reading) khác với trạng thái đã lọc (isPressed), reset đồng hồ chống rung
    if (pressedNow != isPressed) {
        lastDebounceTime = millis();
    }

    // Nếu thời gian chống rung đã trôi qua
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // Nếu trạng thái đã lọc khác với trạng thái thô
        if (pressedNow != isPressed) {
            isPressed = pressedNow; // Cập nhật trạng thái đã lọc
        }
    }
    
    // --- LOGIC ONE-SHOT (CHỈ BÁO CÁO KHI VỪA NHẤN) ---

    // Trả về TRUE chỉ khi:
    // 1. Nút hiện TẠI đang ở trạng thái đã lọc là PRESSED (isPressed == true)
    // 2. Và nút TRƯỚC ĐÓ đang ở trạng thái KHÔNG PRESSED (buttonWasPressed == false)
    // Đồng thời, chúng ta sẽ giảm debounceDelay xuống 20ms để phản ứng nhanh hơn.
    return (isPressed && !buttonWasPressed);
}
