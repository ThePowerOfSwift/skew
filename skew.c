#include "skew.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

void skew(IplImage *src, IplImage *dst)
{
    double angle;
    IplImage *erode, *canny ,*houghlines;
    CvMemStorage* storage;
    CvSeq *lines;
    CvPoint2D32f center;

    CV_FUNCNAME( "skew" );

    __BEGIN__;

    if (!CV_IS_IMAGE( src ))
        CV_ERROR( CV_StsBadArg, "Source must be image");

    if (!CV_IS_IMAGE( dst ))
        CV_ERROR ( CV_StsBadArg, "Destination must be image");


//    CV_8UC3
//    int type = cvGetElemType(src);

    CV_CALL( storage = cvCreateMemStorage(0) );

    erode      = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    canny      = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    houghlines = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

    cvErode(src, erode, NULL, 5);

#ifdef DEBUG
    debug(erode, "Erode", "Skew", NULL);
#endif

    cvCanny(erode, canny, 100, 100, 3);

#ifdef DEBUG
    debug(erode, "Canny", "Skew", NULL);
#endif

    cvCvtColor(canny, houghlines, CV_GRAY2BGR);

    skewGetLines(canny, storage, &lines);

#ifdef DEBUG
    skewDrawLines(houghlines, lines);
    debug(houghlines, "Houghlines", "Skew", NULL);
#endif

    angle = skewGetAngle(lines);

#ifdef DEBUG
        printf("angle = %f\n", angle);
#endif

    center = cvPoint2D32f(src->width / 2, src->height / 2);
    skewRotate(src, dst, center, angle);

    __END__;

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&erode);
    cvReleaseImage(&canny);
    cvReleaseImage(&houghlines);
}

void skewGetLines(IplImage *canny, CvMemStorage* storage, CvSeq **lines)
{

    *lines = cvHoughLines2(canny, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 50, 10, 30, 10);

}

void skewDrawLines(IplImage *src, CvSeq *lines)
{
    for (int i = 0; i < lines->total; i++){
            CvPoint *line = (CvPoint*)cvGetSeqElem(lines, i);
            cvLine(src, line[0], line[1], CV_RGB(0,255,0), 1, CV_AA, 0 );
    }
}

int _skewCmpAnglesCallback(const void *a, const void *b)
{
    const double *da = (const double *)a;
    const double *db = (const double *)b;

    return CV_CMP(*da, *db);
}

double skewGetAngle(CvSeq* lines)
{
    CvPoint* line;
    int i, j, c, max = 0;
    double *angles, angle = 0, a, x, y;

    CV_FUNCNAME( "skew" );

    __BEGIN__;

    CV_CALL( angles = (double *)cvAlloc(lines->total * sizeof(double)) );

    for (i = 0, c = 0; i < lines->total; i++) {

        line = (CvPoint*)cvGetSeqElem(lines, i);
        x = line[1].x - line[0].x;
        y = line[1].y - line[0].y;

        if (x != 0) {
            if ((a = atan(y / x) * 180 / CV_PI) != 0) {
                angles[c++] = ((int)(a * 100 + (a >= 0 ? 0.5 : -0.5))) / 100.0;
            }
        }
    }

    qsort(angles, lines->total, sizeof(double), _skewCmpAnglesCallback);

    for (i = 0; i < lines->total; i++) {

#ifdef DEBUG
        printf("angle: %f\n", angles[i]);
#endif
        for (j = i, c = 0; j < lines->total; j++) {

            if (angles[j] != angles[i]) {
                break;
            }
            c++;
        }

        if (c > max) {
            max = c;
            angle = angles[i];
        }
    }

    __END__;

    cvFree(&angles);
    return angle;
}

void skewRotate(IplImage *src, IplImage *dst, CvPoint2D32f center, double angle)
{
    CvMat* m = cvCreateMat(2, 3, CV_32FC1);

    cv2DRotationMatrix(center, angle, 1, m);
    cvWarpAffine(src, dst, m, CV_INTER_LINEAR, cvScalarAll(0));

    cvReleaseMat(&m);
}

void skewDrawRect(IplImage *src, CvPoint2D32f pt[4])
{
    SKEW_DRAW_LINE(src, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
    SKEW_DRAW_LINE(src, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
    SKEW_DRAW_LINE(src, pt[2].x, pt[2].y, pt[3].x, pt[3].y);
    SKEW_DRAW_LINE(src, pt[3].x, pt[3].y, pt[0].x, pt[0].y);
}
