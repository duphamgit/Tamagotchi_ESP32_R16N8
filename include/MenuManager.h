#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// Định nghĩa cấu trúc cho mỗi mục trong Menu
struct MenuItem {
    const char* label;   // Tên mục (ví dụ: "FEED")
    const char* icon;    // Ký tự Icon (ví dụ: "🍔")
    int menuId;          // ID duy nhất cho mục này
};

// Định nghĩa các ID Menu (Nên khớp với ButtonIndex cho dễ quản lý)
enum MenuID {
    MENU_WIFI    = 0, 
    MENU_BL    = 1, 
    MENU_RF   = 2, 
    MENU_GPS  = 3,
    MENU_TIME  = 4, 
    MENU_ESPnow  = 5, 
    MENU_SETTINGS  = 6, 
    MENU_MISC  = 7,
    MENU_COUNT   = 8 // Tổng số mục Menu
};


class MenuManager {
private:
    TFT_eSPI& tft; // Tham chiếu TFT
    
    // Mảng chứa dữ liệu của các mục Menu
    const MenuItem items[MENU_COUNT] = {
        {"WIFI", " ", MENU_WIFI},
        {"BL", " ", MENU_BL},
        {"RF", " ", MENU_RF},
        {"GPS", " ", MENU_GPS},
        {"TIME", " ", MENU_TIME},
        {"ESPnow", " ", MENU_ESPnow}, 
        {"SETTINGS", " ", MENU_SETTINGS},
        {"MISC", " ", MENU_MISC}
    };
    
    int selectedItem = MENU_WIFI; // Mục hiện đang được chọn

    // Hàm phụ trợ để vẽ từng mục
    void drawItem(int itemIndex, bool isSelected);

public:
    MenuManager(TFT_eSPI& displayRef);
    
    /**
     * @brief Vẽ toàn bộ Menu lên màn hình.
     */
    void drawMenu();

    /**
     * @brief Xử lý input từ các nút bấm (UP, DOWN) và cập nhật mục được chọn.
     * @param pressedIndex Index của nút đã được nhấn (từ ButtonManager).
     */
    void handleInput(int pressedIndex);
    
    /**
     * @brief Tạo hiệu ứng chuyển cảnh cho mục được chọn.
     */
    // 🌟 KHAI BÁO HÀM MỚI 🌟
    void animateSelection(int itemIndex); 

    /**
     * @brief Trả về ID của mục hiện tại đang được chọn.
     */
    int getSelectedItem() const {
        return selectedItem;
    }

    /**
     * @brief Trả về nhãn (label) của một mục Menu dựa trên ID.
     */
    const char* getItemLabel(int id) const { 
        if (id >= 0 && id < MENU_COUNT) {
            return items[id].label;
        }
        return "ERROR";
    }
};

#endif // MENU_MANAGER_H