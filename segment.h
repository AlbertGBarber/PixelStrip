
#ifndef segment_h
#define segment_h

//need to inclued arduino here to get it to compile
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include "segmentSection.h"

//for class explanation see segmentSet.h
class Segment {
	
	public:
		Segment(byte numSections, segmentSection *segSectionsArr, boolean direction);
		
	  byte
		numSec;
	 
	  boolean
		dirct;
		
	  uint16_t
		totalLength,
		getSecStartPixel( byte secNum );
	  int16_t
		getSecLength( byte secNum );
		
	  segmentSection 
		*secPtr;
	  
	private:	
	  uint16_t 
	    getSegTotLen();
};

#endif