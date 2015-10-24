#include <cv.h>
#include <highgui.h>

/*
 * Draw contour
 */
void contoursDrawBorder(IplImage *src);

/*
 *  Find Outline
 */
int contoursGetOutline(IplImage *src, IplImage **dst);

CvRect contoursGetRect(CvBox2D *box);

int contoursGet(IplImage *src, CvMemStorage *storage, CvSeq **contours);

int contorsFindBox(IplImage *src, CvMemStorage* storage, CvBox2D *box);

size_t contoursGetSize(CvSeq *contours);

int _contoursCmpXCallback(const void *a, const void *b);

int _contoursCmpYCallback(const void *a, const void *b);

