#include <Arduino.h> 
#include "SystemMonitor.h" // Class quản lý màn hình và trạng thái hệ thống
#include "InputManager.h"  // Class quản lý đầu vào (nút chạm TP223)
#include <TFT_eSPI.h> 

// --- KHAI BÁO THÔNG SỐ PHẦN CỨNG ---
const int TOUCH_BUTTON_PIN = 13; // GPIO được kết nối với cảm biến chạm TP223 (Ví dụ)

// Khởi tạo đối tượng TFT toàn cục (chỉ 1 lần)
TFT_eSPI tft = TFT_eSPI(); 

// Khởi tạo các đối tượng quản lý, truyền đối tượng TFT vào SystemMonitor
SystemMonitor monitor(tft);
InputManager button(TOUCH_BUTTON_PIN); // Giữ nguyên object để đảm bảo setup() chạy đúng

// --- SETUP ---
void setup() {
    Serial.begin(115200);
    delay(1000); 
    
    // 1. Khởi tạo màn hình và các thông số hiển thị
    monitor.begin(); 

    // 2. Khởi tạo nút bấm và cấu hình GPIO
    button.begin();
    
    // 3. Hiển thị trạng thái bộ nhớ (Chức năng bạn đã hỏi trước đó)
    monitor.displayMemoryStatus(); 
    
    // Chờ 5 giây để xem trạng thái bộ nhớ, sau đó xóa màn hình và sẵn sàng cho LOOP
    delay(5000);
    tft.fillScreen(TFT_BLACK);

    // Hiển thị thông báo hướng dẫn ban đầu
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("READY!");
    tft.setTextSize(1);
    tft.setCursor(10, 40);
    tft.println("Touch GPIO 13 to test.");
}

// --- LOOP (Người Điều Phối) ---
void loop() {
    // 1. Đọc trạng thái RAW trực tiếp
    int rawState = digitalRead(TOUCH_BUTTON_PIN);
    bool isPressedRaw = (rawState == HIGH); // 1 = Pressed

    // --- DEBUG: Hiển thị trạng thái RAW của chân nút bấm ---
    // Giữ nguyên logic này để bạn luôn có thể kiểm tra tín hiệu phần cứng.
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 150);
    tft.printf("RAW State (Pin %d): %d", TOUCH_BUTTON_PIN, rawState);
    
    // 2. Xử lý hiển thị DỰA TRÊN TRẠNG THÁI RAW
    
    // Đặt vị trí con trỏ trước khi in
    tft.setCursor(5, 70); 
    tft.setTextSize(2); // Kích thước chữ lớn cho thông báo

    if (isPressedRaw) {
        // Trạng thái RAW là 1: HIỂN THỊ CHỮ MÀU ĐỎ
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.println("BUTTON PRESSED!");
    } else {
        // Trạng thái RAW là 0: XÓA CHỮ (Vẽ bằng màu nền TFT_BLACK)
        tft.setTextColor(TFT_BLACK, TFT_BLACK); 
        tft.println("BUTTON PRESSED!"); // In cùng một chuỗi để xóa vùng đó
    }

    delay(10); // Đợi ngắn để cho phép các tác vụ nền khác chạy
}
