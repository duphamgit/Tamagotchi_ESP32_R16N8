#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

// 🌟 Đã sửa: Định nghĩa lại các nút 🌟
enum ButtonIndex {
    BTN_UP      = 0, // GPIO 12 - Điều hướng Lùi/Lên
    BTN_DOWN    = 1, // GPIO 11 - Điều hướng Tiến/Xuống
    BTN_SELECT  = 2, // GPIO 10 - Nút CHỌN (Thay thế BTN_LEFT)
    BTN_MENU    = 3, // GPIO 9  - Nút MENU (Thay thế BTN_RIGHT)
    
    BTN_COUNT = 4  
};


struct PhysicalButton {
    int pin; 
    bool isPressed;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay = 100; 
    
    int lastReading = HIGH; 
    
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
    
    // 🌟 KHẮC PHỤC LỖI LINKER: Khai báo mảng PIN tĩnh 🌟
    static const int BUTTON_PINS[BTN_COUNT]; 
};

#endif // BUTTON_MANAGER_H