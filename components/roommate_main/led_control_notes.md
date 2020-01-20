
//Convert RGB to RBG for WS2812
#define RGB(R, G, B)  ((((uint32_t)R) << 16) | (((uint32_t)B) << 8) | G)

// Scale each one down to S percent
#define RGBB(R, G, B, S)  ((((uint32_t)R / (S/100)) << 16) | (((uint32_t)B / (S/100)) << 8) | G / (S/100))


set_led(1, RGB(0xFF, 0x00, 0x00)); // for RED

or

set_led(1, RGBB(0xFF, 0x00, 0x00, 50)); // for RED @ 50% brightness

void set_led(index, RBG);