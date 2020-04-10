#include <FastLED.h>
#include <FastLED_GFX.h>

#define LED_PIN         16
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812
#define BRIGHTNESS      64

#define CANVAS_WIDTH    8
#define CANVAS_HEIGHT   48
#define NUM_LEDS        (CANVAS_WIDTH * CANVAS_HEIGHT)

GFXcanvas canvas(CANVAS_WIDTH, CANVAS_HEIGHT);

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(canvas.getBuffer(), NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  delay(100);
  FastLED.clear();
  FastLED.show();
}

uint8_t panel = 0;

void loop() {
  EVERY_N_SECONDS(1) {
    // canvas.fillScreen(CRGB::Red);
    canvas.fillScreen(CRGB::Blue);
    canvas.drawRect(0,panel,8,8,CRGB::Red);
    canvas.drawRect(0,32,8,8,CRGB::Yellow);
    canvas.drawRect(0,40,8,8,CRGB::Green);
    // canvas.drawLine(0, 0, CANVAS_WIDTH-1, CANVAS_HEIGHT-1, CRGB::White);
    // canvas.drawCircle(4, 4, 2, CRGB(255,255,0));
    panel=panel+8;
    if(panel==32)panel=0;
    FastLED.show();
  }
}

