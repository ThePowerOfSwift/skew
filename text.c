#include "utils.h"

#include "text.h"
#include <cv.h>
#include <highgui.h>

#include <tesseract/capi.h>
#include <leptonica/allheaders.h>

void textGetResult(IplImage *src, TessBaseAPI *handle)
{
    IplImage *gray, *sobel, *bin, *temp;
    IplConvKernel *kern;
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contours;
    int ret;
    PIX *pixs = IplImage2PIX(src);

    gray = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U, 1);
    sobel = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U, 1);
    cvCvtColor(src, gray, CV_BGR2GRAY);

    cvSmooth(gray, gray, CV_GAUSSIAN, 9, 0, 0, 0);
#ifdef DEBUG
    debug(gray, "gray", "text", NULL);
#endif

    cvSobel(gray, sobel, 1, 0, 3);
#ifdef DEBUG
    debug(sobel, "sobel", "text", NULL);
#endif
    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvThreshold(sobel, bin, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif

    kern = cvCreateStructuringElementEx(22, 8, 9, 4, CV_SHAPE_ELLIPSE, NULL);
    temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvMorphologyEx(bin, bin, temp, kern, CV_MOP_CLOSE, 1);
    cvReleaseStructuringElement(&kern);

    cvFindContours(bin, storage,
                              &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));


    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
        c = cvApproxPoly(c, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 5, 1);
        CvRect rect = cvBoundingRect(c, 0);

        cvRectangleR(src, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
    }

    TessBaseAPISetImage2(handle, pixs);
    ret = TessBaseAPIRecognize(handle, NULL);

    text = TessBaseAPIGetUTF8Text(handle);

    fputs(text, stdout);

    pixDestroy(&pixs);
    TessDeleteText(text);
//    IplImage *bin, *mask;x

//    bin = cvCreateImage(cvGetSize(templ), IPL_DEPTH_8U, 1);
//    mask = cvCreateImage(cvGetSize(templ), IPL_DEPTH_8U, 1);

//    cvCvtColor(templ, bin, CV_RGB2GRAY);
//    cvAdaptiveThreshold(bin, mask, 250, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5);

//#ifdef DEBUG
//    debug(mask, "mask", "templ", NULL);
//#endif

//    IplImage *rgb, *small, *grad, *temp, *bin, *connect;
//    IplConvKernel *kern;
//    CvMemStorage *storage = cvCreateMemStorage(0);
//    CvSeq *contours;
//    rgb = cvCreateImage(cvSize(templ->width / 2 + 1, templ->height / 2 +1), templ->depth, templ->nChannels);
//    cvPyrDown(templ, rgb, CV_GAUSSIAN_5x5);

//    small = cvCreateImage(cvGetSize(rgb), IPL_DEPTH_8U, 1);

//    cvCvtColor(rgb, small, CV_BGR2GRAY);

//    grad = cvCreateImage(cvGetSize(small), small->depth, small->nChannels);
//    kern = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_ELLIPSE, NULL);
//    temp = cvCreateImage(cvSize(templ->width, templ->height), IPL_DEPTH_8U, 1);
//    cvMorphologyEx(small, grad, temp, kern, CV_MOP_GRADIENT, 1);
//    cvReleaseStructuringElement(&kern);
////    cvRelease();

//    bin = cvCreateImage(cvGetSize(small), small->depth, small->nChannels);
//    cvThreshold(small, bin, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);

//    connect = cvCreateImage(cvGetSize(bin), bin->depth, bin->nChannels);
//    kern = cvCreateStructuringElementEx(9, 1, 0, 0, CV_SHAPE_RECT, NULL);
//    cvMorphologyEx(bin, connect, temp, kern, CV_MOP_CLOSE, 1);
//    cvReleaseStructuringElement(&kern);

//    cvFindContours(connect, storage,
//                              &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));


//    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
//        CvRect rect = cvBoundingRect(c, 0);
//        cvRectangleR(small, rect, cvScalar(0, 0, 255, 0), 2, 8, 0);
//    }


    #ifdef DEBUG
        debug(src, "mask", "templ", NULL);
    #endif
}
