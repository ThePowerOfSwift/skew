#include "skew.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

int main(int argc, char *argv[])
{

    IplImage *origin = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);

    skewInit();

    skew(origin, 1);

    skewEnd();
}

