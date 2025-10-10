#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>

class InputManager {
private:
    const int TOUCH_PIN;
    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 100; // 100ms
    
    bool isPressed = false; 
    int lastReading = LOW; 

public:
    InputManager(int pin);
    void begin();
    
    // Vẫn gọi trong loop() để CẬP NHẬT trạng thái isPressed
    bool checkButton(); 
    
    // 🌟 HÀM MỚI: Trả về trạng thái chạm LIÊN TỤC 🌟
    bool isTouched() {
        return isPressed;
    }
};

#endif // INPUT_MANAGER_H