#include "skew.h"
#include "contours.h"
#include "utils.h"
#include "templ.h"

#include <stdio.h>
#include <libgen.h>
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

#ifdef DEBUG

    CV_CALL(image = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR));
//    templMatching(image);

//    skew(image, rotated);
//    debug(rotated, "Rotated", "Main");

    contoursGetOutline(image, &rotated);
    debug(rotated, "Rotated", "Main");

    debug_run();

    cvReleaseImage(&image);
    cvReleaseImage(&rotated);
#else
    for (int i = 1; i < argc; i++) {
        filename = basename(argv[i]);
        filepath = (char *)malloc(strlen(output_dir) + strlen(filename)+1);
        strcpy(filepath, output_dir);
        strcat(filepath, filename);

        printf("filename = %s\n", filename);

        CV_CALL(image = cvLoadImage(argv[i], CV_LOAD_IMAGE_COLOR));

        contoursOutline(image, &rotated);

        CV_CALL(cvSaveImage(filepath, rotated, 0));
        free(filepath);
//        free(filename);

        cvReleaseImage(&image);
        cvReleaseImage(&rotated);
    }
#endif
    __END__;
}

