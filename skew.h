#include <cv.h>
#include <highgui.h>

#define SKEW_SHOW 1

void skewInit();

void skew(IplImage *image, int verbose);

CvSeq *skewGetLines(IplImage *canny);

void skewDrawLines(CvSeq *lines);

int skewCmpAnglesCallback(const void *a, const void *b);

double skewGetAngle(CvSeq* lines);

void skewRotate(IplImage *src, IplImage *dst, CvPoint2D32f center, double angle);

void skewEnd();


