#include "cv.h"

#define DEBUG 1

#define MAX_DEBUG_IMAGES 10

#define DEBUG_OUTPUT_DIR "/tmp/"

#define DEBUG_FILENAME_SEPARATOR "_"

#define DEBUG_IMAGENAME_SEPARATOR ":"

#define DEBUG_OUTPUT_FILE_EXTENTION ".jpg"

typedef struct{
    IplImage *image;
    char     *name;
    char     *filename;
} debugImage;

void debug(IplImage *image, char *imageName, char *moduleName);

void debug_run();

IplImage *morph_temp;

#define MORPH(src, dst, operation, radius, iterations) {                                 \
    int  cols = radius * 2 + 1,                                                          \
         rows = cols;                                                                    \
    IplConvKernel *kern = cvCreateStructuringElementEx(cols, rows, radius, radius,       \
                                                       CV_SHAPE_ELLIPSE, NULL);          \
    cvMorphologyEx(src, dst, morph_temp, kern, operation, iterations);                   \
};
