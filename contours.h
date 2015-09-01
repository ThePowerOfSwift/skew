#include <cv.h>
#include <highgui.h>

#define CONTOURS_SHOW 1

void contoursInit();

void contoursDrawBorder(IplImage * image);

void contoursOutline(IplImage *image, int verbose);

CvSeq *countorsGet();

void contoursDraw(CvSeq *countors);

void contoursEnd();
