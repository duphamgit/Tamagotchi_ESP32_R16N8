#include "MenuManager.h"
#include "ButtonManager.h" // Cần để sử dụng enum ButtonIndex

// --- HẰNG SỐ CẤU HÌNH MENU ---
#define MENU_COLUMNS 2
#define MENU_ROWS 2
#define ITEM_WIDTH 120
#define ITEM_HEIGHT 80
#define MARGIN_X 0
#define MARGIN_Y 0

// --- MÀU SẮC ---
#define COLOR_DEFAULT TFT_WHITE
#define COLOR_SELECTED TFT_GREEN
#define COLOR_BACKGROUND TFT_BLACK


MenuManager::MenuManager(TFT_eSPI& displayRef) : tft(displayRef) {}

// --- HÀM HỖ TRỢ: VẼ TỪNG MỤC ---
void MenuManager::drawItem(int itemIndex, bool isSelected) {
    // 1. Tính toán vị trí
    int row = itemIndex / MENU_COLUMNS;
    int col = itemIndex % MENU_COLUMNS;
    
    int x = MARGIN_X + col * ITEM_WIDTH;
    int y = MARGIN_Y + row * ITEM_HEIGHT;
    
    // 2. Định hình màu sắc
    uint16_t bgColor = isSelected ? COLOR_SELECTED : COLOR_BACKGROUND;
    uint16_t textColor = isSelected ? COLOR_BACKGROUND : COLOR_DEFAULT;

    // 3. Vẽ nền và khung
    tft.fillRect(x, y, ITEM_WIDTH, ITEM_HEIGHT, bgColor);
    tft.drawRect(x, y, ITEM_WIDTH, ITEM_HEIGHT, COLOR_DEFAULT);

    // 4. Vẽ ICON (Giữa)
    tft.setTextColor(textColor, bgColor);
    tft.setTextSize(3); 
    tft.setTextDatum(MC_DATUM); // Căn giữa
    tft.drawString(items[itemIndex].icon, x + ITEM_WIDTH/2, y + 25);
    
    // 5. Vẽ nhãn (Dưới)
    tft.setTextSize(2); 
    tft.drawString(items[itemIndex].label, x + ITEM_WIDTH/2, y + ITEM_HEIGHT - 25);
}

// --- HÀM VẼ TOÀN BỘ MENU ---
void MenuManager::drawMenu() {
    tft.fillScreen(TFT_BLACK); 
    
    // Vẽ tất cả 4 mục Menu
    for (int i = 0; i < MENU_COUNT; i++) {
        drawItem(i, i == selectedItem);
    }
}

// --- HÀM XỬ LÝ ĐIỀU HƯỚNG (NAVIGATION LOGIC) ---
void MenuManager::handleInput(int pressedIndex) {
    int newSelection = selectedItem;

    // Logic di chuyển 2x2 (Hàng 0,1 | Hàng 2,3)
    switch (pressedIndex) {
        case BTN_UP:
            // Di chuyển lên (chỉ nếu đang ở hàng dưới)
            if (selectedItem >= MENU_COLUMNS) { 
                newSelection = selectedItem - MENU_COLUMNS;
            }
            break;
            
        case BTN_DOWN:
            // Di chuyển xuống (chỉ nếu đang ở hàng trên)
            if (selectedItem < MENU_COLUMNS) { 
                newSelection = selectedItem + MENU_COLUMNS;
            }
            break;
            
        case BTN_LEFT:
            // Di chuyển sang trái (chỉ nếu đang ở cột 2)
            if (selectedItem % MENU_COLUMNS != 0) { 
                newSelection = selectedItem - 1;
            }
            break;
            
        case BTN_RIGHT:
            // Di chuyển sang phải (chỉ nếu đang ở cột 1)
            if (selectedItem % MENU_COLUMNS != MENU_COLUMNS - 1) { 
                newSelection = selectedItem + 1;
            }
            break;
    }

    // Nếu có thay đổi, cập nhật trạng thái và vẽ lại
    if (newSelection != selectedItem) {
        // Vẽ lại mục cũ (bỏ chọn)
        drawItem(selectedItem, false); 
        
        selectedItem = newSelection;
        
        // Vẽ lại mục mới (chọn)
        drawItem(selectedItem, true);
        
        Serial.printf("Menu: Navigated to: %s (ID: %d)\n", items[selectedItem].label, selectedItem);
    }
}