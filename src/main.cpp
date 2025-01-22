#include <BluetoothSerial.h>
#include <FastLED.h>

#define DEEP_SLEEP_DURATION 30  // sleep x seconds and then wake up

#define LED_PIN 16
#define NUM_LEDS 8 * 8 * 6
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

CRGB leds[NUM_LEDS];

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

BluetoothSerial btSerial;  // Object for Bluetooth
int incoming;
bool enablePaletteDemo = true;
bool enableAnimsLoop = true;
uint8_t speed = 3;
uint8_t mbrightness = 60;

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette = CRGBPalette16(green, green, black, black, purple, purple, black, black, green,
                                 green, black, black, purple, purple, black, black);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, mbrightness, currentBlending);
    colorIndex += 3;
  }
}

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 2000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    lastSecond = secondHand;
    if (secondHand == 0) {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 10) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 15) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 20) {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 25) {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 30) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if (secondHand == 35) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 40) {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 45) {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 50) {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 55) {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }

    if (secondHand == 10) {
      btSerial.print(".");
    }
  }
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
    CRGB::Red,
    CRGB::Gray,  // 'white' is too bright compared to red and blue
    CRGB::Blue, CRGB::Black,

    CRGB::Red,  CRGB::Gray,  CRGB::Blue,  CRGB::Black,

    CRGB::Red,  CRGB::Red,   CRGB::Gray,  CRGB::Gray,
    CRGB::Blue, CRGB::Blue,  CRGB::Black, CRGB::Black};

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("[I][setup] client connected");
  }
  if (event == ESP_SPP_CLOSE_EVT) {
    Serial.println("[I][setup] client disconnected");
  }
}

void gotToSuspend() {
  Serial.println("[I][CMD] suspending..");
  delay(10);  // waiting for writing msg on serial
  // esp_sleep_enable_timer_wakeup(1000000LL * DEEP_SLEEP_DURATION);
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(500);  // power-up safety delay
  btSerial.begin("[I][setup] CubeLED");
  btSerial.register_callback(callback);
  Serial.println("[I][setup] bluetooth serial ready");
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(mbrightness);
  Serial.print("[I][setup] brightness: ");
  Serial.println(mbrightness);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  Serial.println("[I][setup] setup ready");
}

void animPaletteLoop() {
  if (enableAnimsLoop) ChangePalettePeriodically();

  static uint8_t startIndex = 0;
  startIndex = startIndex + speed; /* motion speed */

  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void loop() {
  if (btSerial.isReady()) {
    incoming = btSerial.read();  // Read what we recevive

    if (incoming == 49) {
      digitalWrite(LED_BUILTIN, HIGH);
      enablePaletteDemo = true;
      Serial.println("[V][CMD] animation on");
      btSerial.println("animation turned on");
    }

    if (incoming == 48) {
      digitalWrite(LED_BUILTIN, LOW);
      enablePaletteDemo = false;
      FastLED.clear();
      FastLED.show();
      Serial.println("[V][CMD] animation off");
      btSerial.println("animation turned off");
    }

    if (incoming == 43) {
      btSerial.println("speed up");
      speed++;
      btSerial.print("speed: ");
      btSerial.println(speed);
      Serial.println("[V][CMD] speed Up");
    }

    if (incoming == 45) {
      btSerial.println("speed down");
      if (speed > 0) speed--;
      btSerial.print("speed: ");
      btSerial.println(speed);
      Serial.println("[V][CMD] speed down");
    }

    if (incoming == 80) {
      btSerial.println("shutdown ESP32..");
      Serial.println("[V][CMD] shutdown ESP32..");
      FastLED.clear();
      FastLED.show();
      delay(1000);
      btSerial.println("halt.");
      gotToSuspend();
    }

    if (incoming == 53) {
      btSerial.println("brightness up");
      if (mbrightness < 254) mbrightness++;
      btSerial.print("brightness: ");
      btSerial.println(mbrightness);
      Serial.println("[V][CMD] brightness up");
    }

    if (incoming == 52) {
      btSerial.println("brighness down");
      if (mbrightness > 0) mbrightness--;
      btSerial.print("brightness: ");
      btSerial.println(mbrightness);
      Serial.println("[V][CMD] brightness down");
    }

    if (incoming == 82) {
      btSerial.println("rainbowColors");
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
      Serial.println("[V][CMD] rainbownColors");
    }

    if (incoming == 83) {
      btSerial.println("stopAnimsChange");
      enableAnimsLoop = false;
      Serial.println("[V][CMD] stopAnimsChange");
    }

    if (incoming == 112) {
      btSerial.println("playAnimsChange");
      enableAnimsLoop = true;
      Serial.println("[V][CMD] playAnimsChange");
    }

    if (incoming == 104) {
      Serial.println("[V][CMD] show commands help");
      btSerial.println("Commands:");
      btSerial.println("==========");
      btSerial.println("1: Animation on");
      btSerial.println("0: Animation off");
      btSerial.println("R: resetToRainbow");
      btSerial.println("S: stopAnimsChange");
      btSerial.println("p: playAnimsChange");
      btSerial.println("0: After off, clear");
      btSerial.println("4: Brightness down");
      btSerial.println("5: Brightness up");
      btSerial.println("+: Speed Up");
      btSerial.println("-: Speed Down");
      btSerial.println("P: Power Off");
    }
  }

  if (enablePaletteDemo) animPaletteLoop();
}
