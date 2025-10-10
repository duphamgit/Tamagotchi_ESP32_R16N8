#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

enum ButtonIndex {
    BTN_UP    = 0, // GPIO 12
    BTN_DOWN  = 1, // GPIO 11
    BTN_LEFT  = 2, // GPIO 10
    BTN_RIGHT = 3, // GPIO 9
    
    BTN_COUNT = 4  
};


struct PhysicalButton {
    int pin; 
    bool isPressed;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay = 100; // Đã tăng để tăng độ tin cậy
    
    // Đã thêm để sửa lỗi logic chống rung
    int lastReading = HIGH; // Theo dõi trạng thái thô cuối cùng
    
    // Constructor
    PhysicalButton(int p = 0, bool pressed = false, unsigned long debounceTime = 0)
        : pin(p), isPressed(pressed), lastDebounceTime(debounceTime) {}
};

class ButtonManager {
private:
    static const int NUM_BUTTONS = BTN_COUNT;
    
    PhysicalButton buttons[NUM_BUTTONS]; 

public:
    ButtonManager();
    void begin();
    void update();
    bool isPressed(ButtonIndex index);
};

#endif // BUTTON_MANAGER_H