#include <FastLED.h>
#include "buttons.h"

// How many leds in your strip?
#define NUM_LEDS 320

// Data pin where the LED strip is connected to. This is for WS2811/WS2812. APA102 needs a clock pin.
#define DATA_PIN 12
//#define CLK_PIN 11

#define MAXBRIGHTNESS 192
#define STARTBRIGHTNESS 40

#define MOTION_PIN  18

// Define the array of leds
CRGB leds[NUM_LEDS];

uint8_t hue = 0;
uint8_t chase = 0;
int16_t counter = 0;
int16_t crawlCounter = 0;
int8_t  beaconCounter = 0;
float   timer;
byte    currentBrightness = STARTBRIGHTNESS; // 0-255 will be scaled to 0-MAXBRIGHTNESS

#include <avr/io.h>
#include <avr/interrupt.h>
#define PinInt1 18
volatile int16_t INTS = 0;

#include "effects.h"
typedef void (*functionList)();
functionList effectList[] = {
  sensor,
  inoise8_mover,
  fire,
  threeSine,
  crawl2,
  crawl,
  beacon,
  rainbow,
  cylon,
  twinkles,
};

const byte numEffects = (sizeof(effectList) / sizeof(effectList[0]));
byte currentEffect = 0; // index to the currently running effect

void setup() {
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  LEDS.setBrightness(currentBrightness);
  // Initialize the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(BUTTON_PIN, HIGH);
  dist = random16(12345);

  pinMode(PinInt1, INPUT); // sets the digital pin as output
  attachInterrupt(PinInt1, isrService, FALLING); // interrrupt 1 is data ready

}

void loop() {
  EVERY_N_MILLISECONDS(50) {
    crawlCounter < 323 ? crawlCounter++ : crawlCounter = 0;
  }
  if (counter < 320) {
    counter++;
  } else {
    counter = 0;
    if (beaconCounter < 1) {
      beaconCounter++;
    } else {
      beaconCounter = 0;
    }
  }

  // First slide the led in one direction
  EVERY_N_MILLISECONDS(10) {
    hue++;
  }
  // cycle controls the theater chase animation
  EVERY_N_MILLISECONDS( 100 ) {
    chase++;
    if (chase == 3) {
      chase = 0;
    }
  }
  buttons();
  effectList[currentEffect]();
  FastLED.show();
}

void buttons() {
  uint8_t buttonClick = checkButton();
  switch (buttonClick) {
    case 1:
      currentEffect++;
      counter = 0;
      if (currentEffect > (numEffects - 1)) currentEffect = 0;
      break;
    case 2:
      currentBrightness += 51; // increase the brightness (wraps to lowest)
      if (currentBrightness < 40) currentBrightness = 40;
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
      break;
  }
}
