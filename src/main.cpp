// --- Tên file: src/main.cpp ---

#include <Arduino.h> 
#include "esp_heap_caps.h"          // Dùng để kiểm tra PSRAM và RAM
#include "esp_spi_flash.h"          // Dùng để kiểm tra Flash
#include <TFT_eSPI.h>               // Thư viện TFT
// Nếu bạn sử dụng cách #include "User_Setup.h", hãy bỏ comment dòng dưới.
//#include "User_Setup.h"    

// Khởi tạo đối tượng TFT
TFT_eSPI tft = TFT_eSPI(); 

// --- HÀM KIỂM TRA VÀ HIỆN TRẠNG THÁI BỘ NHỚ LÊN TFT ---
void displayMemoryStatus() {
    // 1. Cài đặt font và màu nền chung
    tft.fillScreen(TFT_BLACK);      // Xóa màn hình cũ
    tft.setTextSize(1);             // Kích thước chữ 2 (khoảng 16px)
    
    int y_pos = 10;
    const int LINE_HEIGHT = 20;  // Khoảng cách an toàn giữa các dòng (Đã khắc phục lỗi chồng đè)

    // Tiêu đề
    tft.setTextColor(TFT_YELLOW);
    tft.setCursor(40, y_pos);
    tft.println("MEMORY STATUS (ESP32-S3)");
    y_pos += LINE_HEIGHT * 2; // Tăng Y để tạo khoảng cách lớn sau tiêu đề

    // 1. KIỂM TRA PSRAM
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(30, y_pos); // <--- Đặt con trỏ Y mới
    tft.printf("PSRAM: Total: %.2f MB | Free: %.2f MB\n", 
                  (float)psram_total / 1024 / 1024, (float)psram_free / 1024 / 1024);
    y_pos += LINE_HEIGHT; // <--- Cập nhật Y cho dòng tiếp theo

    // 2. KIỂM TRA SRAM
    size_t sram_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, y_pos); // <--- Đặt con trỏ Y mới
    tft.printf("SRAM: Free: %.2f KB\n", (float)sram_free / 1024);
    y_pos += LINE_HEIGHT; // <--- Cập nhật Y cho dòng tiếp theo
    
    // 3. KIỂM TRA FLASH
    uint32_t chip_flash_size = spi_flash_get_chip_size();
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(20, y_pos); // <--- Đặt con trỏ Y mới
    tft.printf("FLASH: Total: %.2f MB\n", (float)chip_flash_size / 1024 / 1024);
    y_pos += LINE_HEIGHT + 10; // <--- Cập nhật Y và thêm khoảng cách

    // HIỂN THỊ TRẠNG THÁI KHỞI TẠO
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, y_pos); // <--- Đặt con trỏ Y mới
    tft.println("TFT Init SUCCESS!");
    
    // --- PHẦN IN RA SERIAL MONITOR (Giữ nguyên để đối chiếu) ---
    Serial.println("\n========================================");
    Serial.println("  MEMORY STATUS (ESP32-S3)");
    Serial.println("========================================");
    Serial.printf("PSRAM (8MB): Total: %.2f MB | Free: %.2f MB\n", 
                  (float)psram_total / 1024 / 1024, (float)psram_free / 1024 / 1024);
    Serial.printf("SRAM (320KB): Free: %.2f KB\n", (float)sram_free / 1024);
    Serial.printf("FLASH (16MB): Total: %.2f MB\n", (float)chip_flash_size / 1024 / 1024);
    Serial.println("========================================");
    
    Serial.println("\nSetup finished. TFT Initialized!");
}

// --- SETUP ---
void setup() {
    Serial.begin(115200);
    delay(1000); 
    
    // 1. GỌI HÀM KHỞI TẠO MÀN HÌNH
    tft.init();
    
    // 2. XOAY MÀN HÌNH (0 hoặc 1 tùy vào cách bạn lắp màn hình)
    tft.setRotation(0); 
    
    // Đảm bảo bạn đã bật đèn nền ở đây nếu cần thiết (phụ thuộc vào chân TFT_BL bạn định nghĩa)
    // pinMode(TFT_BL, OUTPUT);
    // digitalWrite(TFT_BL, HIGH); 
    
    // 3. GỌI HÀM HIỂN THỊ TRẠNG THÁI BỘ NHỚ LÊN MÀN HÌNH
    displayMemoryStatus(); 
}

// --- LOOP ---
void loop() {
    // Không làm gì trong loop
    delay(1000);
}