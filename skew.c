#include "skew.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

CvMemStorage* storage;
IplImage *sk_origin, *sk_erode, *sk_canny ,*sk_houghlines, *sk_rotated;

void skewInit()
{
    storage = cvCreateMemStorage(0);
#ifdef SKEW_SHOW
    cvNamedWindow("OriginSkew",  CV_WINDOW_NORMAL);
    cvNamedWindow("Erode",       CV_WINDOW_NORMAL);
    cvNamedWindow("Canny",       CV_WINDOW_NORMAL);
    cvNamedWindow("HoughtLines", CV_WINDOW_NORMAL);
    cvNamedWindow("Rotated",     CV_WINDOW_NORMAL);
#endif
}

void skew(IplImage *image, int verbose)
{
    double angle;
    sk_origin = image;
    sk_erode = cvCloneImage(image);
    sk_canny = cvCreateImage(cvGetSize(image), 8, 1);
    sk_houghlines = cvCreateImage(cvGetSize(image), 8, 3);
    sk_rotated = cvCloneImage(image);

    cvErode(image, sk_erode, NULL, 5);
    cvCanny(sk_erode, sk_canny, 100, 100, 3);
    cvCvtColor(sk_canny, sk_houghlines, CV_GRAY2BGR);

    CvSeq *lines;
    skewGetLines(sk_canny, &lines);

#ifdef SKEW_SHOW
    skewDrawLines(lines);
#endif

    angle = skewGetAngle(lines);

    if (verbose) {
        printf("angle = %f\n", angle);
    }

    CvPoint2D32f center = cvPoint2D32f(image->width / 2, image->height / 2);

    skewRotate(image, sk_rotated, center, angle);
}

void skewGetLines(IplImage *canny, CvSeq **lines)
{

    *lines = cvHoughLines2(canny, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 50, 10, 30, 10);

}


void skewDrawLines(CvSeq *lines)
{
    int i;
    for (i = 0; i < lines->total; i++){
            CvPoint *line = (CvPoint*)cvGetSeqElem(lines, i);
            cvLine(sk_houghlines, line[0], line[1], CV_RGB(0,255,0), 1, CV_AA, 0 );
    }
}

int skewCmpAnglesCallback(const void *a, const void *b)
{
    const double *da = (const double *) a;
    const double *db = (const double *) b;

    return (*da > *db) - (*da < *db);
}

double skewGetAngle(CvSeq* lines)
{
    int i, j, c, max = 0;
    double angle, a;
    double *angles;
    angles = (double *)malloc(lines->total * sizeof(double));

    for (i = 0, c = 0; i < lines->total; i++) {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
        double x = line[1].x - line[0].x,
               y = line[1].y - line[0].y;
        if (x != 0) {
            if ((a = atan(y / x) * 180 / CV_PI) != 0) {
                angles[c++] = ((int)(a * 100 + (a >= 0 ? 0.5 : -0.5))) / 100.0;
            }
        }
    }
    qsort(angles, lines->total, sizeof(double), skewCmpAnglesCallback);
    for (i = 0; i < lines->total; i++) {
        printf("angle: %f\n", angles[i]);
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
    free(angles);
    return angle;
}

void skewRotate(IplImage *src, IplImage *dst, CvPoint2D32f center, double angle)
{
    CvMat* m = cvCreateMat(2, 3, CV_32FC1);
    cv2DRotationMatrix(center, angle, 1, m);
    cvWarpAffine(src, dst, m ,CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
}

void skewEnd()
{
#ifdef SKEW_SHOW
    cvShowImage("OriginSkew", sk_origin);
    cvShowImage("Erode", sk_erode);
    cvShowImage("Canny", sk_canny);
    cvShowImage("HoughtLines", sk_houghlines);
    cvShowImage("Rotated", sk_rotated);
    cvWaitKey(0);
    cvDestroyWindow("OriginSkew");
    cvDestroyWindow("Erode");
    cvDestroyWindow("Canny");
    cvDestroyWindow("HoughtLines");
#endif
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&sk_erode);
    cvReleaseImage(&sk_houghlines);
    cvReleaseImage(&sk_rotated);
}
