#include <cv.h>
#include <highgui.h>

#define CONTOURS_SHOW 1

void contoursInit();

void contoursDrawBorder(IplImage *src);

void contoursOutline(IplImage *src, IplImage *dst);

void countorsGet(IplImage *src, CvMemStorage *storage, CvSeq **contours);

void contoursDraw(IplImage *src, CvSeq *countors);

void contoursEnd();
