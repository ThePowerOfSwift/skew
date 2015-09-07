#include <cv.h>

#define __BEGIN__ __CV_BEGIN__
#define __END__  __CV_END__
#define __EXIT__ __CV_EXIT__

#define MAX_DEBUG_IMAGES 10

#define DEBUG_OUTPUT_DIR "/tmp/"

#define DEBUG_FILENAME_SEPARATOR "_"

#define DEBUG_IMAGENAME_SEPARATOR ":"

#define DEBUG_OUTPUT_FILE_EXTENTION ".jpg"

#define MORPH(src, dst, operation, radius, iterations) {                                 \
    int  cols = radius * 2 + 1,                                                          \
         rows = cols;                                                                    \
    IplImage *temp;                                                                      \
    IplConvKernel *kern;                                                                 \
    temp = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);              \
    CV_CALL(kern = cvCreateStructuringElementEx(cols, rows,                              \
                                                radius, radius,                          \
                                                CV_SHAPE_ELLIPSE, NULL) );               \
    cvMorphologyEx(src, dst, temp, kern, operation, iterations);                         \
    cvReleaseImage(&temp);                                                               \
};                                                                                       \

typedef struct{
    IplImage *image;
    char     *name;
    char     *filename;
} debugImage;

void debug(IplImage *src, char *imageName, char *moduleName);

void debug_run();
