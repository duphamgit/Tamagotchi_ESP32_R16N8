#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Định nghĩa thông số LED
const int LED_PIN = 48;
const int NUM_LEDS = 1; // Chỉ dùng 1 LED RGB

/**
 * @brief Class quản lý đèn LED RGB (NeoPixel/WS2812B) trên GPIO 48.
 */
class LEDManager {
private:
    Adafruit_NeoPixel strip;
    unsigned long lastUpdateTime = 0;
    // Tăng updateInterval (45ms) để làm chậm hiệu ứng, giúp hiệu ứng mờ dần mượt mà và dễ thấy hơn.
    const unsigned long updateInterval = 45; 

    int brightness = 0; // Độ sáng hiện tại (0-255)
    int step = -1;       // Hướng tăng/giảm độ sáng (+1 hoặc -1)
    
    // ĐỘ SÁNG TỐI ĐA (Giảm xuống 40 để tránh chói mắt và tiết kiệm điện)
    const int MAX_BRIGHTNESS = 40; 
    
    /**
     * @brief Đặt lại các biến trạng thái nội bộ cho hiệu ứng Fade/Breathing.
     * Cần thiết để đảm bảo hiệu ứng bắt đầu đúng cách khi chuyển màu.
     */
    void resetFadeState();

public:
    /**
     * @brief Constructor.
     */
    LEDManager();

    /**
     * @brief Khởi tạo module LED.
     */
    void begin();

    /**
     * @brief Đặt màu tĩnh cho LED. Hàm này sẽ reset trạng thái Fade.
     * @param r Giá trị màu Đỏ (0-255).
     * @param g Giá trị màu Xanh Lá (0-255).
     * @param b Giá trị màu Xanh Dương (0-255).
     */
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Thực hiện hiệu ứng mờ dần (Fade/Breathing) màu xanh lá.
     * Cần gọi liên tục trong loop().
     */
    void runGreenFade();

    /**
     * @brief Thực hiện hiệu ứng mờ dần (Fade/Breathing) màu đỏ.
     * Cần gọi liên tục trong loop().
     */
    void runRedFade(); 
};

#endif // LED_MANAGER_H
