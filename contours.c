#include "utils.h"
#include "contours.h"
#include "skew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

size_t contoursGetSize(CvSeq *contours)
{
    CvSeq *c;
    size_t size = 0;
    for (c = contours; c != NULL; c = c->h_next)size++;

    return size;
}


int _contoursCmpXCallback(const void *a, const void *b)
{
    const CvRect *ra = (const CvRect *)a;
    const CvRect *rb = (const CvRect *)b;

    return CV_CMP(ra->x, rb->x);
}

int _contoursCmpYCallback(const void *a, const void *b)
{
    const CvRect *ra = (const CvRect *)a;
    const CvRect *rb = (const CvRect *)b;

    if (abs(ra->y - rb->y) < 5) return 0;

    return CV_CMP(ra->y, rb->y);
}

void contoursDrawBorder(IplImage *src)
{
    int border = 5;
    cvRectangle(src, cvPoint(0, 0), cvPoint(src->width - border, src->height - border),
                cvScalar(255, 255, 255, 0), border, 8, 0);
}

CvRect contoursGetRect(CvBox2D *box)
{
    CvRect rect = cvRect(cvRound(box->center.x - (box->size.width / 2)),
                         cvRound(box->center.y - (box->size.height / 2)),
                         box->size.width, box->size.height);

    // refactor this: if box->size.width > box->size.height

    return rect;
}

IplImage *contoursGetOutlineMorh(IplImage *src, IplImage *temp, int mask)
{
    int radius = 3;
    int cols = radius * 2 + 1;
    int rows = cols;
    IplImage *res;
    IplImage *bin  = cvCreateImage(cvGetSize(src), src->depth, 1);

    cvAdaptiveThreshold(src, bin, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 1);

    if (mask == 1) {
        IplImage *mask = cvCreateImage(cvGetSize(src), src->depth, 1);
        res = cvCreateImage(cvGetSize(src), src->depth, 1);
        cvThreshold(src, mask, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
        cvOr(bin, mask, res, NULL);

        cvReleaseImage(&mask);
    } else {
        res = bin;
    }

    IplConvKernel *element = cvCreateStructuringElementEx(cols, rows, radius, radius, CV_SHAPE_ELLIPSE, NULL);

    cvMorphologyEx(res, res, temp, element, CV_MOP_OPEN, 1);
    cvReleaseStructuringElement(&element);

    radius = 9;
    cols = radius * 2 + 1;
    rows = cols;
    element = cvCreateStructuringElementEx(cols, rows, radius, radius, CV_SHAPE_ELLIPSE, NULL);
    cvMorphologyEx(res, res, temp, element, CV_MOP_CLOSE, 1);
    cvReleaseStructuringElement(&element);

    radius = 7;
    cols = radius * 2 + 1;
    rows = cols;
    element = cvCreateStructuringElementEx(cols, rows, radius, radius, CV_SHAPE_ELLIPSE, NULL);
    cvErode(res, res, element, 1);
    cvDilate(res, res, element, 1);

    contoursDrawBorder(res);

    cvReleaseStructuringElement(&element);
    cvReleaseImage(&temp);

    return res;
}

int contoursGetOutline(IplImage *src, IplImage **dst)
{    
    CvBox2D box;
    CvMemStorage *storage = cvCreateMemStorage(0);
    IplImage *gray = cvCreateImage(cvGetSize(src), src->depth, 1);
    IplImage *temp = cvCreateImage(cvGetSize(src), src->depth, 1);

    cvCvtColor(src, gray, CV_RGB2GRAY);

    IplImage *mop = contoursGetOutlineMorh(gray, temp, 1);
    if ((contorsFindBox(mop, storage, &box)) == 1) {
       cvReleaseImage(&mop);
       mop = contoursGetOutlineMorh(gray, temp, 0);
    }

    if ((contorsFindBox(mop, storage, &box)) != 0) {
        perror("contoursFindBox: Box not found.");
        exit(EXIT_FAILURE);
    }

    IplImage *rotated = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    skewRotate(src, rotated, box.center, box.angle);

    CvRect rect = contoursGetRect(&box);
    cvSetImageROI(rotated, rect);

    IplImage *crop = cvCreateImage(cvGetSize(rotated), IPL_DEPTH_8U, 3);
    cvCopy(rotated, crop, NULL);

    cvResetImageROI(rotated);

    // delete this before refactor contoursGetRect()
    if (crop->width > crop->height) {
        cvReleaseImage(&rotated);
        rotated = cvCreateImage(cvSize(crop->height, crop->width), crop->depth, crop->nChannels);
        cvTranspose(crop, rotated);
        cvFlip(rotated, rotated, 1);

        *dst = cvCloneImage(rotated);
        cvReleaseImage(&rotated);
    } else {
        *dst = cvCloneImage(crop);
    }

    cvReleaseImage(&crop);
    cvReleaseImage(&mop);
    cvReleaseMemStorage(&storage);

    return 0;
}


int contorsFindBox(IplImage *src, CvMemStorage* storage, CvBox2D *box)
{
    CvSeq *contours;
    int ret;
    double area;
    assert((area = src->width * src->height) > 0);

    ret = cvFindContours(src, storage,
                              &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
    if (ret == 0) return 1;

    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
        c = cvApproxPoly(c, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 5, 1);
        double contour_area = fabs(cvContourArea(c, CV_WHOLE_SEQ, 0));
        double ratio = area / contour_area;

        if (ratio > 1.5 && ratio < 6.0) {
            CvBox2D b = cvMinAreaRect2(c, NULL);
            memcpy(box, &b, sizeof(CvBox2D));

            return 0;
        }
    }

    return 1;
}



