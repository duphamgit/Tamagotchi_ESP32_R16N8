// --- Tên file: src/main.cpp ---

#include <Arduino.h> 
#include "esp_heap_caps.h"          // Dùng để kiểm tra PSRAM và RAM
#include "esp_spi_flash.h"          // Dùng để kiểm tra Flash
#include "User_Setup_TFT_eSPI.h"    // Dẫn đến file cấu hình bạn vừa tạo
#include <TFT_eSPI.h>  
// --- HÀM KIỂM TRA VÀ IN TRẠNG THÁI BỘ NHỚ ---
TFT_eSPI tft = TFT_eSPI(); // Khởi tạo đối tượng TFT
void printMemoryStatus() {
    Serial.println("\n========================================");
    Serial.println("  MEMORY STATUS (ESP32-S3)");
    Serial.println("========================================");

    // 1. KIỂM TRA PSRAM (RAM MỞ RỘNG)
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    
    Serial.printf("PSRAM (8MB): Total: %.2f MB | Free: %.2f MB\n", 
                  (float)psram_total / 1024 / 1024, (float)psram_free / 1024 / 1024);

    // 2. KIỂM TRA SRAM (RAM CHÍNH)
    size_t sram_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    Serial.printf("SRAM (320KB): Free: %.2f KB\n", (float)sram_free / 1024);
    
    // 3. KIỂM TRA FLASH (BỘ NHỚ CHƯƠNG TRÌNH & DỮ LIỆU)
    uint32_t chip_flash_size = spi_flash_get_chip_size();
    
    Serial.printf("FLASH (16MB): Total: %.2f MB\n", (float)chip_flash_size / 1024 / 1024);
    
    Serial.println("========================================");
}
// --- SETUP ---
void setup() {
    // Khởi tạo Serial ở tốc độ 115200 (đảm bảo Monitor cũng ở tốc độ này)
    Serial.begin(115200);

    // Dừng một chút để Serial Monitor kịp mở
    delay(1000); 
    
    // GỌI HÀM KIỂM TRA
    printMemoryStatus(); 
    
    Serial.println("\nSetup finished. Ready for next steps!");
    // 1. GỌI HÀM KHỞI TẠO MÀN HÌNH
    tft.init();
    
    // 2. XOAY MÀN HÌNH (1 = ngang, 0 = dọc)
    tft.setRotation(1); 
    
    // 3. ĐỔ MÀU NỀN
    tft.fillScreen(TFT_WHITE);

    // 4. HIỂN THỊ DÒNG CHỮ TEST
    tft.setTextColor(TFT_YELLOW);    // Chọn màu chữ
    tft.setTextSize(2);             // Chọn kích thước chữ
    tft.setCursor(10, 10);          // Đặt vị trí con trỏ (x, y)
    tft.println("TFT Init SUCCESS!");
    
    Serial.println("\nSetup finished. TFT Initialized!");
}

// --- LOOP ---
void loop() {
    // Không làm gì trong loop, chỉ in ra trạng thái 1 lần trong setup
    delay(1000);
}