#include <FastLED.h>

// ================= USER CONFIG =================
#define NUM_LEDS   114      // Set to your strip length
#define DATA_PIN   0       // Set to your data pin
#define COLOR_DELAY 1000    // ms per color
// ===============================================

// Standard FastLED CRGB array
CRGB leds[NUM_LEDS];

// Set up the RGBW emulation (change W3 to W2 for RGBW order if needed)
Rgbw rgbw = Rgbw(
    kRGBWDefaultColorTemp,
    kRGBWExactColors,      // Mode: exact color matching
    W3                     // W placement: W3=GRBW, W2=RGBW
);

typedef SK6812<DATA_PIN, RGB> ControllerT;
static RGBWEmulatedController<ControllerT, GRB> rgbwEmu(rgbw);

const CRGB testColors[] = {
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Cyan,
  CRGB::Blue,
  CRGB::Magenta
};
const uint8_t numTestColors = sizeof(testColors) / sizeof(testColors[0]);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds(&rgbwEmu, leds, NUM_LEDS);
  FastLED.setBrightness(128);
  delay(2000);  // If anything goes wrong, this gives time for re-upload
}

void showAll(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

void loop() {
  for (uint8_t i = 0; i < numTestColors; ++i) {
    for (int pix = 0; pix < NUM_LEDS; ++pix) {
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      leds[pix] = testColors[i];
      FastLED.show();
      delay(COLOR_DELAY);
    }
  }
  // White chase
  for (int pix = 0; pix < NUM_LEDS; ++pix) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[pix] = CRGB::White;
    FastLED.show();
    delay(COLOR_DELAY);
  }
}

