#include "SystemMonitor.h"
#include "esp_heap_caps.h" 
#include "esp_spi_flash.h" 

// Định nghĩa các hằng số màu và kích thước
#define LINE_HEIGHT 20

/**
 * @brief Constructor: Khởi tạo SystemMonitor với tham chiếu TFT.
 * @param displayRef Tham chiếu đến đối tượng TFT_eSPI.
 */
SystemMonitor::SystemMonitor(TFT_eSPI& displayRef) : tft(displayRef) {
    // Không làm gì nhiều ở đây, chỉ gán tham chiếu
}

/**
 * @brief Thiết lập màn hình TFT.
 */
void SystemMonitor::begin() {
    Serial.println("SystemMonitor: Initializing TFT...");
    tft.init();
    tft.setRotation(0); 
    
    // Tùy chọn: Thiết lập đèn nền nếu có chân BL được định nghĩa trong User_Setup.h
    // pinMode(TFT_BL, OUTPUT); 
    // digitalWrite(TFT_BL, HIGH);
    
    Serial.println("SystemMonitor: TFT Initialized!");
}

/**
 * @brief Phương thức nội bộ để vẽ các chỉ số lên màn hình.
 */
void SystemMonitor::drawStatus(int& y_pos) {
    // 1. KIỂM TRA PSRAM
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, y_pos);
    tft.printf("PSRAM: Total: %.2f MB | Free: %.2f MB\n", 
                  (float)psram_total / 1024 / 1024, (float)psram_free / 1024 / 1024);
    y_pos += LINE_HEIGHT;

    // 2. KIỂM TRA SRAM
    size_t sram_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, y_pos);
    tft.printf("SRAM: Free: %.2f KB\n", (float)sram_free / 1024);
    y_pos += LINE_HEIGHT;
    
    // 3. KIỂM TRA FLASH
    uint32_t chip_flash_size = spi_flash_get_chip_size();
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(10, y_pos);
    tft.printf("FLASH: Total: %.2f MB\n", (float)chip_flash_size / 1024 / 1024);
    y_pos += LINE_HEIGHT + 10; 
}


/**
 * @brief Hiển thị toàn bộ trạng thái bộ nhớ lên màn hình và Serial.
 */
void SystemMonitor::displayMemoryStatus() {
    // 1. Cài đặt vẽ
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2); // Kích thước chữ lớn hơn

    int y_pos = 20;
    int x_pos = 40;
    // Tiêu đề
    tft.setTextColor(TFT_YELLOW);
    tft.setCursor(x_pos, y_pos);
    tft.println("MEMORY STATUS");
    y_pos += LINE_HEIGHT * 2; 

    tft.setTextSize(1); // Chữ nhỏ hơn cho nội dung
    
    // 2. Vẽ trạng thái bộ nhớ
    drawStatus(y_pos); 

    // 3. Hiển thị thông báo hoàn thành
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, y_pos);
    tft.println("Init SUCCESS!");

    // 4. In ra Serial Monitor
    Serial.println("\n========================================");
    Serial.println("  SYSTEM STATUS LOG");
    Serial.println("========================================");
}
