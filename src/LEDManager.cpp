#include "LEDManager.h"

/**
 * @brief Constructor.
 */
LEDManager::LEDManager() 
    // Khởi tạo đối tượng Adafruit_NeoPixel
    : strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800) {}

/**
 * @brief Đặt lại các biến trạng thái nội bộ cho hiệu ứng Fade/Breathing.
 * Điều này rất quan trọng để đảm bảo hiệu ứng bắt đầu đúng cách khi chuyển màu hoặc chuyển từ màu tĩnh.
 */
void LEDManager::resetFadeState() {
    brightness = 0; // Bắt đầu lại từ độ sáng tối thiểu (0)
    step = 1;       // Bắt đầu bằng việc tăng độ sáng (Fade In)
    lastUpdateTime = 0; // Đảm bảo lần cập nhật đầu tiên sẽ chạy
}

/**
 * @brief Khởi tạo module LED.
 */
void LEDManager::begin() {
    strip.begin();
    // Chúng ta kiểm soát độ sáng trực tiếp qua giá trị màu RGB
    strip.show(); // Tắt tất cả các LED ban đầu
    resetFadeState(); // Đặt lại trạng thái Fade ban đầu
    Serial.printf("LEDManager: RGB LED initialized on GPIO %d\n", LED_PIN);
}

/**
 * @brief Set a static color for the LED. Hàm này sẽ reset trạng thái Fade.
 * @param r Giá trị màu Đỏ (0-255).
 * @param g Giá trị màu Xanh Lá (0-255).
 * @param b Giá trị màu Xanh Dương (0-255).
 */
void LEDManager::setColor(uint8_t r, uint8_t g, uint8_t b) {
    resetFadeState(); // QUAN TRỌNG: Reset trạng thái khi đặt màu tĩnh
    strip.setPixelColor(0, r, g, b); // Đặt màu cho LED đầu tiên (index 0)
    strip.show(); // Cập nhật hiển thị
}

/**
 * @brief Thực hiện hiệu ứng mờ dần (Fade/Breathing) màu xanh lá.
 * Cần được gọi liên tục trong hàm loop().
 */
void LEDManager::runGreenFade() {
    unsigned long currentMillis = millis();

    // Cập nhật độ sáng nếu đã trôi qua thời gian 'updateInterval' (45ms)
    if (currentMillis - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentMillis;
        
        // 1. Cập nhật độ sáng hiện tại
        brightness += step;

        // 2. Kiểm tra giới hạn: Đảo ngược hướng khi đạt min/max
        if (brightness <= 0) {
            brightness = 0;
            step = 1; // Bắt đầu Fade In
        } else if (brightness >= MAX_BRIGHTNESS) {
            brightness = MAX_BRIGHTNESS;
            step = -1; // Bắt đầu Fade Out
        }

        // 3. Áp dụng độ sáng hiện tại vào thành phần màu Xanh Lá
        // Color value: Red=0, Green=brightness, Blue=0
        strip.setPixelColor(0, 0, brightness, 0); 
        
        strip.show(); // Hiển thị trạng thái mới
    }
}

/**
 * @brief Thực hiện hiệu ứng mờ dần (Fade/Breathing) màu đỏ.
 * Cần được gọi liên tục trong hàm loop().
 */
void LEDManager::runRedFade() {
    unsigned long currentMillis = millis();

    // Cập nhật độ sáng nếu đã trôi qua thời gian 'updateInterval' (45ms)
    if (currentMillis - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentMillis;
        
        // 1. Cập nhật độ sáng hiện tại
        brightness += step;

        // 2. Kiểm tra giới hạn: Đảo ngược hướng khi đạt min/max
        if (brightness <= 0) {
            brightness = 0;
            step = 1; // Bắt đầu Fade In
        } else if (brightness >= MAX_BRIGHTNESS) {
            brightness = MAX_BRIGHTNESS;
            step = -1; // Bắt đầu Fade Out
        }

        // 3. Áp dụng độ sáng hiện tại vào thành phần màu Đỏ
        // Color value: Red=brightness, Green=0, Blue=0
        strip.setPixelColor(0, brightness, 0, 0); 
        
        strip.show(); // Hiển thị trạng thái mới
    }
}
