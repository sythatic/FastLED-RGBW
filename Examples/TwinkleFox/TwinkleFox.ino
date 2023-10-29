#include "FastLED.h"
#include "CRGBW.h"

#define NUM_LEDS      142
#define LED_TYPE      SK6812
#define COLOR_ORDER   RGB
#define DATA_PIN      3
#define VOLTS         5
#define MAX_MA        4000

CRGBW leds[NUM_LEDS];
#define NUM_LEDS_RGB  (NUM_LEDS * 3) // Number of LEDs for RGB color components
CRGB *ledsRGB = (CRGB *) &leds[0];

#define TWINKLE_SPEED 3
#define TWINKLE_DENSITY 4
#define SECONDS_PER_PALETTE 30

CRGB gBackgroundColor = CRGB::Black;

#define AUTO_SELECT_BACKGROUND_COLOR 0
#define COOL_LIKE_INCANDESCENT 0

CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;

void setup() {
  delay(3000); // Safety startup delay
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(ledsRGB, NUM_LEDS_RGB).setCorrection(TypicalLEDStrip);

  chooseNextColorPalette(gTargetPalette);
}

void loop() {
  EVERY_N_SECONDS(SECONDS_PER_PALETTE) {
    chooseNextColorPalette(gTargetPalette);
  }

  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
  }

  drawTwinkles(leds);

  FastLED.show();
}

void drawTwinkles(CRGBW *leds) {
  uint16_t PRNG16 = 11337;
  uint32_t clock32 = millis();

  CRGBW bg;
  if ((AUTO_SELECT_BACKGROUND_COLOR == 1) && (gCurrentPalette[0] == gCurrentPalette[1])) {
    bg = gCurrentPalette[0];
    uint8_t bglight = (bg.r + bg.g + bg.b + bg.white) / 4;
    if (bglight > 64) {
      bg.r = (bg.r * 16) / 255;
      bg.g = (bg.g * 16) / 255;
      bg.b = (bg.b * 16) / 255;
      bg.white = (bg.white * 16) / 255;
    } else if (bglight > 16) {
      bg.r = (bg.r * 64) / 255;
      bg.g = (bg.g * 64) / 255;
      bg.b = (bg.b * 64) / 255;
      bg.white = (bg.white * 64) / 255;
    } else {
      bg.r = (bg.r * 86) / 255;
      bg.g = (bg.g * 86) / 255;
      bg.b = (bg.b * 86) / 255;
      bg.white = (bg.white * 86) / 255;
    }
  } else {
    bg = gBackgroundColor;
  }

  uint8_t backgroundBrightness = (bg.r + bg.g + bg.b + bg.white) / 4;

  for (int i = 0; i < NUM_LEDS; i++) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;
    uint16_t myclockoffset16 = PRNG16;
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8;

    CRGBW c = computeOneTwinkle(myclock30, myunique8);

    uint8_t cbright = (c.r + c.g + c.b + c.white) / 4;
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg.r && !bg.g && !bg.b && !bg.white)) {
      leds[i] = c;
    } else if (deltabright > 0) {
      leds[i].r = (leds[i].r * deltabright * 8) / 255;
      leds[i].g = (leds[i].g * deltabright * 8) / 255;
      leds[i].b = (leds[i].b * deltabright * 8) / 255;
      leds[i].white = (leds[i].white * deltabright * 8) / 255;
    } else {
      leds[i] = bg;
    }
  }
}

CRGBW computeOneTwinkle(uint32_t ms, uint8_t salt) {
  uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGBW c;
  if (bright > 0) {
    CRGB color = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
    c.r = color.r;
    c.g = color.g;
    c.b = color.b;
    c.white = 0;
    if (COOL_LIKE_INCANDESCENT == 1) {
      coolLikeIncandescent(c, fastcycle8);
    }
  } else {
    c.r = 0;
    c.g = 0;
    c.b = 0;
    c.white = 0;
  }
  return c;
}

uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

void coolLikeIncandescent(CRGBW& c, uint8_t phase) {
  if (phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}
const TProgmemRGBPalette16 RedWhiteBlue_p FL_PROGMEM =
{  CRGB::White, CRGB::Red, CRGB::White, CRGB::Red, 
   CRGB::White, CRGB::Red, CRGB::White, CRGB::Red, 
   CRGB::White, CRGB::Red, CRGB::White, CRGB::Red, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue };

const TProgmemRGBPalette16 RedWhiteGreen_p FL_PROGMEM =
{  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
{  Holly_Red, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Red, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Red, Holly_Green, 
   Holly_Red, Holly_Green, Holly_Green, Holly_Red 
};

#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
{  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
{  0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
#define C9_White  0x606820
const TProgmemRGBPalette16 Retro_p FL_PROGMEM =
{  C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue,
   C9_White
};

const TProgmemRGBPalette16* ActivePaletteList[] = {

  //&LavaColors_p
  //&CloudColors_p
  //&OceanColors_p
  //&ForestColors_p
  //&HeatColors_p
  &RainbowColors_p,
  //&PartyColors_p,

  //&Retro_p,
  //&FairyLight_p,
  //&RedWhiteBlue_p,
  //&RedWhiteGreen_p,
  //&Holly_p, 
  //&Snow_p,
};

void chooseNextColorPalette( CRGBPalette16& pal)
{
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1; 
  whichPalette = addmod8( whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}