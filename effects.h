void rainbow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[319 - i] = CHSV(hue - i, 255, 255);
  }
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(240);
  }
}

void allBlack() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(100);
  }
}

void crawl() {
  if (counter % 2 == 0) {
    int i = counter;
    int j;
    counter < 160 ? j = counter + 160 : j = counter - 160;
    leds[319 - i] = leds[i] = leds[319 - j] = leds[j] = CHSV(hue, 255, 255);
    fadeall();
  }
}

void crawl2() {
  allBlack();
  for (int p = 0; p < 19 ; p++) {
    int n = 17 * p + crawlCounter * 5 % 17;
    if (n < NUM_LEDS) {
      leds[n] = CHSV(hue, 255, 255);
    }
    if (n + 5 < NUM_LEDS) {
      leds[n + 5] = CHSV(hue, 255, 255);
    }
  }
}


void beacon() {
  if (beaconCounter == 0) {
    if (counter <= 310 ) {
      for (int j = 0; j < 11 ; j++) {
        leds[319 - counter - j] = CRGB::White;
      }
    }
    if (counter >= 9 ) {
      for (int j = 1; j < 11 ; j++) {
        leds[319 - counter + j] = CRGB::Black;
      }
    }
    counter = counter + 9;
  } else {
    fadeToBlackBy(leds, NUM_LEDS, 5);
  }
}


void cylon() {
  fadeToBlackBy(leds, NUM_LEDS, 5);
  uint16_t j = beatsin8(20, 10, 255);
  j = map(j, 10, 255, 10, 310);
  for (int pos = j - 10 ; pos < j + 10 ; pos++) {
    constrain(pos, 0, 319);
    leds[pos] = CHSV(hue, 255, 192);
  }
}

// Definitions for twinkles.
#define PEAK_COLOR CRGB(100,100,100)
int ledState[NUM_LEDS];
enum {SteadyDim, GettingBrighter, GettingDimmerAgain};

void twinkles() {
  random16_add_entropy(random8());
  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    if ( ledState[i] == SteadyDim) {
      // this pixels is currently: SteadyDim
      // so we randomly consider making it start getting brighter
      if ( random16(0, NUM_LEDS) < 1) {
        ledState[i] = GettingBrighter;
      }

    } else if ( ledState[i] == GettingBrighter ) {
      // this pixels is currently: GettingBrighter
      // so if it's at peak color, switch it to getting dimmer again
      if ( leds[i] >= PEAK_COLOR) {
        ledState[i] = GettingDimmerAgain;
      } else {
        // otherwise, just keep brightening it:
        leds[i] += CRGB(4, 4, 4);
      }

    } else { // getting dimmer again
      // this pixels is currently: GettingDimmerAgain
      // so if it's back to base color, switch it to steady dim
      if (leds[i] <= CRGB(0, 0, 0) ) {
        leds[i] = CRGB(0, 0, 0); // reset to exact base color, in case we overshot
        ledState[i] = SteadyDim;
      } else {
        // otherwise, just keep dimming it down:
        leds[i] -= CRGB(2, 2, 2);
      }
    }
    //      mapTo(i, leds[i]);
  }
}


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 100


void fire()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS / 5];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS / 5; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / (NUM_LEDS / 5)) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS / 5 - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS / 5; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    pixelnumber = (NUM_LEDS / 5 - 1) - j;
    //    pixelnumber = j;
    for (int k = 0; k < 5; k++) {
      leds[pixelnumber * 5 + k] = color;
    }
  }
}

// Params for width and height
const uint16_t kMatrixWidth = 320;
const uint16_t kMatrixHeight = 1;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define LAST_VISIBLE_LED 319
uint16_t XY (uint16_t x, uint16_t y) {
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint16_t XYTable[] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319
  };

  uint16_t i = (y * kMatrixWidth) + x;
  uint16_t j = XYTable[i];
  return j;
}

// Triple Sine Waves
void threeSine() {

  static byte sineOffset = 0; // counter for current position of sine waves

  // Draw one frame of the animation into the LED array
  for (uint16_t x = 0; x < kMatrixWidth; x++) {
    for (uint16_t y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      uint16_t sinDistanceR = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 3 + x * 4)), 2);
      uint16_t sinDistanceG = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 4 + x * 4)), 2);
      uint16_t sinDistanceB = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 5 + x * 4)), 2);

      leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
    }
  }
  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle
}

static int16_t dist;
uint8_t        xscale = 64;
uint8_t        yscale = 64;
CRGBPalette16  currentPalette = LavaColors_p;
CRGBPalette16  targetPalette = OceanColors_p;
uint8_t        maxChanges = 24;

void inoise8_mover() {

  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
    //    fadeToBlackBy(leds, NUM_LEDS, 4);
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }


  for (int i = 1; i < 6; i++) {
    uint8_t locn = inoise16(i * xscale, i * (dist + yscale)) % 255;    // Get a new pixel location from moving noise.
    Serial.println(locn);
    uint8_t pixlen = map(locn, 0, 255, 0, NUM_LEDS / 5);          // Map that to the length of the strand.
    for (int j = 0; j < 5; j++) {
      leds[pixlen * 5 + j] = ColorFromPalette(currentPalette, pixlen, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.
    }
  }
  dist += beatsin8(10, 1, 4);
  fadeToBlackBy(leds, NUM_LEDS, 16);
} // inoise8_mover()



void sensor() {
  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
    //    fadeToBlackBy(leds, NUM_LEDS, 4);
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }

  if (INTS) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette(currentPalette, i, 255, LINEARBLEND);
    }
  }
  INTS = 0;
  fadeToBlackBy(leds, NUM_LEDS, 16);
}

void isrService()
{
  cli();
  INTS++;
  sei();
}

