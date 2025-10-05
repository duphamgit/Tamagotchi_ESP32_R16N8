#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>

/**
 * @brief Class quản lý tất cả các đầu vào (Input) của hệ thống, ví dụ: nút bấm vật lý, cảm biến chạm.
 */
class InputManager {
private:
    const int TOUCH_PIN;
    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50; // 50ms chống rung

    bool isPressed = false;

public:
    /**
     * @brief Constructor.
     * @param pin GPIO được kết nối với cảm biến TP223.
     */
    InputManager(int pin);

    /**
     * @brief Khởi tạo chân GPIO.
     */
    void begin();

    /**
     * @brief Kiểm tra trạng thái nút bấm và xử lý chống rung (debounce).
     * @return true nếu nút vừa được nhấn (phát hiện cạnh lên/xuống).
     */
    bool checkButton();
};

#endif // INPUT_MANAGER_H
