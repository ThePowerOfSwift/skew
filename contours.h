#include <cv.h>
#include <highgui.h>

#define CONTOURS_SHOW 1

void contoursDrawBorder(IplImage *src);

int contoursOutline(IplImage *src, IplImage *dst);

int contoursGet(IplImage *src, CvMemStorage *storage, CvSeq **contours);

int contorsFindBox(IplImage *src, CvSeq *contours, CvBox2D *box);
