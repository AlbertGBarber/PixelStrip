#include "segmentSet.h"

SegmentSet::SegmentSet(byte numSegments, Segment **segmentsArr) :
    numSegs(numSegments), segArr(segmentsArr)
	
	{
		maxSegLength = getMaxSegLength();	
		segNumMaxSegLength = getsegNumMaxSegLength();	
	}

//returns the maxium length across all the segments
uint16_t  SegmentSet::getMaxSegLength(void){
  uint16_t maxSegLength = 0;
  uint16_t totalLength;
  //walk across all the segments
  //if the segment length is greater than the current maxSegLength, store it as the maxSegLength
  for(int i = 0; i < numSegs; i++ ){
	  totalLength = getTotalSegLength(i);
	  if( totalLength > maxSegLength ){
		maxSegLength = totalLength;
	  }
  }
  return maxSegLength;
}

//returns the segment number that has the maxium length across all the segments
uint16_t  SegmentSet::getsegNumMaxSegLength(void){
  uint16_t totalLength;
  //walk across all the segments
  //if the segment length is equal to the maxSegLength, return it
  for(int i = 0; i < numSegs; i++ ){
	  totalLength = getTotalSegLength(i);
	  //if we find the segment with the right length, break out of the loop and return it
	  if( totalLength == maxSegLength ){
		segNumMaxSegLength = i;
		break; 
	  }
  }
  return segNumMaxSegLength;
}

//returns the specified segment's totalLength (segNum is the segment's index in the segments array passed to the constructor)
uint16_t  SegmentSet::getTotalSegLength(byte segNum){
	return getSegProperty(segNum, 0, 0); //secNum = 0, it's not needed for the call
}

//returns the specified segment's number of sections
uint16_t  SegmentSet::getTotalNumSec(byte segNum){
	return getSegProperty(segNum, 0, 1); //secNum = 0, it's not needed for the call
}

//returns the pointer to the specified segment
uint16_t  SegmentSet::getSectionPtr(byte segNum){
	getSegPtr(segNum);
}

//returns the start pixel of the specified section in the specified segment (secNum is the index of the section within the segment array)
int16_t  SegmentSet::getSecStartPixel(byte segNum, byte secNum){
	return getSegProperty(segNum, secNum, 2);
}

//returns the length of the specified section in the specified segment	
int16_t  SegmentSet::getSecLength(byte segNum, byte secNum){
	return getSegProperty(segNum, secNum, 3);
}

//returns the direction of the specified segment
boolean SegmentSet::getSegDirection(byte segNum){
	Segment *ptr = *(segArr + segNum);
	return ptr->dirct;
}

//fetches properties of the segments and segment sections
//note for some calls, the value of secNum doesn't matter
//prop indcates which property to get:
	//0: the specified segment's totalLength (segNum is the segment's index in the segments array passed to the constructor)
	//1: the specified segment's number of sections
	//2: the start pixel of the specified section in the specified segment (secNum is the index of the section within the segment array)
	//3: the length of the specified section in the specified segment
uint16_t  SegmentSet::getSegProperty(byte segNum, byte secNum, byte prop){
	Segment *ptr = *(segArr + segNum); //the pointer to the segment at index segNum
	switch(prop){
		case 0 :
			return ptr->totalLength;
			break;
		case 1 :
			return ptr->numSec;
			break;
		case 2 :
			return ptr->getSecStartPixel(secNum);
			break;
		case 3 :
			return ptr->getSecLength(secNum);
			break;	
	}
}

//returns the pointer to the specified segment
segmentSection*  SegmentSet::getSegPtr(byte segNum){
	Segment *ptr = *(segArr + segNum); //the pointer to the segment at index segNum
			return ptr->secPtr;
}


//sets the direction of the specified segment to the specified direction
void SegmentSet::setSegDirection(byte segNum, boolean direction){
	Segment *ptr = *(segArr + segNum);
	ptr->dirct = direction;
}

//sets all the segments in the segmentSet to the specified direction
void SegmentSet::setAllSegDirection(boolean direction){
	for(int i = 0; i < numSegs; i++ ){
		setSegDirection(i, direction);
	}
}

//flips the direction of every freq segment, starting with the first segment according to startAtFirst
//ie if you have three segments and do flipSegDirectionEvery(2, true), the first and third segment will be flipped
//but if you do flipSegDirectionEvery(2, false), only the second segment will be flipped
void SegmentSet::flipSegDirectionEvery(byte freq, boolean startAtFirst){
	boolean currentDirect;
	//run over the segments and check if they match the flip frequency,
	//if they do, flip the direction
	for(int i = 0; i < numSegs; i++ ){
		if( checkSegFreq(freq, i, startAtFirst) ){
			currentDirect = getSegDirection(i);
			setSegDirection(i, !currentDirect);
		}
	}
}

//sets the direction of every freq segment, starting with the first segment according to startAtFirst
//ie if you have three segments and do setsegDirectionEvery(2, true, true), the first and third segment will be be set to true
//but if you do flipSegDirectionEvery(2, true, false), only the second segment will be set to true
void SegmentSet::setsegDirectionEvery(byte freq, boolean direction, boolean startAtFirst){
	//run over the segments and check if they match the flip frequency,
	//if they do, set the direction
	for(int i = 0; i < numSegs; i++ ){
		if( checkSegFreq(freq, i, startAtFirst) ){
			setSegDirection(i, direction);
		}
	}
}

//checks if a given segment number occurs in the segmentSet at the given freq, accouting for if the 
//count is started at the first segment or not
//ie checkDirectionFlip(2, 1, true) will return false, b/c we are checking for every other segment, and we started with the first
//but, 	checkDirectionFlip(2, 1, false)	will return true, b/c the we are not starting on the first segment
boolean SegmentSet::checkSegFreq(byte freq, byte num, boolean startAtFirst){
	byte segNum;
	byte testNum;
	//if we're checking the first segment, and startAtFirst is true, we automatically
	//return true
	if(startAtFirst && num == 0){
		return true;
	}
	
	//b/c 0 % (any num) == 0, if we want to start at first, we just use the passed in num
	//otherwise we need to skip ahead one, as we are offsetting from the first segment
	if( startAtFirst ){
		testNum = num;
	} else {
		testNum = num + 1;
	}
	
	//if the segment occurs at the freq, and it isn't off the end of the segmentSet
	//then return true
	if(testNum % freq == 0 && segNum < numSegs){
		return true;
	} else {
		return false;
	}
}