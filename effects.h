void addGlitter(CRGB * leds, uint8_t num_leds) {
  if ( random8() < chanceOfGlitter) {
    leds[random16(num_leds)] += CRGB::White;
  }
}

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

void flash() {
  if (beaconCounter == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
    }
    counter = counter + 64;
  } else {
    fadeToBlackBy(leds, NUM_LEDS, 60);
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
  fadeToBlackBy(leds, NUM_LEDS, 1);
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
#define COOLING  80

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 100


void fire()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS / ledblock];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS / ledblock; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / (NUM_LEDS / ledblock)) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS / ledblock - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS / ledblock; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    pixelnumber = (NUM_LEDS / ledblock - 1) - j;
    //    pixelnumber = j;
    for (int k = 0; k < 5; k++) {
      leds[pixelnumber * 5 + k] = color;
    }
  }
}

void fire2() {
  // Use Accel data to modify COOLING value.
  readAccel();

  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS / ledblock];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS / ledblock; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / (NUM_LEDS / ledblock)) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS / ledblock - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < map(vectorDiff, 0, 30, 30, 255) ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255));
    //map(vectorDiff, 0, 30, 30, 255)) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS / ledblock; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    pixelnumber = (NUM_LEDS / ledblock - 1) - j;
    for (int k = 0; k < 5; k++) {
      leds[pixelnumber * 5 + k] = color;
    }
  }
}

// Params for width and height
const uint16_t kMatrixWidth = 320;
const uint16_t kMatrixHeight = 1;

//#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define LAST_VISIBLE_LED 319
uint16_t XY (uint16_t x, uint16_t y) {
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint16_t XYTable[] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271,
    272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287,
    288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303,
    304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319
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

void inoise8_mover() {
  static int16_t dist = random(12345);
  static const uint8_t  xscale = 64;
  static const uint8_t  yscale = 64;
  static CRGBPalette16  currentPalette = LavaColors_p;
  static CRGBPalette16  targetPalette = OceanColors_p;
  static uint8_t        maxChanges = 24;

  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
    //    fadeToBlackBy(leds, NUM_LEDS, 4);
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }


  for (int i = 1; i < 6; i++) {
    uint8_t locn = inoise16(i * xscale, i * (dist + yscale)) % 255;    // Get a new pixel location from moving noise.
    uint8_t pixlen = map(locn, 0, 255, 0, NUM_LEDS / ledblock);          // Map that to the length of the strand.
    for (int j = 0; j < 5; j++) {
      leds[pixlen * 5 + j] = ColorFromPalette(currentPalette, pixlen, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.
    }
  }
  dist += beatsin8(10, 1, 4);
  fadeToBlackBy(leds, NUM_LEDS, 16);
} // inoise8_mover()

void ripple() {
  static int ledStateNew[NUM_LEDS];
  static unsigned long oldtime = 0;
  static unsigned long newtime = 0;
  static uint8_t       colour;                                               // Ripple colour is randomized.
  static int           center = 0;                                               // Center of the current ripple.
  static int           peakspersec = 0;
  static int           peakcount = 0;

  EVERY_N_MILLISECONDS(1000) {
    peakspersec = peakcount;                                  // Count the peaks per second. This value will become the foreground hue.
  }

  newtime = millis();
  readAccel();
  if (currentDiff > 10 && (newtime > (oldtime + 30))) { // Check for a peak, which is 30 > the average, but wait at least 60ms for another.
    random16_add_entropy(random());
    center = random(NUM_LEDS);
    ledState[center] = random16(25, constrain(currentDiff, 45, currentDiff * 4));
    peakcount++;
    oldtime = newtime;
    currentDiff /= currentDiff;
  }

  EVERY_N_MILLISECONDS(10) {
    fadeToBlackBy(leds, NUM_LEDS, 10);                        // 8 bit, 1 = slow, 255 = fast
    for (int i = 0; i < NUM_LEDS; i++) {
      switch (ledState[i]) {
        case 0:
          break;
        default:                                              // Middle of the ripples.
          colour = (peakspersec * 10) % 255;                  // More peaks/s = higher the hue colour.
          if (i + 1 < NUM_LEDS) {
            ledStateNew[i + 1] = ledState[i] - 1;
          }
          if (i - 1 >= 0) {
            ledStateNew[i - 1] = ledState[i] - 1;
          }
          ledStateNew[i] = ledState[i] - 1;
          //          leds[constrain((center + step), 0, NUM_LEDS - 1)] += CHSV(colour, 255, myfade / step * 10);  // Simple wrap from Marc Miller.
          //          leds[constrain((center - step), 0, NUM_LEDS - 1)] += CHSV(colour, 255, myfade / step * 10);
          leds[i] = CHSV(colour, 255, 255);
          break;
      } // switch step
    }
    for (int i = 0; i < NUM_LEDS; i++) {
      ledState[i] = ledStateNew[i];
    }
  }
} // ripple()

void sensor() {
  // variables for some of our effects
  static CRGBPalette16  currentPalette = LavaColors_p;
  static CRGBPalette16  targetPalette = OceanColors_p;
  static uint8_t        maxChanges = 24;


  readAccel();
  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
    //    fadeToBlackBy(leds, NUM_LEDS, 4);
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }

  if (currentDiff > 1) {
    for (int i = 0; i < NUM_LEDS; i++) {
      //      leds[i] = ColorFromPalette(currentPalette, i, adjustedBrightness, LINEARBLEND);
      leds[i] = ColorFromPalette(currentPalette, i, constrain(255 * 2 / currentDiff, 0, 255), LINEARBLEND);
    }
    currentDiff /= currentDiff;
  }
  fadeToBlackBy(leds, NUM_LEDS, 8);
}


const uint8_t kxMatrixWidth = 5;
const uint8_t kxMatrixHeight = 64;

uint16_t XY2( uint8_t x, uint8_t y) {
  return (y * kxMatrixWidth) + x;
}

void plasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  //  if (effectInit == false) {
  //    effectInit = true;
  //    effectDelay = 10;
  //  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kxMatrixWidth; x++) {
    for (int y = 0; y < kxMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY2(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)
}

void rider() {

  static byte riderPos = 0;

  //  // startup tasks
  //  if (effectInit == false) {
  //    effectInit = true;
  //    effectDelay = 5;
  //    riderPos = 0;
  //  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kxMatrixWidth; x++) {
    int brightness = abs(x * (256 / kxMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(hue, 255, brightness);
    for (byte y = 0; y < kxMatrixHeight; y++) {
      leds[XY2(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic
}

void glitter() {

  //  // startup tasks
  //  if (effectInit == false) {
  //    effectInit = true;
  //    effectDelay = 15;
  //  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kxMatrixWidth; x++) {
    for (int y = 0; y < kxMatrixHeight; y++) {
      leds[XY2(x, y)] = CHSV(hue, 255, random8(5) * 63);
    }
  }
}


void blurpattern2()
{
  static uint8_t kBorderWidth = 0;
  static uint8_t kSquareWidth = 64;
  //  if (effectInit == false) {
  //    effectInit = true;
  //    effectDelay = 10;
  //    fadingActive = false;
  //  }

  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  //  uint8_t blurAmount = dim8_raw( beatsin8(3, 64, 64) );
  //  blur1d( leds, 320, blurAmount);

  // Use three out-of-sync sine waves
  uint8_t  i = beatsin16(  91 / 2, kBorderWidth, kSquareWidth - kBorderWidth);
  uint8_t  j = beatsin16( 109 / 2, kBorderWidth, kSquareWidth - kBorderWidth);
  uint8_t  k = beatsin16(  73 / 2, kBorderWidth, kSquareWidth - kBorderWidth);

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();
  leds[XY2( i, j)] += CHSV( ms / 29, 200, 255);
  leds[XY2( j, k)] += CHSV( ms / 41, 200, 255);
  leds[XY2( k, i)] += CHSV( ms / 73, 200, 255);
}


