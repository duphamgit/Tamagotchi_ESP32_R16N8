#include "SystemMonitor.h"
#include "esp_heap_caps.h" 
#include "esp_spi_flash.h" 
#include "esp_system.h" 

// Định nghĩa các hằng số màu và kích thước
#define LINE_HEIGHT 25 // Tăng chiều cao dòng để có khoảng trống tốt hơn
#define DATA_START_Y 35 // Vị trí Y bắt đầu vẽ dữ liệu (sau tiêu đề)
#define LINE_CLEAR_WIDTH 235 // Chiều rộng vùng xóa: gần bằng màn hình

SystemMonitor::SystemMonitor(TFT_eSPI& displayRef) : tft(displayRef) {}

void SystemMonitor::begin() {
    Serial.println("SystemMonitor: Initializing TFT...");
    Serial.println("SystemMonitor: TFT Initialized!");
}

/**
 * @brief Phương thức nội bộ để vẽ các chỉ số lên màn hình (chỉ vẽ giá trị).
 */
void SystemMonitor::drawStatus(int& y_pos) {
    
    // 🌟 FIX: Không xóa toàn bộ, chỉ xóa cục bộ từng dòng 🌟
    // Thay vì xóa vùng lớn, ta sẽ xóa từng dòng trước khi vẽ. 
    // y_pos (tạm) sẽ là biến cục bộ để quản lý vị trí vẽ.
    int current_y = DATA_START_Y;
    
    // 1. KIỂM TRA PSRAM
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    
    tft.fillRect(5, current_y, LINE_CLEAR_WIDTH, LINE_HEIGHT, TFT_BLACK); // Xóa dòng cũ
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, current_y + 5); // Cộng thêm 5 để căn giữa
    tft.printf("PSRAM: Free: %.2f MB / Total: %.2f MB", (float)psram_free / 1024 / 1024, (float)psram_total / 1024 / 1024);
    current_y += LINE_HEIGHT;

    // 2. KIỂM TRA SRAM
    size_t sram_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t sram_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    
    tft.fillRect(5, current_y, LINE_CLEAR_WIDTH, LINE_HEIGHT, TFT_BLACK); // Xóa dòng cũ
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, current_y + 5);
    tft.printf("SRAM: Free: %.2f KB / Total: %.2f KB", (float)sram_free / 1024, (float)sram_total / 1024);
    current_y += LINE_HEIGHT;
    
    // 3. THÔNG TIN CHIP FLASH
    uint32_t chip_flash_size = spi_flash_get_chip_size();

    tft.fillRect(5, current_y, LINE_CLEAR_WIDTH, LINE_HEIGHT, TFT_BLACK); // Xóa dòng cũ
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(10, current_y + 5);
    tft.printf("FLASH: Total: %.2f MB", (float)chip_flash_size / 1024 / 1024);
    current_y += LINE_HEIGHT; 
    
    // 4. CHIP REVISION
    tft.fillRect(5, current_y, LINE_CLEAR_WIDTH, LINE_HEIGHT, TFT_BLACK); // Xóa dòng cũ
    tft.setTextColor(TFT_MAGENTA);
    tft.setCursor(10, current_y + 5);
    tft.printf("Rev: %d / Model: ESP32-S3", ESP.getChipRevision());
    current_y += LINE_HEIGHT;

    y_pos = current_y; // Cập nhật y_pos (dù không dùng)
}


/**
 * @brief Hiển thị toàn bộ trạng thái bộ nhớ lên màn hình và Serial.
 */
void SystemMonitor::displayMemoryStatus() {
    
    tft.setTextSize(1); // Kích thước chữ lớn hơn

    // CHỈ XÓA VÀ VẼ LẠI TIÊU ĐỀ KHI CHƯA VẼ
    // Chúng ta sẽ chỉ vẽ tiêu đề một lần khi chuyển sang trạng thái MONITOR (trong main.cpp).
    // Ở đây chỉ cần gọi drawStatus để vẽ các giá trị.
    
    int y_pos = 0; 
    drawStatus(y_pos); 
}