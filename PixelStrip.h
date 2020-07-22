#ifndef PixelStrip_h
#define PixelStrip_h
 
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include "Adafruit_NeoPixel.h"
#include "Adafruit_GFX.h"
#include "RGB.h"
#include "segmentSection.h"

#include "segment.h"
#include "segmentSet.h"

#if !defined(SIZE)
    #define SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define min(a,b) ((a)<(b)?(a):(b))

#define stopPatternCheck() ({ \
	if( (pixelStripStopPattern) ){ \
	    if( !(subFunct) ){ \
		    (pixelStripStopPattern) = false; \
		} else { \
			(subFunct) = false; \
		} \
		break; \
	} \
}) 

//setup and function for button pattern switching
/* void setup() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), buttonHandle, RISING); // May need to change
  strip.begin();
  strip.setBrightness(normalBrightness);

  strip.show();

  randomSeed(analogRead(2));
}

//!!!! this code is meant to handle long and short presses, but instead seems to sense double clicks
//!!!! this needs checking
//handles both short and double presses
//double presses switch to the next pattern, while short presses restart the current pattern
void buttonHandle() { //interrupt with debounce
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 100)  // ignores interupts for 100 milliseconds to debounce
  {
    strip.pixelStripStopPattern = true;
    interrupt_time = millis();
    //if the button is held down for a long press time (600 ms) then we do nothing,
    //otherwise it's a short press and we advance the sequence counter
    while ( digitalRead(2) == HIGH && interrupt_time - last_interrupt_time < 600) {
      interrupt_time = millis();
    }
    if (interrupt_time - last_interrupt_time < 400) {
      patternCounter++;
    }
  }
  last_interrupt_time = interrupt_time;

}    */

//segment notes
//for any function with seg in name
//segments grouped into  sections of a strip
//mostly useful for pixels arranged in shapes
//segments are defined as:
//segments[x][3] = { { segment number , starting pixel, ending pixel } }
//x is the total number of segements
//segement number is the segement group that the set of pixels is attached to
//starting and ending pixel indicate the range of pixels in thr group
//looking ringSegments below, they are split into 5 segments, each with 1 group
//this structure lets you group non-consecutive sets of pixels into the same segement
//segements must be orderd by segment numnber in ascending order!
 
class PixelStrip : public Adafruit_NeoPixel {
	public:
		PixelStrip(uint16_t n, uint8_t p, uint8_t t);
		//~PixelStrip();
	
	void
		stripOff(),
		fillStrip(uint32_t color, boolean showNow),
		fillStripSection(uint32_t color, uint16_t startPixel, uint16_t endPixel, boolean showNow),
		fillRainbow( boolean showNow ),
		fillRainbowSection(uint16_t startPixel, uint16_t endPixel, boolean showNow),
		resizeBgPallet(uint8_t newSize),
		fillBgGradientRGB(),
		setBgPallet(RGB pallet[], uint8_t palletLength),
		genRandBgPallet(uint8_t palletLength),
		shiftGradient(uint16_t numCycles, int wait),
		dimPixel(int pixel),
		dimStrip(),
		splitColor( uint32_t color, uint8_t rgbComps[3] ),
		makeRGBpalletFromPallet(uint32_t pallet[], RGB palletRGB[], byte palletLength),
		genRandPallet(uint32_t pallet[], uint8_t palletLength),
		fadePixel(uint16_t pixel, RGB startColor, RGB endColor, int steps, int wait),
		crossFadeRGB(RGB startColor, RGB endColor, int steps, int wait),
		crossFadeColor(uint32_t startColor, uint32_t endColor, int steps, int wait),
		crossFadeCycle(uint8_t pattern[], uint8_t patternLength, uint32_t pallet[], int numCycles, int steps, int wait),
		crossFadeCycleRand(uint8_t numColors, int numCycles, int steps, int wait),
		crossFadeRandomPattern(uint32_t pattern[], int wait, int steps),
		randomColors(int32_t BgColor, boolean sparkleOn, int32_t preferedColor, int resetRate, int numPixels, int runTime),
		randomColorSet(int32_t BgColor, boolean sparkleOn, uint32_t pallet[], uint8_t palletLength, int resetRate, int numPixels, int runTime),
		twinkle(int32_t BgColor, int32_t preferedColor, uint8_t mode, int resetRate, int numPixels, int runTime),
		twinkleSet(int32_t BgColor, uint32_t pallet[], uint8_t palletLength, uint8_t mode, int resetRate, int numPixels, int runTime),
		fire(int32_t BgColor, boolean BgColorOn, uint32_t preferedColor, int fadeRate, int fadeSteps, int numPixels, int randFactor, int runTime),
		doFireV2(uint32_t pallet[], uint8_t palletLength, int Cooling, int Sparking, uint16_t flameLength, boolean repeat, boolean forward, boolean alternating, boolean blend, int numCycles, int wait),
		fireV2(RGB pallet[], uint8_t palletLength, int Cooling, int Sparking, uint16_t flameLength, boolean repeat, boolean forward, boolean alternating, boolean blend, int wait),
		setPixelHeatColor(int Pixel, byte temperature),
		setPixelHeatColorPallet(int Pixel, byte temperature, RGB pallet[], uint8_t palletLength),
		setPixelHeatColorPalletNoBlend(int Pixel, byte temperature, RGB pallet[], uint8_t palletLength),
		fireV3(uint32_t preferedColor, int runTime),
		theaterChase(uint32_t color, int32_t BgColor, uint8_t wait, uint8_t numCycles, uint8_t spacing),
		theaterChaseRainbow(int32_t BgColor, uint8_t wait, uint8_t spacing, uint8_t shiftFactor),
		rainbow(uint8_t wait),
		rainbowCycle(uint8_t wait, uint8_t numCycles),
		solidRainbowCycle(uint8_t wait, uint8_t numCycles),
		colorWipe(uint32_t color, uint16_t wipeLength, int wait, boolean forward, boolean alternate, boolean simult),
		colorWipeRandom(uint8_t style, uint32_t numColors, uint16_t wipeLength, int wait, boolean forward, boolean alternate, boolean simult),
		colorWipePallet(uint8_t style, boolean simult, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], uint16_t wipeLength, int wait, boolean forward, boolean alternate),
		colorWipeAdvanced(uint8_t style, boolean simult, int16_t LEDPattern[][2], uint8_t LEDPatternLength, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], uint16_t wipeLength, int wait),
		colorWipeRainbow(int wait, uint16_t wipeLength, boolean forward, boolean alternate, boolean segmented, boolean simult),
		colorWipeGradient(int wait, uint16_t wipeLength, boolean forward, boolean alternate, boolean simult),
		buildSimplePattern(byte patternScfld[][2], uint8_t patternScfldLength, byte pattern[], uint8_t patternLength),
		doRepeatSimplePattern(byte pattern[], uint8_t patternLength, uint32_t pallet[], uint8_t palletLength, uint8_t wait, uint8_t steps, uint8_t numCycles, boolean forward),
		crossFadePattern( byte pattern[][2], uint8_t patternLength, uint32_t pallet[], uint8_t wait, uint8_t steps),
		drawPattern( byte pattern[][2], uint8_t patternLength, uint32_t pallet[]),
		simpleStreamerRand( byte numColors, uint32_t preferedColor, byte streamerLength, byte spacing, int32_t BgColor, boolean forward, byte numCycles, int wait),
		simpleStreamer( byte pattern[], byte patternLength, uint32_t pallet[], byte streamerLength, byte spacing, int32_t BgColor, boolean forward, byte numCycles, int wait),
		gradientCycle(byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean forward, int wait),
		gradientCycleRand( byte numColors, byte gradSteps, uint16_t numCycles, boolean forward, int wait),
		rainbowPatternCycle( byte pattern[][2], uint8_t patternLength, uint8_t shiftFactor,  uint8_t wait, uint8_t steps),
		patternSweepSetRand( uint8_t numRandLeds, uint32_t pallet[],  uint8_t palletLength, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, uint8_t colorMode, int wait, int numCycles),
		patternSweepRand( uint8_t numRandLeds, int32_t preferedColor, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, uint8_t colorMode, int wait, int numCycles),
		patternSweep( uint16_t pattern[][3], uint8_t patternLength,  uint32_t pallet[], int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, uint8_t colorMode, int wait, int numCycles),
		patternSweepRainbowRand( uint8_t numRandLeds, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, int wait, int numCycles),
		patternSweepRainbow( uint16_t pattern[][2], uint8_t patternLength, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, int wait, int numCycles),
		patternSweepRepeatRand(byte numColors, uint32_t preferedColor, int32_t BgColor, int trails, byte trailLength, boolean scanner, boolean direction, byte eyeSize, byte spacing, uint8_t colorMode, int wait, int numCycles ),
		patternSweepRepeat(byte colorPattern[], byte colorPatternLength, uint32_t pallet[], int32_t BgColor, int trails, byte trailLength, boolean scanner, boolean direction, byte eyeSize, byte spacing, uint8_t colorMode, int wait, int numCycles ),
		shooterFixedLength( uint16_t sectionLength, uint32_t pallet[], byte palletLength, int32_t BgColor, uint8_t spawnChance, byte maxNumShooters, int trails, int8_t trailLength, uint8_t colorMode, boolean direction, boolean alternating, int wait, int numCycles),
		shooter( uint16_t sections[][2], byte numSections, uint32_t pallet[], byte palletLength, int32_t BgColor, uint8_t spawnChance, byte maxNumShooters, int trails, int8_t trailLength, uint8_t colorMode, boolean direction, boolean alternating, int wait, int numCycles),
		fireworksRand( uint8_t numRandLeds, int32_t preferedColor, int32_t BgColor, uint8_t dimFactor, uint8_t colorMode, int wait, int numCycles),
		fireworksSet( uint8_t numRandLeds, uint8_t dimFactor, uint32_t pallet[],  uint8_t palletLength, int32_t BgColor, uint8_t colorMode, int wait, int numCycles),
		fillSegBgGradientRGB(SegmentSet segmentSet),
		segGradientWaves(SegmentSet segmentSet, uint16_t numCycles, boolean inToOut, int wait, int steps),
		segGradientCycleRand(SegmentSet segmentSet, byte numColors, byte gradSteps, uint16_t numCycles, boolean inToOut, uint8_t type, int wait),
		segGradientCycle(SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean inToOut, int wait),
		segGradientCycle2(SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean inToOut, int wait),
		segGradientCycleSweep(SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean inToOut, int wait),
		fillSegColor(SegmentSet segmentSet, byte segNum, uint32_t color),
		shooterSeg( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, int32_t BgColor, uint8_t spawnChance, byte maxNumShooters, int trails, uint8_t trailLength, uint8_t colorMode, boolean direction, boolean alternating, int wait, int numCycles),
		waves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint16_t numCycles, boolean inToOut, uint16_t wait, uint8_t steps),
		randomWaves( SegmentSet segmentSet, byte numColors, byte waveThickness, byte spacing, byte numCycles, boolean inToOut, byte wait, byte steps),
		simpleWaves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte waveThickness, byte spacing, uint32_t BgColor, byte numCycles, boolean inToOut, byte wait, byte steps),
		singleWave( SegmentSet segmentSet, uint32_t waveColor, uint32_t BgColor, uint8_t waveThickness, uint8_t numCycles, boolean inToOut, uint8_t wait, uint8_t steps),
		rainbowWave( SegmentSet segmentSet, uint8_t numCycles, boolean inToOut, uint8_t wait, uint8_t steps),
		rainbowSingleWave( SegmentSet segmentSet, uint8_t waveThickness, uint8_t shiftFactor, uint8_t mode, uint8_t wait, uint8_t steps),
		fadingWavesRand( SegmentSet segmentSet, uint8_t numColors, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, int numCycles, uint8_t wait, int steps),
		fadingWaves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint8_t totalFadeLength, uint8_t spacing, boolean direct, int numCycles, uint8_t wait, int steps),
		sonarWavesRand( SegmentSet segmentSet, uint8_t numColors, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait),
		sonarWaves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait),
		colorSpin( SegmentSet segmentSet, byte spinPattern[], uint8_t spinPatternLength, uint32_t pallet[], int32_t BgColor, uint8_t colorMode, boolean repeat, int numCycles, int wait ),
		drawSegLine(SegmentSet segmentSet, byte lineNum, byte Pattern[], uint32_t pallet[], uint8_t colorMode, uint8_t bgColorMode, boolean brReplace),
		drawSegLineSection(SegmentSet segmentSet, uint8_t startSeg, uint8_t endseg, byte lineNum, byte Pattern[], uint32_t pallet[], uint8_t colorMode, uint8_t bgColorMode, boolean brReplace),
		colorSpinSimple( SegmentSet segmentSet, byte numColors, int32_t prefColor, int32_t BgColor, uint8_t sweepLength, int numSweeps, byte sweepSpacing, byte patternMode, byte colorMode, int numCycles, int wait ),
		colorSpinSimpleSet( SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], int32_t BgColor, byte sweepLength, int numSweeps, byte sweepSpacing, byte patternMode, byte colorMode, int numCycles, int wait ),
		drawSegLineSimple(SegmentSet segmentSet, byte lineNum, uint32_t color, uint8_t colorMode),
		drawSegLineSimpleSection(SegmentSet segmentSet, uint8_t startSeg, uint8_t endSeg, byte lineNum, uint32_t color, uint8_t colorMode),
		spiralsSet( SegmentSet segmentSet, byte spinPattern[], uint8_t spinPatternLength, uint32_t pallet[], byte colorPattern[], int numCycles, int wait ),
		spiralsRand( SegmentSet segmentSet, byte spinPattern[], uint8_t spinPatternLength, byte numColors, int32_t prefColor, uint32_t BgColor, int numCycles, int wait ),
		colorSpinRainbow( SegmentSet segmentSet, boolean forward, int numCycles, int wait ),
		sonarSpinRand( SegmentSet segmentSet, uint8_t numColors, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait),
		sonarSpin( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean direct, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait),
		sparkSeg( SegmentSet segmentSet, byte sparkChance, byte maxSparks, byte numColors, uint32_t prefColor, byte colorMode, uint32_t BgColor, byte BgColorMode, boolean direction, int numCycles, int wait ),
		sparkSegSet( SegmentSet segmentSet, byte sparkChance, byte maxSparks, uint32_t pallet[], uint8_t palletLength, byte colorMode, uint32_t BgColor, byte BgColorMode, boolean direction, int numCycles, int wait ),
		colorWipeRandomSeg( SegmentSet segmentSet, uint8_t style, uint32_t numColors, int8_t wipeLength, uint8_t wait, boolean forward, boolean alternate, boolean simult),
		colorWipeSeg(SegmentSet segmentSet, uint32_t color, int8_t wipeLength, uint8_t wait, boolean forward, boolean alternate, boolean simult),
		colorWipePalletSeg(SegmentSet segmentSet, uint8_t style, boolean simult, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], int8_t wipeLength, uint8_t wait, boolean forward, boolean alternate),
		colorWipeRainbowSeg(SegmentSet segmentSet, uint8_t wait, uint8_t wipeLength, boolean forward, boolean alternate, boolean segmented, boolean simult),
		colorWipeAdvSegment( SegmentSet segmentSet, uint8_t style, boolean simult, int16_t LEDPattern[][2], uint8_t LEDPatternLength, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], uint8_t wipeLength, uint8_t wait),
		colorWipeSections( SegmentSet segmentSet, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], byte style, boolean forward, boolean alternate, boolean inToOut, uint8_t wait),
		colorWipeRainbowSec(SegmentSet segmentSet, uint8_t wait, boolean forward, boolean alternate, boolean inToOut, byte style ),
		colorWipeSegRadial(SegmentSet segmentSet, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], byte style, boolean forward, uint8_t wait),
		colorWipeRainbowSegRadial(SegmentSet segmentSet, uint8_t wait, boolean forward),
		doFireV2Seg(SegmentSet segmentSet, uint32_t pallet[], uint8_t palletLength, int Cooling, int Sparking, boolean blend, int numCycles, int wait),
		fireV2Seg(SegmentSet segmentSet, uint16_t heatSegStarts[], uint16_t segmentSetLength, RGB pallet[], uint8_t palletLength, int Cooling, int Sparking, boolean blend, int wait);
		
	int16_t	
	    getSegmentPixel(SegmentSet segmentSet, byte segNum, uint16_t num),
		getPixelNumFromLineNum(SegmentSet segmentSet, uint16_t maxSegLength, uint8_t segNum, byte lineNum);
	
	uint16_t 
		bgPalletStripSize;
		
	uint32_t
		dimColor(uint32_t color, uint8_t factor),
		getBgGradientColor(int16_t ledLocation),
		getSegBgGradientColor(SegmentSet segmentSet, uint16_t segNum),
		desaturate(uint32_t color, byte percent),
		Wheel(byte WheelPos),
		getCrossFadeStep(RGB startColor, RGB endColor, int steps, int step),
		drawSegLineExtraColorModes(SegmentSet segmentSet, uint16_t maxSegLength, int16_t pixelNum, byte lineNum, uint8_t segNum, uint8_t colorMode),
		randColor();
	
	boolean
		pixelStripStopPattern = false,
		subFunct = false;
	
	//holder for the a background pallet for gradients, note that this is golbal!
	//when setting a pallet for the Bg, call setBgPallet(newPallet, SIZE(newPallet)) to set it
	//with the RGB pallet you want. You must do this each time you change the background pallet
	//this will also set bgPalletStripSize which is the length of the current pallet
	//After doing this you can call the gradient functions on the new pallet
	RGB  
		*bgPalletStrip,
		makeRGBFromColor(uint32_t color);

	private:
		void
			incrementLedLocation( uint16_t localPattern[][3], uint8_t patternLength, uint8_t eyeSize, boolean scanner),
			setBackground(int32_t BgColor),
			setBackgroundSingle(int16_t ledLocation, int32_t BgColor),
			patternSweepSetPixelColor(int16_t ledLocation, uint32_t color, uint8_t colorMode, uint8_t dimFactor),
			patternSweepGetLedDirection(int8_t directionArray[2], uint8_t ledDirection, int8_t trails, uint8_t eyeSize);
		const uint16_t
			numLEDs;       // Number of RGB LEDs in strip
		const uint8_t
			type,             // Pixel flags (400 vs 800 KHz, RGB vs GRB color)
			rByteIndex,       // Index of red byte within each 3-byte pixel
			gByteIndex,       // Index of green byte
			bByteIndex;       // Index of blue byte
		int16_t
			patternSweepGetTrailLed(int8_t directionArray[2], uint16_t modAmmount, int16_t ledLocation, int8_t distance);
		boolean
			checkLimitStreamer(boolean forward, int16_t nextStreamStart);
	
	
};
 
#endif