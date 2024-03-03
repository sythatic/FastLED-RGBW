#ifndef CRGBW_h
#define CRGBW_h

struct CRGBW  {
	union {
		struct {
			union {
				uint8_t g;
				uint8_t green;
			};
			union {
				uint8_t r;
				uint8_t red;
			};
			union {
				uint8_t b;
				uint8_t blue;
			};
			union {
				uint8_t w;
				uint8_t white;
			};
		};
		uint8_t raw[4];
	};
	CRGBW(){}
	CRGBW(uint8_t red, uint8_t green, uint8_t blue, uint8_t white){
		r = red;
		g = green;
		b = blue;
		w = white;
	}
	inline void operator = (const CRGB c) __attribute__((always_inline)){ 
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->white = 0;
	}
};
inline uint16_t getRGBWsize(uint16_t nleds){
	uint16_t nbytes = nleds * 4;
	if(nbytes % 3 > 0) return nbytes / 3 + 1;
	else return nbytes / 3;
}

#endif