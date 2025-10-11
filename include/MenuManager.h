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
    MENU_FEED    = 0, 
    MENU_PLAY    = 1, 
    MENU_CLEAN   = 2, 
    MENU_STATUS  = 3, 
    MENU_COUNT   = 4
};


class MenuManager {
private:
    TFT_eSPI& tft; // Tham chiếu TFT
    
    // Mảng chứa dữ liệu của các mục Menu
    const MenuItem items[MENU_COUNT] = {
        {"FEED", "x", MENU_FEED},
        {"PLAY", "x", MENU_PLAY},
        {"CLEAN", "x", MENU_CLEAN},
        {"STATUS", "x", MENU_STATUS}
    };
    
    int selectedItem = MENU_FEED; // Mục hiện đang được chọn

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