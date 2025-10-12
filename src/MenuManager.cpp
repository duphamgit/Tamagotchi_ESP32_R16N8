#include "MenuManager.h"
#include "ButtonManager.h" 

// --- HẰNG SỐ CẤU HÌNH MENU ---
#define MENU_COLUMNS 2
#define MENU_ROWS 2
#define ITEM_WIDTH 120
#define ITEM_HEIGHT 70
#define MARGIN_X 0
#define MARGIN_Y 0

// --- MÀU SẮC ---
#define COLOR_DEFAULT TFT_GREEN
#define COLOR_SELECTED TFT_DARKGREEN
#define COLOR_BACKGROUND TFT_BLACK


MenuManager::MenuManager(TFT_eSPI& displayRef) : tft(displayRef) {}

// --- HÀM HỖ TRỢ: VẼ TỪNG MỤC ---
void MenuManager::drawItem(int itemIndex, bool isSelected) {
    // Tọa độ và kích thước chuẩn
    int row = itemIndex / MENU_COLUMNS;
    int col = itemIndex % MENU_COLUMNS;
    
    int x = MARGIN_X + col * ITEM_WIDTH;
    int y = MARGIN_Y + row * ITEM_HEIGHT;
    
    uint16_t bgColor = isSelected ? COLOR_SELECTED : COLOR_BACKGROUND;
    uint16_t textColor = isSelected ? COLOR_BACKGROUND : COLOR_DEFAULT;

    // 1. Vẽ nền và khung
    tft.fillRect(x, y, ITEM_WIDTH, ITEM_HEIGHT, bgColor);
    tft.drawRect(x, y, ITEM_WIDTH, ITEM_HEIGHT, COLOR_DEFAULT);

    // 2. Vẽ ICON và Label
    tft.setTextColor(textColor, bgColor);
    tft.setTextSize(3); 
    tft.setTextDatum(MC_DATUM); 
    tft.drawString(items[itemIndex].icon, x + ITEM_WIDTH/2, y + 25);
    
    tft.setTextSize(2); 
    tft.drawString(items[itemIndex].label, x + ITEM_WIDTH/2, y + ITEM_HEIGHT - 25);
}


// 🌟 HÀM MỚI: TẠO HIỆU ỨNG CHUYỂN ĐỘNG CHO MỤC ĐƯỢC CHỌN 🌟
void MenuManager::animateSelection(int itemIndex) {
    // 1. Lấy tọa độ chuẩn
    int row = itemIndex / MENU_COLUMNS;
    int col = itemIndex % MENU_COLUMNS;
    int x_base = MARGIN_X + col * ITEM_WIDTH;
    int y_base = MARGIN_Y + row * ITEM_HEIGHT;

    // Kích thước hiệu ứng (Co lại)
    const int SHRINK_SIZE = 10; 
    
    // Màu sắc
    uint16_t bgColor = COLOR_SELECTED; 
    uint16_t textColor = COLOR_BACKGROUND;

    // --- BƯỚC 1: CO LẠI (Shrink) ---
    int x_shrink = x_base + SHRINK_SIZE / 2;
    int y_shrink = y_base + SHRINK_SIZE / 2;
    int w_shrink = ITEM_WIDTH - SHRINK_SIZE;
    int h_shrink = ITEM_HEIGHT - SHRINK_SIZE;

    // Vẽ nền co lại
    tft.fillRect(x_shrink, y_shrink, w_shrink, h_shrink, bgColor);
    tft.drawRect(x_shrink, y_shrink, w_shrink, h_shrink, COLOR_DEFAULT);

    // Vẽ ICON và Label ở vị trí co lại (dùng tọa độ căn giữa chuẩn)
    tft.setTextColor(textColor, bgColor);
    tft.setTextSize(3); 
    tft.setTextDatum(MC_DATUM); 
    tft.drawString(items[itemIndex].icon, x_base + ITEM_WIDTH/2, y_base + 25);
    tft.setTextSize(2); 
    tft.drawString(items[itemIndex].label, x_base + ITEM_WIDTH/2, y_base + ITEM_HEIGHT - 25);
    
    delay(50); // Giữ hiệu ứng trong 50ms

    // --- BƯỚC 2: PHÓNG TO VỀ CHUẨN (Stretch/Snap back) ---
    drawItem(itemIndex, true);
}


// --- HÀM VẼ TOÀN BỘ MENU ---
void MenuManager::drawMenu() {
    tft.fillScreen(TFT_BLACK); 
    for (int i = 0; i < MENU_COUNT; i++) {
        drawItem(i, i == selectedItem);
    }
}

// --- HÀM XỬ LÝ ĐIỀU HƯỚNG ---
void MenuManager::handleInput(int pressedIndex) {
    int newSelection = selectedItem;

    // Logic điều hướng 2 nút TUẦN TỰ
    switch (pressedIndex) {
        case BTN_DOWN: // Di chuyển TIẾN
            newSelection = (selectedItem + 1) % MENU_COUNT;
            break;
            
        case BTN_UP: // Di chuyển LÙI
            newSelection = (selectedItem - 1 + MENU_COUNT) % MENU_COUNT;
            break;
            
        default: return; 
    }

    if (newSelection != selectedItem) {
        // 1. Vẽ lại mục cũ (bỏ chọn)
        drawItem(selectedItem, false); 
        
        selectedItem = newSelection;
        
        // 2. SỬ DỤNG HÀM ANIMATE MỚI
        animateSelection(selectedItem);

        Serial.printf("Menu: Navigated sequentially to: %s (ID: %d)\n", items[selectedItem].label, selectedItem);
    }
}