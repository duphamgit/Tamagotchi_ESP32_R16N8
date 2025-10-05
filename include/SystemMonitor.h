#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <Arduino.h>
#include <TFT_eSPI.h>

/**
 * @brief Class quản lý việc khởi tạo màn hình và hiển thị trạng thái hệ thống (RAM, PSRAM, Flash).
 * * Class này chịu trách nhiệm duy nhất là hiển thị (Presentation Layer).
 */
class SystemMonitor {
private:
    TFT_eSPI& tft; // Tham chiếu đến đối tượng TFT_eSPI (dùng tham chiếu để tránh khởi tạo 2 lần)

    /**
     * @brief Phương thức nội bộ để vẽ các chỉ số lên màn hình.
     * @param y_pos Vị trí Y bắt đầu của dòng văn bản.
     */
    void drawStatus(int& y_pos);
    
public:
    /**
     * @brief Khởi tạo SystemMonitor.
     * @param displayRef Tham chiếu đến đối tượng TFT_eSPI đã được khởi tạo.
     */
    SystemMonitor(TFT_eSPI& displayRef);

    /**
     * @brief Thiết lập màn hình TFT (gọi tft.init(), setRotation).
     */
    void begin();

    /**
     * @brief Hiển thị toàn bộ trạng thái bộ nhớ lên màn hình và Serial.
     */
    void displayMemoryStatus();
};

#endif // SYSTEM_MONITOR_H
