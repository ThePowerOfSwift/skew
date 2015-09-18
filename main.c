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
    IplImage *image, *rotated, *templ1;

    CV_FUNCNAME("main");

    __BEGIN__;

#ifdef DEBUG

////    templTempl();

//    CV_ASSERT(image = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR));

//////    templMatching(image);

//////    skew(image, rotated);
//////    debug(rotated, "Rotated", "Main");

//    contoursGetOutline(image, &rotated);


//    templatesMatching2(rotated);

////    debug(rotated, "Rotated", "Main", NULL);

//     debug_run();

//    cvReleaseImage(&image);
//    cvReleaseImage(&rotated);
#else
    char *filepath, *filename, output_dir[] = "/tmp/";

    for (int i = 1; i < argc; i++) {
        int templ = -1;
        filename = basename(argv[i]);
        filepath = (char *)malloc(strlen(output_dir) + strlen(filename)+3);
        strcpy(filepath, output_dir);
//        strcat(filepath, filename);

        printf("filename = %s\n", filename);

        CV_CALL(image = cvLoadImage(argv[i], CV_LOAD_IMAGE_COLOR));

        contoursGetOutline(image, &rotated);

        switch ((templ = templatesMatching2(rotated))) {
//        case TPL_1_180:
//            skewRotate(rotated, rotated, cvPoint2D32f(rotated->width / 2, rotated->height / 2), 180);
        case TPL_1_NORMAL:
              strcat(filepath, "t1/");
//              strcat(filepath, filename);
            break;
//        case TPL_2_180:
//            skewRotate(rotated, rotated, cvPoint2D32f(rotated->width / 2, rotated->height / 2), 180);
        case TPL_2_NORMAL:
            strcat(filepath, "t2/");
//            strcat(filepath, filename);
            break;
        default:
            strcat(filepath, "t/");
            printf("filename = %s is 180\n", filename);
            break;
        }

//        if (templ != -1) {
            strcat(filepath, filename);
            CV_CALL(cvSaveImage(filepath, rotated, 0));
//        }

        free(filepath);
//        free(filename);

//        cvReleaseImage(&image);
        cvReleaseImage(&rotated);
        cvReleaseImage(&image);
    }
#endif
    __END__;
}

