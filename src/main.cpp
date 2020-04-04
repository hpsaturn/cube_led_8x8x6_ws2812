/**
 * Dice implementation on Digispark with NeoPixel of 12bits (WS2812)
 * 
 * Framework: PlatformIO - Arduino 
 * 
 * Author: @hpsaturn 2019-2020 
 * 
 * Revision:
 * 0000 :
 * ***********************************************************************************/

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel *strip;
int pixelFormat = NEO_GRB + NEO_KHZ800;
int pin = 16;             // Ring Led input
int numPixels = 384;      // leds on hardware
uint8_t brightness = 30;  // After that is loaded from eeprom
                          // and changet it via key
#define RNDPIN      17    // for random generator, P4 is analog input 2

uint32_t getRandomColor() {
  return strip->Color(random(brightness), random(brightness), random(brightness));
}

void loadColor(uint32_t color, int wait) {
  for(int i=0; i<numPixels; i++) {    // For each pixel in strip...
    strip->setPixelColor(i, color);   // Set pixel's color (in RAM)
    strip->show();                    // Update strip to match
    delay(wait);                      // Pause for a moment
  }
}

void loadImage(uint32_t *image, int wait){  // Load complete image on the ring
  for(int i=0; i<numPixels; i++) {    
    strip->setPixelColor(i,image[i]);
    strip->show();
    delay(wait);
  }
}

void rainbow(int wait) {                // Adafruit rainbow (see library for explanation)
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<numPixels; i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / numPixels);
      strip->setPixelColor(i, strip->gamma32(strip->ColorHSV(pixelHue)));
    }
    strip->show();
    delay(wait);
  }
}

void setup() {
  // initialize pseudo-random number generator with some random value
  int seed = analogRead(RNDPIN);
  // debugBlink(seed);
  randomSeed(seed);
  strip = new Adafruit_NeoPixel(numPixels, pin, pixelFormat);
  strip->begin();
  // initialize brightness
  strip->setBrightness(brightness);
  strip->show();
  strip->begin(); // INITIALIZE NeoPixel strip object (REQUIRED) 
}

void loop() {
  rainbow(1);
  //uint32_t cbg = strip->Color(0,255,0);    // background color
  //loadColor(cbg,100);
  //delay(10);
}
