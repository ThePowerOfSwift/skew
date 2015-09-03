#include "utils.h"
#include "contours.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

void contoursDrawBorder(IplImage *src)
{
    int border = 5;
    cvRectangle(src,
                cvPoint(0, 0),
                cvPoint(src->width - border, src->height - border),
                cvScalar(255, 255, 255, 0), border, 8, 0);
}

void contoursOutline(IplImage *src, IplImage *dst)
{
    IplImage *gray, *rgb, *bin;
    CvMemStorage *storage;
    CvSeq *contours;

    CV_FUNCNAME( "contoursOutline" );

    __BEGIN__;

    CV_CALL( storage = cvCreateMemStorage(0) );

    gray   = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    bin    = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    rgb    = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

    contoursDrawBorder(src);

    cvCvtColor(src, gray, CV_RGB2GRAY);
    cvAdaptiveThreshold(gray, bin, 250, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 1);

    cvCvtColor(bin, rgb, CV_GRAY2RGB);
    MORPH(rgb, rgb, CV_MOP_OPEN, 3, 1);

    cvCvtColor(rgb, gray, CV_RGB2GRAY);
    cvThreshold(gray, bin, 195, 255, CV_THRESH_BINARY);

    cvCvtColor(bin, rgb, CV_GRAY2RGB);
    MORPH(rgb, rgb, CV_MOP_CLOSE, 9, 1);

    cvCvtColor(rgb, bin, CV_RGB2GRAY);

#ifdef DEBUG
    debug(bin, "Binary", "Contours");
#endif
    countorsGet(bin, storage, &contours);
#ifdef DEBUG
    contoursDraw(dst, contours);
#endif

    __END__;
}

void countorsGet(IplImage *src, CvMemStorage* storage, CvSeq **contours)
{
    CV_FUNCNAME( "countorsGet" );

    __BEGIN__;
    CV_CALL( cvFindContours(src, storage,
                   contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0)) );

   __END__;
}

void contoursDraw(IplImage *src, CvSeq *countors)
{
    CvSeq *contour;
    CvRect rect;
    for (contour = countors; contour != 0; contour = contour->h_next) {
        cvDrawContours(src, contour, CV_RGB(255, 216, 0), CV_RGB(0, 0, 250), 0, 1, 8, cvPoint(0, 0));
        rect = cvBoundingRect(contour, 0);
        cvRectangle(src,
                    cvPoint(rect.x, rect.y),
                    cvPoint(rect.x + rect.width, rect.y + rect.height),
                    cvScalar(0, 0, 255, 0),
                    2, 8, 0);
    }
}
