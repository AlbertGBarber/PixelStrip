
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
		totalLength;
	  int16_t
	    getSecStartPixel( byte secNum ),
		getSecLength( byte secNum );
		
	  segmentSection 
		*secPtr;
	  
	private:	
	  uint16_t 
	    getSegTotLen();
};

#endif