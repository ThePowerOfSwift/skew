

#include <cv.h>
#include <highgui.h>
#include <tesseract/capi.h>
#include <leptonica/allheaders.h>


void textGetResult(IplImage *src, TessBaseAPI *handle);

int textCriteriaMinWidth(int width);

int textCriteriaMinHeight(int height);

int textCriteria(CvRect rect);
