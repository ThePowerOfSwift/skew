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

int contorsFindBox(IplImage *src, CvSeq *contours, CvBox2D *box);
