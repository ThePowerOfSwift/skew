#include "skew.h"
#include "contours.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

extern IplImage *morph_temp;

int main(int argc, char *argv[])
{
    IplImage *image, *rotated;

    CV_FUNCNAME("main");

    __BEGIN__;

    CV_CALL(image = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR));

    rotated = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    skew(image, rotated);
#ifdef DEBUG
    debug(rotated, "Rotated", "Main");
#endif

    contoursOutline(rotated, rotated);
#ifdef DEBUG
    debug(rotated, "Countors", "Main");
#endif

    __END__;

#ifdef DEBUG
    debug_run();
#endif

    cvReleaseImage(&image);
    cvReleaseImage(&rotated);
}

