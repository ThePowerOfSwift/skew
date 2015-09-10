#include "templ.h"
#include "utils.h"
#include "skew.h"
#include <cv.h>
#include <highgui.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>

/*
 * Create template
 */
IplImage *templCreateTempl(char *dir, char *files[], char *filename, int filesc)
{
    IplImage *images[filesc];
    IplImage *image, *templ;
    char *filepath;
    int width = 0, height = 0;
    int i;

    for (i = 0; i < filesc; i++) {
        filepath = (char *)malloc(strlen(dir) + strlen(files[i]) + 1);
        if (!filepath)
            perror("malloc");

        strcpy(filepath, dir);
        strcat(filepath, files[i]);

        image = cvLoadImage(filepath, CV_LOAD_IMAGE_COLOR);
        width = CV_IMAX(width, image->width);
        height = CV_IMAX(height, image->height);
        images[i] = image;

        free(filepath);
    }

    for (i = 0; i < filesc; i++) {
        image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

        cvResize(images[i], image, CV_INTER_LINEAR);
        cvSmooth(image, image, CV_MEDIAN, 31, 31, 0, 0);

        cvReleaseImage(&images[i]);
        images[i] = image;

        // save image here!
    }

    templ = cvCloneImage(images[0]);

    for (i = 1; i < filesc; i++) {
        cvAddWeighted(templ, 0.5, images[i], 0.5, 0.0, templ);
    }
#ifdef DEBUG
    debug(templ, filename, "Templ");
#endif

    if (filename != NULL) {
        filepath = (char *)alloca(strlen(dir) + strlen(filename) + 1);
        if (!filepath)
            perror("malloc");
        strcpy(filepath, dir);
        strcat(filepath, filename);

        cvSaveImage(filepath, templ, 0);
    }

    return templ;
}

IplImage *templGet(char *templ, char *files[], int filesc)
{
    char *dir = NULL;
    char *filename = NULL;
    IplImage *t = NULL;

    if (strcmp(templ, TEMPLATE1) == 0) {
        dir = (char *)TEMPLATE_DIR_1;
        filename = (char *)TEMPLATE1;
    }

    if (strcmp(templ, TEMPLATE_DIR_2) == 0) {
        dir = (char *)TEMPLATE_DIR_2;
        filename = (char *)TEMPLATE1;
    }

    if (dir != NULL && filename != NULL) {
        char *filepath = (char *)alloca(strlen(dir) + strlen(filename) + 1);
        strcpy(filepath, dir);
        strcat(filepath, filename);

        if ((t = cvLoadImage(filepath, CV_LOAD_IMAGE_COLOR)) == NULL) {
            t = templCreateTempl(dir, files, filename, filesc);
        }
    }

    return t;
}

void templMatching(IplImage *src)
{
    IplImage *templ1, *templ2, *image, *diff;

    CvScalar avg[4];

    templ1 = templGet(TEMPLATE1,
                     (char *[]){"1.jpg", "2.jpg", "4.jpg", "9.jpg"},
                                    4);
    image = cvCreateImage(cvGetSize(templ1), IPL_DEPTH_8U, 3);
    diff = cvCreateImage(cvGetSize(templ1), IPL_DEPTH_8U, 3);

    cvResize(src, image, CV_INTER_LINEAR);
    cvSmooth(image, image, CV_MEDIAN, 31, 31, 0, 0);

    cvSub(templ1, image, diff, NULL);
    avg[0] = cvAvg(diff, NULL);

#ifdef DEBUG
    debug(diff, "sub1", "templ");
#endif

    skewRotate(templ1, templ1, cvPoint2D32f(templ1->width / 2.0, templ1->height / 2.0), 180);
    cvSub(templ1, image, diff, NULL);
    avg[1] = cvAvg(diff, NULL);

#ifdef DEBUG
    debug(diff, "sub2", "templ");
#endif
}
