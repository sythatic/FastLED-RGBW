#include "FastLED.h"
#include "../CRGBW.h"

#define NUM_LEDS 143
#define DATA_PIN 2
#define COLOR_ORDER   RGB
#define LED_TYPE   SK6812

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

const uint8_t brightness = 5;

void setup() { 
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(ledsRGB, getRGBWsize(NUM_LEDS));
	FastLED.setBrightness(brightness);
	FastLED.show();
}
void loop(){
	rainbowLoop();
}
void rainbow(){
	static uint8_t hue;
 
	for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CHSV((i * 256 / NUM_LEDS) + hue, 255, 255);
	}
	FastLED.show();
	hue++;
}
void rainbowLoop(){
	long millisIn = millis();
	long loopTime = 5000; // 5 seconds
 
	while(millis() < millisIn + loopTime){
		rainbow();
		delay(5);
	}
}
