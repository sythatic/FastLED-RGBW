#include <SK6812.h>

SK6812 LED(2);

RGBW color1 = {0, 0, 255, 50}; // 255 blue, 50 white

void setup() {
	LED.set_output(4); // Digital Pin 4
	
	LED.set_rgbw(0, color1); // Set first LED to color1
}

void loop() {
	
	LED.set_rgbw(1, {0, 0, 0, 255}); // Set second LED to white (using only W channel)
	LED.sync(); // Send the values to the LEDs
	delay(500);
	
	LED.set_rgbw(1, {255, 255, 255, 0}); // Set second LED to white (using only RGB channels)
	LED.sync();
	delay(500);
}