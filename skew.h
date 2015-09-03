#include <cv.h>
#include <highgui.h>

void skew(IplImage *src, IplImage *dst);

void skewGetLines(IplImage *canny, CvMemStorage *storage, CvSeq **lines);

void skewDrawLines(IplImage *src, CvSeq *lines);

int _skewCmpAnglesCallback(const void *a, const void *b);

double skewGetAngle(CvSeq* lines);

void skewRotate(IplImage *src, IplImage *dst, double angle);
