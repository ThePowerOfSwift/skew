#include <cv.h>

//#define TEXT 1

#define __BEGIN__ __CV_BEGIN__
#define __END__  __CV_END__
#define __EXIT__ __CV_EXIT__

#define cvQueryHistValue_1D( hist, idx0 ) \
    ((float)cvGetReal1D( (hist)->bins, (idx0)))

#define MAX_DEBUG_IMAGES 10

#define DEBUG_OUTPUT_DIR "/tmp/"

#define DEBUG_FILENAME_SEPARATOR "_"

#define DEBUG_IMAGENAME_SEPARATOR ":"

#define DEBUG_OUTPUT_FILE_EXTENTION ".png"

#define DEBUG_HIST_WINDOW_TITLE "Hist"

#define DEBUG_HIST_WINDOW_TITLE_RED "red"

#define DEBUG_HIST_WINDOW_TITLE_GREEN "green"

#define DEBUG_HIST_WINDOW_TITLE_BLUE "blue"


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
    cvReleaseStructuringElement(&kern);                                                  \
};                                                                                       \

typedef struct {
    IplImage *r;
    IplImage *g;
    IplImage *b;
} debugHist;

typedef struct{
    IplImage  *image;
    char      *title;
    char      *filename;
    int       drawHist;
    char      *titleHist[4];
    debugHist *hist;
} debugImage;

void debug(IplImage *src, char *imageName, char *moduleName, int drawHist);

void debug_run();

IplImage *drawHistogram(CvHistogram *histogram, float scaleX, float scaleY);
