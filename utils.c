#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/cxmisc.h>
#include <highgui.h>

int countDebugImages = 0;

debugImage *debugImages[MAX_DEBUG_IMAGES];

void debug(IplImage *image, char *imageName, char *moduleName)
{
    CV_FUNCNAME("debug");
    debugImage *img;

    CV_ASSERT(countDebugImages <= MAX_DEBUG_IMAGES);

    img = malloc(sizeof(debugImage));

    CV_ASSERT(img);

    img->name = malloc(strlen(imageName) +
                       strlen(moduleName) +
                       strlen(DEBUG_IMAGENAME_SEPARATOR));

    CV_ASSERT(img->name);

    strcpy(img->name, moduleName);
    strcat(img->name, DEBUG_IMAGENAME_SEPARATOR);
    strcat(img->name, imageName);

    img->filename = malloc(strlen(DEBUG_OUTPUT_DIR) +
                           strlen(imageName) +
                           strlen(moduleName) +
                           strlen(DEBUG_FILENAME_SEPARATOR) +
                           strlen(DEBUG_OUTPUT_FILE_EXTENTION) + 1);

    CV_ASSERT(img->filename);

    strcpy(img->filename, DEBUG_OUTPUT_DIR);
    strcat(img->filename, moduleName);
    strcat(img->filename, DEBUG_FILENAME_SEPARATOR);
    strcat(img->filename, imageName);
    strcat(img->filename, DEBUG_OUTPUT_FILE_EXTENTION);

    debugImages[countDebugImages++] = img;

    CV_ASSERT(cvSaveImage(img->filename, image, 0));

exit:
    exit(EXIT_FAILURE);
}

void debug_run()
{
    CV_FUNCNAME("debug");
    debugImage *img;
    for (int i = 0; i < countDebugImages; i++) {
        img = debugImages[i];
        CV_ASSERT(cvNamedWindow(img->name, CV_WINDOW_NORMAL));
        img->image = cvLoadImage(img->filename, CV_LOAD_IMAGE_COLOR);
        CV_ASSERT(img->image);
        cvShowImage(img->name, img->image);
    }

    while (1) {
        if (cvWaitKey(100) == 27) {
            break;
        }
    }

    for (int i = 0; i < countDebugImages; i++ ) {
        img = debugImages[i];
        cvDestroyWindow(img->name);
        free(img->filename);
        free(img->name);
        cvReleaseImage(&img->image);
        free(img);
    }
exit:
    exit(EXIT_FAILURE);
}
