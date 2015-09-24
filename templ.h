#include <cv.h>
#include <highgui.h>

#define TEMPLATE_DIR_1 "/home/andrei/img/pas/templ1/"

#define TEMPLATE_DIR_2 "/home/andrei/img/pas/templ2/"

#define TEMPLATE1 "template1.jpg"

#define TEMPLATE2 "template2.jpg"

#define TPL_1_NORMAL 1

#define TPL_2_NORMAL 2

CvRect templCrossCorrelation(IplImage *src, IplImage *templ, int type);

IplImage *templCreateTempl(char *dir, char *files[], char *filename, int filesc);

int templatesMatching2(IplImage *src, IplImage *templ1, IplImage *templ2);

IplImage *templCreateTempl(char *dir, char *files[], char *filename, int filesc);

IplImage *templGet(char *templ, char *files[], int filesc);

void templCreateBinMask(IplImage *templ);
