#include <FastLED.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

#include "buttons.h"

// How many leds in your strip?
#define NUM_LEDS 320
const int8_t ledblock = 5;

// Data pin where the LED strip is connected to. This is for WS2811/WS2812. APA102 needs a clock pin.
#define DATA_PIN 12
//#define CLK_PIN  11

#define MAXBRIGHTNESS 192
#define STARTBRIGHTNESS 40

// Define the array of leds
CRGB leds[NUM_LEDS];

// Main variables
uint8_t hue = 0;
uint8_t chase = 0;
int16_t counter = 0;
int16_t crawlCounter = 0;
int8_t  beaconCounter = 0;
float   timer;
byte    currentBrightness = STARTBRIGHTNESS; // 0-255 will be scaled to 0-MAXBRIGHTNESS
bool    autoCycle;
//bool    eepromOutdated;

fract8 chanceOfGlitter = 100;

uint16_t effectDelay = 0;
long effectMillis;
long currentMillis;

// Accelerometer initialization and associated variables and constants
Adafruit_MMA8451 mma = Adafruit_MMA8451();
float accel;
float vectorDiff;
float currentDiff;
float maxdiff;
const float gravity = 9.8;

#include "accel.h"
#include "effects.h"

const uint8_t runMode = 0; // Set to 1 for testing effects in the list1

typedef void (*functionList)();
functionList effectList0[] = {
  ripple,
  sensor,
  beacon,
  flash,
  glitter,
  rider,
  plasma,
  inoise8_mover,
  fire,
  //  fire2,
  threeSine,
  crawl2,
  crawl,
  rainbow,
  cylon,
  twinkles,
};

functionList effectList1[] = {
  blurpattern2,
};

byte currentEffect = 0; // index to the currently running effect
byte numEffects;

void setup() {
  //  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  LEDS.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // check to see if EEPROM has been used yet
  // if so, load the stored settings
  byte eepromWasWritten = EEPROM.read(0);
  if (eepromWasWritten == 99) {
    currentEffect =     EEPROM.read(1);
    autoCycle =         EEPROM.read(2);
    currentBrightness = EEPROM.read(3);
    //    randomizedEffect =  EEPROM.read(4);
    //    cycleTime =         EEPROM.read(5) * 1000;
  }

  // set global brightness value
  FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));

  // Initialize the button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(BUTTON_PIN, HIGH);
  mma.begin();
  mma.setRange(MMA8451_RANGE_4_G);

  switch (runMode) {
    case 0: // normal patterns
      numEffects = (sizeof(effectList0) / sizeof(effectList0[0]));
      break;

    case 1: // Christmas patterns
      numEffects = (sizeof(effectList1) / sizeof(effectList1[0]));
      break;
  }

}

void loop() {
  currentMillis = millis(); // save the current timer value

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

  // Read the buttons
  buttons();

  // Effects!
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;

    switch (runMode) {
      case 0:
        effectList0[currentEffect]();
        break;

      case 1:
        effectList1[currentEffect]();
        break;
    }

    random16_add_entropy(1); // make the random values a bit more random-ish
  }
  FastLED.show();

  // Save the current time for state.
  now = millis();
  checkEEPROM();
}

void buttons() {
  uint8_t buttonClick = checkButton();
  switch (buttonClick) {
    case 1:
      currentEffect++;
      if (currentEffect > (numEffects - 1)) currentEffect = 0;
      counter = 0;
      break;
    case 2:
      currentBrightness += 51; // increase the brightness (wraps to lowest)
      if (currentBrightness < 40) currentBrightness = 40;
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
      break;
  }
}

// write EEPROM value if it's different from stored value
void updateEEPROM(byte location, byte value) {
  if (!autoCycle) {
    if (EEPROM.read(location) != value) EEPROM.write(location, value);
  }
}

// Write settings to EEPROM if necessary
void checkEEPROM() {
  if (eepromOutdated) {
    if (now - eepromMillis > EEPROMDELAY) {
      updateEEPROM(0, 99);
      updateEEPROM(1, currentEffect);
      updateEEPROM(2, autoCycle);
      updateEEPROM(3, currentBrightness);
      //      updateEEPROM(4, randomizedEffect);
      //      updateEEPROM(5, paletteN);
      //      updateEEPROM(6, randomPalette);
      //      updateEEPROM(7, cycleTime/1000);
      eepromOutdated = false;
    }
  }
}

