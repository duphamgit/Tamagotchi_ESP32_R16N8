#include <Arduino.h> 
#include <TFT_eSPI.h> 
#include "SystemMonitor.h" 
#include "InputManager.h"  
#include "ButtonManager.h" 
#include "LEDManager.h" 
#include "MenuManager.h" 
#include "WifiManager.h" // Thêm WifiManager

// --- KHAI BÁO CÁC HẰNG SỐ HIỂN THỊ TOÀN CỤC ---
const int MSG_X = 5;
const int MSG_Y = 165; 
const int MSG_WIDTH = 230; 
const int MSG_HEIGHT = 20; 

// --- KHAI BÁO THÔNG SỐ PHẦN CỨNG TOÀN CỤC ---
const int TOUCH_BUTTON_PIN = 13; 

// --- KHAI BÁO ENUM CHO TRẠNG THÁI HIỂN THỊ ---
enum DisplayState {
    STATE_MENU,       
    STATE_MONITOR,    // Màn hình System Monitor
    STATE_WIFI_SCAN,  // Màn hình quét Wi-Fi (TRẠNG THÁI MỚI)
    STATE_INPUT_WIFI_PASSWORD, // Màn hình nhập mật khẩu Wi-Fi
    STATE_WIFI_CONNECTING // Màn hình kết nối Wi-Fi
};

// --- KHAI BÁO CÁC ĐỐI TƯỢNG TOÀN CỤC (GLOBAL OBJECTS) ---
TFT_eSPI tft = TFT_eSPI(); 
SystemMonitor monitor(tft);
InputManager touchButton(TOUCH_BUTTON_PIN); 
ButtonManager physicalButtons;              
LEDManager ledControl;
MenuManager menuManager(tft); 
WifiManager wifiManager(tft); // Khởi tạo WifiManager

// --- BIẾN TRẠNG THÁI TOÀN CỤC ---
DisplayState currentState = STATE_MENU;
bool needsRedraw = true; // Cờ để theo dõi khi nào cần vẽ lại
// --- CÁC HÀM HỖ TRỢ ---
/**
 * @brief Kiểm tra nút bấm vật lý (one-shot, chỉ trả về true 1 lần khi nhấn).
 * Đảm bảo logic debounce và one-shot hoạt động ổn định.
 */
bool checkPhysicalButtonOneShot(ButtonIndex index) {
    // Khai báo static một lần để giữ trạng thái nhấn giữa các lần gọi hàm
    static bool wasPressed[BTN_COUNT] = {false};
    
    // Nếu nút đang được nhấn (sau khi đã debounce trong ButtonManager::update())
    if (physicalButtons.isPressed(index)) {
        // Nếu đây là lần đầu tiên chúng ta phát hiện nút được nhấn
        if (!wasPressed[index]) {
            wasPressed[index] = true; // Đặt cờ là đã nhấn
            return true;              // Trả về TRUE, kích hoạt hành động
        }
        // Nếu đã được nhấn (wasPressed[index] là true), tiếp tục trả về FALSE
    } else {
        // Nếu nút đã được nhả ra (không còn nhấn nữa)
        wasPressed[index] = false; // Đặt lại cờ, sẵn sàng cho lần nhấn tiếp theo
    }
    return false;
}

// --- SETUP ---
void setup() {
    Serial.begin(115200);
    Serial.println("\n--- System Booting Up ---");
    
    // Khởi tạo TFT
    tft.init();
    tft.setRotation(0); // Đã sửa: Chuyển về chế độ dọc (Portrait)
    tft.fillScreen(TFT_BLACK);
    
    // Khởi tạo các quản lý
    physicalButtons.begin();
    ledControl.begin();
    wifiManager.begin(); // Khởi tạo Wi-Fi
    // Khởi tạo trạng thái ban đầu
    currentState = STATE_MENU;
    menuManager.drawMenu();
}

// --- LOOP ---
void loop() {
    // 1. Cập nhật input (Phải gọi thường xuyên)
    physicalButtons.update();
    
    // 2. XỬ LÝ CHUYỂN TRẠNG THÁI CƠ BẢN (Nút MENU)
    if (checkPhysicalButtonOneShot(BTN_MENU)) { 
        if (currentState != STATE_MENU) {
            // Thoát khỏi bất kỳ màn hình nào khác -> Chuyển về MENU
            currentState = STATE_MENU;
            menuManager.drawMenu(); // Vẽ lại Menu
            Serial.println("State Changed: -> MENU");
            //Tắt đèn Led
            ledControl.setColor(0, 0, 0); // Tắt LED khi về menu
        }
    }

    // 3. XỬ LÝ LOGIC DỰA TRÊN TRẠNG THÁI HIỆN TẠI
    
    switch (currentState) {
        case STATE_MENU:
            if (checkPhysicalButtonOneShot(BTN_UP)) { 
                menuManager.handleInput(BTN_UP);
            } else if (checkPhysicalButtonOneShot(BTN_DOWN)) { 
                menuManager.handleInput(BTN_DOWN);
            } 
            
            if (checkPhysicalButtonOneShot(BTN_SELECT)) { 
                int selectedID = menuManager.getSelectedItem();
                const char* selectedLabel = menuManager.getItemLabel(selectedID); 
                
                Serial.printf("ACTION: Selected Item (ID %d): %s\n", selectedID, selectedLabel);
                
                if (selectedID == MENU_WIFI) {
                    needsRedraw = true;
                    currentState = STATE_WIFI_SCAN;
                    wifiManager.drawScreen();
                    wifiManager.scanNetworks();
                    Serial.println("State Changed: MENU -> WIFI SCAN. Starting scan...");
                } else {
                    menuManager.animateSelection(selectedID);
                    tft.fillRect(MSG_X, MSG_Y, MSG_WIDTH, MSG_HEIGHT, TFT_BLACK); 
                    tft.setCursor(MSG_X, MSG_Y); 
                    tft.setTextSize(2);
                    tft.setTextColor(TFT_GREEN, TFT_BLACK);
                    tft.printf("Selected: %s", selectedLabel);
                }
            }
            break;

// -----------------------------------------------------------------------------

        case STATE_WIFI_SCAN:
            ledControl.runGreenFade();
            
            if (checkPhysicalButtonOneShot(BTN_UP)) {
                wifiManager.handleInput(BTN_UP);
            } else if (checkPhysicalButtonOneShot(BTN_DOWN)) {
                wifiManager.handleInput(BTN_DOWN);
            }
            
            if (checkPhysicalButtonOneShot(BTN_SELECT)) {
                currentState = STATE_INPUT_WIFI_PASSWORD;
            }
            break;

// -----------------------------------------------------------------------------

        case STATE_INPUT_WIFI_PASSWORD:
            if (needsRedraw) {
                wifiManager.drawInputPasswordScreen();
                needsRedraw = false; // Đặt lại cờ sau khi vẽ
            }
            // Xử lý nút SELECT để xác nhận mật khẩu và CHUYỂN TRẠNG THÁI
            if (checkPhysicalButtonOneShot(BTN_SELECT)) {
                currentState = STATE_WIFI_CONNECTING;
                needsRedraw = true;
                Serial.println("State Changed: INPUT WIFI PASSWORD -> WIFI CONNECTING.");
            }
            break; 

// -----------------------------------------------------------------------------

        case STATE_WIFI_CONNECTING:
            if (needsRedraw) {
                wifiManager.drawConnectingScreen();
                needsRedraw = false; // Đặt lại cờ sau khi vẽ
            }
            break;

// -----------------------------------------------------------------------------
        
        default:
            break; 
    }
    delay(10); 
}