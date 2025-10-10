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
    MENU_FEED    = 0, // Tương ứng với BTN_UP
    MENU_PLAY    = 1, // Tương ứng với BTN_DOWN
    MENU_CLEAN   = 2, // Tương ứng với BTN_LEFT
    MENU_STATUS  = 3, // Tương ứng với BTN_RIGHT
    MENU_COUNT   = 4
};


class MenuManager {
private:
    TFT_eSPI& tft; // Tham chiếu TFT
    
    // Mảng chứa dữ liệu của các mục Menu (là Private, chỉ được truy cập qua Getter)
    const MenuItem items[MENU_COUNT] = {
        {"FEED", "🍔", MENU_FEED},
        {"PLAY", "⚽", MENU_PLAY},
        {"CLEAN", "🧼", MENU_CLEAN},
        {"STATUS", "📊", MENU_STATUS}
    };
    
    int selectedItem = MENU_FEED; // Mục hiện đang được chọn

    // Hàm phụ trợ để vẽ từng mục (nên là private vì chỉ được gọi nội bộ)
    void drawItem(int itemIndex, bool isSelected);

public:
    MenuManager(TFT_eSPI& displayRef);
    
    /**
     * @brief Vẽ toàn bộ Menu lên màn hình.
     */
    void drawMenu();

    /**
     * @brief Xử lý input từ các nút bấm (UP, DOWN, LEFT, RIGHT) và cập nhật mục được chọn.
     * @param pressedIndex Index của nút đã được nhấn (từ ButtonManager).
     */
    void handleInput(int pressedIndex);
    
    /**
     * @brief Trả về ID của mục hiện tại đang được chọn (dùng cho logic game).
     */
    int getSelectedItem() const {
        return selectedItem;
    }

    /**
     * @brief Trả về nhãn (label) của một mục Menu dựa trên ID.
     * @param id ID của mục Menu cần lấy nhãn.
     * @return Con trỏ tới chuỗi nhãn của mục.
     */
    const char* getItemLabel(int id) const { 
        if (id >= 0 && id < MENU_COUNT) {
            return items[id].label;
        }
        return "ERROR";
    }

    // 🌟 KHAI BÁO CÔNG KHAI mảng items để fix lỗi biên dịch (Tùy chọn, nếu không muốn dùng Getter) 🌟
    // Nếu bạn muốn giữ logic cũ trong main.cpp (truy cập trực tiếp), bạn có thể
    // di chuyển const MenuItem items[MENU_COUNT] từ private lên public. 
    // Nhưng cách dùng getItemLabel() là chuẩn C++ hơn.

};

#endif // MENU_MANAGER_H