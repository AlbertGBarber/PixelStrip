typedef struct {
  byte segmentNum;
  byte numSections;
  segmentSection *segmentSectionPtr;
  uint16_t totalLength; //= getSegmentTotalLength(segmentSectionPtr, numSections);
} segment;
