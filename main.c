#include "skew.h"
#include "contours.h"
#include "utils.h"
#include "templ.h"
#include "text.h"

#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <tesseract/capi.h>
#include <leptonica/allheaders.h>



extern IplImage *morph_temp;

int main(int argc, char *argv[])
{
    IplImage *image, *rotated, *templ1, *templ2;



    int ret;

    CV_FUNCNAME("main");

    __BEGIN__;

//#ifdef DEBUG

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
//#else
    char *filepath, *filename, output_dir[] = "/tmp/";

    templ1 = templGet(TEMPLATE1,
                     (char *[]){"0.png", "1.jpg", "2.jpg", "25.jpg", "37.jpg"},
                     5);

    templ2 = templGet(TEMPLATE2,
                      (char *[]){"0.png", "30.jpg", "34.jpg", "38.jpg", "42.jpg"},
                      5);
#ifdef TEXT
    TessBaseAPI *handle = TessBaseAPICreate();
    ret = TessBaseAPIInit3(handle, NULL, "bel+eng+rus");
#endif
    for (int i = 1; i < argc; i++) {
        int templ = -1;
        filename = basename(argv[i]);
        filepath = (char *)malloc(strlen(output_dir) + strlen(filename)+3);
        strcpy(filepath, output_dir);
//        strcat(filepath, filename);

        CV_CALL(image = cvLoadImage(argv[i], CV_LOAD_IMAGE_COLOR));
//        img = pixRead(argv[i]);




        contoursGetOutline(image, &rotated);


        switch ((templ = templatesMatching2(rotated, templ1, templ2))) {
//        case TPL_1_180:
//            skewRotate(rotated, rotated, cvPoint2D32f(rotated->width / 2, rotated->height / 2), 180);
        case TPL_1_NORMAL:
              strcat(filepath, "t1/");
//              strcat(filepath, filename);
#ifdef TEXT
              textGetResult(rotated, handle);
#else
//              textGetResult(rotated, NULL);
              testMorp(rotated);
#endif



            break;
//        case TPL_2_180:
//            skewRotate(rotated, rotated, cvPoint2D32f(rotated->width / 2, rotated->height / 2), 180);
        case TPL_2_NORMAL:
            strcat(filepath, "t2/");
#ifdef TEXT
              textGetResult(rotated, handle);
#else
//              textGetResult(rotated, NULL);
            testMorp(rotated);
#endif
//            strcat(filepath, filename);
//            textGetResult(rotated);
            break;
        default:
            strcat(filepath, "t/");
#ifdef TEXT
              textGetResult(rotated, handle);
#else
              textGetResult(rotated, NULL);
#endif


            break;
        }



#ifdef DEBUG
    debug_run();
#endif

#ifdef TEXT
        TessBaseAPIEnd(handle);
        TessBaseAPIDelete(handle);
#endif

        strcat(filepath, filename);
        CV_CALL(cvSaveImage(filepath, rotated, 0));
        printf("# %s\n", filename);


        free(filepath);
//        free(filename);

//        cvReleaseImage(&image);
        cvReleaseImage(&rotated);
        cvReleaseImage(&image);
    }



    __END__;

    cvReleaseImage(&templ1);
    cvReleaseImage(&templ2);
}

