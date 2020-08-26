#include "PixelStrip.h"
#include "Adafruit_NeoPixel.h"


PixelStrip::PixelStrip(uint16_t n, uint8_t p, uint8_t t):
Adafruit_NeoPixel(n, p, t),
				 numLEDs(n),
				 type(t),
				 //index of r,g,b color values in the 32bit packed color number
				 rByteIndex(16),
				 gByteIndex(8),
				 bByteIndex(0)
				 {};
//improvments TODO:
			  //. write an exponential dimming function (and allow trail length setting on patternSweep) (done, but exp dimming might be off)
			  //. change dimming function to always use crossfade function, need to make funct to return bg color
			  //. allow switching pallets on the fly --> wont be needed with nodelay rewrite
			  //. BgColor setting code is repeated a few times --> make into funct? (done)
			  //. allow setting bg as a gradient? (done)
			  //. allow using bg gradient for the main color in patternSweep, colorSpin, and colorWipe functions (done)
			  //. check for repeates of single pixel BgColor setting code (done)
			  //. go through comments, make sure they outline all functionality
			  //. check variable types for consistency
			  //. see if drawPattern funct can be used more
			  //. rewrite splitColor to return array of rgb comps (done)
			  //. allow setting of zero point for fireV2 (done, along with changing the color)
			  //. look at fire, update it
			  //. rewrite crossfadeRandomPattern to take a pallet (need a search function to make pallets)
			  //. rewrite patterSweep to set last pixels off instead of redrawing whole BG each time (done)
			  //. rewrite patterSweep patterns to include -1, 1 instead of 1, 0 for direction
			  //. allow pattern sweep to contain different trail types in one pattern
			  //. check if palletLength is needed in color wipe functs (done)
			  //. check limits on color wipe functs
			  //. let you choose direction for pattern sweep rand
			  //. make dimming variables global, including the pattenSweep rainbow adjustments
			  //. remove patternSweep rainbow functs? not needed since b/c of color mode
			  //. re-write spinPattern, it is very restrictive about BG
			  //. add step rates to simple repeat, and wave functions?
			  //. patternSweepRepeat is memory with large patterns (sorta fixed with gradientCycle)
			  //. add radial rainbow to drawLineSimple (done, need to update/replace functions that call it) (done)
			  //. simpleRepeatPattern takes up to much memory, isnt 255 pixel compatible
			  //. add a direction to segments, which treats their pixels in reverse order (done) -----> fox segment functions to not clash with this
			  //. make RGB stored in PROGMEM, not sure how to work with dynamic ones (canceled, dont use RGB for variables in .ino)
			  //. fix end points on gradientCycle (done sorta, the dot doesn't always happen, depends on the lengths)
			  //. make shooter take an array of variable lengths --> write helper function for a fixed length {done} --> also made one that takes segments (but only continuons ones) (done)
			  //. look at reworking patternSweep, it's slow with a lot of pixels, rework desaturate funct
			  //. for shooterSeg, let you split the segments into pieces?
			  //. rewrite do simple repeat pattern to not use crossfadeRandom, instead repeat the pattern down the strip
			  //. blend the colors in fireV2 for a more nuanced fire (done)
			  //. add the ability to shift a rainbow bg / anything that uses wheel by adding and offset parameter (you could also do this of the BgGradient)
			  //. adjust segGradientCycleSweep to account for dummy leds
			  //. merge sonarWave and sonarSpin into one funct b/c they share the same code mostly

//Patterns TODO:
		  //. a proper fireworks routine (trails fade over time), could be a mod of the patternSweep with exponentially dimming trails
		  //. Draw pattern based on segments, (basically a static colorSpin)
		  //. a proper rainbow mode for color spin, cycles through rainbow based on longest segment? (done)
		  //. a colorWipe version of colorSpin (done)
		  //. a "shooter" pattern, like patternSweep, but the pixels only cycle round once (done)
		  //. an automated section color wipe, wipes a series of different sized sections in colors (done)
		  //. a segmented version of PatternSweep
		  //. segmented version of firev2 (done)
		  //. a function that cycles through an array of patterns
		  //. function that crossfades between colors (done w/ random too)
		  //. a radial version of the color spinFunction (ie a waves function that takes patterns)
		  //. a stacking pattern I guess
		  //. a simple function to crossfade through a set of colors (done)
		  //. a less memory intensive streamer function (done)
		  //. low memory version of waves (done)
		  //. a colorWipe function that does random length wipes
		  //. radial color wipe funct (done)
		  //. a static waves function (ie a BG that doesn't change as waves move), write like a radial patternSweep (done? sonar?)
		  //. random color waves function (done)
		  //. color spin simple pallet rand (easier to gen a random pallet seperatly)
		  //. simple steamer where it takes a pallet, no pattern (like simpleWaves)
		  //. shooter that takes only one color (maybe use the palletLength for the color and a mode?) (right now, you need to make a one length pallet)
		  //. a gradient segment function that repeats the same gradient down each segment (done)
		  //. add a rainbow mode to shooter seg where the rainbow is destributed down the seg
		  //. gradientSweep rainbow (needs direction setting in segment definition)

void PixelStrip::stripOff(void) {
  clear();
  show();
}

void PixelStrip::fillStrip(uint32_t color, boolean showNow) {
  fillStripSection(color, 0, numLEDs - 1, showNow);
}

//fills a section of the strip with a color, automatically showing is optional
void PixelStrip::fillStripSection(uint32_t color, uint16_t startPixel, uint16_t endPixel, boolean showNow){

  for (uint16_t i = startPixel; i <= endPixel; i++ ) {
    setPixelColor(i, color);
  }
  if(showNow){
	  show();
  }
}

//makes the strip a rainbow
void PixelStrip::fillRainbow(boolean showNow) {
  fillRainbowSection(0, numLEDs - 1, showNow);
}

//fills a section of the strip with a rainbow, automatically showing is optional
void PixelStrip::fillRainbowSection(uint16_t startPixel, uint16_t endPixel, boolean showNow){

  for (uint16_t  i = startPixel; i <= endPixel; i++) {
    setPixelColor(i, Wheel(((i * 256 / numLEDs)) & 255));
  }
  if(showNow){
    show();
  }
}

//re-allocates memory for the BgPallet, where newSize is the length of the new pallet
//and also sets bgPalletStripSize equal to the newSize
void PixelStrip::resizeBgPallet(uint8_t newSize){
	bgPalletStripSize = newSize;
	bgPalletStrip = (RGB*) realloc(bgPalletStrip, newSize * sizeof(RGB));
}

//copies an RGB pallet into the BgPallet, re-sizing if needed
void PixelStrip::setBgPallet(RGB pallet[], uint8_t palletLength){
	if(palletLength != bgPalletStripSize){
		resizeBgPallet(palletLength);
	}
	for(int i = 0; i < palletLength; i++){
		bgPalletStrip[i] = pallet[i];
	}
}

//sets the BgPallet to a palletLength number of random colors
void PixelStrip::genRandBgPallet(uint8_t palletLength){
    RGB pallet[palletLength];
	for(int i = 0; i < palletLength; i++){
		pallet[i].r = random(255);
		pallet[i].g = random(255);
		pallet[i].b = random(255);
		pallet[i].color32 = Color(pallet[i].r, pallet[i].g , pallet[i].b);
	}
	setBgPallet(pallet, palletLength);
}

//draws a gradient onto the strip based on the BgPallet
//the pallet can be any resonable length (fitting 10 colors onto a 20 length strip will proably look odd)
//the gradient will be created with even spacing for each color
//if the colors don't fit evenly, they should be streched to fit (needs more testing)
void PixelStrip::fillBgGradientRGB(){
	uint32_t color;
	uint16_t ledLocation;

	//makes the gradient sections as long as possible
	//uint16_t steps = ( numLEDs / (bgPalletStripSize - 1) );
	uint16_t steps = ceil(float(numLEDs) / (bgPalletStripSize - 1));

	//for each bgColor, (-1 because we dont want to start crossfading the last color)
	//crossfade towards the next color in the BgPallet
	for(int i = 0; i < (bgPalletStripSize - 1); i++){
		for(int j = 0; j <= steps; j++){
			color = getCrossFadeStep( bgPalletStrip[i], bgPalletStrip[ (i + 1) % bgPalletStripSize], steps, j);
			ledLocation = (i * steps + j); //how many colors we've gone through plus the step we're on
			if(ledLocation >= numLEDs){
				break;
			}
			setPixelColor(ledLocation, color );
		}
	}
}

//returns the crossfaded color of a specific pixel based on the BgPallet
//works like a reverse fillBgGradientRGB(), so the variable names are kept the same
uint32_t PixelStrip::getBgGradientColor(int16_t ledLocation){
	if(ledLocation < 0){ //if it's a dummy pixel, return off
		return 0;
	}
	//uint16_t steps = ceil(float(numLEDs) / (bgPalletStripSize - 1));
	uint16_t steps = ( numLEDs / (bgPalletStripSize - 1) ) ;
	int i = 0;
	int j = 0;

	//determine what color section of the crossfade we're in
	while( ( i + 1 ) * steps < ledLocation ){
		i++;
	}

	//get the crossfade step
	j = ledLocation - (i * steps );

	return getCrossFadeStep( bgPalletStrip[i], bgPalletStrip[ (i + 1) % bgPalletStripSize], steps, j) ;
	//setPixelColor(ledLocation,  getCrossFadeStep( bgPalletStrip[i], bgPalletStrip[ (i + 1) % bgPalletStripSize], steps, j) );
}

//moves the Bg gradient along the strip for numCycles cycles, with wait time inbetween each cycle
void PixelStrip::shiftGradient(uint16_t numCycles, int wait){
	uint32_t color;
	for(int i = 0; i < numCycles; i++){
		for(int j = 0; j < numLEDs; j++){
			color = getBgGradientColor( j );
			setPixelColor( (j + i) % numLEDs, color);
		}
		show();
		delay(wait);
	}
}

//just a package of commonly repeated code, not intended for direct use
//fill the strip with a BgColor, or a rainbow
//Special commands:
//-1 fills with rainbow
//-2 will fill with a random color
//-3 wil fill with whatever the BgPallet is set to
void PixelStrip::setBackground(int32_t BgColor){
	if( BgColor == -1 ){  //not a real color fill with a rainbow
		fillRainbow(false);
	} else if( BgColor == -2 ){ //pick a background at random (mostly used to create "rave" effects)
		fillStrip(randColor(), false);
	} else if( BgColor == -3){
		fillBgGradientRGB();
	} else {
		fillStrip(BgColor, false);
	}
}

//just a package of commonly repeated code, not intended for direct use
//like swetBackground, but only does a specific pixel
//Special commands:
//-1 fills with rainbow
//-2 will fill with a random color
//-3 wil fill with whatever the BgPallet is set to
void PixelStrip::setBackgroundSingle( int16_t ledLocation, int32_t BgColor){
	uint32_t color;
	if( BgColor == -1 ){  //not a real color fill with a rainbow
		color = Wheel( ( ledLocation ) * 256 / numLEDs );
	} else if( BgColor == -2 ){
		color =  randColor();
	} else if( BgColor == -3 ) {
		color = getBgGradientColor(ledLocation);
	} else {
		color = BgColor;
	}
	setPixelColor(ledLocation, color);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t PixelStrip::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  yield();
  if (WheelPos < 85) {
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

uint32_t PixelStrip::randColor(){
	return Color(random(255), random(255), random(255));
}

void PixelStrip::genRandPallet(uint32_t pallet[], uint8_t palletLength){
	for(int i = 0; i < palletLength; i++){
		pallet[i] = randColor();
	}
}

//dims a specific color by a factor
uint32_t PixelStrip::dimColor(uint32_t color, uint8_t factor) {

  uint8_t rgb[3];

  splitColor( color, rgb );
  //Shift R, G and B components factor bit(2) to the right
  uint32_t dimColor = Color(rgb[0] >> factor, rgb[1] >> factor, rgb[2] >> factor);
  return dimColor;
}

//dims an individual pixel
void PixelStrip::dimPixel(int pixel) {
  setPixelColor( pixel, dimColor( getPixelColor(pixel), 1 ) );
}

//dims the entire strip using DimColor, does not display the strip
void PixelStrip::dimStrip() {
  for (int i = 0; i < numLEDs; i++) {
    dimPixel(i);
  }
}

//variable dimming, takes a color and a percent between 0 and 100
//and dims it by the percentage value (100 is no dim, 0 is off)
//dimming is non-linear to more closly match human brigtness perception
uint32_t PixelStrip::desaturate(uint32_t color, byte percent){

	//a defensize check to prevent overflow
	if(percent >= 100) {
		return color;
	}

	//dimPow is completly emperical, but seems to work for most pixels
	const float dimPow = 0.833;
	float multiplier = pow( float(percent), dimPow ) / 100;

	byte rgb[3];

	splitColor( color, rgb );

	byte rgbNew[3];
	rgbNew[0] =  rgb[0] * multiplier;
	rgbNew[1] =  rgb[1] * multiplier;
	rgbNew[2] =  rgb[2] * multiplier;

	return Color( rgbNew[0], rgbNew[1], rgbNew[2] );
}

//returns an array of rgb byte values from a 32bit packed color value
//the byteIndex is set in the pixelStrip constructor
void PixelStrip::splitColor( uint32_t color, uint8_t rgbComps[3] ) {
    rgbComps[0] = (uint8_t) ( color >> rByteIndex );
    rgbComps[1] = (uint8_t) ( color >> gByteIndex );
    rgbComps[2] = (uint8_t) ( color >> bByteIndex );
}

//Fade pixel x from startColor to endColor in a number of steps with
//a set wait time inbetween each step
void PixelStrip::fadePixel(uint16_t pixel, RGB startColor, RGB endColor, int steps, int wait) {
  uint32_t newColor;
  for (int i = 0; i <= steps; i++)
  {
    newColor = getCrossFadeStep( startColor, endColor, steps, i);

    setPixelColor(pixel, newColor);
    show();
    delay(wait);
  }
}

//returns an RGB struct for given color
RGB PixelStrip::makeRGBFromColor(uint32_t color){
	RGB newRGB;
	uint8_t colorSplit[3];
	splitColor( color, colorSplit );

	newRGB.r = colorSplit[0];
	newRGB.g = colorSplit[1];
	newRGB.b = colorSplit[2];
	newRGB.color32 = color;

	return newRGB;
}

//translates a pallet's entries to RGB format
//input pallet is uint32_t array of colors
//output pallet is RGB array of colors
void PixelStrip::makeRGBpalletFromPallet(uint32_t pallet[], RGB palletRGB[], byte palletLength){
	for(uint8_t i = 0; i < palletLength; i++){
		palletRGB[i] = makeRGBFromColor( pallet[i] );
	}
}

//Crossfade entire screen from startColor to endColor in a number of steps with
//a set wait time inbetween each step
void PixelStrip::crossFadeRGB(RGB startColor, RGB endColor, int steps, int wait) {
  uint32_t newColor;
  for (int i = 0; i <= steps; i++) {
	yield();
    stopPatternCheck();
    newColor = getCrossFadeStep( startColor, endColor, steps, i);

    fillStrip(newColor, false);
    show();
    delay(wait);
  }
}

//Crossfade entire screen from startColor to endColor in a number of steps with
//a set wait time inbetween each step
void PixelStrip::crossFadeColor(uint32_t startColor, uint32_t endColor, int steps, int wait) {
  subFunct = true; //for stopPatternCheck, so we break out in the calling function too
  RGB startColorRGB = makeRGBFromColor(startColor);
  RGB endColorRGB = makeRGBFromColor(endColor);

  crossFadeRGB(startColorRGB, endColorRGB, steps, wait);
}

//gets the crossfades color between start and end colors according to the step in a sequence of steps
uint32_t PixelStrip::getCrossFadeStep( RGB startColor, RGB endColor, int steps, int step){
	uint8_t newColor[3];
	int32_t step32 = (int32_t)step;

	newColor[0] = (int16_t)startColor.r + (int32_t)( ( (int16_t)endColor.r - (int16_t)startColor.r ) * step32) / steps;
    newColor[1] = (int16_t)startColor.g + (int32_t)( ( (int16_t)endColor.g - (int16_t)startColor.g ) * step32) / steps;
    newColor[2] = (int16_t)startColor.b + (int32_t)( ( (int16_t)endColor.b - (int16_t)startColor.b ) * step32) / steps ;

	//Serial.println(newColor[2]);
	return Color(newColor[0], newColor[1], newColor[2]);
}

//crossfades the entire strip through a series of colors
//the pattern is a sequence of color indecies for the pallet
//the function cycles through the pattern, for numCycles
//ie a pattern of {1, 4, 5} would do the 2nd color in the pallet, then the 6th, then the 5th
//each fade is done in steps with wait between each step
void PixelStrip::crossFadeCycle(uint8_t pattern[], uint8_t patternLength, uint32_t pallet[], int numCycles, int steps, int wait){
	uint32_t startColor, nextColor;

	for(int i = 0; i < numCycles; i++){
		yield();
		stopPatternCheck();

		startColor = pallet[ pattern[i % patternLength] ];
		nextColor = pallet [ pattern[(i + 1) % patternLength] ];

		crossFadeColor(startColor, nextColor, steps, wait);
	}

}

//crossfades the entire strip through random colors for a certain number of cycles
void PixelStrip::crossFadeCycleRand(uint8_t numColors, int numCycles, int steps, int wait){
	uint32_t startColor, nextColor;
	//initialize some start and end colors
	startColor = randColor();
	nextColor = randColor();
	//run through numCycles and crossfade
	for(int i = 0; i < numCycles; i++){
		yield();
		stopPatternCheck();

		crossFadeColor(startColor, nextColor, steps, wait);

		startColor = nextColor;
		nextColor = randColor();
	}
}

//todo, rerwite this and depending functions to use a pallet instead of storing a
//whole strips worth of 32bit values
//cross fades the strip from the current pattern on the strip to a final pattern in
//a number of steps with a set wait time inbetween each step
void PixelStrip::crossFadeRandomPattern(uint32_t pattern[], int wait, int steps) {
  uint8_t prevBrightness = getBrightness();
  int stepDiff;
  uint32_t currentColor;
  uint8_t endColorSplit[3];
  uint8_t currentColorSplit[3];
  uint8_t newColorSplit[3];

  for (int j = 0; j < steps; j++) {
	stopPatternCheck();
    stepDiff = ( steps - j );
    setBrightness(255); //set brightness to max to get the correct 32bit color values
    for (int i = 0; i < numLEDs; i++) {
      currentColor = getPixelColor(i);

	  //unpack current and end color
	  splitColor( currentColor , currentColorSplit );

	  splitColor( pattern[i] , endColorSplit );

	  //slightly different formula to other cross fades, lowers memory usage b/c we don't need
	  //both start and end colors, but we need to get the current color each loop
      newColorSplit[0] = currentColorSplit[0] + ( int(endColorSplit[0]) -  currentColorSplit[0] ) /  stepDiff  ;
      newColorSplit[1] = currentColorSplit[1] + ( int(endColorSplit[1]) -  currentColorSplit[1] ) /  stepDiff  ;
      newColorSplit[2] = currentColorSplit[2] + ( int(endColorSplit[2]) -  currentColorSplit[2] ) /  stepDiff  ;

      setPixelColor(i, Color(newColorSplit[0], newColorSplit[1], newColorSplit[2]));
    }
    setBrightness(prevBrightness);
    show();
    delay(wait);
  }
}

//calls the randomColorsSet function, but with only a single or no colors in the pallet
//passing -1 as the preferedColor, set pixels to have a random colors
void PixelStrip::randomColors(int32_t BgColor, boolean sparkleOn, int32_t preferedColor, int resetRate, int numPixels, int runTime) {
  uint32_t pallet[1] = {0};
  uint8_t palletLength;
  //if preferedColor is negative (ie no preference)
  //the colors will be picked at random (set by passing a palletLength of 0 to randomColorSet)
  if(preferedColor < 0){
	palletLength = 0;
  } else {
	 palletLength = 1;
	 pallet[0] = preferedColor;
  }
  randomColorSet( BgColor, sparkleOn, pallet, palletLength, resetRate, numPixels, runTime);
}

//sets a random set of pixels to a random or indicated color(s), a series of times over a indicated time period
//a series of colors for the random pixels can be indicated, the function will randomly pick from these colors to display
//the strip can be given a prefered background color, and the pixels can be given a prefered color
//if BgColor == -1, the background will be set to a rainbow
//if BgColor < -1, a background will be picked at random each cycle (rave mode)
//sparkleOn is true, the strip will dim pixels instead of setting a background
//numPixels is the number of random pixels per cycle
//all other variables should be self explanatory
void PixelStrip::randomColorSet(int32_t BgColor, boolean sparkleOn, uint32_t pallet[], uint8_t palletLength, int resetRate, int numPixels, int runTime) {
	  int startTime = millis();
	  int currentTime = startTime;
	  int resetTime = startTime - resetRate;
	  uint32_t color;

	  while( (currentTime - startTime) < runTime ){
		yield();
		stopPatternCheck();
		currentTime = millis();
		if( (currentTime - resetTime) >= resetRate ){
		  resetTime = currentTime;

			//controls the background setting, when sparkleOn is true, the strip will dim with each cycle
			//instead of setting a background
			//known issue, cant have a background and sparkle
			if( sparkleOn ){
			    dimStrip();
			} else {
				setBackground( BgColor );
			}

			//sets a random set of pixels to a random or indicated color(s)
			for (int i = 0; i < numPixels; i++) {
				int j = random(numLEDs);
				switch(palletLength){
					case 0 : //0 pallet length, no pallet, so set colors at random
					  color = randColor();
					  break;
					case 1 : //pallet lenght one means all the pixels must be the same color
					  color = pallet[0];
					  break;
					default : //we're picking from a set of colors
					  color = pallet[ random(palletLength) ];
					  break;
				}
				setPixelColor(j, color);
			}
		show();
	    }
	}
}

//sets a random number of pixels on, one after another then resets
//passing -1 as BgColor sets it to rainbow
//passing -1 as preferedColor will set thr colors randomly
//modes:
//0: turns one pixel on after another and then resets all
//1: same a 0, but turns them off one after another
//2: same as 0, but each cycle, a new pixel is turned on while an old is turned off (first on first off)
void PixelStrip::twinkle(int32_t BgColor, int32_t preferedColor, uint8_t mode, int resetRate, int numPixels, int runTime) {
  uint32_t pallet[1] = {0};
  uint8_t palletLength;
  //if preferedColor is negative (ie no preference)
  //the colors will be picked at random (set by passing a palletLength of 0 to randomColorSet)
  if(preferedColor < 0){
	palletLength = 0;
  } else {
	 palletLength = 1;
	 pallet[0] = preferedColor;
  }
  twinkleSet(BgColor, pallet, palletLength, mode, resetRate, numPixels, runTime);
}

//sets a random number of pixels on, one after another then resets
//can pass in a specific pallet of colors to choose from, otherwise they'll be set randomly
//passing -1 as BgColor sets it to rainbow
//passing -3 sets Bg to BgPallet gradient
//note that there is no "rave mode", passing -2 or less as the BgColor will result in rainbow
//modes:
//0: turns one pixel on after another and then resets all
//1: same a 0, but turns them off one after another
//2: same as 0, but each cycle, a new pixel is turned on while an old is turned off (first on first off)
void PixelStrip::twinkleSet(int32_t BgColor, uint32_t pallet[], uint8_t palletLength, uint8_t mode, int resetRate, int numPixels, int runTime) {
  int startTime = millis();
  int currentTime = startTime;
  int resetTime = startTime - resetRate;
  uint32_t color;
  uint8_t pixelSet[numPixels];
  uint8_t pixelIndex = numPixels - 1;  //set to numPixels to generate a background on the fist loop
  uint32_t BgColorDis;
  boolean firstCycle = true;
  boolean modeOneOn = false;

  //prevent random color in the setBackground function, this won't work correctly b/c
  //we need to know the BgColor for twinkling
  if( BgColor == -2){
	 BgColor = 0;
  }

  //loop until it's time to turn on a pixel at pixelIndex,
  //then increment the pixelIndex
  //the pixelIndex is reset once the numPixels have been turned on
  while ((currentTime - startTime) < runTime) {
	yield();
	stopPatternCheck();
    currentTime = millis();
    if ((currentTime - resetTime) >= resetRate) {
      resetTime = currentTime;

      //fills the strip with a background color once per reset cycle
	  if( pixelIndex == (numPixels - 1) ){
		pixelIndex = 0;

		//make a new set of leds to light up
		//and set the background color
		//needs to run on the first cycle to create starting point for other modes
	   if( (mode == 0) || firstCycle){ // mode 0 reseting all the on leds
		 firstCycle = false;
		 for(int i = 0; i < numPixels; i++){
			pixelSet[i] = random(numLEDs);
		 }

	   setBackground( BgColor );

	   } else if ( mode == 1 ) {  //we need to flag mode one to start if we've run one cycle
			modeOneOn = true;
	   } else if (mode == 2) { //set a new led to turn on and turn off an old
	        pixelIndex = numPixels - 2;  //set the index to just do one run
	        uint8_t offPixel = pixelSet[0]; //led to turn off (first on first off)
		    for(int i = 1; i < numPixels ; i++){ //shift the array back one
				pixelSet[i - 1] = pixelSet[i];
			}
			uint8_t newPixel = random(numLEDs); //set a new pixel to turn on and put it in the array
			pixelSet[numPixels - 1] = newPixel;

			//set the off pixel to the background color
			setBackgroundSingle(offPixel, BgColor);
	   }
	  }

      //turns on the next pixel in the pixelSet, choosing a color based on the options
      switch(palletLength){
			case 0 : //0 pallet length, no pallet, so set colors at random
				color = randColor();
				break;
			case 1 : //pallet length one means all the pixels must be the same color
				color = pallet[0];
				break;
			default : //we're picking from a set of colors
				color = pallet[ random(palletLength) ];
				break;
	  }

	  //if we're in mode 1, and we've gone through a cycle, we need to
	  //turn the leds off instead of on, and then reset to the first cylce again
	  if( modeOneOn ){
		//set the off pixel to the background color
		setBackgroundSingle( pixelSet[pixelIndex], BgColor );

		//if we've turned off all the leds, we need to run the first cycle again
	    if( pixelIndex == (numPixels - 2) ) {
	      firstCycle = true;
		  modeOneOn =  false;
		}
	  } else {
	    setPixelColor(pixelSet[pixelIndex], color);
	  }
      show();
	  pixelIndex++;
    }
  }
}

//a pulsating kind of fire effect, more like embers than a full fire
//old code, needs looking at
//takes a BgColor and a main color and dims numPixels a random amount each cycle
void PixelStrip::fire(int32_t BgColor, boolean BgColorOn, uint32_t preferedColor, int fadeRate, int fadeSteps, int numPixels, int randFactor, int runTime) {
  uint32_t pattern[numLEDs];
  int startTime = millis();
  int currentTime = startTime;
  int i, j, rand, red, green, blue;
  uint32_t color;
  uint32_t bgColor;
  boolean preferedColorSetting = true;
  if (preferedColor == 0) {
    preferedColorSetting = false;
  };

  while ((currentTime - startTime) < runTime) {
	yield();
	stopPatternCheck();
    currentTime = millis();
	if( BgColorOn ){
		if(BgColor < 0){ //if BgColor is negative (impossible), we'll do a rainbow as the Bg
			for (int i = 0; i < numLEDs; i++) {
			  pattern[i]= Wheel(((i * 256 / numLEDs)) & 255);
			}
		} else {
			for (int i = 0; i < numLEDs; i++) {
			  pattern[i] = BgColor;
			}
		}
	} else {
		for (int i = 0; i < numLEDs; i++) {
		  pattern[i] = randColor();
		}
	}
    for (int i = 0; i < numPixels; i++) {
      j = random(numLEDs);
      rand = random(randFactor);
      if (preferedColorSetting) {
		uint8_t newColor[3];
		splitColor(preferedColor, newColor);
		red = newColor[0] * rand / 255;
        blue = newColor[1] * rand / 255 ;
        green = newColor[2] * rand / 255;
        color = Color(red, green, blue);
      } else {
        color = randColor();
      }
      pattern[j] = color;
    }
    show();
    crossFadeRandomPattern(pattern, fadeRate, fadeSteps);
  }
}

//does fireV2 for numCycles cycles
void PixelStrip::doFireV2(uint32_t pallet[], uint8_t palletLength, int Cooling, int Sparking, uint16_t flameLength, boolean repeat, boolean forward, boolean alternating, boolean blend, int numCycles, int wait){
	stripOff(); //clear the strip, b/c fire doesn't if the flame is shorter than the strip

	//need to make an RGB pallet for fireV2
	RGB palletRGB[palletLength];
	//recreate the supplied pallet in RGB
	makeRGBpalletFromPallet(pallet,palletRGB, palletLength);

	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		fireV2(palletRGB, palletLength, Cooling, Sparking, flameLength, repeat, forward, alternating, blend, wait);
	}
}

//A traditional fire loop, most useful for strips with a diffuser
//code modified from https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#fire
//draws a a random fire along the strip
//takes an RGB pallet[] of colors which are used to generate the fire:
	//the pallet must be ordered from the coldest to the hottest color (generally lightest to darkest)
	//pallets can be of any length, although 3 - 4 colors seems to work best for normal fires
	//try { Color(255, 0, 0), Color(255, 143, 0) , Color(255, 255, 255) } for a tranditional fire
	//or {Color(238, 130, 238),  Color(123, 7, 197) , Color(225, 0, 127), Color(255, 255, 255) } for a pink/purple fire
//The other variables determine how the fire is drawn:
	//Cooling: indicates how fast a flame cools down. More cooling means shorter flames,
	//recommended values are between 20 and 100. 50 seems the nicest.

	//Sparking: indicates the chance (out of 255) that a spark will ignite. A higher value makes the fire more active.
	//Suggested values lay between 50 and 200. 120 works well.

	//flameLength: sets the maximum length of the strip taken up by the flame

	//repeat: if true, then the fire will repeat as many times as possible along the strip,
	//ie for a 60 pixel strip, with flameLength 10, and repeat on, you would get 10 fires, one after another
	//note that the fire is repeated, so each fire section is identical

	//forward: sets the direction that the inital fire burns in,
	//setting it true means the fire starts at zero, otherwise it will start at flameLength from the beginning of the strip

	//alternating: if true, then subesequent fires will reverse direction
	//for example you can generate a fire starting from the the middle of the strip by setting flameLength to halve the strip length
	//repeat to true, direction to false, and alternating to true

	//wait sets the time between drawing new flames
void PixelStrip::fireV2(RGB pallet[], uint8_t palletLength, int Cooling, int Sparking, uint16_t flameLength, boolean repeat, boolean forward, boolean alternating, boolean blend, int wait) {

  byte heat[flameLength];
  uint16_t ledLocation;
  int cooldown;
  uint8_t numFlames;
  boolean forwardLocal = forward; //pevent overwriting the variable passed in

  //if we're repeating, we want to cram as many flames as possible
  //we round up the number of flames that will fit
  //otherwise we just do one flame
  if(repeat){
	 numFlames = ceil( float(numLEDs) / flameLength);
  } else {
	 numFlames = 1;
  }

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < flameLength; i++) {
    cooldown = random(0, ((Cooling * 10) / flameLength) + 2);

    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k = (flameLength - 1); k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(min( 7, flameLength ));
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4. For each flame, convert heat to LED colors
  for(int i = 0; i < numFlames; i++){
	for( int j = 0; j < flameLength; j++) {
		//if we're going forward we need to start at the "zero point" of each flame
		//and draw forward
		//otherwise we start from the flame end point and draw backwards
		if(forwardLocal){
			ledLocation = j + (i * flameLength);
		} else {
			ledLocation = ( (i + 1) * flameLength) - (j);
		}

		//if we're repeating we might try to draw a significant portion of the
		//the flame off the strip. If that happens, then we don't need to draw anymore
		//and can break out of the loop
		if(ledLocation > numLEDs || ledLocation < 0){
			break;
		}
		if(blend){
			setPixelHeatColorPallet( ledLocation, heat[j], pallet, palletLength);
		}else{
			setPixelHeatColorPalletNoBlend(ledLocation, heat[j], pallet, palletLength);
		}
	}
	//if we're alternating, we just flip the forward value
	if(alternating){
		forwardLocal = !forwardLocal;
	}
  }

  show();
  delay(wait);
}

//original code from https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#fire
//sets heat colors according to a traditional fire,
//not currently used
void PixelStrip::setPixelHeatColor(int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixelColor(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixelColor(Pixel, heatramp, 0, 0);
  }
}

//!!WARNING no not try to merge setPixelHeatColorPallet and setPixelHeatColorPalletNoBlend
//!!Doing so causes a voodoo bug when passing in blend (using Serial.println(blend) fixes it, so it's voodoo)
//sets heat colors using a pallet, dimming the colors a bit depending on their heat
//essentially divides the possible heat range into sections,
//determines which section the heat is in, and sets the color
 //if blend is true we'll try to blend between colors to get a cleaner fire
//the set color is a blend between the current color and the next based on the pixel's temperature
//this produces a more nuanced fire
//this uses more processing power, so turn it off if the animation is too slow (or you're having color bugs)
void PixelStrip::setPixelHeatColorPallet(int Pixel, byte temperature, RGB pallet[], uint8_t palletLength) {
    //byte t192 = map(temperature, 0, 255, 3, 0);
	uint32_t color;
	uint8_t segSize = 255 / palletLength;
	uint8_t targetColor;
	uint8_t segTempStep;

  
    //determine which color the heat belongs to and blend it
	for(int i = (palletLength - 1); i >= 0; i--){
		//if we find the right section, we don't need to loop any more, so we break
		if(temperature > (i * segSize) ){
			//color = dimColor(pallet[i], t192);
			//if blend is true we'll try to blend between colors to get a cleaner fire
			//this uses more processing power, so turn it off if the animation is too slow (or you're having color bugs)
			//if we're at the end last color, we don't need to blend, prevent us from running off the pallet end
			targetColor = min(i + 1, palletLength - 1);
			//segTempStep = max((int)temperature - (i * segSize), 0);
			//blend between the current color an the next based on the what temperature the pixel is at
			color = getCrossFadeStep( pallet[i], pallet[targetColor], segSize, temperature - (i * segSize) );
			break;	
		}
	}
	setPixelColor(Pixel, color);
}

//sets heat colors using a pallet, dimming the colors a bit depending on their heat
//essentially divides the possible heat range into sections,
//determines which section the heat is in, and sets the color
void PixelStrip::setPixelHeatColorPalletNoBlend(int Pixel, byte temperature, RGB pallet[], uint8_t palletLength) {
	//byte t192 = map(temperature, 0, 255, 3, 0);
	uint32_t color;
	uint8_t segSize = 255 / palletLength;
	uint8_t targetColor;
	uint8_t segTempStep;
  
	//determine which color the heat belongs to and blend it
	for(int i = (palletLength - 1); i >= 0; i--){
		//if we find the right section, we don't need to loop any more, so we break
		if(temperature > (i * segSize) ){
			color = pallet[i].color32;
			break;
		}
	}
	setPixelColor(Pixel, color);
}

//more of a flickering effect than fire
void PixelStrip::fireV3(uint32_t preferedColor, int runTime) {
  //  Flicker, based on our initial RGB values
  int startTime = millis();
  int currentTime = startTime;
  while ((currentTime - startTime) < runTime) {
	yield();
	stopPatternCheck();
    currentTime = millis();
    for (int i = 0; i < numLEDs; i++) {
      int flicker = random(0, 150);
	  uint8_t colorComps[3];
	  splitColor(preferedColor, colorComps);
	  int red = colorComps[0] - flicker;
      int blue = colorComps[1] - flicker;
      int green = colorComps[2] - flicker;
      if (green < 0) green = 0;
      if (red < 0) red = 0;
      if (blue < 0) blue = 0;
      setPixelColor(i, red, green, blue );
    }
    show();

    //  Adjust the delay here, if you'd like.  Right now, it randomizes the
    //  color switch delay to give a sense of realism
    delay(random(10, 113));
  }
}

//Theatre-style crawling lights.
//passing -1 as BgColor, sets it to a rainbow, -2 sets randomly, -3 sets to Bg gradient
void PixelStrip::theaterChase(uint32_t color, int32_t BgColor, uint8_t wait, uint8_t numCycles, uint8_t spacing) {
	for (int j = 0; j < numCycles; j++) {
		stopPatternCheck();
		for (int q = 0; q < spacing; q++) {
			for (int i = 0; i < numLEDs; i = i + spacing) {
				setPixelColor(i + q, color);  //turn every (spacing) pixel on
			}
			show();
			delay(wait);

			//turn every (spacing) pixel to BgColor
			for (int i = 0; i < numLEDs; i = i + spacing) {
				setBackgroundSingle( i + q, BgColor);
			}
		}
	}
}

//Theatre-style crawling lights with rainbow effect
//passing -2 as BgColor sets it randomly, -3 sets to Bg gradient
void PixelStrip::theaterChaseRainbow(int32_t BgColor, uint8_t wait, uint8_t spacing, uint8_t shiftFactor) {
  if(BgColor == -1){ //prevent rainbow Bg as nothing would happen
	BgColor = 0;
  }
  for (int j = 0; j < 256; j= j + shiftFactor) {   // cycle all 256 colors in the wheel
    stopPatternCheck();
    for (int q = 0; q < spacing; q++) {
      for (int i = 0; i < numLEDs; i = i + spacing) {
        setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every (spacing) pixel on
      }
      show();

      delay(wait);
		//turn every (spacing) pixel for the BgColor
      for (int i = 0; i < numLEDs; i = i + spacing) {
        setBackgroundSingle( i + q, BgColor);
      }
    }
  }
}

//shifts a rainbow down the strip
void PixelStrip::rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
	yield();
	stopPatternCheck();
    for (i = 0; i < numLEDs; i++) {
      setPixelColor(i, Wheel((i + j) & 255));
    }
    show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void PixelStrip::rainbowCycle(uint8_t wait, uint8_t numCycles) {
  uint16_t i, j;
  for (j = 0; j < 256 * numCycles; j++) { // cycles thru all colors on wheel
	stopPatternCheck();
    for (i = 0; i < numLEDs; i++) {
      setPixelColor(i, Wheel(((i * 256 / numLEDs) + j) & 255));
    }
    show();
    delay(wait);
  }
}

// Cycles the whole strip trhough a rainbow, filling it with one rainbow color at a time
void PixelStrip::solidRainbowCycle(uint8_t wait, uint8_t numCycles) {
  uint16_t i;
  uint32_t color;

  for (i = 0; i < 256 * numCycles; i++) { // cycles thru all colors on wheel
	stopPatternCheck();
	color = Wheel(i & 255);
    fillStrip(color, false);
    show();
    delay(wait);
  }
}

//color wipes a random set of colors
//numColors sets how many colors to wipe
//see colorWipe and colorWipePallet for more details
//style sets the type of wipe:
	//1: normal wipe, each segment is a color from the pattern (with wrapping)
	//2: each individual segment will alternated colors according to the pattern
	//3: rainbow mode 1, the rainbow will be evenly distributed across all segements together
	//4: rainbow mode 2, the rainbow will be distributed across each segment individually
	//5: color wipes whatever the BgPallet gradient is set to
void PixelStrip::colorWipeRandom(uint8_t style, uint32_t numColors, uint16_t segLength, int wait, boolean forward, boolean alternate, boolean simult){
	uint32_t pallet[numColors];
	byte pattern[numColors];
	for(int i = 0; i < numColors; i++){
		pallet[i] = randColor();
		pattern[i] = i;
	}
	colorWipePallet(style, simult, pattern, numColors, pallet, segLength, wait, forward, alternate);
}

//Fill the pixels one after the other with a color
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//segLength sets how long each colorwipe is. The strip is divided evenly for the wipe
//alternate sets if all segemtents wipe the same direction or alternate
void PixelStrip::colorWipe(uint32_t color, uint16_t wipeLength, int wait, boolean forward, boolean alternate, boolean simult){
	uint32_t pallet[1] = {color};
	byte pattern[1] = {0};
	colorWipePallet(1, simult, pattern, 1, pallet, wipeLength, wait, forward, alternate);
}

//Fill the pixels one after the other with a color
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//segLength sets how long each colorwipe is. The strip is divided evenly for the wipe
//alternate sets if all segemtents wipe the same direction or alternate
//the pattern and pallet set the order of colors for the wipes:
//pattern is a set of pallet indexs ex: {1, 5, 4}
//pallet is a set of colors ex {blue, green, red}
//simult sets if the wipes should occur simultaneously or one after another
//style sets the type of wipe:
//1: normal wipe, each segment is a color from the pattern (with wrapping)
//2: each individual segment will alternated colors according to the pattern
//3: rainbow mode 1, the rainbow will be evenly distributed across all segements together
//4: rainbow mode 2, the rainbow will be distributed across each segment individually
//5: color wipes whatever the BgPallet gradient is set to
void PixelStrip::colorWipePallet(uint8_t style, boolean simult, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], uint16_t wipeLength, int wait, boolean forward, boolean alternate) {
  uint8_t numSegs = ceil(float(numLEDs) / wipeLength);
  boolean forwardLocal = forward; //copy of forward, as it may change in the loops

  int16_t LEDPattern[numSegs][2];
  //set loop parameters for a forward loop
  int8_t directionConst = 1;
  uint16_t segStart = 0;

  //if we want to go backwards, change the loop parameters
  if(!forwardLocal){
	directionConst = -1;
    segStart = wipeLength - 1;
  }

	for (int j = 0; j < numSegs; j++) {
		if(alternate){ //if we're alternating, we switch the diection variables with each segemnt
			if(forwardLocal){
			   directionConst = 1;
			   segStart = 0;
			} else {
			   directionConst = -1;
			   segStart = wipeLength - 1;
			}
			forwardLocal = !forwardLocal;
		}
		LEDPattern[j][0] = (j * wipeLength) + segStart;
		LEDPattern[j][1] = directionConst;

  }
  colorWipeAdvanced(style, simult, LEDPattern, numSegs, colorPattern, colorPatternLength, pallet, wipeLength, wait);
}

//advanced color wiping function
//takes a LEDPattern[][2] with structure:  { {startPoint, direction} , {startPoint, direction}, etc }
//a colorPattenr[] with structure: {pallet index, pallet index, etc}
//a pallet, and the wipe length
//and wipes the leds from the startPoints, in a direction for the wipe length
//simult sets if the wipes should occur simultaneously or one after another
//style sets the type of wipe:
//1: normal wipe, each segment is a color from the pattern (with wrapping)
//2: each individual segment will alternated colors according to the pattern
//3: rainbow mode 1, the rainbow will be evenly distributed across all segements together
//4: rainbow mode 2, the rainbow will be distributed across each segment individually
//5: color wipes whatever the BgPallet gradient is set to
void PixelStrip::colorWipeAdvanced(uint8_t style, boolean simult, int16_t LEDPattern[][2], uint8_t LEDPatternLength, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], uint16_t wipeLength, int wait){
  uint16_t ledLocation; //led location for wipe

  //set the inner and outer loop limits
  //allows reusing of loops to do a sequential wipe (one segment after another)
  //or a simultaneous wipe (all segments at once)
  uint16_t outerLoopLimit = LEDPatternLength;
  uint16_t innerloopLimit =  wipeLength;
  uint16_t wipeIndex;
  uint16_t LEDPatternIndex;
  uint32_t color;

  if(simult){
	outerLoopLimit = wipeLength;
	innerloopLimit = LEDPatternLength;
  }

  //loops through each segment, setting one led at a time
  for(int i = 0; i < outerLoopLimit; i++){
	stopPatternCheck();
	for (int j = 0; j < innerloopLimit; j++) {

		//mode switching adjusts from sequential to simultaneous
		//by matching the loop indexs
		if(simult){
			wipeIndex = i;
			LEDPatternIndex = j;
		} else {
			wipeIndex = j;
			LEDPatternIndex = i;
		}

		//sets the led location; adds the current loop value corrosponding
		//to the wipelength multiplied by the direction indicator to the lED start point
		ledLocation = LEDPattern[LEDPatternIndex][0] + ( wipeIndex * LEDPattern[LEDPatternIndex][1] );

		//skin any leds that fall off the strip (allows repeated wipes to cover prime numbered strips)
		if( (ledLocation >= numLEDs) || (ledLocation < 0) ){ //if we've run over the number of leds, stop the loop
		  continue;
		}

		//set the pixel color according to the style
		switch(style){
			case 1 : //color wipes each color from pallet in turn
			  color = pallet[ colorPattern[ LEDPatternIndex % colorPatternLength ] ];
			break;

			case 2 : //color wipes alternating colors from pallet along the wipe
			  color = pallet[ colorPattern[ wipeIndex % colorPatternLength ] ];
			break;

			case 3 :  //color wipes a rainbow spread evenly across the strip
			  color = Wheel( (ledLocation  * 256 / numLEDs) & 255 );
			break;

			case 4 :  //color wipes a rainbow spreak evenly across each segment
			  color = Wheel( (ledLocation  * 256 / wipeLength) & 255);
			break;

			case 5 : //color wipes whatever the BgPallet gradient is set to
			  color = getBgGradientColor(ledLocation);
			break;
		}

		setPixelColor( ledLocation, color );

		if(!simult){
	      show();
	      delay(wait);
		}
	}
	if(simult){
		show();
		delay(wait);
	}
  }
}

// Fill the pixels one after the other with a color in a rainbow pattern
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//segmented sets the style of rainbow for each segment,
//either distributed across the strip, or across each segement
void PixelStrip::colorWipeRainbow(int wait, uint16_t wipeLength, boolean forward, boolean alternate, boolean segmented, boolean simult) {
  uint8_t style = 3;
  if(segmented){
	  style = 4;
  }
  colorWipePallet(style, simult, 0, 0, 0, wipeLength, wait, forward, alternate);
}

//Fill the pixels one after the other with a color in a the Bg gradient pattern
//forward == true moves pattern forward, ie incrementing pixel steps
//false moves it backward
void PixelStrip::colorWipeGradient(int wait, uint16_t wipeLength, boolean forward, boolean alternate, boolean simult) {
  colorWipePallet(5, simult, 0, 0, 0, wipeLength, wait, forward, alternate);
}

//builds a pattern for use with simpleRepeatPattern functions
//input is a x2 array of color lengths and pallet indexes, for example,
//patternScfld[3][2] = { {10, 1}, {5, 4}, {8, 2} }; would return a pattern of 10 pixels with pallet index 1, followed by 5 pixels with
//index 4, and 8 pixels of index 2, for a pattern with total length 23.
//NOTE you cannot return arrays in c, so you must pass in an array for the pattern you want to fill in,
//this array must be of the correct length to match the total number of pixels in the patten.
void PixelStrip::buildSimplePattern(byte patternScfld[][2], uint8_t patternScfldLength, byte pattern[], uint8_t patternLength){
	int patternIndex = 0;

	//fill in the pattern, by steping through the patternScfld
	//and filling sections of the returned pattern by advancing the
	//starting index with patternIndex
	for(int i = 0; i < patternScfldLength; i++){
		for(int j = 0; j < patternScfld[i][0]; j++ ){
			pattern[patternIndex + j] = patternScfld[i][1];
		}
		patternIndex = patternIndex + patternScfld[i][0];
	}
}

//takes a simple pattern and repeats it over the entire strip
//the pattern[] is an ordered set of indexs to the pallet
//for example: pattern[4] {0, 1, 4, 2} paired with a pallet of {off, red, blue, green, white}
//would produce a repeating pattern of off, red, white, blue along the strip
//the pattern will shift done the strip for a number of cycles (numCycles), either forward or backward
//fading in (steps) with a (wait) time between each step
void PixelStrip::doRepeatSimplePattern(byte pattern[], uint8_t patternLength, uint32_t pallet[], uint8_t palletLength, uint8_t wait, uint8_t steps, uint8_t numCycles, boolean forward) {
  byte localPattern[patternLength]; //a copy of the pattern, it can be shifted without changing the original pattern
  uint32_t localPatternColors[patternLength]; //array of colors used to store crossfaded color for each pixel in the pattern
  RGB palletRGB[palletLength]; //RGB version of the pallet, used for crossfading
  uint16_t startPixel;
  uint16_t nextPixel;

  //fill in the RGB pallet using the pallet
  makeRGBpalletFromPallet(pallet, palletRGB, palletLength);

  //copy the pattern into the local pattern
  for (int j = 0; j < patternLength; j++) {
	localPattern[j] = pattern[j];
  }

  //we want to copy the pattern along the whole strip
  //so we need to work out how many copies will fit
  //we round up, the last copy will be cut off
  int numSets = ceil( (float)numLEDs/ patternLength );

  //set of loop variables, controls the direction of the drawing loop
  uint16_t startPoint = 0;
  int increment = 1;

  //if we're not going forward, we want to change the loop variables so we work backwards
  if(!forward){
	startPoint = numLEDs - 1;
	increment = -1;
  }

  //run the process for numCycles
  for (int i = 0; i < numCycles ; i++ ) {
	stopPatternCheck(); //checks for external button presses to break the loop
	//for each cycle we run through {steps} number of steps
	//in each step we crossfade each pixel in the pattern towards the next color in the pattern
	//(wrapping for the last pixel)
	//then we draw the new color pattern {numSets} times along the strip
	//finally we advance the localPattern and start the next cycle
	for(int k =0; k < steps; k++){
		//for each pixel in the pattern, get the crossfaded color between it's current color and the next in the pattern
		for (int j = 0; j < patternLength; j++) {
		  localPatternColors[j] = getCrossFadeStep( palletRGB[localPattern[j]], palletRGB[ localPattern[ (j+1) % patternLength ]  ], steps, k);
		}
		//we repeat the localPatternColors array as many times as possible along the strip
		//if we hit the start or the end of the strip we break out of the loop
		for(int k = 0; k < numSets; k++){
			//pixel to begin the next pattern at
			//if we're running forwards, we start at 0 and add the number of pattern lengths
			//if we're running backwards, we start at numLEDs-1 and subtract the number of pattern lengths
			startPixel = startPoint + patternLength * k * increment;
			for (int j = 0; j < patternLength; j++) {
				nextPixel = startPixel + (j * increment); //next pixel to color, offset by the start pixel, in the driection of increment

				//break out if we run off the strip, we've drawn all the pattern copies
				if(nextPixel > numLEDs || nextPixel < 0 ){
					break;
				}

				setPixelColor(nextPixel, localPatternColors[j]);
			}
		}
		//show the crossfaded pattern
		show();
		delay(wait);
	}
	//we have finished a crossfading cycle
	//adnace the localPattern to cycle to the next colors
	for (int j = 0; j < patternLength; j++) {
		localPattern[j] = pattern[ ( (i + j + 1) % patternLength ) ];
	}
  }
}

//crossfades to the strip to a specified pattern
//the pattern is a 2 dimensional matrix with index structure [pixel number][pallet index]
//where the pallet is an array of 32bit colors
//the strip crossfades to this pattern in (steps) with (wait) between each step
void PixelStrip::crossFadePattern( byte pattern[][2], uint8_t patternLength, uint32_t pallet[], uint8_t wait, uint8_t steps) {
  uint32_t colorPattern[numLEDs];
  for ( int i = 0; i < numLEDs; i++ ) {
    colorPattern[i] = 0; //sets all the pixels to 0 (off), overwrites whatever is randomly there by default
  }
  for ( int i = 0; i < patternLength; i++ ) {
    colorPattern[ pattern[i][0] ] = pallet[ pattern[i][1] ];
  }
  crossFadeRandomPattern(colorPattern, wait, steps);
}

//takes a pattern of pixels and fades it through the rainbow
//the pattern is a 2 dimensional matrix with index structure [pixel number][pallet index]
//for this function, the pallet index isn't used, and can be set to any byte integer
//the (shiftFactor) determines how many steps through the rainbow are taken
//the pattern crossfades through the colors in (steps) with (wait) time between each step
void PixelStrip::rainbowPatternCycle( byte pattern[][2], uint8_t patternLength, uint8_t shiftFactor,  uint8_t wait, uint8_t steps) {
  for (int i = 0; i < patternLength; i++) {
    pattern[i][1] = 1;
  }
  uint32_t pallet[2] = {0, 1};
  for (int j = 0; j < 256;) {
	stopPatternCheck();
    pallet[1] = Wheel(j % 255);
    crossFadePattern(pattern, patternLength, pallet, wait, steps);
    j = j + shiftFactor;
  }
}

//draws a pattern onto the strip and shows it
//the pattern is a 2 dimensional matrix with index structure [pixel number][pallet index]
//where the pallet is an array of 32bit colors
void PixelStrip::drawPattern( byte pattern[][2], uint8_t patternLength, uint32_t pallet[]){
  for ( int i = 0; i < patternLength; i++ ) {
    setPixelColor(pattern[i][0], pallet[ pattern[i][1] ]);
  }
  show();
}

//does a simpleStreamer with random colors, or one prefered color
//passing numColors > 1 will do random of that many colors
//pssing one will use the preferedColor\
//see simpleStreamer for explanation of other variables
void PixelStrip::simpleStreamerRand( byte numColors, uint32_t preferedColor, byte streamerLength, byte spacing, int32_t BgColor, boolean forward, byte numCycles, int wait){
	byte patternLength = numColors;

	uint32_t pallet[ patternLength ];
	byte pattern[ patternLength ];

	if(numColors == 1){
		pattern[0] = 1;
		pallet[0] = preferedColor;
	} else {
		for(int i = 0; i < numColors; i++){
			pattern[i] = i;
			pallet[i] = randColor();
		}
	}
	simpleStreamer( pattern, patternLength, pallet, streamerLength, spacing, BgColor, forward, numCycles, wait);
}

//does a series of simple streamers moving either forwards or beckwards along the strip
//pattern[] specifies the sequence of colors (using pallet indexes)
//the pattern repeats down the strip as many times as possible
//pallet is the set of colors
//streamerLenght is the length of each streamer
//spacing is the gap between streamers
//BgColor is the color of the spacing (passing -1 will set it to a rainbow)
void PixelStrip::simpleStreamer( byte pattern[], byte patternLength, uint32_t pallet[], byte streamerLength, byte spacing, int32_t BgColor, boolean forward, byte numCycles, int wait){
	uint16_t streamSize = streamerLength + spacing; //the total streamer size including the spacing

	uint16_t numCyclesShift = numCycles + streamerLength; //the number of cycles needs to be offset (see below)
	//counting variables: colorIndex determines which color to display,
	//nextStreamStart is the last pixel of the streamer,
	//nextLedOn is the next led to turn on in the strip
	//intNextStreamStart is the starting value of nextStreamStart for each loop
	uint8_t colorIndex;
	int16_t nextStreamStart, nextLedOn, nextLedOff, intNextStreamStart;
	uint32_t nextColor; //color of the next led to be on
	uint8_t prevBrightness = getBrightness(); //need to save the current brightness

	//default directional values, set to the forward case
	int8_t dirctMulti = 1; //multiplier for the direction, either forwards or backwards down the strip
	intNextStreamStart = 0;

	if(!forward){
		dirctMulti = -1;
		intNextStreamStart = numLEDs - 1;
	}

	//fill in the background for the inital streamers
	setBackground(BgColor);

	//fill in the inital set of streamers (so they don't fill in during the first few cycles)
	nextStreamStart = intNextStreamStart;
	//run over the strip and draw as many streamers as will fit
	//the loop limits and increments are adjusted depending on if we're going forwards or backwards
	while( checkLimitStreamer(forward, nextStreamStart) ){
			colorIndex = ( nextStreamStart / streamSize ) % patternLength;
			nextColor = pallet[ pattern[colorIndex] ];
			//fill in the streamers either starting at the strip start, or at the strip end
			if(forward){
				fillStripSection( nextColor, nextStreamStart , nextStreamStart + streamerLength - 1, false);
			} else {
				fillStripSection( nextColor, nextStreamStart - streamerLength + 1, nextStreamStart, false);
			}
			nextStreamStart += dirctMulti * streamSize;
	}
	show();


	//run for a certain number of cycles (offset by streamerLength because we draw in the intial streamer)
	for(int i = streamerLength ; i <= numCyclesShift ; i++){
		stopPatternCheck();
		yield();
		nextStreamStart = intNextStreamStart;

		//we need to set the brighness to max, b/c we use getPixelColor,
		//lower brightnesses modify the color numbers, so we need everything at max to get the right values
		setBrightness(255);

		//run along the strip for as meany streamSizes as possible
		//the loop limits and increments are adjusted depending on if we're going forwards or backwards
		//In each streamSized section we advance the onLED and turn another off
		//the color is first picked based on what color we should be on
		//the the color is picked based on the color of the previous pixel
		while( checkLimitStreamer(forward, nextStreamStart) ){
			nextLedOn = nextStreamStart + ( dirctMulti * (i % streamSize) );
			//if we're on the first streamer, we need to shift in a new color using the pattern
			//(or if the streamSize is 1, then each pixel is the next color)
			//otherwise we just pick the color of the pixel before or after us
			if(nextStreamStart == intNextStreamStart || streamSize == 1){
				colorIndex = ( (nextStreamStart + i) / streamSize ) % patternLength;
				nextColor = pallet[ pattern[ colorIndex ] ] ;
			} else {
				nextColor = getPixelColor(nextLedOn - dirctMulti * 1);
			}

			//set the next pixel on and turn off the last pixel in the streamer
			setPixelColor( nextLedOn , nextColor );
			nextLedOff = nextLedOn - dirctMulti * streamerLength;
			//switch the offLed to the BgColor, if spacing is 0 then there is no Bg so we don't
			if(spacing > 0){
				setBackgroundSingle( nextLedOff, BgColor );
			}

			nextStreamStart += dirctMulti * (streamSize);
		}
		setBrightness(prevBrightness);
		show();
		delay(wait);
	}
}

//loop limit checker function for the simpleStreamer function
//if we're goinging forward then the counter advances positivly until we reach numLEDs
//otherwise we're going backwards, and we need to check if the counter has passed below 0
boolean PixelStrip::checkLimitStreamer(boolean forward, int16_t nextStreamStart){
	if(forward){
		return nextStreamStart <= numLEDs;
	} else {
		return nextStreamStart >= 0;
	}
};

//cycles a random set of color gradients across the strip
//numColors set how many colors to cycle
//gradSteps is how long each gradient transition is; ie 4 will mean colors transition across 4 leds
//patterns which don't fully fit on the segments will still work, colors will be cycled on
//inToOut sets the direction of the cycles, either starting at the last segment or the first
void PixelStrip::gradientCycleRand( byte numColors, byte gradSteps, uint16_t numCycles, boolean forward, int wait){

	uint32_t pallet[ numColors ];
	byte pattern[ numColors ];

	for(int i = 0; i < numColors; i++){
		pattern[i] = i;
		pallet[i] = randColor();
	}

	gradientCycle(pattern, numColors, pallet, numColors, gradSteps, numCycles, forward, wait);
}

//cycles a gradients across the strip
//pattern an array of pallet color indecies, they are drawn in order of the pattern with wrapping
//pallet is an array of color values
//gradSteps is how long each gradient transition is; ie 4 will mean colors transition across 4 leds
//patterns which don't fully fit on the strip will still work, colors will be cycled on
//forward sets the direction of the cycles, either starting at the last led or the first
//NOTE it might be best to set the gradSteps so that they dont fit an integer number of times on the strip
void PixelStrip::gradientCycle(byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean forward, int wait){

	uint32_t color;
	uint8_t nextPattern, currentPattern, nextLed;
	uint8_t prevBrightness = getBrightness(); //need to save the current brightness

	//loop direction variables (default is last led to first)
	int16_t endLimit = numLEDs;
	int16_t startLimit = 0;
	int8_t loopStep = 1;
	int16_t firstLed = numLEDs - 1;

	//switch the loop variables if we want first led to last
	if(forward){
		endLimit = -1;
		startLimit = numLEDs - 1;
		loopStep = -1;
		firstLed = 0;
	}

	RGB palletRGB[palletLength]; //need to make an RGB pallet for getCrossFadeStep
	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);

	uint16_t patternCount = 0;	//tracks what how many patterns we've gone through

	//inital pattern indecies, first pair of colors in the pattern
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//we need to draw the initial gradients on the strip
	//to do this we run across all the leds
	//every gradsteps number of leds, we rotate the gradient colors,
	//transitioning from the current color to the next
    for(int i = 0; i < numLEDs; i ++ ){

		//if we've gone through gradSteps cycles
		//a color transition is finished and we need to move to the next color
		if(i % gradSteps == 0){
			patternCount++;
			currentPattern = nextPattern;
			nextPattern = pattern[ patternCount % patternLength ];
		}

		//get the crossfade between the current and next color, where the transition is gradsteps long
		color = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], gradSteps, ( i % gradSteps ) );
		setPixelColor( i , color );
	}

	show();
	delay(wait);

	//initialize loop variables for main cycle
	patternCount = 1; //pattern count must start at 1 b/c we've already drawn the first gradient set
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//the main cycle
	//every gradSteps we switch to the next color
	//b/c it's hard to caculate the correct colors across the whole strip, and redraw it
	//for each step, we set the color of the start or end pixel (depending on the motion direction)
    //and then set the next pixel color to that of the previous pixel
	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		//if we've gone through gradSteps cycles
		//a color transition is finished and we need to move to the next color
		if(i % gradSteps == 0){
			patternCount++;
			currentPattern = nextPattern;
			nextPattern = pattern[ patternCount % patternLength ];
		}

		//we need to use getPixelColor(), which only returns the correct color at full brightness
		setBrightness(255);

		//cycle through the strip, if we're at the outer or inner led (depen on direction)
		//set it to the next transistion step, otherwise just copy the color of the previous led
		for(int j = startLimit; j != endLimit; j += loopStep){
			if( j == firstLed ){
				color = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], gradSteps, ( i % gradSteps ) );
			} else {
				color = getPixelColor( j + loopStep );
			}
			setPixelColor( j , color );
		}

		//reset the brighness and show the strip
		setBrightness(prevBrightness);

		show();
		delay(wait);
	}
}

//moves a certain number of leds along a strip in a specified direction, with either 0, 1, or 2 tails (like meteors going along the strip)
//the number of leds is specified, but their colors, inital locations, and directions will be picked randomly
//note that for this function, colors will be picked randomly from a passed in pallet unless
//passing in a pallet length of zero, will cause all of the leds colors to be chosen randomly
void PixelStrip::patternSweepSetRand( uint8_t numRandLeds, uint32_t pallet[],  uint8_t palletLength, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, uint8_t colorMode, int wait, int runTime){
 //initialize a randPattern, matching the pattern structure for patternSweep(), setting the length to match the number of random leds
  uint16_t randPattern[numRandLeds][3];

  //Fill in the randPattern using the pallet
  //randPattern matches the pattern structure:
  //patterns are a y x 3 matrix where y is the number of leds numRandLeds,
  //each row is specified as such { led position, led color (index of pallet color), motion direction }
  for(int i = 0; i < numRandLeds; i++){
	  randPattern[i][0] = random(numLEDs); //pick a random starting point for each led
	  if(palletLength == 1){ //pick a color index from the pallet, if the pallet is only length 1, just pick it
		randPattern[i][2] = 0;
	  } else if( palletLength == 0 ) { //set the pallet index for the local pattern, and choose a random color for it
		randPattern[i][2] = i;
		pallet[i] = randColor();
	  } else { //the default, pick a pallet color at random for each led
		randPattern[i][2] = random(palletLength);
	  }
	  randPattern[i][1] = random(2); //choose a random direction
  }
  patternSweep(randPattern, numRandLeds, pallet, BgColor, trails, trailLength, scanner, eyeSize, colorMode, wait, runTime);
}

//moves a certain number of leds along a strip in a specified direction, with either 0, 1, or 2 tails (like meteors going along the strip)
//the number of leds is specified, but their colors, inital locations, and directions will be picked randomly
//passing -1 as the preferedColor will do random set of colors
//passing -1 as the BgColor will set the background to a rainbow
//passing a trails value of 5, will do fireworks, eyesize will be the dimFactor
//(see patternSweep for more details)
void PixelStrip::patternSweepRand( uint8_t numRandLeds, int32_t preferedColor, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, uint8_t colorMode, int wait, int numCycles) {

  //TODO, fix double check of preferedColor
  //initialize a pallet for passing into patternSweep()
  //if we a given a prefered color then the pallet only needs to be length 1
  //otherwise it needs to be the same length as the numRandLeds
  int palletLength;
  if( preferedColor > -1 ){
	 palletLength = 1;
  } else {
	 palletLength = numRandLeds;
  }
  uint32_t pallet[palletLength];

   //if a perfered color is passed in, use it otherwise we will generate the colors and pallet in patternSweepSetRand();
   if( preferedColor > -1 ){
	  pallet[0] = preferedColor;  //since all the moving leds will be one color, the pallet can be length one
   } else {
	   palletLength = 0; //if we want to run random colors, we need to pass in a pallet length of 0
   }
   if( trails == 5 ){
	 fireworksSet( numRandLeds, eyeSize, pallet, palletLength, BgColor, colorMode, wait, numCycles);
   } else {
	 patternSweepSetRand( numRandLeds, pallet, palletLength, BgColor, trails, trailLength, scanner, eyeSize, colorMode, wait, numCycles);
   }
}


//moves a certain number of leds along a strip in a specified direction, either with or without tails (like meteors going along the strip)
//the number of leds,their colors, inital locations, and directions are specified in a yx3 pattern array  with
//each row specified as such { led position, motion direction, led color (index of pallet color)}
//passing -1 as BgColor will set it to rainbow, -3 sets to Bg gradient
//trails:
		//1: one trail extending behind pixel
		//2: a trail in front and behind pixel
		//-1: a trail in front of pixel
		//3: infinite trails (trails are not dimmed or removed unless passed over by another pixel)
//Scanner: in scanner mode, the pixels do not wrap around the end of the strip, instead they reverse direction (trails also do not extend past end of strip)
//eyeSize sets the with of the front pixel (0 is one pixel)
//colorMode determines what colors are used for the pixels
	//mode 1: use the pallet for the colors
	//mode 2: use a rainbow spread across the strip for colors
	//mode 3: use the BgGradient for colors
void PixelStrip::patternSweep( uint16_t pattern[][3], uint8_t patternLength, uint32_t pallet[], int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, uint8_t colorMode, int wait, int numCycles){
	//initialize a local pattern, matching the passed in pattern structure
	//patterns are a y x 3 matrix where y is the number of leds numRandLeds,
	//each row is specified as such { led position, led color (index of pallet color), motion direction }
	uint16_t localPattern[patternLength][3];

	//a quick clear if we're streaming colors, otherwise the strip is filled with a background
	if(trails > 2) {
		stripOff();
	} else {
		setBackground( BgColor );
	};

	//copy the original pattern into the local pattern so that we don't modify the original
	for (int i = 0; i < patternLength; i++) {
		localPattern[i][0] = pattern[i][0];
		localPattern[i][1] = pattern[i][1];
		localPattern[i][2] = pattern[i][2];
	}

	//code block below configures function to run for a set amount of time instead of cycles
	//***************************************
	//setup some time variables for controlling the loop
	// int startTime = millis();
	// int currentTime = startTime;
	// int resetTime = startTime - wait;
	// int steps = 0;

	//run the pattern
	// while ((currentTime - startTime) < runTime) {
		// currentTime = millis();
		// if ((currentTime - resetTime) >= wait) {
		  // resetTime = currentTime;
	//********************************

	//variables for creating tails
	uint16_t modAmmount = numLEDs; //needed for wrapping the trails between the start and end of the strip
	int8_t directionArray[2]; //[direction multiplier, eyeSize]
	int8_t directionArray2[2] = {-1, 0}; //for 2 trails case

	uint16_t ledLocation; //front led location
	uint16_t trailLedLocation;

	//if we're in scanner mode, we dont want the trails to wrap,
	//so we want to keep the mod larger than the ledLocation can reach
	//ie the numLEDs plus the tailLength, as this is will allow the tail to extend off the strip,
	//which will be discarded by the setPixelColor func, as the pixel is out of bounds
	if(scanner){
		modAmmount = numLEDs + trailLength + 1;
	}

	byte percentDim = 100 / (trailLength + 1); //dim percentage so that the trail almost reaches total dimming at the trail end (+1 prevents it being off)
	boolean lastPixel = false; //checker for if we've passed the trails end
	uint8_t dimFactor = 100; //dynamic dim factor for creating trail

	//run the particles for a certain number of cycles
	//for each cycle, moves the leds to their next positions
	//then sets the leds at the previous location to the BgColor (including trail ends)
	//then draws new trails if needed
	//then draws the new front leds
	//this way we never overwrite front leds or trails with the BgColor
	for(int i = 0; i <= numCycles; i++){
		stopPatternCheck();
		yield();
		//run through each of the leds in the local pattern and incement of decrement their position, wrapping where needed
		incrementLedLocation( localPattern, patternLength, eyeSize, scanner);

		//if we have 3 trails or more, we are in streamer mode, so we don't touch the previous leds
		if(trails < 3){
			//for each led in the pattern, switch the previous led to the BgColor
			for (int j = 0; j < patternLength; j++) {
				ledLocation = localPattern[j][0];

				//get the led direction values
				patternSweepGetLedDirection(directionArray, localPattern[j][1], trails, eyeSize);
				//if we don't have trails, we just need to turn off the previous pixel
				//otherwise we need to switch the pixel at the end of the trail
				if(trails == 0){
					trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, 1);
					setBackgroundSingle(trailLedLocation, BgColor);
				} else {
					trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, trailLength + 1);
					setBackgroundSingle(trailLedLocation, BgColor);

					 //if we have two trails, we also need switch the pixel at the end of the positive trail
					 //note that for two trails patternSweepGetLedDirection sets a direction array for the positive trail
					 //so we use directionArray2, which is for the negative trail
					if(trails == 2){
						trailLedLocation = patternSweepGetTrailLed(directionArray2, modAmmount, ledLocation, trailLength + 1);
						setBackgroundSingle(trailLedLocation, BgColor);

					} else if(trails == -1){ //if the trail is reversed, we need to switch the previous pixel (b/c it's not over written by the trail)
						trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, -1); //-1 because it's in the reverse direction to the motion
						setBackgroundSingle(trailLedLocation, BgColor);
					}

				}
			}

			//if we have trails, draw them for each pixel in the pattern
			if(trails != 0 ){
				for (int j = 0; j < patternLength; j++) {
					ledLocation = localPattern[j][0];
					patternSweepGetLedDirection(directionArray, localPattern[j][1], trails, eyeSize);

					//draw a trail, extending positively or negativly along the strip, dimming with each step, wrapping according to the modAmmount
					for( int k = 1 ; k <= trailLength ; k++){

						//dimming factor for the trail member, (100 is no dimming)
						dimFactor = 100 - (percentDim * k);

						//get the led location of each trail value, with wrapping (unless we're in scanner mode)
						trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, k);

						patternSweepSetPixelColor(trailLedLocation, pallet[ localPattern[j][2] ], colorMode, dimFactor);

						//if we have two trails, we also need to draw the negative trail
						//note that for two trails patternSweepGetLedDirection sets a direction array for the positive trail
						//so we use directionArray2, which is for the negative trail
						if(trails == 2){
							trailLedLocation = patternSweepGetTrailLed(directionArray2, modAmmount, ledLocation, k);
							patternSweepSetPixelColor(trailLedLocation, pallet[ localPattern[j][2] ], colorMode, dimFactor);
						}
					}
				}
			}

		}

		//draw the main body of the particle (including eyesize)
		//this is done last so that tails of other particles don't overwrite
		for (int j = 0; j < patternLength; j++) {
			//set the pixel color at the incremented position
			for(int k = 0; k <= eyeSize; k++){
				if(scanner){ //if we're in scanner mode, we dont want to wrap, so no mod
					ledLocation = ( localPattern[j][0] + k );
				} else {
					ledLocation = ( localPattern[j][0] + k ) % numLEDs;
				}
				patternSweepSetPixelColor(ledLocation, pallet[ localPattern[j][2] ], colorMode, 100);
			}
		}
	    show();
	    delay(wait); //uncomment if switching from numCycles to timer based loop
    }
}

//a private function for setting pixel colors for the patternSweep function
//takes the led location, if it needs to be a rainbow or not, the dimFactor, and a rainbow dim adjustment factor
//and colors a pixel the appropriate color, desaturated by the appropriate factor
//note, that rainbowDimAdjst only applies to the rainbow, it adjusts the dimfactor
//this was determined to be needed, b/c rainbow colors don't visually dim like single colors
void PixelStrip::patternSweepSetPixelColor(int16_t ledLocation, uint32_t color, uint8_t colorMode, uint8_t dimFactor){
	uint32_t colorFinal;

	if(ledLocation < 0){
		return;
	}
	//make below golbal!!!
	float rainbowDimAdjst = 1; //emperical rainbow dim adjustment (rainbow pixels don't seem to look as dim as solid colors)
	//if we're in rainbow mode, we need to set the pixel to a fader rainbow color accordin to it's location
	//otherwise, set it to the passed in color
	if(colorMode == 1){
		colorFinal = desaturate( color , dimFactor );
	} else if(colorMode == 2){
		colorFinal = desaturate( Wheel( ( ledLocation ) * 256 / numLEDs ), dimFactor / rainbowDimAdjst);
	} else {
		colorFinal = desaturate( getBgGradientColor(ledLocation) , dimFactor );
	}
	setPixelColor( ledLocation , colorFinal );
}

//returns the position of a trail led based on the front led location, direction array, and the mod ammount
int16_t PixelStrip::patternSweepGetTrailLed(int8_t directionArray[2], uint16_t modAmmount, int16_t ledLocation, int8_t distance){
	return (int16_t(ledLocation) + modAmmount + ( directionArray[0] * distance ) + directionArray[1] ) % modAmmount;
}

//writes into the passed directionArray
//directionArray[0] is the led's direction multiplier
//directionArray[1] is the eyeSize used (only positive trails need to take account of the eyeSize)
void PixelStrip::patternSweepGetLedDirection(int8_t directionArray[2], uint8_t ledDirection, int8_t trails, uint8_t eyeSize){

	//determine the type of trail, and set the direction values
	if(trails == 2){ //two trails, this sets variables for the positive tail, the negative one is drawn seperately
		directionArray[0] = 1;
		directionArray[1] = eyeSize;
	} else if( trails == -1){ //reversed trails, same as case 1, but we switch the inner if result
		if( ledDirection == 1 ){
			directionArray[0] = 1;
			directionArray[1] = eyeSize;
		} else {
			directionArray[0] = -1;
			directionArray[1] = 0;
		}
	} else { // one or zero trails
		if( ledDirection == 1 ){
			//going forwards so the tail extends negatively along the strip
			//we don't need to account for the eyeSize, because the eye extends positivly
			directionArray[0] = -1;
			directionArray[1] = 0;
		} else {
			//going backwards so the trail extends positivly along the strip
			//and we need to create the tail after the eye length
			directionArray[0] = 1;
			directionArray[1] = eyeSize;
		}
	}
}

//increments the led location for all leds in a pattern, specifically for use with patternSweep functions
void PixelStrip::incrementLedLocation( uint16_t localPattern[][3], uint8_t patternLength, uint8_t eyeSize, boolean scanner){
	for (int i = 0; i < patternLength; i++) {
		if ( localPattern[i][1] == 1 ) {
		  //going forward: increment, if going past the strip length, set back to zero
		  //if scanner mode is on, reverse the direction instead of wrapping
		  if ( (localPattern[i][0] + eyeSize) >= numLEDs ) {
			  if(scanner){
				 localPattern[i][1] = 0;
			  } else {
				 localPattern[i][0] = 0;
			  }
		  } else {
			localPattern[i][0]++ ;
		  }
		} else {
		  //going backward: decrement, if going below zero, loop back to strip end
		  //note we cast the local pattern to an int for the wrapping check b/c it's normally unsigned
		  //if scanner mode is on, reverse the direction instead of wrapping
		  if ( int(localPattern[i][0]) - 1 < 0) {
			  if(scanner){
				localPattern[i][1] = 1;
			  } else {
				localPattern[i][0] = numLEDs;
			  }
		  } else {
			localPattern[i][0]-- ;
		  }
		}
	}
}

//sets a random set of leds moving accross the strip in random directions, following a rainbow color pattern
void PixelStrip::patternSweepRainbowRand( uint8_t numRandLeds, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, int wait, int numCycles){
	uint16_t pattern[numRandLeds][2];
	for(int i = 0; i < numRandLeds; i++){
		pattern[i][0] = random(numLEDs);
		pattern[i][1] = random(2);
	}
	patternSweepRainbow( pattern, numRandLeds, BgColor, trails, trailLength, scanner, eyeSize, wait, numCycles);
}

//sweeps a pattern of pixels across the strip, the color of the pixels will corrospond to a rainbow spread across the strip
//The pattern passed in is a yx2 array where each entry represents a pixel start location and a direction
//ex: pattern[3][2] = { {1, 0}, {5, 1}, {10, 0} } would be pixels starting at 1, and 10, moving forward, and a pixel at 5 moving backwards
//eyeSize sets the number of additional pixels to light before dimming (0 does 1 pixel)
void PixelStrip::patternSweepRainbow( uint16_t pattern[][2], uint8_t patternLength, int32_t BgColor, int trails, uint8_t trailLength, boolean scanner, uint8_t eyeSize, int wait, int numCycles){

	uint16_t localPattern[patternLength][3]; //note that the third index is never used, it exists to allow calling patternSweep
	//if the direction of the first pixel is not 0, 1 then we will create a pattern randomly of length patternLength

	//copy the passed in pattern into a 3 columb pattern for patternSweep
	for (int i = 0; i < patternLength; i++) {
		localPattern[i][0] = pattern[i][0];
		localPattern[i][1] = pattern[i][1];
		localPattern[i][2] = 0;
    }
	//make an empty pallet to pass to pattenSweep
	uint32_t pallet[1];
	pallet[0] = 0;
	patternSweep( localPattern, patternLength, pallet, BgColor, trails, trailLength, scanner, eyeSize, 2, wait, numCycles);
}

//does a patternSweepRepeat with random colors, or one prefered color
//passing numColors > 1 will do random of that many colors
//pssing one will use the preferedColor\
//see patternSweepRepeat / patternSweep for explanation of other variables
void PixelStrip::patternSweepRepeatRand(byte numColors, uint32_t preferedColor, int32_t BgColor, int trails, byte trailLength, boolean scanner, boolean direction, byte eyeSize, byte spacing, uint8_t colorMode, int wait, int numCycles ){

	uint32_t pallet[ numColors ];
	byte pattern[ numColors ];

	if(numColors == 1){
		pattern[0] = 1;
		pallet[0] = preferedColor;
	} else {
		for(int i = 0; i < numColors; i++){
			pattern[i] = i;
			pallet[i] = randColor();
		}
	}
	patternSweepRepeat(pattern, numColors, pallet, BgColor, trails, trailLength, scanner, direction, eyeSize, spacing, colorMode, wait, numCycles );
}

//does a similar thing to simpleStreamer, but using patternSweep
//ie, it makes as many streamers as possible across the strip according to the colorPattern
//note that if the streamers do not fit into the strip an even number of times you will get overlap
//also, unlike simpleStreamer, the pattern is only created once, so if you have more colors than possible, the extras won't show up
//see patternSweep for explanation of other variables
void PixelStrip::patternSweepRepeat(byte colorPattern[], byte colorPatternLength, uint32_t pallet[], int32_t BgColor, int trails, byte trailLength, boolean scanner, boolean direction, byte eyeSize, byte spacing, uint8_t colorMode, int wait, int numCycles ){
	//total particle length, ie the strip length needed for one pariticle, it's eye, trail, and spacing
	uint16_t totalParticalLength = eyeSize + trailLength + spacing + 1;

	//if we had two trails, then we need to add the extra trial length
	if(trails == 2){
		totalParticalLength += trailLength;
	}

	//how many particles we can fit in the strip
	uint8_t totalParticles =  ceil( float(numLEDs) / totalParticalLength ) ;
	//uint8_t totalParticles = (numLEDs) / totalParticalLength ;

	uint16_t pattern[totalParticles][3];

	//if we're:
		//going forwards with negative trails
		//have two trails
		//going backwards with negative trails
	//we need to offset the start point to account for the trail behind the particle
	byte startOffset = 0;
	if( (trails == 1 && direction == 1) || trails == 2 || (trails == -1 && direction == 0)){
		startOffset = eyeSize + trailLength;
	}

	for (int i = 0; i < totalParticles; i++) {
		pattern[i][0] = (i * totalParticalLength) + startOffset;//position
		pattern[i][1] = direction; //direction
		pattern[i][2] = colorPattern[i % colorPatternLength]; //color index
    }
	patternSweep( pattern, totalParticles, pallet, BgColor, trails, trailLength, scanner, eyeSize, colorMode, wait, numCycles);
}

//does a shooter, with the strip being split into sections of sectionLength, if an integer number of sections do \
//not fit in the strip, the last section will be shortened
//Otherwise, uses same inputs as shooter (see shoother for details)
void PixelStrip::shooterFixedLength( uint16_t sectionLength, uint32_t pallet[], byte palletLength, int32_t BgColor, uint8_t spawnChance, byte maxNumShooters, int trails, int8_t trailLength, uint8_t colorMode, boolean direction, boolean alternating, int wait, int numCycles){
	byte numSections = ceil(float(numLEDs) / (sectionLength)); //number of possible sections, rounded up to allow a partial section at the end
	uint16_t sections[numSections][2]; //holds { start pixel, length of the section }

	//the start pixel of each is i * sectionLength (totlength)
	//if the totLength + sectionLength <= numLEDs then we still have room for a full section on the strip
	//otherwise, we need to just use whatever length is left, given by numLEDs - totLength
	for(int i = 0; i < numSections; i++){
		uint16_t totLength = i * sectionLength;
		sections[i][0] = totLength;
		if( (totLength + sectionLength) <= numLEDs){
			sections[i][1] = sectionLength;
		} else {
			sections[i][1] = numLEDs - totLength ;
		}
	}

	shooter( sections, numSections, pallet, palletLength, BgColor, spawnChance, maxNumShooters, trails, trailLength, colorMode, direction, alternating, wait, numCycles);
}

//spawns shooters along sections of the strip, a shooter is like a patternSweep, but it terminates when it gets to the end of a section
//sections[][2] an array of strip setions structured as start points and lengths ie { { 3, 12} } indicates a section starting at 3, and length 12 (incl starting pixel)
//the colors of shooters will be choosen randomly from the pallet
//passing -1 as BgColor will set it to rainbow, -3 sets to Bg gradient
//spawnChance is the percentage chance to spawn a shooter on each cycle
//maxNumShooters is the maximum number of shooters that can be active in one section at one time
//trails:
		//0: no trails
		//1: one trail extending behind pixel
		//3: infinite trails (trails are not dimmed or removed unless passed over by another pixel)
//trailLength sets how long trails are
//colorMode determines what colors are used for the pixels
	//mode 1: use the pallet for the colors
	//mode 2: use a rainbow spread across the strip for colors
	//mode 3: use the BgGradient for colors
	//mode 4: use a random pallet for the colors, number of colors will be the palletLength
//direction set the direction shooters move down the sections, true will move positivly, false will move negativly
//alternating will alternate the dirction for each subsequent section
//wait is how fast the shooters move
//numCycles is total number of cycles the system will run
void PixelStrip::shooter( uint16_t sections[][2], byte numSections, uint32_t pallet[], byte palletLength, int32_t BgColor, uint8_t spawnChance, byte maxNumShooters, int trails, int8_t trailLength, uint8_t colorMode, boolean direction, boolean alternating, int wait, int numCycles){
	int16_t shooterLocs[numSections][maxNumShooters]; //locations of shooters
	int8_t directions[numSections]; //the step rate for each section, ie -1 or 1 determined by the direction variable and alternating
	uint8_t colors[numSections][maxNumShooters]; //holds a color pallet color index for each of the sections
	//boolean spawnOk[numSections]; //if true, then the section is not at capacity for shooters
	boolean shooterActive[numSections][maxNumShooters]; //records if a shooter is active in a section
	int8_t directionArray[2] = {0,0}; //used for patternSweepGetTrailLed();
	boolean spawnOkTest = true; //if true, then a shooter and its trail are not within the spawn zone

	int16_t ledLocation, trailLedLocation; //location of the front of the current shooter, location of trail leds
	int16_t sectionEnd, sectionStart; //end point of a given section, the start point of a given section, both are int16_t's b/c they can be subtracted from
	//We dont want the trails to wrap into other sections,
	//so we want to keep the mod larger than the ledLocation can reach
	//ie the numLEDs plus the tailLength, as this is will allow the tail to extend off the section,
	//which will be discarded
	int16_t modAmmount = numLEDs + trailLength + 1;

	uint8_t chance; //stores random roll for spawning

	//used for generating a random pallet, we don't want to modify the passed in one
	//as a rule if color mode is 4 we generate a random pallet of length palletLength
	//then we set color mode to 1 to pick randomly from the pallet
	uint32_t palletCopy[palletLength];
	if(colorMode == 4){
		colorMode = 1;
		genRandPallet(palletCopy, palletLength);
	} else {
		for(int i = 0; i < palletLength; i++){
			palletCopy[i] = pallet[i];
		}
	}

	//ensure that if we have no trails, we have 0 trail length
	if(trails == 0 || trails == 3){
		trailLength = 0;
	}

	//run through the number of sections
	//set all spawns to true, and activeShooters to false
	//also setup direction array, if we're alternating flip the direction for each section
	for(int i = 0; i < numSections; i++){
		//spawnOk[i] = true;
		for(int j = 0; j < maxNumShooters; j++){
			shooterActive[i][j] = false;
		}
		if(direction){
			directions[i] = 1;
		} else {
			directions[i] = -1;
		}
		if(alternating){
			direction = !direction;
		}
	}

	//a quick clear if we're streaming colors, otherwise the strip is filled with a background
	if(trails > 2) {
		stripOff();
	} else {
		setBackground( BgColor );
	}

	byte percentDim = 100 / (trailLength + 1); //dim percentage so that the trail almost reaches total dimming at the trail end (+1 prevents it being off)
	uint8_t dimFactor = 100; //dynamic dim factor for creating trail

	//run the system for a certain number of cycles
	//for each cycle, if a shooter can spawn, try to spawn one,
    //if a shooter is active, move it to its next positions
	//then sets the leds at the previous location to the BgColor (accouting for trails)
	//then draws new trails if needed
	//then draws the new front leds
	//this way we never overwrite front leds or trails with the BgColor
	for(int k = 0; k <= numCycles; k++){
		stopPatternCheck();
		yield();
		//for each section check if we can spawn a shooter, if so try to spawn one
		//a shooter can only spawn if the max number of shooters hasn't been reached, and if the shooter isn't active
		//if not, keep drawing the current shooter
		for(int i = 0; i < numSections; i++){
			sectionEnd = sections[i][0] + sections[i][1] - 1;
			sectionStart = sections[i][0];
			for(int j = 0; j < maxNumShooters; j++){

				//if all the shooters trails have passed the end/start of the section (depending on direction), then we can
			    //spawn a new shooter
				spawnOkTest = true;
				for(int k = 0; k < maxNumShooters; k++){
					if(shooterActive[i][k]){
						ledLocation = shooterLocs[i][k];
						if( ( ledLocation >= (sectionEnd - trailLength) ) || ( ledLocation <= (sectionStart + trailLength) ) ){
							spawnOkTest = false;
						}
					}
				}

				if( spawnOkTest && !shooterActive[i][j] ){
					//try to spawn shooter
					chance = random(100);
					//if we spawn a shooter, stop more from spawing (prevents overlapping),
					//pick a color, and set the shooter's start point, and set it to active
					if(chance <= spawnChance){
						//spawnOk[i] = false;
						shooterActive[i][j] = true;
						colors[i][j] = random(palletLength);
						//if we're going forward, start the shooter at the section's first pixel
						//otherwise we're going backwards, start it at the last pixel
						if(directions[i] == 1){
							shooterLocs[i][j] = sectionStart;
						} else {
							shooterLocs[i][j] = sectionEnd;
						}
					}
			    //if the current shooter is active, incrment it and draw trails
				} else if(shooterActive[i][j]) {
					//we're running a pixel
					ledLocation = shooterLocs[i][j];

					//if we're in streamer mode, we don't turn off any pixels, new ones just cancel old out
					if(trails < 3){
						directionArray[0] = -1 * directions[i]; //due to the way  patternSweepGetTrailLed() works, the direction array must be reversed

						//if we don't have trails, we just need to turn off the previous pixel
						//otherwise we need to switch the pixel at the end of the trail
						if(trails == 0){
							trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, 1);
							setBackgroundSingle(trailLedLocation, BgColor);
						} else {
							trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, trailLength + 1);
							setBackgroundSingle(trailLedLocation, BgColor);
						}

						//if we have trails, draw them for the trail length
						if(trails != 0 ){
							//draw a trail, extending positively or negativly along the strip, dimming with each step
							for( int k = 1 ; k <= trailLength ; k++){

								//dimming factor for the trail member, (100 is no dimming)
								dimFactor = 100 - (percentDim * k);

								//get the led location of each trail value, with wrapping (unless we're in scanner mode)
								trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, k);

								//if we're within the section bounds, draw the next trail led
								if( ( trailLedLocation <= sectionEnd ) && ( trailLedLocation >= sectionStart ) ){
									patternSweepSetPixelColor(trailLedLocation, palletCopy[ colors[i][j] ], colorMode, dimFactor);
								}
							}
						}
					}

					//draw the main body of the shooter, but
					//only if it falls within the section
					//this is done last so that tails of other particles don't overwrite
					if( ( ledLocation <= sectionEnd ) && ( ledLocation >= sectionStart ) ){
						patternSweepSetPixelColor(ledLocation, palletCopy[ colors[i][j] ], colorMode, 100);
					}

					//if a shooter has reached the end of the section, plus the trail length, the shooter is now inactive, otherwise, move the shooter one step
					//the reason why we include the trail length is to allow the trail to move off the section
					//anything that falls outside the section won't be drawn
					if( ( ledLocation > (sectionEnd + trailLength) ) || ( ledLocation < (sectionStart - trailLength) ) ){
						shooterActive[i][j] = false;
					} else {
						shooterLocs[i][j] = ledLocation + directions[i];
					}
				}
			}
		}
		show();
		delay(wait);
	}
}

//does a random set of fireworks, //the number of leds is specified, but their colors, and inital locations will be picked randomly
//b/c fireworks is basically a patternSweep, we can call patternSweepRand (with trails = 5 for fireworks), to do the setup
void PixelStrip::fireworksRand( uint8_t numRandLeds, int32_t preferedColor, int32_t BgColor, uint8_t dimFactor, uint8_t colorMode, int wait, int numCycles){
	patternSweepRand( numRandLeds, preferedColor, BgColor, 5, 3, false, dimFactor, colorMode, wait, numCycles);
}

//does a set of fireworks / ripples at points along the strip
//one firework starts as three points of light, and then turns into two seperate pattern sweept leds
//can be passed in a pallet to use or pass in 0 as the pallet length to use random colors
//the locations of the fireworks can only be random
//numRandLeds sets the number of fireworks for each cycle
//numCycles sets how many sets of fireworks to do
//wait sets the speed of the whole animation
void PixelStrip::fireworksSet( uint8_t numRandLeds, uint8_t dimFactor, uint32_t pallet[],  uint8_t palletLength, int32_t BgColor, uint8_t colorMode, int wait, int numCycles){
 //initialize a randPattern, matching the pattern structure for patternSweep(), setting the length to match the number of random leds
  uint16_t randPattern[ numRandLeds * 2 ][3];
  uint8_t dimmedPalletLength = palletLength;
  if( palletLength == 0) { dimmedPalletLength = numRandLeds; } //if we're doing random colors, we need to set the pallet length correctly
  uint32_t dimmedPallet[dimmedPalletLength];

  //Fill in the randPattern using the pallet
  //randPattern matches the pattern structure:ZAZ
  //patterns are a y x 3 matrix where y is the number of leds numRandLeds,
  //each row is specified as such { led position, led color (index of pallet color), motion direction }
  //for fireworks, pixels are composed in pairs, one going in each direction
  //nore that colorModes 2, 3 will not look as good as color mode 1 due to dimming
  for(int j = 0; j < numCycles; j++){
	  subFunct = true; //for stopPatternCheck, so we break out in the calling function too
	  stopPatternCheck();
	  stripOff();
	  for(int i = 0; i < (numRandLeds * 2); i = i + 2){
		  uint32_t fireworkColor;
		  int fireworkStart = random(1, numLEDs - 2); //pick a random starting point for each led
		  randPattern[i][0] = fireworkStart;
		  randPattern[i + 1][0] = fireworkStart;
		  if(palletLength == 1){ //pick a color index from the pallet, if the pallet is only length 1, just pick it
			randPattern[i][2] = 0;
			randPattern[i + 1][2] = 0;
			fireworkColor = pallet[0];
			dimmedPallet[0] = dimColor( fireworkColor, dimFactor );
		  } else if( palletLength == 0 ){ //set the pallet index for the local pattern, and choose a random color for it
			randPattern[i][2] = i / 2; //dimmed pallet is not double length, so we need to adjust our index
			randPattern[i + 1][2] = i / 2;
			fireworkColor = randColor();
			dimmedPallet[i / 2] = dimColor( fireworkColor, dimFactor );
		  } else {  //we're choosing from a pallet, pick a color randomly from the pallet, and dim it
			uint8_t palletIndex = random(dimmedPalletLength);
			randPattern[i][2] = palletIndex;
			randPattern[i + 1][2] = palletIndex;
			fireworkColor = pallet[palletIndex];
			dimmedPallet[palletIndex] = dimColor( fireworkColor, dimFactor );;
		  }
		  randPattern[i][1] = 0;
		  randPattern[i + 1][1] = 1;
		  setPixelColor( fireworkStart - 1 , dimColor( fireworkColor, 2 ) );
		  setPixelColor( fireworkStart , fireworkColor );
		  setPixelColor( fireworkStart + 1 , dimColor( fireworkColor, 2 ) );
	  }
	  show(); //show the initial firework pixel and wait before starting the sweep
	  delay( wait );
	  patternSweep(randPattern, numRandLeds * 2, dimmedPallet, BgColor, -1, 2, false, 0, colorMode, wait, 3);
  }
}

//draws the bgGradient across segments
void PixelStrip::fillSegBgGradientRGB(SegmentSet segmentSet){
	byte numSegs = segmentSet.numSegs;
	byte segNum;
	uint32_t color;
	uint8_t steps = numSegs / (bgPalletStripSize - 1);

	//for each bgColor, (-1 because we dont want to start crossfading the last color)
	//crossfade towards the next color in the BgPallet
	for(int i = 0; i < (bgPalletStripSize - 1); i++){
		for(int j = 0; j <= steps; j++){
			color = getCrossFadeStep( bgPalletStrip[i], bgPalletStrip[ (i + 1) % bgPalletStripSize], steps, j);
			segNum = (i * steps + j); //how many colors we've gone through plus the step we're on
			if(segNum < numSegs){ //the last step will always runover the segments, but is needed to get to the final color
				fillSegColor(segmentSet, segNum, color);
			}
		}
	}
}

//returns the segmented Bg gradient color for the specified segment based on what's in the bgGradient
uint32_t PixelStrip::getSegBgGradientColor(SegmentSet segmentSet, uint16_t segNum){
	byte numSegs = segmentSet.numSegs;
	uint32_t color;
	uint8_t steps = numSegs / (bgPalletStripSize - 1);
	int i = 0;
	int j = 0;
	while( ( i + 1) * steps < segNum){
		i++;
	}
	j = segNum - (i * steps);
	return getCrossFadeStep( bgPalletStrip[i], bgPalletStrip[ (i + 1) % bgPalletStripSize], steps, j);
}

//does a wave using the bgGradient as the pattern of colors
void PixelStrip::segGradientWaves(SegmentSet segmentSet, uint16_t numCycles, boolean inToOut, int wait, int steps){
	byte numSegs = segmentSet.numSegs;
	uint32_t pallet[numSegs];
	byte pattern[numSegs];

	for (int i = 0; i < numSegs; i++){
		pattern[i] = i;
		pallet[i] = getSegBgGradientColor(segmentSet, i);
	}

	waves(segmentSet, pallet, numSegs, pattern, numSegs, numCycles, inToOut, wait, steps);
}

//cycles a random set of color gradients across segments
//numColors set how many colors to cycle
//gradSteps is how long each gradient transition is; ie 4 will mean colors transition across 4 segments
//typically, it will look best if you have a transition that is longer than the number of segs, with a low wait time
//patterns which don't fully fit on the segments will still work, colors will be cycled on
//inToOut sets the direction of the cycles, either starting at the last segment or the first
//type sets the type of gradient function, 1 will use segGradientCycle(), 2 will use segGradientCycleSweep()
void PixelStrip::segGradientCycleRand(SegmentSet segmentSet, byte numColors, byte gradSteps, uint16_t numCycles, boolean inToOut, uint8_t type, int wait){

	uint32_t pallet[ numColors ];
	byte pattern[ numColors ];

	for(int i = 0; i < numColors; i++){
		pattern[i] = i;
		pallet[i] = randColor();
	}
	if(type == 1){
		segGradientCycle(segmentSet, pattern, numColors, pallet, numColors, gradSteps, numCycles, inToOut, wait);
	} else {
		segGradientCycleSweep(segmentSet, pattern, numColors, pallet, numColors, gradSteps, numCycles, inToOut, wait);
	}
}

//cycles a gradients across segments
//pattern an array of pallet color indecies, they are drawn in order of the pattern with wrapping
//pallet is an array of color values
//gradSteps is how long each gradient transition is; ie 4 will mean colors transition across 4 segments
//typically, it will look best if you have a transition that is longer than the number of segs, with a low wait time
//patterns which don't fully fit on the segments will still work, colors will be cycled on
//inToOut sets the direction of the cycles, either starting at the last segment or the first
void PixelStrip::segGradientCycle(SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean inToOut, int wait){
	byte numSegs = segmentSet.numSegs;
	uint32_t segColor;
	uint8_t nextPattern, currentPattern, nextSeg;
	uint8_t prevBrightness = getBrightness(); //need to save the current brightness
	int16_t firstRealPixel[numSegs];
	//get the first non-dummy pixel of each segment (dummy pixels are always -1)
	for(int i = 0; i < numSegs; i++){
		for(int j =0; j < segmentSet.getTotalSegLength(i); j++){
			if( getSegmentPixel(segmentSet, i, j) >= 0){
				firstRealPixel[i] = getSegmentPixel(segmentSet, i, j);
				break;
			}
		}
	}

	//loop driection variables (default is last segment to first)
	int8_t endLimit = numSegs;
	int8_t startLimit = 0;
	int8_t loopStep = 1;
	int8_t firstSegment =  numSegs - 1;

	//switch the loop variables if we want first segment to last
	if(!inToOut){
		endLimit = -1;
		startLimit = numSegs - 1;
		loopStep = -1;
		firstSegment = 0;
	}

	RGB palletRGB[palletLength]; //need to make an RGB pallet for getCrossFadeStep

	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);

	uint16_t patternCount;	//tracks what how many patterns we've gone through

	//inital pattern indecies
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//we need to draw the initial gradients on the segments
	//to do this we run across all the segments
	//every gradsteps number of segments, we rotate the gradient colors,
	//transitioning from the current color to the next
    for(int i = 0; i < numSegs; i++ ){

		//if we've gone through gradSteps cycles
		//a color transition is finished and we need to move to the next color
		if(i % gradSteps == 0){
			patternCount++;
			currentPattern = nextPattern;
			nextPattern = pattern[ patternCount % patternLength ];
		}

		//get the crossfade between the current and next color, where the transition is gradsteps long
		segColor = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], gradSteps, ( i % gradSteps ) );
		fillSegColor(segmentSet, i, segColor);
	}

	show();
	delay(wait);

	//initialize loop variables for main cycle
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//the main cycle
	//colors the outer or inner segment (depending on direction) to a transition color
	//between the two active colors (currentPattern, nextPattern)
	//while also shifting the previous outer or inner color inwards or outwards respecitivly
	//every gradSteps we switch to the next color
	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		//if we've gone through gradSteps cycles
		//a color transition is finished and we need to move to the next color
		if(i % gradSteps == 0){
			patternCount++;
			currentPattern = nextPattern;
			nextPattern = pattern[ patternCount % patternLength ];
		}
		prevBrightness = getBrightness();
		//we need to use getPixelColor(), which only returns the correct color at full brightness
		setBrightness(255);

		//cycle through the segments, if we're at the outer or inner segment (depen on direction)
		//set it to the next transistion step, otherwise just copy the color of the previous segment
		for(int j = startLimit; j != endLimit; j += loopStep){
			if(j == firstSegment){
				segColor = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], gradSteps, ( i % gradSteps ) );
			} else {
				//get color of first pixel in segment, all segment pixels are the same color

				segColor = getPixelColor(  firstRealPixel[j + loopStep] );  //segments[ segGroups[ j + loopStep ][0] ][1]
			}
			fillSegColor(segmentSet, j, segColor);
		}

		//reset the brighness and show the strip
		setBrightness(prevBrightness);

		show();
		delay(wait);
	}
}

//CURRENTLY DOESN'T WORK
//A different implmentation of gradientCycle() trying to avoid using getPixelColor()
//doesn't currently work, it can't assign the next color correctly
//the gradient uses the same inputs as segGradientCycle
void PixelStrip::segGradientCycle2(SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean inToOut, int wait){
	byte numSegs = segmentSet.numSegs;
	uint32_t segColor;
	uint8_t nextPattern, currentPattern, nextSeg;
	uint8_t prevBrightness = getBrightness(); //need to save the current brightness

	//loop driection variables (default is last segment to first)
	int8_t endLimit = numSegs;
	int8_t startLimit = 0;
	int8_t loopStep = 1;
	int8_t firstSegment =  numSegs - 1;

	//switch the loop variables if we want first segment to last
	if(!inToOut){
		endLimit = -1;
		startLimit = numSegs - 1;
		loopStep = -1;
		firstSegment = 0;
	}

	RGB palletRGB[palletLength]; //need to make an RGB pallet for getCrossFadeStep

	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);

	uint16_t patternCount;	//tracks what how many patterns we've gone through

	//inital pattern indecies
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//used for drawing the inital gradient transitions
	//determines how many gradients can fit across the segements
    int numRepeats = ceil( float(numSegs) / gradSteps);

	//initialize loop variables for main cycle
	patternCount = 1; //pattern count must start at 1 b/c we've already drawn the first gradient set
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	byte colorPairs[numSegs][2];
	uint16_t offSet[numSegs];

	for(int i = 0; i < numSegs; i++ ){
		colorPairs[i][0] = pattern[0];
		colorPairs[i][1] = pattern[1];
		offSet[i] = i;
	}


	//the main cycle
	//colors the outer or inner segment (depending on direction) to a transition color
	//between the two active colors (currentPattern, nextPattern)
	//while also shifting the previous outer or inner color inwards or outwards respecitivly
	//every gradSteps we switch to the next color
	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		for(int j = 0; j < numSegs; j++){
			//if we've gone through gradSteps cycles
			//a color transition is finished and we need to move to the next color
			if(i != 0) {
				if( ( offSet[j] + i ) % gradSteps == 0){
					colorPairs[j][0] = colorPairs[j][1];
					colorPairs[j][1] = pattern[( (i + offSet[j]) / gradSteps ) % patternLength];
				}
			}

			segColor = getCrossFadeStep( palletRGB[ colorPairs[j][0] ], palletRGB[ colorPairs[j][1] ], gradSteps, ( (offSet[j] + i) % gradSteps ) );
			fillSegColor(segmentSet, j, segColor);
		}
		show();
		delay(wait);

	}
}

//WARNING, does not support dummy pixels b/c the function must get the color of each pixel to set the next
//does a gradient sweep on the longest segment, and maps it to the other segments,
//so you should get a gradient sweep across all segments at the same time
void PixelStrip::segGradientCycleSweep(SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], byte palletLength, byte gradSteps, uint16_t numCycles, boolean inToOut, int wait){
	uint16_t maxSegLength = segmentSet.maxSegLength;
	uint8_t segNumMaxSegLength = segmentSet.segNumMaxSegLength;
	uint32_t pixelColor;
	uint16_t pixel;
	uint8_t nextPattern, currentPattern, nextSeg;
	uint8_t prevBrightness = getBrightness(); //need to save the current brightness

	//loop driection variables (default is last segment to first)
	int8_t endLimit = maxSegLength;
	int8_t startLimit = 0;
	int8_t loopStep = 1;
	int8_t firstPixel = maxSegLength - 1;
	int8_t firstPixelOffset;


	//switch the loop variables if we want first segment to last
	if(!inToOut){
		endLimit = -1;
		startLimit = maxSegLength - 1;
		loopStep = -1;
		firstPixel = 0;
	}

	RGB palletRGB[palletLength]; //need to make an RGB pallet for getCrossFadeStep

	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);

	uint16_t patternCount;	//tracks what how many patterns we've gone through

	//inital pattern indecies
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//we need to draw the initial gradients on the segments
	//to do this we run across the longest segment, and drawSegLines to color the other segments
	//every gradsteps number of pixels, we rotate the gradient colors,
	//transitioning from the current color to the next
	for(int i = 0; i < maxSegLength; i++){

		//if we've gone through gradSteps cycles
		//a color transition is finished and we need to move to the next color
		if(i % gradSteps == 0){
			patternCount++;
			currentPattern = nextPattern;
			nextPattern = pattern[ patternCount % patternLength ];
		}

		//get the crossfade between the current and next color, where the transition is gradsteps long
		pixelColor = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], gradSteps, ( i % gradSteps ) );
		//draw a line across all of the segments at the ith line number
		//color mode 1 b/c we are only ever drawing a solid color
		drawSegLineSimple(segmentSet, i, pixelColor, 1);

	}

	show();
	delay(wait);

	//initialize loop variables for main cycle
	nextPattern = pattern[ 1 ];
	currentPattern = pattern[ 0 ];

	//the main cycle
	//colors the last or first segment line (depending on direction) to a transition color
	//between the two active colors (currentPattern, nextPattern)
	//while also shifting the previous line's color to the next line
	//every gradSteps we switch to the next color
	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		//if we've gone through gradSteps cycles
		//a color transition is finished and we need to move to the next color
		if(i % gradSteps == 0){
			patternCount++;
			currentPattern = nextPattern;
			nextPattern = pattern[ patternCount % patternLength ];
		}

		prevBrightness = getBrightness();
		//we need to use getPixelColor(), which only returns the correct color at full brightness
		setBrightness(255);

		//cycle through the max length segment, if we're at the first of last pixel (dep on direction)
		//set it to the next transistion step, otherwise just copy the color of the previous pixel
		for(int j = startLimit; j != endLimit; j += loopStep){
			if(j == firstPixel ){
				pixelColor = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], gradSteps, ( i % gradSteps ) );
			} else {
				//get color of previous pixel (search past dummy pixels)
				pixel = getSegmentPixel(segmentSet, segNumMaxSegLength, j + loopStep);
				pixelColor = getPixelColor( pixel );
			}
			drawSegLineSimple(segmentSet, j, pixelColor, 1);
		}

		//reset the brighness and show the strip
		setBrightness(prevBrightness);

		show();
		delay(wait);
	}
}

//fills a segment with a specific color
void PixelStrip::fillSegColor(SegmentSet segmentSet, byte segNum, uint32_t color){
	uint16_t startPixel;
	int16_t length;
	int step;
	byte numSecs = segmentSet.getTotalNumSec(segNum);
	//run through segment's sections, fetch the startPixel and length of each, then color each pixel
	for(int i = 0; i < numSecs; i++){
		startPixel = segmentSet.getSecStartPixel(segNum, i);
		length = segmentSet.getSecLength(segNum, i);
		step = (length > 0) - (length < 0); //acount for negative lengths
		for (int j = startPixel; j != (startPixel + length); j += step ) {
		  setPixelColor( j, color );
		}
	}
}

//spawns shooters along segment of the strip, a shooter is like a patternSweep, but it terminates when it gets to the end of a segment
//the colors of shooters will be choosen randomly from the pallet
//passing -1 as BgColor will set it to rainbow, -3 sets to Bg gradient
//spawnChance is the percentage chance to spawn a shooter on each cycle
//maxNumShooters is the maximum number of shooters that can be active in one section at one time
//trails:
		//0: no trails
		//1: one trail extending behind pixel
		//3: infinite trails (trails are not dimmed or removed unless passed over by another pixel)
//trailLength sets how long trails are
//colorMode determines what colors are used for the pixels
	//mode 1: use the pallet for the colors
	//mode 2: use a rainbow spread across the strip for colors
	//mode 3: use the BgGradient for colors
	//mode 4: use a random pallet for the colors, number of colors will be the palletLength
//direction set the direction shooters move down the sections, true will move positivly, false will move negativly
//alternating will alternate the dirction for each subsequent section
//wait is how fast the shooters move
//numCycles is total number of cycles the system will run
void PixelStrip::shooterSeg( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, int32_t BgColor, uint8_t spawnChance, byte maxNumShooters, int trails, uint8_t trailLength, uint8_t colorMode, boolean direction, boolean alternating, int wait, int numCycles){
	byte numSegs = segmentSet.numSegs;
	int16_t shooterLocs[numSegs][maxNumShooters]; //locations of shooters
	int8_t directions[numSegs]; //the step rate for each section, ie -1 or 1 determined by the direction variable and alternating
	uint8_t colors[numSegs][maxNumShooters]; //holds a color pallet color index for each of the sections
	//boolean spawnOk[numSegs]; //if true, then the section is not at capacity for shooters
	boolean shooterActive[numSegs][maxNumShooters]; //records if a shooter is active in a section
	//boolean displayShooter[numSegs][maxNumShooters]; //prototype to force leds into random starting postions (but not draw them until they reset once)
	int8_t directionArray[2] = {0,0}; //used for patternSweepGetTrailLed();
	boolean spawnOkTest = true; //if true, then a shooter and its trail are not within the spawn zone
	byte totalShootersActive = 0;

	int16_t ledLocation, trailLedLocation; //location of the front of the current shooter, location of trail leds
	int16_t sectionEnd, sectionStart; //end point of a given section, the start point of a given section, both are int16_t's b/c they can be subtracted from
	int16_t ledSegLoc; //actual location of the led on the segment that we want to turn on / off
	//We dont want the trails to wrap into other sections,
	//so we want to keep the mod larger than the ledLocation can reach
	//ie the numLEDs plus the tailLength, as this is will allow the tail to extend off the section,
	//which will be discarded
	int16_t modAmmount;

	uint8_t chance; //stores random roll for spawning

	ledLocation = 0;
	

	//used for generating a random pallet, we don't want to modify the passed in one
	//as a rule if color mode is 4 we generate a random pallet of length palletLength
	//then we set color mode to 1 to pick randomly from the pallet
	uint32_t palletCopy[palletLength];
	if(colorMode == 4){
		colorMode = 1;
		genRandPallet(palletCopy, palletLength);
	} else {
		for(int i = 0; i < palletLength; i++){
			palletCopy[i] = pallet[i];
		}
	}

	//ensure that if we have no trails, we have 0 trail length
	if(trails == 0 || trails == 3){
		trailLength = 0;
	}

	//run through the number of sections
	//set all spawns to true, and activeShooters to false
	//also setup direction array, if we're alternating flip the direction for each section
	for(int i = 0; i < numSegs; i++){
		//spawnOk[i] = true;
		for(int j = 0; j < maxNumShooters; j++){
			shooterActive[i][j] = false;
			//used to garrentee even spacing of leds (uncomment displayShooter lines for this to work)
			//shooterLocs[i][j] = random( segmentSet.getTotalSegLength(i)); //-directions[i]*(random(5* maxNumShooters) + trailLength);
			//displayShooter[i][j] = true;
		}
		if(direction){
			directions[i] = 1;
		} else {
			directions[i] = -1;
		}
		if(alternating){
			direction = !direction;
		}
	}

	//fill the segments with the BgColor, if we're streaming turn them off (BgColor will be covered up)
	if(trails > 2) {
		BgColor = 0;
	}
	for(int i = 0; i < numSegs; i++){
		for(int j = 0; j < segmentSet.getTotalSegLength(i); j++){
			ledSegLoc = getSegmentPixel(segmentSet, i, j);
			setBackgroundSingle(ledSegLoc, BgColor);
		}
	}

	byte percentDim = 100 / (trailLength + 1); //dim percentage so that the trail almost reaches total dimming at the trail end (+1 prevents it being off)
	uint8_t dimFactor = 100; //dynamic dim factor for creating trail

	//run the system for a certain number of cycles
	//for each cycle, if a shooter can spawn, try to spawn one,
    //if a shooter is active, move it to its next positions
	//then sets the leds at the previous location to the BgColor (accouting for trails)
	//then draws new trails if needed
	//then draws the new front leds
	//this way we never overwrite front leds or trails with the BgColor
	for(int k = 0; k <= numCycles; k++){
		//randomSeed(random(100));
		stopPatternCheck();
		yield();
		//for each section check if we can spawn a shooter, if so try to spawn one
		//a shooter can only spawn if the max number of shooters hasn't been reached, and if the shooter isn't active
		//if not, keep drawing the current shooter
		for(int i = 0; i < numSegs; i++){
			sectionEnd = segmentSet.getTotalSegLength(i) - 1;
			sectionStart = 0;
			modAmmount = segmentSet.getTotalSegLength(i) + trailLength + 1;
			for(int j = 0; j < maxNumShooters; j++){

				//if all the shooters trails have passed the end/start of the section (depending on direction), then we can
			    //spawn a new shooter
				spawnOkTest = true;
				for(int k = 0; k < maxNumShooters; k++){
					if(shooterActive[i][k]){
						ledLocation = shooterLocs[i][k];
						if( ( ledLocation >= (sectionEnd - trailLength) && directions[i] == -1 ) || ( ledLocation <= (sectionStart + trailLength) && directions[i] == 1 ) ){
							spawnOkTest = false;
							break;
						}
					}
				}

				//if no trails are in the spawn area, and the shooter isn't active, try to spawn a new one
				if( spawnOkTest && !shooterActive[i][j] ){
					//try to spawn shooter
					chance = random(100);
					//if we spawn a shooter, stop more from spawing (prevents overlapping),
					//pick a color, and set the shooter's start point, and set it to active
					if(chance <= spawnChance){
						//spawnOk[i] = false;
						shooterActive[i][j] = true;
						totalShootersActive++;
						colors[i][j] = random(palletLength);
						//if we're going forward, start the shooter at the section's first pixel
						//otherwise we're going backwards, start it at the last pixel
						if(directions[i] == 1){
							shooterLocs[i][j] = sectionStart;
						} else {
							shooterLocs[i][j] = sectionEnd;
						}
					}
			    //if the shooter is acive, we need to increment it and draw trails
				} else if(shooterActive[i][j]) {

					ledLocation = shooterLocs[i][j];
					//if(displayShooter[i][j]){
					//if we're in streamer mode, we don't turn off any pixels, new ones just cancel old out
					if(trails < 3){
						directionArray[0] = - directions[i]; //due to the way  patternSweepGetTrailLed() works, the direction array must be reversed

						//if we don't have trails, we just need to turn off the previous pixel
						//otherwise we need to switch the pixel at the end of the trail
						if(trails == 0){
							//patternSweepGetTrailLed(int8_t directionArray[2], uint16_t modAmmount, int16_t ledLocation, int8_t distance)
							trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, 1);
							ledSegLoc = getSegmentPixel(segmentSet, i, trailLedLocation);
							setBackgroundSingle(ledSegLoc, BgColor);
						} else {
							trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, trailLength + 1);
							ledSegLoc = getSegmentPixel(segmentSet, i, trailLedLocation);
							setBackgroundSingle(ledSegLoc, BgColor);
						}

						//if we have trails, draw them for the trail length
						if(trails != 0 ){
							//draw a trail, extending positively or negativly along the strip, dimming with each step
							for( int k = 1 ; k <= trailLength ; k++){

								//get the led location of each trail value, with wrapping (unless we're in scanner mode)
								trailLedLocation = patternSweepGetTrailLed(directionArray, modAmmount, ledLocation, k);

								//if we're within the section bounds, draw the next trail led
								if( ( trailLedLocation <= sectionEnd ) && ( trailLedLocation >= sectionStart ) ){
									//dimming factor for the trail member, (100 is no dimming)
									dimFactor = 100 - (percentDim * k);
									ledSegLoc = getSegmentPixel(segmentSet, i, trailLedLocation);
									patternSweepSetPixelColor(ledSegLoc, palletCopy[ colors[i][j] ], colorMode, dimFactor);
								}
							}
						}
					}
				

					//draw the main body of the shooter, but
					//only if it falls within the section
					//this is done last so that tails of other particles don't overwrite
					if( ( ledLocation <= sectionEnd ) && ( ledLocation >= sectionStart ) ){
						ledSegLoc = getSegmentPixel(segmentSet, i, ledLocation);
						patternSweepSetPixelColor(ledSegLoc, palletCopy[ colors[i][j] ], colorMode, 100);
					}
					//} //wrapper for commented displayShooter check above

					//if a shooter has reached the end of the section, plus the trail length, the shooter is now inactive, otherwise, move the shooter one step
					//the reason why we include the trail length is to allow the trail to move off the section
					//anything that falls outside the section won't be drawn
					if( ( ledLocation > (sectionEnd + trailLength) ) || ( ledLocation < (sectionStart - trailLength) ) ){
						shooterActive[i][j] = false;
						//displayShooter[i][j] = true;
						totalShootersActive--;
					} else {
						shooterLocs[i][j] = ledLocation + directions[i];
					}
				}
			}
		}
		show();
		delay(wait);
	}
}


//crossfades segments of a strip one after another through a set of colors for a certain number of cycles, works best for ring, square, or other shapes of pixels
//see other segment functions for notes on segments
//the pattern is the colors that the segments will shift through, referencing the pallet ex: wavepattern[6]  = { 6, 1, 6, 1, 6, 1 };, would shift the segments
//between pallet[1] and pallet[6]. Patterns can be as short or as long as you like, they will repeat
//inToOut indicates whether the waves shift from the last segment to first, or visa versa.
//BgColor sets the color of the omissions, passing -1 will set them to a rainbow
void PixelStrip::waves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint16_t numCycles, boolean inToOut, uint16_t wait, uint8_t steps){
    byte numSegs = segmentSet.numSegs;
	RGB palletRGB[palletLength];

	uint32_t segColor;
	int8_t nextPattern, currentPattern;

	//creates an RGB pallet from the passed in one
	//note that it converts all colors in the pallet
	//so watch your memory useage if you've pass in a big pallet
	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);

	uint8_t cycleStart = numCycles;
	uint8_t cycleEnd = 0;
	int cycleStep = -1;

	if(inToOut){
		cycleStart = 0;
		cycleEnd = numCycles;
		cycleStep = 1;
	}

	//for the total number of cycles, crossfade each segment to the next color in steps
	for (int k = cycleStart; k != cycleEnd; k += cycleStep) {
		stopPatternCheck();
	    yield();
		for(int l = 0; l <= steps; l++){
			for (int i = 0; i < numSegs; i++) {
				nextPattern = pattern[ (i + k + cycleStep) % patternLength ];
				currentPattern = pattern[ (i + k) % patternLength ];
				segColor = getCrossFadeStep( palletRGB[ currentPattern ], palletRGB[ nextPattern ], steps, l);  //select color
				fillSegColor(segmentSet, i, segColor);
			}
			show();
			delay(wait);
		}
    }

}

//generates a random set of colors of length numColors, and passes this as a pallet/pattern to waves();\
//numColors can the greater than numSegs, the new colors will shift in
//waves will be waveThickness thick, and be seperated by spacing length
//the color of the spacing is currently always blank
//see waves for descriptions of other variables
void PixelStrip::randomWaves(SegmentSet segmentSet, byte numColors, byte waveThickness, byte spacing, byte numCycles, boolean inToOut, byte wait, byte steps){
	uint32_t pallet[numColors];

	for(int i = 0; i < numColors; i++ ){
		pallet[i] = randColor();
	}

	simpleWaves(segmentSet, pallet, numColors, waveThickness, spacing, 0, numCycles, inToOut, wait, steps);
}

//builds and runs a wave based on a pallet
//will use every color in the pallet to create waves of each color of waveThickness thick, and with spacing length in between each wave
//the spacing will be set to the BgColor
void PixelStrip::simpleWaves(SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte waveThickness, byte spacing, uint32_t BgColor, byte numCycles, boolean inToOut, byte wait, byte steps){
	byte waveSize = waveThickness + spacing;
	byte totalPatternLength = palletLength * (waveSize);
	byte pattern[totalPatternLength]; //pattern for waves()
	byte currentPatternIndex = 0;

	//waves() just takes a pallet, so we need to make a new pallet that includes the BgColor
	//the BgColor is placed at the end of the pallet
	uint32_t palletWithBg[palletLength + 1];
	for(int i = 0; i < palletLength; i++){
		palletWithBg[i] = pallet[i];
	}
	palletWithBg[palletLength] = BgColor;

	//we need to fill in the pattern
	//run through the pallet, and for each color, fill in a wavethickness portion of the pattern
	//and then a spacing thickness portion of the pattern with the BgColor
	//everytime we fill in part of the pattern, we advance currentPatternIndex
	for(int i = 0; i < palletLength; i++ ){
		for(int j = 0; j < waveThickness ; j++){
			pattern[currentPatternIndex] = i;
			currentPatternIndex++;
		}

		for(int k = 0; k < spacing ; k++){
			pattern[currentPatternIndex] = palletLength;
			currentPatternIndex++;
		}
	}

	waves( segmentSet, palletWithBg, palletLength + 1, pattern, totalPatternLength, numCycles, inToOut, wait, steps);
}

//does a similar thing to waves(), but does one wave of a certain thickness
//basically it builds a wave pattern and then runs it
//waveThickness sets the number of segments illuminated at once, (useful on pixels arranged in shapes)
//the BgColor in this function sets the color of the pixels that fill in the anything not omitted, you can only do solid colors for this
void PixelStrip::singleWave( SegmentSet segmentSet, uint32_t waveColor, uint32_t BgColor, uint8_t waveThickness, uint8_t numCycles, boolean inToOut, uint8_t wait, uint8_t steps) {
  byte numSegs = segmentSet.numSegs;
  byte pattern[numSegs];
  for (int i = 0; i < numSegs; i++) {
	  pattern[i] = 0;
    if (i >= numSegs - waveThickness) {
      pattern[i] = 1;
    }
  }
  uint32_t pallet[2] = {BgColor, waveColor};
  waves( segmentSet, pallet, 2, pattern, numSegs, numCycles, inToOut, wait, steps );
 }

//a combination of rainbow and wave functions, builds a rainbow pattern based on the number of segments and then cycles it
void PixelStrip::rainbowWave( SegmentSet segmentSet, uint8_t numCycles, boolean inToOut, uint8_t wait, uint8_t steps) {
  byte numSegs = segmentSet.numSegs;
  byte pattern[numSegs];
  uint32_t pallet[numSegs];
  for (int i = 0; i < numSegs; i++) {
	 pattern[i] = i;
	 pallet[i] = Wheel(((i * 256 / numSegs)) & 255);
  }
  waves( segmentSet, pallet, numSegs, pattern, numSegs, numCycles, inToOut, wait, steps );
 }

//does a similar thing to waves(), but cycles through the rainbow
//waveThickness sets the number of segments illuminated at once, (useful on pixels arranged in shapes)
//shiftFactor determines how fast you go step through the rainbow
//modes: 0 segments move from out to in
//       1 segments move from in to out
//       2 segments alterntate between in to out and out to in
void PixelStrip::rainbowSingleWave( SegmentSet segmentSet, uint8_t waveThickness, uint8_t shiftFactor, uint8_t mode, uint8_t wait, uint8_t steps) {
  byte numSegs = segmentSet.numSegs;
  boolean inToOut = mode;
  byte pattern[numSegs];
  for (int i = 0; i < numSegs; i++) {
    pattern[i] = 0;
    if (i >= numSegs - waveThickness) {
      pattern[i] = 1;
    }
  }
  uint32_t pallet[2] = {0, 1};
  for (int j = 0; j < 256;) {
    pallet[1] = Wheel( j & 255);
    //pallet[0] = Wheel( -1 * ( (j % 256 ) - (255) ) );
    waves( segmentSet, pallet, 2, pattern, numSegs, (numSegs - waveThickness), inToOut, wait, steps );
    if ( mode == 2) {
      inToOut = !inToOut;
    }
    j = j + shiftFactor;
  }
}

//does a set of fading waves with random colors
 void PixelStrip::fadingWavesRand( SegmentSet segmentSet, uint8_t numColors, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, int numCycles, uint8_t wait, int steps){
	 uint32_t pallet[numColors];
	 byte pattern[numColors];
	 for(int i = 0; i < numColors; i++){
		 pallet[i] = randColor();
		 pattern[i] = i;
	 }
	 fadingWaves(segmentSet, pallet, numColors, pattern, numColors, totalFadeLength, spacing, inToOut, numCycles, wait, steps);
 }

 //does a set of waves where the color fades in fadelength steps
 //this is fairly memory intensive, as a color is created for each fade step, for each color in the pattern
 //for a less memory intensive alternative, use sonarWaves
void PixelStrip::fadingWaves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint8_t totalFadeLength, uint8_t spacing, boolean direct, int numCycles, uint8_t wait, int steps){
	byte numSegs = segmentSet.numSegs;
	int numColors = totalFadeLength * patternLength;
	int fadeAndSpacingLength = (totalFadeLength + spacing);
	int totalPatternLength = patternLength * fadeAndSpacingLength;
	uint32_t wavePallet[numColors + 1];
	wavePallet[numColors] = 0;
	uint8_t wavePattern[totalPatternLength];
	int adjIndex = 0;

	for(int i = 0; i < patternLength; i++){
		for(int j = 0; j < totalFadeLength; j++){
			adjIndex = j;
			//if we're going from the last segment to the first, we need to draw the fade in the reverse direction
			if(direct){ adjIndex = totalFadeLength - 1 - j;}
			wavePallet[i * totalFadeLength + j] = desaturate( pallet[pattern[i]] , (adjIndex * 100) / (totalFadeLength - 1) );// getCrossFadeStep( palletRGB[ pattern[i] ], BgColorRGB, totalFadeLength -1, j);//;
			wavePattern[i * fadeAndSpacingLength + j] = i * totalFadeLength + j;
		}
		for(int j = totalFadeLength; j < fadeAndSpacingLength; j++){
			wavePattern[i * fadeAndSpacingLength + j] = numColors;
		}
	}

	waves( segmentSet, wavePallet, numColors + 1, wavePattern, totalPatternLength, numCycles, direct, wait, steps );
}

//does a set of sonarWaves with random colors
 void PixelStrip::sonarWavesRand( SegmentSet segmentSet, uint8_t numColors, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait){
	 uint32_t pallet[numColors];
	 byte pattern[numColors];
	 for(int i = 0; i < numColors; i++){
		 pallet[i] = randColor();
		 pattern[i] = i;
	 }
	 sonarWaves( segmentSet, pallet, numColors, pattern, numColors, BgColor, totalFadeLength, spacing, inToOut, alternate, bounce, bounceChange, numCycles, wait);
 }

 //does a set of waves where the color fades in fadelength steps
 //takes a set of colors in a pattern (using pallet as the colors) and will do a wave of each color in order
 //as many waves as possible will be fit onto the segmentSet at once,
 //you can control how long each wave is using the totalFadeLength (includes head and tail) and the spacing (between waves)
 //direct sets the direction of the waves (true => waves move from first segment to last)
 //setting alternate true will swap the direction of every other wave
 //setting bounce true will cause each wave to reverse direction at the final/first segment, instead of wraping
 //bounce change sets when a wave's color changes (true => every bounce, false => every full cycle)
void PixelStrip::sonarWaves( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean direct, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait){
	byte numSegs = segmentSet.numSegs;
	uint8_t numSimultWaves = ceil( (float)numSegs / (totalFadeLength + spacing) ); //number of waves that can fit on the segmentSet, rounded up
	int8_t activeWavePos[numSimultWaves][3]; //pallet, wave head location, wave direction increment; for how ever many waves are active at once
	int8_t startDirect[numSimultWaves]; //initial starting direction for a wave (used for changing colors when bouncing)
	int8_t wavePos; //marker for tracking trail location
	int8_t totalSpace = (numSimultWaves * (totalFadeLength + spacing)); //total space taken up by all the active waves (can be > numSegs)
	uint8_t nextPalletIndex; //placeholder for next color for a wave
	int kTemp = 0;

	//populate activeWavePos matrix array for the initally active waves
	for(int i = 0; i < numSimultWaves; i++){
		activeWavePos[i][0] = i % palletLength; //wave color
		//set vars depending on if we're going forward or backward, or alternating
		if(direct || (alternate && (i % 2 != 0))){
			//place the head of the waves, starting at the end of the segmentSet, we need to draw the first wave part way along the segments
			activeWavePos[i][1] = totalSpace - 1 - i * (totalFadeLength + spacing);
			activeWavePos[i][2] = 1; //set direction increment positive
			startDirect[i] = 1; //initial wave direction
			if(numSimultWaves == 1){ //if we just have one wave, we'll start it at the beginning of the segments for looks
				activeWavePos[i][1] = 0;
			}
		} else {
			activeWavePos[i][1] = i * (totalFadeLength + spacing);
			activeWavePos[i][2] = -1;//set direction increment positive
			startDirect[i] = -1; //initial wave direction
			if(numSimultWaves == 1){ //if we just have one wave, we'll start it at the end of the segments for looks
				activeWavePos[i][1] = numSegs - 1;
			}
		}
	}

	RGB palletRGB[palletLength];
	RGB BgColorRGB = makeRGBFromColor(BgColor);

	uint32_t segColor;

	//creates an RGB pallet from the passed in one
	//note that it converts all colors in the pallet
	//so watch your memory useage if you've pass in a big pallet
	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);


	//for eqch cycle, draw all the waves and fades, then determine the next wave position change color if needed
	for (int i = 0; i <= numCycles; i++) {
		stopPatternCheck();
	    yield();
		//if we're bouncing, then the trails are not touched after a bounce b/c the wave changes direction
		//this slowly fades the remaing trails
		if(bounce){
			dimStrip();
		}
			//for each wave, draw the trail, increment the wave head postion, and change colors if needed
			for (int j = 0; j < numSimultWaves; j++) {
				for(int k = 0; k < totalFadeLength; k++){
					wavePos = (activeWavePos[j][1] - k * activeWavePos[j][2]);
					if(!bounce){
						wavePos = (totalSpace + wavePos) % totalSpace ; //wrap the trail if we're not bouncing, fixing for negative case
				    }
					//the trail can be drawn off the segments, skip these cases
					if(wavePos > numSegs - 1 || wavePos < 0){
						continue;
				    }
					//fill the segment with a faded trail (totalFadeLength - 1, makes the color do to the BgColor on the last step)
					//kTemp = k;
					//if(k > 0 ){ kTemp++; }
					//segColor = desaturate( pallet[ activeWavePos[j][0] ] , (100 - k * 100) / (totalFadeLength -1) );
				    segColor = getCrossFadeStep( palletRGB[ activeWavePos[j][0] ], BgColorRGB, totalFadeLength - 1, k);
					if( k > 0 ){ segColor = dimColor(segColor, 1); }; //real quick, dirty way to make the trails dim more dramatically, replace this with HSV dimming later
					fillSegColor(segmentSet, wavePos, segColor);
				}
				nextPalletIndex = (activeWavePos[j][0] + numSimultWaves) % palletLength; //get the next color ready (skip forward by numSimultWaves, so stop repeating colors)
				activeWavePos[j][1] = activeWavePos[j][1] + activeWavePos[j][2]; //get the next wave head position
				//if bounce, check to change direction / color, otherwise check for color change
				if(bounce){
					//if we're going forward, and we hit the last segment, reverse the wave direction
					if(activeWavePos[j][1] > numSegs - 1 && activeWavePos[j][2] == 1){
						activeWavePos[j][2] = -1;
						//if the wave started at then last segment, change it's color
						if(startDirect[j] == -1 || bounceChange){
						   activeWavePos[j][0] = nextPalletIndex;
						}
						//if we're going backward, and we hit the first segment, reverse the wave direction
					} else if(activeWavePos[j][1] < 0 && activeWavePos[j][2] == -1){
						activeWavePos[j][2] = 1;
						//if the wave started at then first segment, change it's color
						if(startDirect[j] == 1 || bounceChange){
						   activeWavePos[j][0] = nextPalletIndex;
						}
					}
				} else{
					activeWavePos[j][1] = ( totalSpace + activeWavePos[j][1]) % totalSpace; //wrap the head position if we're not bouncing
					//if were moving forward, and hit totalSpace, or if we're going backward and hit the first segment, change colors
					if( ( direct && (activeWavePos[j][1] == totalSpace -1) ) || ( !direct && (activeWavePos[j][1] == 0 ) ) ){
						activeWavePos[j][0] = nextPalletIndex;
					}
				}
			}
		show();
		delay(wait);
    }
}

//rotates a pattern across segments at the same time
//useful for pixels arranged in shapes (esp circles)
//a pattern might looks like:
// byte spinPattern[(5 + 2) * 2] = {
   // 0, 5, 4, 1, 4, 1, 4,
   // 5, 11, 1, 4, 1, 4, 1,
// };
//where the first two indexs of each line are the pattern start and end points on the longest segment
//the remaining numbers on each line are pallet color references for each segment
//you must provide pallet references for all segments
//spin patterns will probably only work for a specific segment set, so make sure you are careful when calling the function
//repeat (true) will repeat the pattern along the segements as many times as possible, if an even number of patterns doesn't fit onto the strip, the extra will be rotated on/off the strip
//in repeat mode, the background will be the zeroth index of the pallet, background color modes still work
//EXTRA NOTES:
//BgColor Modes:
		//they match color modes, but you input a neg number for bgColor
		//ie BgColor = -2 ==> mode 2: rainbow spread across the longest segment
		//if you just want a solid background, just pass in a color;
//Color modes are as follows:
	//mode 1: colors match pattern
	//mode 2: rainbow spread across the longest segment
	//mode 3: use the bgSeg gradient
	//mode 4: use the non segmented BgGradient
	//mode 5; rainbow spread across all segments
	//mode 6: rainbow spread across whole strip
//ISSUES:
	//dimming is incompatible with different BgColors, and repeating
    //you can change the code here and in drawSegLine to do a rainbow on a pixel by pixel basis, but it may lag
void PixelStrip::colorSpin( SegmentSet segmentSet, byte spinPattern[], uint8_t spinPatternLength, uint32_t pallet[], int32_t BgColor, uint8_t colorMode, boolean repeat, int numCycles, int wait ){

	byte numSegs = segmentSet.numSegs;
	//rewrites the spinPattern array into a better representaion
	//ie, two 2d arrays, one with the pattern lengths, and one with the patterns
	//ie { 0, 5, 4, 1, 4, 1, 4, } <--row of original spinPattern goes to {0, 5} (spinPatternLengths) and { 4, 1, 4, 1, 4, } (spinPatternPatterns)
	//index's of the pattern lengths and the patterns are the same
	byte patternSecLength = (numSegs + 2); //length of each pattern (the start and end points and the pallet index's for each segment)
	uint8_t numPatterns = spinPatternLength / patternSecLength; //how many  patterns we have
	byte spinPatternLengths[numPatterns][2]; //array of pattern lengths
	byte spinPatternPatterns[numPatterns][numSegs]; //array of patterns
	byte colorModeBg = 1;

	//copy the orignial spinPattern into two seperate arrays
	for(int i = 0; i < numPatterns; i++){
		for(int j = 0; j < 2; j++ ){
		  spinPatternLengths[i][j] = spinPattern[i * patternSecLength + j];
		}
		for(int k = 0; k < numSegs; k++ ){
		  spinPatternPatterns[i][k] = spinPattern[i * patternSecLength + k + 2];
		}
	}

	//find the maximum segment length
	uint16_t maxSegLength = segmentSet.maxSegLength;

	//in repeat mode, the pattern is repeated across the segments
	//the mmodAmount determines when the pattern will start to wrap
	//for repeat patterns we want as many as possible on the strip, overflowing if needed
	//the total pattern length is taken as the last pattern length index
	//this means the pattern must go in consecutive order
	//if we're not repeating, we only want to draw the pattern once, so we set the patternLength to the maximum	seg length
	uint16_t totalPatternLength = maxSegLength;
	uint16_t modAmount = totalPatternLength;
	if(repeat){
		totalPatternLength = spinPatternLengths[numPatterns - 1][1];
		//determinehow many repeats we have, rounding up so that we always fill the strip
		modAmount = totalPatternLength * ceil(float(maxSegLength) / totalPatternLength);
    } else {
		//if the spin pattern is longer than the segment, we still want to draw the whole thing
		//so we'll use the pattern length as the mod amount
		//otherwise we'll just use the longest segment length
		if(spinPatternLengths[numPatterns - 1][1] > maxSegLength) {
			totalPatternLength = spinPatternLengths[numPatterns - 1][1];
		} else {
			totalPatternLength = maxSegLength;
		}
		modAmount = totalPatternLength;
	}

	//map the BgColor to the correct color mode for drawSimpleLine
	switch(BgColor){
		case -1: //for combalability with old code, (-1 used to indicate rainbow BG)
			colorModeBg = 2;
		case -2:
			colorModeBg = 2;
			break;
		case -3:
			colorModeBg = 3;
			break;
		case -4:
			colorModeBg = 4;
			break;
		case -5:
			colorModeBg = 5;
			break;
		case -6:
			colorModeBg = 6;
		    break;
		default:
			colorModeBg = 1;
	}

	//fill in initial background
	for(int i = 0; i < maxSegLength; i++){
		drawSegLineSimple( segmentSet, i, BgColor, colorModeBg);
	}

	uint16_t nextLineOn;
	uint16_t nextLineOff;

	uint8_t repeatCount = 0;
	//draw an initial pattern, this means the pattern is already draw when the motion starts,
	//so it doesn't have to fill itself in
	do{  //if we haven't reached the maximum length we draw another pattern
		for(int i = 0; i < numPatterns; i++ ){ //run over each pattern and draw it, line by line
			for(int j = spinPatternLengths[i][0]; j < spinPatternLengths[i][1]; j++){
				nextLineOn = ( j + repeatCount ) % modAmount;
				if(nextLineOn > maxSegLength - 1){
					continue;
				}
			    drawSegLine( segmentSet, nextLineOn, spinPatternPatterns[i], pallet, colorMode, colorModeBg, repeat);
			}
		}
		repeatCount += totalPatternLength;
	}while( repeatCount < maxSegLength);
	show();

	//draw patterns for a number of cycles, shifting forward by one each time,
	for(int i = 0; i <= numCycles; i++){
		stopPatternCheck();
	    yield();
		repeatCount = 0;
		do{ //if we haven't reached the out maximum length we draw another pattern
			if(!repeat){ //if we're in repeat mode, we don't need to fill in the background b/c the pattern is repeated
				for(int j = 0; j < numPatterns; j++ ){ //turn off the lines for all the patterns, this lets on lines overwrite off ones if needed (line when two lines are consecutive)
					nextLineOff = ( spinPatternLengths[j][0] + i - 1 ) % modAmount; //we start at the tail of each pattern: spinPatternLengths[j][0]
					//if we have a pattern longer than the strip, part of it will be cut off, we don't try to draw it
					if(nextLineOff > maxSegLength - 1){
						continue;
					}
					drawSegLineSimple( segmentSet, nextLineOff, BgColor, colorModeBg);
				}
			}
			for(int j = 0; j < numPatterns; j++ ){  //draw the next line on for each pattern in the next position
				nextLineOn = (spinPatternLengths[j][1] + i + repeatCount - 1 ) % modAmount;
				//for repeat patterns that are longer than the strip we will try to write lines that are off the end of the strip, we skip these
				if(nextLineOn > maxSegLength - 1){
					continue;
				}
				drawSegLine( segmentSet, nextLineOn, spinPatternPatterns[j], pallet, colorMode, colorModeBg, repeat );
			}
		repeatCount += totalPatternLength;
		}while( repeatCount < maxSegLength);
		show();
		delay(wait);
	}
}

//draws a line between segments, does its best to make a straight line
//the segment colors follow the provided pattern of pallet indecies
//the pattern length must match the number of segments
void PixelStrip::drawSegLine(SegmentSet segmentSet, byte lineNum, byte Pattern[], uint32_t pallet[], uint8_t colorMode, uint8_t bgColorMode, boolean brReplace){
	byte numSegs = segmentSet.numSegs;
	drawSegLineSection(segmentSet, 0, numSegs - 1, lineNum, Pattern, pallet, colorMode, bgColorMode, brReplace);
}

//draws a line between segments, from startSeg to endSeg (including endSeg), does its best to make a straight line
//the segment colors follow the provided pattern of pallet indecies
//the pattern length must match the number of segments!
//if brReplace is true, any part of the pattern with a value of zero (ie first element in the pallet) will be treated as a background colored pixel
//and will use bgColorMode for it's color mode (same modes as colorMode)
//if you don't want this, set brReplace to false
//Color modes are as follows:
//mode 2: rainbow spread across the longest segment
//mode 3: use the bgSeg gradient
//mode 4: use the non segmented BgGradient
//mode 5; rainbow spread across all segments
//mode 6: rainbow spread across whole strip
void PixelStrip::drawSegLineSection(SegmentSet segmentSet, uint8_t startSeg, uint8_t endseg, byte lineNum, byte Pattern[], uint32_t pallet[], uint8_t colorMode, uint8_t bgColorMode, boolean brReplace){
	uint16_t maxSegLength = segmentSet.maxSegLength;
	int16_t pixelNum = 0;
	uint32_t colorFinal = 0;
	uint8_t colorModeTemp = colorMode;
	for(int i = startSeg; i <= endseg; i++) {  //for each segment
	    colorModeTemp = colorMode;
	    pixelNum = getPixelNumFromLineNum(segmentSet, maxSegLength, i, lineNum);
		if(brReplace && Pattern[i] == 0){
			colorModeTemp = bgColorMode;
		}
		if(colorModeTemp > 1){
			colorFinal = drawSegLineExtraColorModes(segmentSet, maxSegLength, pixelNum, lineNum, i, colorModeTemp);
		} else {
			colorFinal = pallet[ Pattern[i] ];
		}
		setPixelColor( pixelNum , colorFinal );
	}
}

//retuns the pixel number located on segment segNum located along line lineNum where the total number of lines matches the pixels in the longest segment
//in other words, will return a pixel such that you can draw a straight line accross all segments, using the longest segment as the basis
int16_t PixelStrip::getPixelNumFromLineNum(SegmentSet segmentSet, uint16_t maxSegLength, uint8_t segNum, byte lineNum){
	return getSegmentPixel(segmentSet, segNum, (uint16_t(lineNum) * uint16_t(segmentSet.getTotalSegLength(segNum)) / maxSegLength) );
}

//Color modes are as follows:
//mode 2: rainbow spread across the longest segment
//mode 3: use the bgSeg gradient
//mode 4: use the non segmented BgGradient
//mode 5; rainbow spread across all segments
//mode 6: rainbow spread across whole strip
uint32_t PixelStrip::drawSegLineExtraColorModes(SegmentSet segmentSet, uint16_t maxSegLength, int16_t pixelNum, byte lineNum, uint8_t segNum, uint8_t colorMode){
	uint32_t colorFinal = 0;
	byte numSegs = segmentSet.numSegs;
	if(colorMode == 2){ //colors each line according to a rainbow mapped to the longest segment
		colorFinal = Wheel((lineNum * 256 / maxSegLength) & 255);
	} else if(colorMode == 3) { //colors each line to match the BgGradient segmented
		colorFinal = getSegBgGradientColor(segmentSet, segNum);
	} else if( colorMode == 4){ //colors each line to match the BgGradient
		colorFinal = getBgGradientColor( pixelNum );
	} else if( colorMode == 5 ){ //colors each line according to a rainbow spread across all segments
		colorFinal = Wheel((segNum * 256 / numSegs) & 255);
	} else if( colorMode == 6 ){ //colors each line according to a rainbow spread across whole strip
		colorFinal = Wheel((pixelNum * 256 / numLEDs) & 255);
	}
	return colorFinal;
}

//finds the pixel number of the pixel at a given position in a segment
//ie we want to find the pixel number of the 5th pixel in the second segment
//if the segment is in the reverse direction we want the pixel for the end of the segment
int16_t PixelStrip::getSegmentPixel(SegmentSet segmentSet, byte segNum, uint16_t num){
	// num is the index of the pixel in the segment and is 0 based
	// segmentNum index of the segement in the segment array
	boolean segDirection = segmentSet.getSegDirection(segNum);
	uint16_t numSec = segmentSet.getTotalNumSec(segNum);
	uint16_t count = 0;
	uint16_t prevCount = 0;
	int16_t secLength;
	uint16_t AbsSecLength;
	int secLengthSign;

	//counting loop setup variables, the default is a ascending segment, so we count forward
	int16_t step = 1;
	int16_t endLimit = numSec;
	int16_t startLimit = 0;

	//if the segment is decending, we want to count backwards, so we change the loop variables
	if( !segDirection ){
		step = -1;
		endLimit = -1;
		startLimit = numSec -1;
	}
	//run through each segment section, summing the lengths of the sections,
	//starting at the end or beginning of the segment depending on direction
	//if the sum is larger than the number we want, then the pixel is in the current section
	//use the section to get the physical pixel number
	for(int i = startLimit; i != endLimit; i += step){
		secLength = segmentSet.getSecLength(segNum, i); //sec length can be negative
		secLengthSign = (secLength > 0) - (secLength < 0);
		AbsSecLength = abs(secLength);
		prevCount = count;
		count += AbsSecLength ; //always add a positive sec length, we want to know the physical length of each section

		//if the count is greater than the number we want (num always starts at 0, so secLength will always be one longer than the max num in the section)
		//the num'th pixel is in the current segment.
		//for ascending segments:
		//we add the difference between how many pixel's we've counted
		//and how many we want (ie num - prevCount) to the section's starting pixel
		//for descending segments:
		//we add the section length and subtract the difference (num - prevCount) - 1

		//unless the secLength is 1, then it's just the start pixel
		if( count > num ){
			if( secLength == 1 ){
				return segmentSet.getSecStartPixel(segNum, i);
			} else if(segDirection) {
				return (segmentSet.getSecStartPixel(segNum, i) + secLengthSign * (num - prevCount) );
			} else {
				return (segmentSet.getSecStartPixel(segNum, i) + secLengthSign * ( AbsSecLength - (num - prevCount) - 1) ) ;
			}

		}
	}
}

//see colorSpinSimpleSet
//lets you do a colorSpinSimpleSet without making a pattern
//if you pass in 1 as the numColors, then the prefColor will be used for all the sweeps
//pass -1 as BgColor to set as rainbow
//otherwise, a pattern will be created randomly using a number  (numColors) of colors
void PixelStrip::colorSpinSimple( SegmentSet segmentSet, byte numColors, int32_t prefColor, int32_t BgColor, uint8_t sweepLength, int numSweeps, byte sweepSpacing, byte patternMode, byte colorMode, int numCycles, int wait ){
	 uint32_t pallet[numColors];
	 byte pattern[numColors];
	 if(colorMode == 1){ //if we're doing a solid color
		 if( numColors > 1){ //more than one color, so we'll pick at random
			 for(int i = 0; i < numColors; i++ ){
				pattern[i] = i;
				pallet[i] = randColor();
			 }
		 } else {  //one color, use it
			 pattern[0] = 0;
			 pallet[0] = prefColor;
			 patternMode = 0;  //only pattern mode 0 makes sense for a single color
		 }
	 }
	 colorSpinSimpleSet( segmentSet, pattern, numColors, pallet, BgColor, sweepLength, numSweeps, sweepSpacing, patternMode, colorMode, numCycles, wait );
}

//builds a spin pattern according to a given color pattern and executes a color spin
//the pattern parameters are:
//sweepLength: how long each colored section of the pattern should be
//			   this length applies to each color in the pattern
//numSweeps: how many total color sweeps to run
//			 ie; 2 would to do sets of the pattern, which each color being sweepLength long
//			 passing -1 will make the pattern repeat as many times as possible, try to make sure an complete number of patterns can fit in the longest segment
//sweepSpacing: the spacing between each color, this is filled in with the BgColor
//patternMode: Determines how the pattern is drawn for the sweeps
//			   0 : the default, each sweep is a solid color from the pattern
//			   1 : each sweep is the same, the pattern is copied down each sweep
//			   2 : like 1, but the pattern is shifted for each subsequent sweep
//Color modes are as follows:
	//mode 1: just solid color
    //mode 2: rainbow spread across the longest segment
    //mode 3: use the bgSeg gradient
	//mode 4: use the non segmented BgGradient
	//mode 4; rainbow spread across all segments
	//mode 6: rainbow spread across whole strip
void PixelStrip::colorSpinSimpleSet( SegmentSet segmentSet, byte pattern[], byte patternLength, uint32_t pallet[], int32_t BgColor, byte sweepLength, int numSweeps, byte sweepSpacing, byte patternMode, byte colorMode, int numCycles, int wait ){
    byte numSegs = segmentSet.numSegs;

	uint16_t maxSegLength = segmentSet.maxSegLength;

	uint8_t numSweepsDrawn;
	if( numSweeps < 0){
	//if we want to turn on repeat mode, we don't actually want to use the colorSpin repeat, because we can't easily have a Bg
	//instead we will create our own repeat by filling in a sweep + spacing for all the colors in the pattern, repeating if needed

		//if drawing all the patterns (and spacing) is longer than the max seg, we draw one sweep for each color
		//otherwise, we draw sweeps until we fill up the longest segment
		if( (patternLength * (sweepLength + sweepSpacing) ) > maxSegLength){
			numSweepsDrawn = patternLength;
		} else{
			numSweepsDrawn = ceil( float(maxSegLength) / ( sweepLength + sweepSpacing) ) ;
		}
	} else {
		numSweepsDrawn = numSweeps;
	}

	uint8_t patternSecLength = (numSegs + 2); //length of each pattern (the start and end points and the pallet index's for each segment)

	//setup our own spinPattern
	uint8_t spinPatternLength = patternSecLength * numSweepsDrawn;
	byte spinPattern[ spinPatternLength ];

	byte patternIndexOn;
	//copy the spinPattern, and fill in the color pattern
	for(int i = 0; i < numSweepsDrawn; i++){
		//fill in each pattern section start / end indexes
		spinPattern[i * patternSecLength] = (sweepLength + sweepSpacing) * i + sweepSpacing; //add an extra buffer sweepSpacing to account for wrapping
		spinPattern[i * patternSecLength + 1] = spinPattern[i * patternSecLength] + sweepLength; //previous line + sweepLength
		//fill in the actual color pattern for start / end indexes
		if(colorMode == 1){ //if we're in color mode 1, we need to fill in a pattern for the colors
			for(int j = 0; j < numSegs; j++){
				switch(patternMode){
					case 0:
						patternIndexOn = pattern[ i % patternLength ];  //default, each line is one color
						break;
					case 1:
						 patternIndexOn = pattern[ j % patternLength ];  //each line is the pattern
						 break;
					case 2:
						patternIndexOn = pattern[ (i - j + numSegs ) % patternLength ];  //each line is the pattern incremented per line
						break;
					default:
						patternIndexOn = pattern[ i % patternLength ];
				};
				spinPattern[i * patternSecLength + j + 2] = patternIndexOn;
			}
		}
    }
	colorSpin( segmentSet, spinPattern, spinPatternLength, pallet, BgColor, colorMode, false, numCycles, wait );
}

//draws a segment line of one color, does not need a pallet or pattern, passing -1 as the color will do a rainbow based on the Wheel() function
void PixelStrip::drawSegLineSimple(SegmentSet segmentSet, byte lineNum, uint32_t color, uint8_t colorMode){
	byte numSegs = segmentSet.numSegs;
	drawSegLineSimpleSection(segmentSet, 0, numSegs - 1, lineNum, color, colorMode);
}

//draws a segment line of one color betweeb startSeg and endSeg (including endSeg)
//does not need a pallet or pattern,
//Color modes are as follows:
	//mode 1: just solid color
	//mode 2: rainbow spread across the longest segment
	//mode 3: use the bgSeg gradient
	//mode 4: use the non segmented BgGradient
	//mode 5: rainbow spread across all segments
	//mode 6: rainbow spread across whole strip
void PixelStrip::drawSegLineSimpleSection(SegmentSet segmentSet, uint8_t startSeg, uint8_t endSeg, byte lineNum, uint32_t color, uint8_t colorMode){
	uint16_t maxSegLength = segmentSet.maxSegLength;
	int16_t pixelNum = 0;
	uint32_t colorFinal = color;
	for(int i = startSeg; i <= endSeg; i++) {  //for each segment, set the color, if we're in rainbow mode, set the rainbow color
		pixelNum = getPixelNumFromLineNum(segmentSet, maxSegLength, i, lineNum);
		if(colorMode > 1){
			colorFinal = drawSegLineExtraColorModes(segmentSet, maxSegLength, pixelNum, lineNum, i, colorMode);
		}
		setPixelColor( pixelNum , colorFinal );
	}
}


//fairly experimental and unpredictable
//takes a given spiral pattern and fills it in with a set of colors
//b/c it's hard to generate a spiral, it only fills in the colors, you must provide the pattern intervals
//a spiral pattern looks like:
// spinPattern4[(5 + 2) * 5] = {
   // 0, 2, 4, 0, 0, 0, 0,
   // 2, 3, 0, 4, 0, 0, 0,
   // 3, 4, 0, 0, 4, 0, 0,
   // 4, 5, 0, 0, 0, 4, 0,
   // 5, 6, 0, 0, 0, 0, 4,
// };
//where the "4"'s are filled in by this function
//NOTE that spirals makes a copy of the spinPattern, this might take up too much memory
//in this case call colorSpin() directly with the spinPattern, using a randomly generated pallet
//the section of the matrix filled in by the "4"'s does not need to be square (it's width must equal the number of segs)
//the pallet contains the colors, while the colorPattern contains pallet indexs
//the spiral is colored in order of the color pattern
//note that the colorPallet must be numSegs long
void PixelStrip::spiralsSet( SegmentSet segmentSet, byte spinPattern[], uint8_t spinPatternLength, uint32_t pallet[], byte colorPattern[], int numCycles, int wait ){
	byte numSegs = segmentSet.numSegs;

	byte spinPatternCopy[ (numSegs + 2) * spinPatternLength];

	byte patternSecLength = (numSegs + 2); //length of each pattern (the start and end points and the pallet index's for each segment)
	uint8_t numPatterns = spinPatternLength / patternSecLength; //how many  patterns we have

	//copy the spinPattern, and fill in the color pattern
	for(int i = 0; i < numPatterns; i++){
		spinPatternCopy[i * patternSecLength] = spinPattern[i * patternSecLength];
		spinPatternCopy[i * patternSecLength + 1] = spinPattern[i * patternSecLength + 1];
		//if( i < numSegs ){ //you might need blank sections at the end of a spiral to make it work
			for(int j = 0; j < numSegs; j++){  //( i + j ) % numSegs
			  spinPatternCopy[i * patternSecLength + j + 2]	= colorPattern[ (j - i + numSegs) % numSegs ];
			}
		//} else {
			// for(int j = 0; j < numSegs; j++){
			  // spinPatternCopy[i * patternSecLength + j + 2]	= colorPattern[ numSegs - 1 ];
			// }
			//spinPatternCopy[(numPatterns - 1) * patternSecLength + numSegs + 1]	= colorPattern[ 0 ];
		//}
    }
    colorSpin( segmentSet, spinPatternCopy, spinPatternLength, pallet, 0, 1, true, numCycles, wait );
}

//see spiralsSet
//makes a spiral given a spiral pattern
//selects colors randomly, or uses a prefered color
//numColors is the spiral thickness
//to choose a random color(s), pass -1 as prefColor
//TODO to do a rainbow, pass -2 as prefColor, needs rainbow to be set on pixel by pixel basis
//the rest of the spiral pattern will be filled with BgColor
void PixelStrip::spiralsRand( SegmentSet segmentSet, byte spinPattern[], uint8_t spinPatternLength, byte numColors, int32_t prefColor, uint32_t BgColor, int numCycles, int wait ){
  byte numSegs = segmentSet.numSegs;
  byte colorPattern[numSegs];

  //because we're gonna use repeat mode we need to set the background as part of the pattern, so we tack it on
  uint32_t pallet[numColors + 1];
  pallet[numColors] = BgColor;

  for(int i = 0; i < numSegs; i++){
	  colorPattern[i] = numColors;
  }
  for(int i = 0; i < numColors; i++){
	  if( prefColor < 0){
		pallet[i] = randColor();
	  // } else if( prefColor == -2){
		// pallet[i] = 1;
	  } else {
		 pallet[i] = prefColor;
	  }
      colorPattern[i] = i;
  }
  spiralsSet( segmentSet, spinPattern, spinPatternLength, pallet, colorPattern, numCycles, wait );
}

//creates a rainbow pattern using the maximum segment length and spins it
void PixelStrip::colorSpinRainbow( SegmentSet segmentSet, boolean forward, int numCycles, int wait ){
	byte numSegs = segmentSet.numSegs;
	uint16_t maxSegLength = segmentSet.maxSegLength;
	uint32_t pixelColor;
	uint16_t pixelNum;

	//alternate way to do it, more memory intensive, no direction control
	// uint32_t pallet[maxSegLength];
	// uint8_t pattern[maxSegLength];
	// for(int i = 0; i < maxSegLength; i++ ){
		// pallet[i] = Wheel((i * 256 / maxSegLength) & 255);
		// pattern[i] = i;
	// }
	// colorSpinSimpleSet( segmentSet, pattern, maxSegLength, pallet, 0, 1, maxSegLength, 0, 0, 1, numCycles, wait );

	//we always want to draw the line colors opposed to the direction
	//ie if we're going forward, we'll draw the last color first
	//this creates a smooth transition on odd length segments
	//the variables below control the loop direction
	int8_t start = maxSegLength - 1;
	int8_t end = -1;
	int8_t step = -1;

	if(!forward){
		start = 0;
	    end = maxSegLength;
		step = 1;
	}

	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		//cycle through the max length segment, and draw the rainbow, shifting by i
		for(int j = start; j != end; j+= step){
			pixelColor = Wheel( ( ( (i + j) % maxSegLength * 256 / maxSegLength)) & 255);
			if(forward){
				pixelNum = maxSegLength - j - 1 ;
			} else {
				pixelNum = j;
			}
			drawSegLineSimple(segmentSet, pixelNum, pixelColor, 1);
		}
		show();
		delay(wait);
	}
}

//does a set of sonarSpin with random colors
 void PixelStrip::sonarSpinRand( SegmentSet segmentSet, uint8_t numColors, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean inToOut, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait){
	 uint32_t pallet[numColors];
	 byte pattern[numColors];
	 for(int i = 0; i < numColors; i++){
		 pallet[i] = randColor();
		 pattern[i] = i;
	 }
	 sonarSpin( segmentSet, pallet, numColors, pattern, numColors, BgColor, totalFadeLength, spacing, inToOut, alternate, bounce, bounceChange, numCycles, wait);
 }

 //does a set of color spins where the color fades in fadelength steps
 //takes a set of colors in a pattern (using pallet as the colors) and will do a wave of each color in order
 //as many fades as possible will be fit onto the segmentSet at once (governed by the longest segment),
 //you can control how long each wave is using the totalFadeLength (includes head and tail) and the spacing (between waves)
 //direct sets the direction of the waves (true => waves move from first segment to last)
 //setting alternate true will swap the direction of every other wave
 //setting bounce true will cause each wave to reverse direction at the final/first segment, instead of wraping
 //bounce change sets when a wave's color changes (true => every bounce, false => every full cycle)
void PixelStrip::sonarSpin( SegmentSet segmentSet, uint32_t pallet[], byte palletLength, byte pattern[], uint8_t patternLength, uint32_t BgColor, uint8_t totalFadeLength, uint8_t spacing, boolean direct, boolean alternate, boolean bounce, boolean bounceChange, int numCycles, uint8_t wait){
	byte numSegs = segmentSet.numSegs;
	uint16_t maxSegLength = segmentSet.maxSegLength;
	uint8_t numSimultWaves = ceil( (float)maxSegLength / (totalFadeLength + spacing) ); //number of waves that can fit on the segmentSet, rounded up
	int8_t activeWavePos[numSimultWaves][3]; //pallet, wave head location, wave direction increment; for how ever many waves are active at once
	int8_t startDirect[numSimultWaves]; //initial starting direction for a wave (used for changing colors when bouncing)
	int8_t wavePos; //marker for tracking trail location
	int8_t totalSpace = (numSimultWaves * (totalFadeLength + spacing)); //total space taken up by all the active waves (can be > maxSegLength)
	uint8_t nextPalletIndex; //placeholder for next color for a wave

	//populate activeWavePos matrix array for the initally active waves
	for(int i = 0; i < numSimultWaves; i++){
		activeWavePos[i][0] = i % palletLength; //wave color
		//set vars depending on if we're going forward or backward, or alternating
		if(direct || (alternate && (i % 2 != 0))){
			//place the head of the waves, starting at the end of the segmentSet, we need to draw the first wave part way along the segments
			activeWavePos[i][1] = totalSpace - 1 - i * (totalFadeLength + spacing);
			activeWavePos[i][2] = 1; //set direction increment positive
			startDirect[i] = 1; //initial wave direction
			if(numSimultWaves == 1){ //if we just have one wave, we'll start it at the beginning of the segments for looks
				activeWavePos[i][1] = 0;
			}
		} else {
			activeWavePos[i][1] = i * (totalFadeLength + spacing);
			activeWavePos[i][2] = -1;//set direction increment positive
			startDirect[i] = -1; //initial wave direction
			if(numSimultWaves == 1){ //if we just have one wave, we'll start it at the end of the segments for looks
				activeWavePos[i][1] = numSegs - 1;
			}
		}
	}

	RGB palletRGB[palletLength];
	RGB BgColorRGB = makeRGBFromColor(BgColor);

	uint32_t lineColor;

	//creates an RGB pallet from the passed in one
	//note that it converts all colors in the pallet
	//so watch your memory useage if you've pass in a big pallet
	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);


	//for eqch cycle, draw all the waves and fades, then determine the next wave position change color if needed
	for (int i = 0; i <= numCycles; i++) {
		stopPatternCheck();
	    yield();
		//if we're bouncing, then the trails are not touched after a bounce b/c the wave changes direction
		//this slowly fades the remaing trails
		if(bounce){
			dimStrip();
		}
			//for each wave, draw the trail, increment the wave head postion, and change colors if needed
			for (int j = 0; j < numSimultWaves; j++) {
				for(int k = 0; k < totalFadeLength; k++){
					wavePos = (activeWavePos[j][1] - k * activeWavePos[j][2]);
					if(!bounce){
						wavePos = (totalSpace + wavePos) % totalSpace ; //wrap the trail if we're not bouncing, fixing for negative case
				    }
					//the trail can be drawn off the segments, skip these cases
					if(wavePos > maxSegLength - 1 || wavePos < 0){
						continue;
				    }
					//fill the segment with a faded trail (totalFadeLength - 1, makes the color do to the BgColor on the last step)
					//kTemp = k;
					//if(k > 0 ){ kTemp++; }
					//segColor = desaturate( pallet[ activeWavePos[j][0] ] , (100 - k * 100) / (totalFadeLength -1) );
				    lineColor = getCrossFadeStep( palletRGB[ activeWavePos[j][0] ], BgColorRGB, totalFadeLength - 1, k);
					if( k > 0 ){ lineColor = dimColor(lineColor, 1); }; //real quick, dirty way to make the trails dim more dramatically, replace this with HSV dimming later
					drawSegLineSimple(segmentSet, wavePos, lineColor, 1);
				}
				nextPalletIndex = (activeWavePos[j][0] + numSimultWaves) % palletLength; //get the next color ready (skip forward by numSimultWaves, so stop repeating colors)
				activeWavePos[j][1] = activeWavePos[j][1] + activeWavePos[j][2]; //get the next wave head position
				//if bounce, check to change direction / color, otherwise check for color change
				if(bounce){
					//if we're going forward, and we hit the last pixel, reverse the wave direction
					if(activeWavePos[j][1] > maxSegLength - 1 && activeWavePos[j][2] == 1){
						activeWavePos[j][2] = -1;
						//if the wave started at then last segment, change it's color
						if(startDirect[j] == -1 || bounceChange){
						   activeWavePos[j][0] = nextPalletIndex;
						}
						//if we're going backward, and we hit the first pixel, reverse the wave direction
					} else if(activeWavePos[j][1] < 0 && activeWavePos[j][2] == -1){
						activeWavePos[j][2] = 1;
						//if the wave started at then first segment, change it's color
						if(startDirect[j] == 1 || bounceChange){
						   activeWavePos[j][0] = nextPalletIndex;
						}
					}
				} else{
					activeWavePos[j][1] = ( totalSpace + activeWavePos[j][1]) % totalSpace; //wrap the head position if we're not bouncing
					//if were moving forward, and hit totalSpace, or if we're going backward and hit the first segment, change colors
					if( ( direct && (activeWavePos[j][1] == totalSpace -1) ) || ( !direct && (activeWavePos[j][1] == 0 ) ) ){
						activeWavePos[j][0] = nextPalletIndex;
					}
				}
			}
		show();
		delay(wait);
    }
}

//spawns a random set of sparks that move in a radial direction (false is in to out)
//sparks sparkChance to spawn (0 - 100), with maxSparks being able to exist on any given line
//numColors sets number of colors to choose randomly
//setting numColors to 1, and passing in a prefered color will use it
//Color modes are as follows (for both Bg and main color):
	//mode 1: just solid color
	//mode 2: rainbow spread across the longest segment
	//mode 3: use the bgSeg gradient
	//mode 4: use the non segmented BgGradient
	//mode 5: rainbow spread across all segments
void PixelStrip::sparkSeg( SegmentSet segmentSet, byte sparkChance, byte maxSparks, byte numColors, uint32_t prefColor, byte colorMode, uint32_t BgColor, byte BgColorMode, boolean direction, int numCycles, int wait ){
	if(colorMode == 1){
		uint32_t pallet[numColors];
		if( numColors > 1){ //more than one color, so we'll pick at random
			genRandPallet(pallet, numColors);
		} else {  //one color, use it
			pallet[0] = prefColor;
		}
		sparkSegSet( segmentSet, sparkChance, maxSparks, pallet, numColors, 1, BgColor, BgColorMode, direction, numCycles, wait );
	} else {
		//if we're not using color mode 1, our pallet should be empty
		uint32_t pallet[1];
		pallet[0] = 0;
		sparkSegSet( segmentSet, sparkChance, maxSparks, pallet, 1, colorMode, BgColor, BgColorMode, direction, numCycles, wait );
	}
}

//spawns a random set of sparks that move in a radial direction (false is in to out)
//sparks sparkChance to spawn (0 - 100), with maxSparks being able to exist on any given line
//colors for sparks are choosen randomly from the pallet[]
//Color modes are as follows (for both Bg and main color):
	//mode 1: just solid color
	//mode 2: rainbow spread across the longest segment
	//mode 3: use the bgSeg gradient
	//mode 4: use the non segmented BgGradient
	//mode 5: rainbow spread across all segments
void PixelStrip::sparkSegSet( SegmentSet segmentSet, byte sparkChance, byte maxSparks, uint32_t pallet[], uint8_t palletLength, byte colorMode, uint32_t BgColor, byte BgColorMode, boolean direction, int numCycles, int wait ){

	byte numSegs = segmentSet.numSegs;

	byte spawnChance;

	//direction variables (default is sparks traveling in to out)
	int8_t step = -1; //step amount
	uint8_t outerLimit = 0; //last segment sparks can reach
	uint8_t startSeg = numSegs - 1; //segement sparks start at

	if(!direction){
		step = 1;
		outerLimit = (numSegs - 1);
		startSeg = 0;
	}

	uint16_t maxSegLength = segmentSet.maxSegLength;

	//fill the segments with the BgColor
	for(int i = 0; i < maxSegLength; i++){
		drawSegLineSimple(segmentSet, i, BgColor, BgColorMode);
	}

	//array of spark locations, formulated into maxSpark rows with maxSegLength columbs
	//each cell contains a spark's segment position, with the cell's index being the spark's line
	//-1 in a cell indicates the spark is not active
	int8_t sparkLocations[maxSparks][maxSegLength];

	//array of spark colors, corrosponds to sparkLocations, but each cell contains the spark's color index
	byte sparkColors[maxSparks][maxSegLength];

	//initialize the spark arrays
	for(int i = 0; i < maxSparks; i++){
		for(int j = 0; j < maxSegLength; j++){
			sparkLocations[i][j] = -1;
			sparkColors[i][j] = 0;
		}
	}
	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		for(int j = 0; j < maxSparks; j++){
			for(int k = 0; k < maxSegLength; k++){
				//if the spark is active we turn off its current location and move it forward
				//if it's at the last segment we set it to inactive
				//otherwise we try to spawn a new spark
				if( ( sparkLocations[j][k] != -1 ) && ( sparkLocations[j][k] != outerLimit ) ){
					//turn the current spark location off
					drawSegLineSimpleSection(segmentSet, sparkLocations[j][k], sparkLocations[j][k], k, BgColor, BgColorMode);
					//increment the spark
					sparkLocations[j][k] += step;
					//turn the new spark location on
					drawSegLineSimpleSection(segmentSet, sparkLocations[j][k], sparkLocations[j][k], k, pallet[ sparkColors[j][k] ], colorMode);
				} else if( sparkLocations[j][k] == outerLimit ){
					//turn the current spark location off
					drawSegLineSimpleSection(segmentSet, outerLimit, outerLimit, k, BgColor, BgColorMode);
					//set the spark to inactive
					sparkLocations[j][k] = -1;
				} else {
					//roll to see if we spawn a spark, if so, start the spark, and give it a color
					spawnChance = random(100);
					if(spawnChance <= sparkChance){
						sparkLocations[j][k] = startSeg;
						sparkColors[j][k] = random(palletLength);
						drawSegLineSimpleSection(segmentSet, startSeg, startSeg, k, pallet[ sparkColors[j][k] ], colorMode);
					}
				}
			}
		}
		show();
		delay(wait);
	}
}

//color wipes a random set of colors
//passing -1 as wipeLength will call colorWipeSections(), -2 will call colorWipeSegRadial()
//numColors sets how many colors to wipe
//see colorWipe and colorWipePallet for more details
//style sets the type of wipe:
	//see colorWipeAdvSegment(), colorWipeSections(), and colorWipeSegRadial() comments for style guides
void PixelStrip::colorWipeRandomSeg( SegmentSet segmentSet, uint8_t style, uint32_t numColors, int8_t wipeLength, uint8_t wait, boolean forward, boolean alternate, boolean simult){
	uint32_t pallet[numColors];
	byte pattern[numColors];
	for(int i = 0; i < numColors; i++){
		pallet[i] = randColor();
		pattern[i] = i;
	}
	colorWipePalletSeg(segmentSet, style, simult, pattern, numColors, pallet, wipeLength, wait, forward, alternate);
}

//Fill the pixels one after the other with a color
//passing -1 as wipeLength will call colorWipeSections(), -2 will call colorWipeSegRadial()
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//segLength sets how long each colorwipe is. The strip is divided evenly for the wipe
//alternate sets if all segemtents wipe the same direction or alternate
//simult sets if the wipes should occur simultaneously or one after another
//NOTE; simult sets inToOut for colorWipeSections() (ie starting with last or first segment), it does nothing for colorWipeSegRadial()
//style sets the type of wipe:
	//see colorWipeAdvSegment(), colorWipeSections(), and colorWipeSegRadial() comments for style guides
void PixelStrip::colorWipeSeg(SegmentSet segmentSet, uint32_t color, int8_t wipeLength, uint8_t wait, boolean forward, boolean alternate, boolean simult){
	uint32_t pallet[1] = {color};
	byte pattern[1] = {0};
	colorWipePalletSeg(segmentSet, 1, simult, pattern, 1, pallet, wipeLength, wait, forward, alternate);
}

//Fills in all segments at the same time, according to the longest segment (like colorSpin)
//passing 0 as wipelength will do a wipe with wipelength equla to the maximum segment length ( a shorthand for wiping all the segments at once)
//passing -1 as wipeLength will call colorWipeSections(), -2 will call colorWipeSegRadial()
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//segLength sets how long each colorwipe is. The strip is divided evenly for the wipe
//alternate sets if all segemtents wipe the same direction or alternate
//the pattern and pallet set the order of colors for the wipes:
//pattern is a set of pallet indexs ex: {1, 5, 4}
//pallet is a set of colors ex {blue, green, red}
//simult sets if the wipes should occur simultaneously or one after another
//NOTE; simult sets inToOut for colorWipeSections() (ie starting with last or first segment), it does nothing for colorWipeSegRadial()
//style sets the type of wipe:
	//see colorWipeAdvSegment(), colorWipeSections(), and colorWipeSegRadial() comments for style guides
void PixelStrip::colorWipePalletSeg(SegmentSet segmentSet, uint8_t style, boolean simult, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], int8_t wipeLength, uint8_t wait, boolean forward, boolean alternate) {
  byte numSegs = segmentSet.numSegs;

  if(wipeLength >= 0){  //switches from doing a segmented wipe to a section wipe

	  uint16_t maxSegLength = segmentSet.maxSegLength;

	  //fix for shorthand for maxseglength
	  if(wipeLength == 0){
	     wipeLength = maxSegLength;
	  }

	  byte segGroups[numSegs][2];

	  uint8_t numWipes = ceil(float(maxSegLength) / wipeLength);

	  boolean forwardLocal = forward; //copy of forward, as it may change in the loops

	  int16_t LEDPattern[numWipes][2];
	  //set loop parameters for a forward loop
	  int8_t directionConst = 1;
	  uint8_t wipeStart = 0;

	  //if we want to go backwards, change the loop parameters
	  if(!forwardLocal){
		directionConst = -1;
		wipeStart = wipeLength - 1;
	  }

		for (int j = 0; j < numWipes; j++) {
			if(alternate){ //if we're alternating, we switch the direction variables with each segemnt
				if(forwardLocal){
				   directionConst = 1;
				   wipeStart = 0;
				} else {
				   directionConst = -1;
				   wipeStart = wipeLength - 1;
				}
				forwardLocal = !forwardLocal;
			}
			LEDPattern[j][0] = (j * wipeLength) + wipeStart;
			LEDPattern[j][1] = directionConst;
	  }
	  colorWipeAdvSegment( segmentSet, style, simult, LEDPattern, numWipes, colorPattern, colorPatternLength, pallet, wipeLength, wait);
  } else if(wipeLength == -1){
	  colorWipeSections( segmentSet, colorPattern, colorPatternLength, pallet, style, forward, alternate, simult, wait);
  } else {
	  colorWipeSegRadial( segmentSet, colorPattern, colorPatternLength, pallet, style, forward, wait);
  }
}

// Fill the pixels one after the other with a color in a rainbow pattern
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//segmented sets the style of rainbow for each segment,
void PixelStrip::colorWipeRainbowSeg(SegmentSet segmentSet, uint8_t wait, uint8_t wipeLength, boolean forward, boolean alternate, boolean segmented, boolean simult) {
 byte numSegs = segmentSet.numSegs;
 if(segmented){
		uint32_t pallet[numSegs];
		byte colorPattern[numSegs];
		for(int i = 0; i < numSegs; i++){
			colorPattern[i] = i;
			pallet[i] = Wheel( (i * 256 / numSegs ) & 255);;
		}
		//style is 3 b/c we want to set each segment to a color
		colorWipePalletSeg( segmentSet, 3, simult, colorPattern, numSegs, pallet, wipeLength, wait, forward, alternate);
	} else {
		colorWipePalletSeg( segmentSet, 4, simult, 0, 0, 0, wipeLength, wait, forward, alternate);
	}
}

//advanced color wiping function
//Fills in all segments at the same time, according to the longest segment (like colorSpin)
//takes a LEDPattern[][2] with structure:  { {startPoint, direction} , {startPoint, direction}, etc }
//a colorPattenr[] with structure: {pallet index, pallet index, etc}
//a pallet, and the wipe length
//passing 0 as wipelength will do a wipe with wipelength equla to the maximum segment length ( a shorthand for wiping all the segments at once)
//and wipes the leds from the startPoints, in a direction for the wipe length
//simult sets if the wipes should occur simultaneously or one after another
//style sets the type of wipe:
//1: normal wipe, each segment is a color from the pattern (with wrapping)
//2: each individual segment will alternated colors according to the pattern
//3: each segment will alternate colors
//4: rainbow mode, the rainbow will be distributed across each segment individually
//5: BgGradientSeg will be used
//6: BgGradient will be used
//7: rainbow spread across all segments
void PixelStrip::colorWipeAdvSegment( SegmentSet segmentSet, uint8_t style, boolean simult, int16_t LEDPattern[][2], uint8_t LEDPatternLength, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], uint8_t wipeLength, uint8_t wait){
  byte numSegs = segmentSet.numSegs;
  //find the maximum segment length
  uint16_t maxSegLength = segmentSet.maxSegLength;

  //if wipelength is 0 (not a usefull number) we'll do a fully synchronized wipe,
  //ie setting wipelength to the maximum segment length
  if(wipeLength == 0){
	 wipeLength = maxSegLength;
  }

  byte segColorPattern[numSegs];

  uint16_t ledLocation; //led location for wipe

  //set the inner and outer loop limits
  //allows reusing of loops to do a sequential wipe (one segment after another)
  //or a simultaneous wipe (all segments at once)
  uint8_t outerLoopLimit = LEDPatternLength;
  uint8_t innerloopLimit = wipeLength;
  int wipeIndex;
  int LEDPatternIndex;
  uint32_t color;
  byte colorModeBg;

  if(simult){
	outerLoopLimit = wipeLength;
	innerloopLimit = LEDPatternLength;
  }

  //if needed, map the style to the correct color mode for drawSimpleLine
  switch(style){
		case 4:
			colorModeBg = 2;
			break;
		case 5:
			colorModeBg = 3;
			break;
		case 6:
			colorModeBg = 4;
			break;
		case 7:
			colorModeBg = 5;
			break;
		default:
			colorModeBg = 1;
  }

  //loops through each segment, setting one led at a time
  for(int i = 0; i < outerLoopLimit; i++){
	stopPatternCheck();
	yield();
	for (int j = 0; j < innerloopLimit; j++) {

		//mode switching adjusts from sequential to simultaneous
		//by matching the loop indexs
		if(simult){
			wipeIndex = i;
			LEDPatternIndex = j;
		} else {
			wipeIndex = j;
			LEDPatternIndex = i;
		}

		//sets the led location; adds the current loop value corrosponding
		//to the wipelength multiplied by the direction indicator to the lED start point
		ledLocation = LEDPattern[LEDPatternIndex][0] + ( wipeIndex * LEDPattern[LEDPatternIndex][1] );
		//if we've run over the number of leds, stop the loop, but only if we're not trying to start a wipe outside
		//ie starting > maxSegLength is ok, as long as we're wiping backwards
		//commented, needs testing for speed effects
		// if( (ledLocation >= maxSegLength && LEDPattern[LEDPatternIndex][1] != -1) || (ledLocation < 0 && LEDPattern[LEDPatternIndex][1] != 1) ){
		  // break;
		// }
		for(int k = 0; k < numSegs; k++){
			//set the pixel color according to the style
			switch(style){
				case 1 : //color wipes each color from pallet in turn
				  segColorPattern[k] = colorPattern[ LEDPatternIndex % colorPatternLength ];
				break;

				case 2 : //color wipes alternating colors from pallet along the wipe
				   segColorPattern[k] = colorPattern[ wipeIndex % colorPatternLength ];
				break;

				case 3 : //alternates colors for each segment
				   segColorPattern[k] = colorPattern[ k % colorPatternLength ];
				break;
			}
		}

		if(style > 3){ //if we're drawing a rainbow/gradient, we need to use drawSegLineSimple();
		    drawSegLineSimple( segmentSet, ledLocation, 0, colorModeBg);
		} else {
			drawSegLine( segmentSet, ledLocation, segColorPattern, pallet, 1, 1, false);
		}

		if(!simult){
	      show();
	      delay(wait);
		}
	}
	if(simult){
		show();
		delay(wait);
	}
  }
}

//a sightly different color wiping function for segments,
//(this function can be called from other segmented color wipe functions, except rainbow, by passing -1 as the wipe length)
//color wipes each segment sequentially
//colors are choosen according to colorPattern[], pallet[], and the style.
//colorPattern[] contains a set of indexs of pallet
//there are 7 styles:
		//1: normal wipe, each segment is a color from the pattern (with wrapping)
		//2: each individual segment will alternated colors according to the pattern
		//3: rainbow mode 1, the rainbow will be evenly distributed across all segements together
		//4: rainbow mode 2, the rainbow will be distributed across each segment individually
		//5: rainbow mode 3, rainbow is distrubuted across segements as groups
		//6: use the BgSegGradient
		//7: use the BgGradient
//forward sets the directions of the wipes (true sets wipes starting at zeroth pixel)
//alternating set to true will change the alternate the directions of the wipes
void PixelStrip::colorWipeSections( SegmentSet segmentSet, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], byte style, boolean forward, boolean alternate, boolean inToOut, uint8_t wait){
	byte numSegs = segmentSet.numSegs;
    //find the maximum segment length
    uint16_t maxSegLength = segmentSet.maxSegLength;

	//loop variables for changing the direction of the wipe
	int8_t loopDirct, firstSeg, lastSeg, segLoopDirct ;
	int16_t startLimit, endLimit, segLength;

	int16_t ledLocation;

	uint32_t color;

	//loop variables for the order of the wipe (default is first segment first)
	firstSeg = 0;
	lastSeg = numSegs;
	segLoopDirct = 1;

	//if we want last segment first, switch loop variables
	if(inToOut){
		firstSeg = numSegs - 1;
		lastSeg = -1;
		segLoopDirct = -1;
	}

	//loops through each segment, seting one led at a time
	for(int i = firstSeg; i != lastSeg; i += segLoopDirct){
		stopPatternCheck();
		yield();
		//if we're going forward then the inner loop needs to run positivly through the segment
		//otherwise it needs to more negetivly
		segLength = segmentSet.getTotalSegLength(i);
		if(forward){  //positive: start from zeroth segment pixel and run to last
			loopDirct = 1;
			startLimit = 0;
			endLimit = segLength;
		} else {  //negative: start from end of segment and run till beginning
			loopDirct = -1;
			startLimit = segLength - 1;
			endLimit = -1;
		}
		for (int j = startLimit; j != endLimit; j += loopDirct) {
			//get the location of the next pixel based on out position in the segement
			ledLocation = getSegmentPixel(segmentSet, i, j);

			//set the pixel color according to the style
			switch(style){
				case 1 : //color wipes each color from pallet in turn
				  color = pallet[ colorPattern[ i % colorPatternLength ] ];
				break;

				case 2 : //color wipes alternating colors from pallet along the wipe
				  color = pallet[ colorPattern[ j % colorPatternLength ] ];
				break;

				case 3 :  //color wipes a rainbow spread evenly across the strip
				  color = Wheel( (ledLocation  * 256 / numLEDs) & 255 );
				break;

				case 4 :  //color wipes a rainbow spreak evenly across each segment
				  color = Wheel( (j * 256 / segLength) & 255);
				break;

				case 5 :  //color wipes a rainbow spreak evenly across each segment
				  color = Wheel( (i * 256 / numSegs) & 255);
				break;

				case 6 :  //color wipes the BgGradientSeg
				  color = getSegBgGradientColor(segmentSet, i);
				break;

				case 7 : //color wipes the BgGradient
				  color = getBgGradientColor(ledLocation);
				break;
			}

			setPixelColor( ledLocation, color );

			show();
			delay(wait);
		}
		//if we're alternating, we need to switch the direction of the next wipe
		if(alternate){
			forward = !forward;
		}
	}
}

//Fill the pixels segment after the other with a color in a rainbow pattern
//(this function can be called from other segmented color wipe functions, except rainbow, by passing -2 as the wipe length)
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
//style is either 3, 4, or 5:
		//3: rainbow mode 1, the rainbow will be evenly distributed across all segements together
		//4: rainbow mode 2, the rainbow will be distributed across each segment individually
		//5: rainbow mode 3, rainbow is distrubuted across segements as groups
void PixelStrip::colorWipeRainbowSec(SegmentSet segmentSet, uint8_t wait, boolean forward, boolean alternate, boolean inToOut, byte style ) {
  colorWipePalletSeg(segmentSet, style, false, 0, 0, 0, -1, wait, forward, alternate);
}

//colorwipes in the radial direction according to a color pattern
//(this function can be called from other segmented color wipe functions, except rainbow, by passing -2 as the wipe length)
//forward sets the direction
//Styles:
	//1: the color pattern is repeated along the longest segment, and lines are drawn from it
	//2: the color pattern is repeated between each segement, eg: each segment is a solid color
	//3: use the BgGradientSeg
	//4: rainbow spread across the longest segment
	//5: use the non segmented BgGradient
	//6: rainbow spread across all segments
void PixelStrip::colorWipeSegRadial(SegmentSet segmentSet, byte colorPattern[], uint8_t colorPatternLength, uint32_t pallet[], byte style, boolean forward, uint8_t wait){
	byte numSegs = segmentSet.numSegs;
    //find the maximum segment length
    uint16_t maxSegLength = segmentSet.maxSegLength;

	uint8_t pattern[numSegs];

	//outer loop limit variables, default is to move in the positive direction
	uint8_t startSeg = 0;
	int8_t endSeg = numSegs;
	int8_t step = 1;
	uint8_t colorModeBg = 1;

	 //if needed, map the style to the correct color mode for drawSimpleLine
	switch(style){
		case 3:
			colorModeBg = 3;
			break;
		case 4:
			colorModeBg = 2;
			break;
		case 5:
			colorModeBg = 4;
			break;
		case 6:
			colorModeBg = 5;
			break;
		default:
			colorModeBg = 1;
  }

	//if we're not going positive, we need to adjust the outer loop variables
	//ie, we need to start the loop at the last segment, and move backwards
	if(!forward){
		startSeg = numSegs - 1;
		endSeg = -1;
	    step = -1;
	}

	//loop through the segments,
	//for each step, loop across the longest segment and draw a line section at each step
	//the line segements are drawn in single pixel chunksa according to the current segment
	//the color of the chunk is determined bu the style, and is put into the pattern[]
	for(int i = startSeg; i != endSeg; i += step){
		stopPatternCheck();
		yield();
		for(int j = 0; j < maxSegLength; j++){

			//set the pixel color according to the style
			switch(style){
				case 1 : //colors follow the color pattern across the longest segment
				  pattern[i] = colorPattern[j % colorPatternLength];
				break;

				case 2 : //colors follow the color pattern, alternating for each segment
				  pattern[i] = colorPattern[i % colorPatternLength];
				break;
			}

			//if style > 2 we're doing a wipe using drawSegLineSimpleSection's special functions
			if(style > 2){
				drawSegLineSimpleSection( segmentSet, i, i, j, 0, colorModeBg);
			} else {
				drawSegLineSection(segmentSet, i, i, j, pattern, pallet, 1, 1, false);
			}
		}
		show();
		delay(wait);
	}

}

// Fill the segments one after the other with a color in a rainbow pattern
//forward == true moves pattern forward, ie incementing pixel steps
//false moves it backward
void PixelStrip::colorWipeRainbowSegRadial(SegmentSet segmentSet, uint8_t wait, boolean forward) {
    byte numSegs = segmentSet.numSegs;
	uint32_t pallet[numSegs];
	byte colorPattern[numSegs];
	for(int i = 0; i < numSegs; i++){
		colorPattern[i] = i;
		pallet[i] = Wheel( (i * 256 / numSegs ) & 255);;
	}
	colorWipeSegRadial(segmentSet, colorPattern, numSegs, pallet, 2, forward, wait);
}
//add mirror mode
//does fireV2Seg for numCycles cycles
void PixelStrip::doFireV2Seg(SegmentSet segmentSet, uint32_t pallet[], uint8_t palletLength, int Cooling, int Sparking, boolean blend, int numCycles, int wait){
	//stripOff(); //clear the strip, b/c fire doesn't if the flame is shorter than the strip
	byte numSegs = segmentSet.numSegs;

	//need to make an RGB pallet for fireV2
	RGB palletRGB[palletLength];
	//recreate the supplied pallet in RGB
	makeRGBpalletFromPallet( pallet, palletRGB, palletLength);

	//The fire works by storing heat values at points on the strip
    //b/c you cannot make variable length arrays, the heat values for the whole segmentSet
    //are stored in one array, using offsets to ensure we only work on the section that
    //corrosponds to the corret segment
    //the offsets are stored in heatSegStarts in order of the segments
    //the heat array is sized by adding up the length of the segments
    //an alternate way do do this would be to have an array of heats with lengths
    //equal to the maxSegLength, but this is memory inefficient
    uint16_t segmentSetLength = 0;
    uint16_t heatSegStarts[numSegs];
    //get the total segmentSet length, and set the starting offsets
    for(int i = 0; i < numSegs; i++){

		heatSegStarts[i] = segmentSetLength; //the start offsets are just the lengths of each segment
		segmentSetLength += segmentSet.getTotalSegLength(i);
    }

	for(int i = 0; i < numCycles; i++){
		stopPatternCheck();
		yield();
		fireV2Seg(segmentSet, heatSegStarts, segmentSetLength, palletRGB, palletLength, Cooling, Sparking, blend, wait);
	}
}

//A traditional fire loop, most useful for strips with a diffuser
//code modified from https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#fire
//draws a a random fire along the strip
//Works with segments, draws an individual fire along each segment
//Note that this doesn't work well with small segments, minimum length is 15+
//takes an RGB pallet[] of colors which are used to generate the fire:
	//the pallet must be ordered from the coldest to the hottest color (generally lightest to darkest)
	//pallets can be of any length, although 3 - 4 colors seems to work best for normal fires
	//try { Color(255, 0, 0), Color(255, 143, 0) , Color(255, 255, 255) } for a tranditional fire
	//or {Color(238, 130, 238),  Color(123, 7, 197) , Color(225, 0, 127), Color(255, 255, 255) } for a pink/purple fire
//The other variables determine how the fire is drawn:
	//Cooling: indicates how fast a flame cools down. More cooling means shorter flames,
	//recommended values are between 20 and 100. 50 seems the nicest.

	//Sparking: indicates the chance (out of 255) that a spark will ignite. A higher value makes the fire more active.
	//Suggested values lay between 50 and 200. 120 works well.
	//wait sets the time between drawing new flames
void PixelStrip::fireV2Seg(SegmentSet segmentSet, uint16_t heatSegStarts[], uint16_t segmentSetLength, RGB pallet[], uint8_t palletLength, int Cooling, int Sparking, boolean blend, int wait) {
    byte numSegs = segmentSet.numSegs;
	uint8_t currentBrightness = getBrightness();

	byte heat[segmentSetLength];


    uint16_t  segLength, heatSecStart, heatIndex, pixelSecNum;
    int16_t ledLocation;
	int cooldown;


    //For each segment do the following:
    for(int j = 0; j < numSegs; j++){
	    segLength = segmentSet.getTotalSegLength(j);
	    heatSecStart = heatSegStarts[j]; //current segment's start point in the heat array

	    // Step 1. Cool down every cell a little
	    for( int i = 0; i < segLength; i++) {

			heatIndex = i + heatSecStart; //adjusted index for heat array
			cooldown = random(0, ((Cooling * 10) / segLength) + 2);

			//if cooldown would turn off a pixel, turn if off,
			//otherwise just cool it down
			if(cooldown > heat[heatIndex]) {
			  heat[heatIndex] = 0;
			} else {
			  heat[heatIndex] = heat[heatIndex]-cooldown;
			}

	    }

	    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
	    for( int k = (segLength - 1); k >= 2; k--) {
			heatIndex = k + heatSecStart; //adjusted index for heat array
			heat[heatIndex] = (heat[heatIndex - 1] + heat[heatIndex - 2] + heat[heatIndex - 2] ) / 3;
	    }

	    // Step 3.  Randomly ignite new 'sparks' near the bottom
	    if( random(255) < Sparking ) {
			int y = random(min( 7, segLength )); //pick a random pixel near the start of the strip
			heatIndex = y + heatSecStart; //adjusted index for heat array
			heat[heatIndex] = heat[heatIndex] + random(160,255); //add a random bit of heat
	    }
		//there is a strange bug where the colors become incorrect, this is probably due to brighness color value destruction
		//setting the brightness to max, and then reseting it fixes this
        setBrightness(255);
	    // Step 4. For each flame, convert heat to LED colors
		for( int i = 0; i < segLength; i++) {
			ledLocation = getSegmentPixel(segmentSet, j, i);
			if(blend){
				setPixelHeatColorPallet( ledLocation, heat[i + heatSecStart], pallet, palletLength);
			} else {
				setPixelHeatColorPalletNoBlend( ledLocation, heat[i + heatSecStart], pallet, palletLength);
			}
		}
		setBrightness(currentBrightness);
    }
    show();
    delay(wait);
}




