#include <cv.h>
#include <highgui.h>

#define SKEW_DRAW_LINE(src, pt1x, pt1y, pt2x, pt2y) {                                   \
    cvLine(src,                                                                         \
           cvPoint(cvRound(pt1x), cvRound(pt1y)),                                       \
           cvPoint(cvRound(pt2x), cvRound(pt2y)), cvScalar(0, 0, 255, 0), 2, 8, 0);     \
};

void skew(IplImage *src, IplImage *dst);

void skewGetLines(IplImage *canny, CvMemStorage *storage, CvSeq **lines);

void skewDrawLines(IplImage *src, CvSeq *lines);

int _skewCmpAnglesCallback(const void *a, const void *b);

double skewGetAngle(CvSeq* lines);

void skewRotate(IplImage *src, IplImage *dst, CvPoint2D32f center, double angle);

void skewDrawRect(IplImage *src, CvPoint2D32f pt[]);
