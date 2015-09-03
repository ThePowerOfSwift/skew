#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cv.h>
#include <highgui.h>

int countDebugImages = 0;

debugImage *debugImages[MAX_DEBUG_IMAGES];

void debug(IplImage *src, char *imageName, char *moduleName)
{    
    debugImage *img;

    CV_FUNCNAME( "debug" );

    __BEGIN__;

    if (!CV_IS_IMAGE( src )) {
        CV_ERROR( CV_StsBadArg,
                  "Source must be image");
    }

    CV_ASSERT(countDebugImages <= MAX_DEBUG_IMAGES);

    CV_CALL( img = (debugImage *)cvAlloc(sizeof(debugImage)) );

    CV_CALL( img->name = (char *)cvAlloc(strlen(imageName) +
                       strlen(moduleName) +
                       strlen(DEBUG_IMAGENAME_SEPARATOR)) );

    strcpy(img->name, moduleName);
    strcat(img->name, DEBUG_IMAGENAME_SEPARATOR);
    strcat(img->name, imageName);

    CV_CALL( img->filename = (char *)cvAlloc(strlen(DEBUG_OUTPUT_DIR) +
                           strlen(imageName) +
                           strlen(moduleName) +
                           strlen(DEBUG_FILENAME_SEPARATOR) +
                           strlen(DEBUG_OUTPUT_FILE_EXTENTION) + 1) );

    strcpy(img->filename, DEBUG_OUTPUT_DIR);
    strcat(img->filename, moduleName);
    strcat(img->filename, DEBUG_FILENAME_SEPARATOR);
    strcat(img->filename, imageName);
    strcat(img->filename, DEBUG_OUTPUT_FILE_EXTENTION);

    CV_CALL( cvSaveImage(img->filename, src, 0) );

    debugImages[countDebugImages++] = img;

    __END__;
}

void debug_run()
{
    debugImage *img;

    CV_FUNCNAME("debug_run");

    __BEGIN__;

    for (int i = 0; i < countDebugImages; i++) {
        img = debugImages[i];
        CV_CALL( cvNamedWindow(img->name, CV_WINDOW_NORMAL) );
        CV_CALL( img->image = cvLoadImage(img->filename, CV_LOAD_IMAGE_COLOR) );
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
        cvReleaseImage(&img->image);
        cvFree(&img->filename);
        cvFree(&img->name);
        cvFree(&img);
    }
    __END__;
}
