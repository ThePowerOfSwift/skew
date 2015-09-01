#include "contours.h"
#include "main.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

CvMemStorage* storage;
IplImage *cntr_origin, *cntr_gray, *cntr_rgb, *cntr_bin, *cntr_contours;

void contoursInit()
{
    storage = cvCreateMemStorage(0);
#ifdef CONTOURS_SHOW
    cvNamedWindow("OriginContours", CV_WINDOW_NORMAL);
    cvNamedWindow("Binary",         CV_WINDOW_NORMAL);
    cvNamedWindow("Contours",       CV_WINDOW_NORMAL);
#endif
}

void contoursDrawBorder(IplImage * image)
{
    int border = 5;
    cvRectangle(image,
                cvPoint(0, 0),
                cvPoint(image->width - border, image->height - border),
                cvScalar(255, 255, 255, 0), border, 8, 0);
}

void contoursOutline(IplImage *image, int verbose)
{
    cntr_origin   = image;
    cntr_gray     = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    cntr_bin      = cvCreateImage(cvGetSize(image),  IPL_DEPTH_8U, 1);
    cntr_rgb      = cvCreateImage(cvGetSize(image),  IPL_DEPTH_8U, 3);
    cntr_contours = cvCloneImage(image);

    contoursDrawBorder(image);

    cvCvtColor(image, cntr_gray, CV_RGB2GRAY);
    cvAdaptiveThreshold(cntr_gray, cntr_bin, 250, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 1);

    cvCvtColor(cntr_bin, cntr_rgb, CV_GRAY2RGB);
    MORPH(cntr_rgb, cntr_rgb, CV_MOP_OPEN, 3, 1);

    cvCvtColor(cntr_rgb, cntr_gray, CV_RGB2GRAY);
    cvThreshold(cntr_gray, cntr_bin, 195, 255, CV_THRESH_BINARY);

    cvCvtColor(cntr_bin, cntr_rgb, CV_GRAY2RGB);
    MORPH(cntr_rgb, cntr_rgb, CV_MOP_CLOSE, 9, 1);

    cvCvtColor(cntr_rgb, cntr_bin, CV_RGB2GRAY);

#ifdef CONTOURS_SHOW
//    cvShowImage("Binary", cntr_bin);
    debug(cntr_bin, "Binary", "Countors");
#endif

    CvSeq *contours = countorsGet();
#ifdef CONTOURS_SHOW
    contoursDraw(contours);
#endif

}

CvSeq *countorsGet()
{
    CvSeq *contours;
    cvFindContours(cntr_bin, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0 ));
    return contours;
}

void contoursDraw(CvSeq *countors)
{
    CvSeq *countor;
    CvRect rect;
    for (countor = countors; countor != 0; countor = countor->h_next) {
        cvDrawContours(cntr_contours, countor, CV_RGB(255, 216, 0), CV_RGB(0, 0, 250), 0, 1, 8, cvPoint(0, 0));
        rect = cvBoundingRect(countor, 0);
        cvRectangle(cntr_contours,
                    cvPoint(rect.x, rect.y),
                    cvPoint(rect.x + rect.width, rect.y + rect.height),
                    cvScalar(0, 0, 255, 0),
                    2, 8, 0);
    }
}

void contoursEnd()
{
#ifdef CONTOURS_SHOW
    cvShowImage("OriginContours", cntr_origin);
//    cvShowImage("Binary",         cntr_bin);
    cvShowImage("Contours",       cntr_contours);
    cvWaitKey(0);
    cvDestroyWindow("OriginContours");
    cvDestroyWindow("Binary");
    cvDestroyWindow("Contours");
#endif
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&cntr_gray);
    cvReleaseImage(&cntr_bin);
    cvReleaseImage(&cntr_rgb);
    cvReleaseImage(&cntr_contours);
}

