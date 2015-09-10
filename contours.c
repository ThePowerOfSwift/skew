#include "utils.h"
#include "contours.h"
#include "skew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

void contoursDrawBorder(IplImage *src)
{
    int border = 5;
    cvRectangle(src, cvPoint(0, 0), cvPoint(src->width - border, src->height - border),
                cvScalar(255, 255, 255, 0), border, 8, 0);
}

CvRect contoursGetRect(CvBox2D *box)
{
    CvRect rect;
    rect = cvRect(cvRound(box->center.x - (box->size.width / 2)),
                         cvRound(box->center.y - (box->size.height / 2)),
                         box->size.width, box->size.height);

    // refactor this: if box->size.width > box->size.height

    return rect;
}

int contoursGetOutline(IplImage *src, IplImage *dst)
{
    IplImage *t, *rgb, *bin, *crop;
    CvMemStorage *storage;
    CvSeq *contours;
    CvBox2D box;
    int ret = 0;

    CV_FUNCNAME( "contoursOutline" );

    __BEGIN__;

    CV_CALL( storage = cvCreateMemStorage(0) );

    t    = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    bin  = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    rgb  = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

    contoursDrawBorder(src);

    cvCvtColor(src, bin, CV_RGB2GRAY);
    cvAdaptiveThreshold(bin, bin, 250, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 1);

    cvCvtColor(bin, rgb, CV_GRAY2RGB);
    MORPH(rgb, rgb, CV_MOP_OPEN, 3, 1);

    cvCvtColor(rgb, bin, CV_RGB2GRAY);
    cvThreshold(bin, bin, 195, 255, CV_THRESH_BINARY);

    cvCvtColor(bin, rgb, CV_GRAY2RGB);
    MORPH(rgb, rgb, CV_MOP_CLOSE, 9, 1);


    cvCvtColor(rgb, bin, CV_RGB2GRAY);

#ifdef DEBUG
    debug(bin, "Binary", "Contours");
#endif

    if ((ret = contoursGet(bin, storage, &contours)) <= 0) {
        perror("contoursGet: Contours not found.");
        return -1;
    }

    memset(&box, 0, sizeof(CvBox2D));
    if ((contorsFindBox(src, contours, &box)) != 0) {
        perror("contoursGet: Box not found.");
        return -1;
    }

    skewRotate(src, src, box.center, box.angle);

    CvRect rect = contoursGetRect(&box);
    cvSetImageROI(src, rect);
    crop = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
    cvCopy(src, crop, NULL);
    cvResetImageROI(src);

#ifdef DEBUG
    debug(crop, "Crop", "Contour");
#endif

    // delete this before refactor contoursGetRect()
    if (crop->width > crop->height) {
        IplImage *rotated = cvCreateImage(cvSize(crop->height, crop->width), crop->depth, crop->nChannels);
        cvTranspose(crop, rotated);
        cvFlip(rotated, rotated, -1);

#ifdef DEBUG
        debug(rotated, "Rotated", "contours");
#endif

        cvReleaseImage(&rotated);
    }

    __END__;

    cvReleaseImage(&t);
    cvReleaseImage(&rgb);
    cvReleaseImage(&bin);
    cvReleaseImage(&crop);

    return ret;
}

int contoursGet(IplImage *src, CvMemStorage* storage, CvSeq **contours)
{
    int ret = 0;

    CV_FUNCNAME( "countorsGet" );

    __BEGIN__;

    if (!CV_IS_IMAGE( src ))
            CV_ERROR( CV_StsBadArg, "Source must be image");

    if (!CV_IS_STORAGE( storage ))
            CV_ERROR( CV_StsBadArg, "Storage must be Memory storage");

    ret = cvFindContours(src, storage,
                          contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

   __END__;

   return ret;
}

int contorsFindBox(IplImage *src, CvSeq *contours, CvBox2D *box)
{
    CvBox2D b;
    double area;
    int ret = 1;

    CV_FUNCNAME("contorsGetRect");

    __BEGIN__;

    if (!CV_IS_IMAGE( src ))
        CV_ERROR( CV_StsBadArg, "Source must be image");

    if (!CV_IS_SEQ( contours ))
        CV_ERROR( CV_StsBadArg, "Contours must be Sequence");

    CV_ASSERT ((area = src->width * src->height) > 0);

#ifdef DEBUG
    printf("area=%f\n", area);
#endif

    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
        double contour_area = fabs(cvContourArea(c, CV_WHOLE_SEQ, 0));
        double ratio = area / contour_area;

        if (ratio > 1.5 && ratio < 6.0) {
            b = cvMinAreaRect2(c, NULL);
            memcpy(box, &b, sizeof(CvBox2D));
//#ifdef DEBUG
//            CvPoint2D32f points[4];
//            cvBoxPoints(box, points);
//            skewDrawRect(src, points);
//#endif

#ifdef DEBUG
            double px = b.center.x - (b.size.width / 2);
            double py = b.center.y - (b.size.height / 2);
            IplImage  *_src = cvCloneImage(src);
            SKEW_DRAW_LINE(_src, px, py, b.center.x, b.center.y);
            CvRect rect = cvRect(cvRound(px), cvRound(py), b.size.width, b.size.height);
            cvRectangleR(_src, rect, cvScalar(0, 0, 255, 0), 2, 8, 0);
            debug(_src, "contorsGetRect", "Contours");
            cvReleaseImage(&_src);
#endif
            ret = 0;
            __EXIT__;
        }

#ifdef DEBUG
        printf("contour_area=%f\n", contour_area);
#endif
    }

    __END__;

    return ret;
}



