#include "LEDManager.h"

/**
 * @brief Constructor.
 */
LEDManager::LEDManager() 
    // Initialize the Adafruit_NeoPixel object
    : strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800) {}

/**
 * @brief Initialize the LED module.
 */
void LEDManager::begin() {
    strip.begin();
    // We control brightness directly via the color value, not setBrightness()
    strip.show(); // Turn off all LEDs initially
    Serial.printf("LEDManager: RGB LED initialized on GPIO %d\n", LED_PIN);
}

/**
 * @brief Set a static color for the LED.
 * @param r Red value (0-255).
 * @param g Green value (0-255).
 * @param b Blue value (0-255).
 */
void LEDManager::setColor(uint8_t r, uint8_t g, uint8_t b) {
    strip.setPixelColor(0, r, g, b); // Set color for the first LED (index 0)
    strip.show(); // Update display
}

/**
 * @brief Execute the gradual fade in and out effect (Fade/Breathing) in green.
 */
void LEDManager::runGreenFade() {
    unsigned long currentMillis = millis();

    // Update the brightness value every 'updateInterval' (10ms)
    if (currentMillis - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentMillis;
        
        // 1. Update the current brightness (brightness) by the step value (step)
        brightness += step;

        // 2. Check limits: Reverse the direction when min/max is reached
        if (brightness <= 0) {
            brightness = 0;
            step = 1; // Start fading in (getting brighter)
        } else if (brightness >= MAX_BRIGHTNESS) {
            brightness = MAX_BRIGHTNESS;
            step = -1; // Start fading out (getting dimmer)
        }

        // 3. Apply the current brightness to the Green color component
        // Color value: Red=0, Green=brightness (max 80), Blue=0
        strip.setPixelColor(0, 0, brightness, 0); 
        
        strip.show(); // Display the new state
    }
}
