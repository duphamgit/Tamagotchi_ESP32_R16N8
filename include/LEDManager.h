#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Định nghĩa thông số LED
const int LED_PIN = 48;
const int NUM_LEDS = 1; // Chỉ dùng 1 LED RGB

/**
 * @brief Class quản lý đèn LED RGB (NeoPixel/WS2812B) trên GPIO 47.
 */
class LEDManager {
private:
    Adafruit_NeoPixel strip;
    unsigned long lastUpdateTime = 0;
    const unsigned long updateInterval = 45; // Cập nhật độ sáng mỗi 10ms (tạo chuyển động mượt)

    int brightness = 0; // Độ sáng hiện tại (0-255)
    int step = -1;       // Hướng tăng/giảm độ sáng (+1 hoặc -1)
    
    // ĐỘ SÁNG TỐI ĐA MỚI (Giảm từ 255 xuống 80 để bảo vệ mắt)
    const int MAX_BRIGHTNESS = 40; 

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
     * @brief Đặt màu tĩnh cho LED.
     * @param r Giá trị màu Đỏ (0-255).
     * @param g Giá trị màu Xanh Lá (0-255).
     * @param b Giá trị màu Xanh Dương (0-255).
     */
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Thực hiện hiệu ứng sáng từ từ và tắt từ từ (Fade/Breathing) màu xanh lá cây.
     */
    void runGreenFade();
};

#endif // LED_MANAGER_H
