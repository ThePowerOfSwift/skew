#include "templ.h"
#include "utils.h"
#include "skew.h"
#include <cv.h>
#include <highgui.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
//#include <tesseract/capi.h>
//#include <leptonica/allheaders.h>

CvRect templCrossCorrelation(IplImage *src, IplImage *templ, int type)
{
    CvRect roi = cvRect(0, templ->height/2, templ->width/2, templ->height/2);
    IplImage *t = cvCreateImage(cvSize(roi.width, roi.height), templ->depth, templ->nChannels);

    cvSetImageROI(templ, roi);
    cvCopy(templ, t, NULL);
    cvResetImageROI(templ);

    IplImage *result = cvCreateImage(cvSize(src->width-t->width+1, src->height-t->height+1), 32, 1);

    cvMatchTemplate(src, t, result, CV_TM_SQDIFF);

    double min_val, max_val;
    CvPoint min_loc, max_loc;

    cvMinMaxLoc(result, &min_val, &max_val, &min_loc, &max_loc, 0);

    CvRect match  = cvRect(min_loc.x, min_loc.y, t->width-1, t->height-1);
    CvRect match2 = cvRect(min_loc.x, min_loc.y - t->height, t->width*2, t->height*2);

    if (match2.y < 0) match2.y = 0;

#ifdef DEBUG
//    cvRectangleR(src, match, CV_RGB(255, 0, 0), 1, 8, 0);
#endif
//    cvNormalize(result, result, 1, 0, CV_MINMAX, NULL);

//    debug(result, "Result", "templ", NULL);
//    debug(t,      "t",      "templ", NULL);
//    debug(src,    "src",    "templ", NULL);
//    debug(templ,  "templ",  "templ", NULL);
    if (type == 1) {
        return match;
    }

    return match2;
}

/*
 * Create template
 */
IplImage *templCreateTempl(char *dir, char *files[], char *filename, int filesc)
{
    IplImage *images[filesc];
    IplImage *image, *templ, *src1, *t, *result, *temp;
    char *filepath;
    int width = 0, height = 0;
    int i;

    for (i = 0; i < filesc; i++) {
        filepath = (char *)malloc(strlen(dir) + strlen(files[i]) + 1);

        strcpy(filepath, dir);
        strcat(filepath, files[i]);

        images[i] = cvLoadImage(filepath, CV_LOAD_IMAGE_COLOR);
        // find max
        width  = CV_IMAX(width, images[i]->width);
        height = CV_IMAX(height, images[i]->height);

        free(filepath);
    }

    // comment this if need max
    for (i = 0; i < filesc; i++) {
        // find min
        width  = CV_IMIN(width, images[i]->width);
        height = CV_IMIN(height, images[i]->height);
    }

    // get template
    templ = images[0];
    for (i = 1; i < filesc; i++) {
        CvRect rect = templCrossCorrelation(images[i], templ, 2);
//        cvRectangleR(images[i], rect, CV_RGB(255, 0, 0), 1, 8, 0);

        cvSetImageROI(images[i], rect);
        temp = cvCreateImage(cvSize(rect.width, rect.height), images[i]->depth, images[i]->nChannels);
        cvCopy(images[i], temp, NULL);

        //cvSmooth(image, image, CV_MEDIAN, 31, 31, 0, 0);
        image = cvCreateImage(cvSize(width, height), images[i]->depth, images[i]->nChannels);
        cvResize(temp, image, CV_INTER_LINEAR);

        CV_SWAP(images[i], image, t);

        cvReleaseImage(&t);
        cvReleaseImage(&temp);
        // save image here!
    }

    // applying template
    src1  = images[1];
    IplImage *res = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    for (i = 2; i < filesc; i++) {
        cvAddWeighted(src1, 0.5, images[i], 0.5, 0, res);
        src1 = res;
        cvReleaseImage(&images[i]);
    }

#ifdef DEBUG
    debug(res, filename, "Templ", 0);
#endif

    if (filename != NULL) {
        filepath = (char *)alloca(strlen(dir) + strlen(filename) + 1);

        strcpy(filepath, dir);
        strcat(filepath, filename);

        cvSaveImage(filepath, res, 0);
    }

    return res;
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

    if (strcmp(templ, TEMPLATE2) == 0) {
        dir = (char *)TEMPLATE_DIR_2;
        filename = (char *)TEMPLATE2;
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

int templateValidate(IplImage *src, IplImage *templ1, IplImage *templ2)
{
    int ret = 0;

    CvRect r1 = templCrossCorrelation(src, templ1, 1);
    CvRect r2 = templCrossCorrelation(src, templ2, 1);

#ifdef DEBUG
//    cvRectangleR(src, r1, CV_RGB(255, 0, 0), 1, 8, 0);
//    cvRectangleR(src, r2, CV_RGB(0, 255, 0), 1, 8, 0);
//    cvLine(src, cvPoint(src->width / 2, 0), cvPoint(src->width / 2, src->height), CV_RGB(0, 0, 255), 1, 8, 0);
//    cvLine(src, cvPoint(0, src->height / 2), cvPoint(src->width, src->height / 2), CV_RGB(0, 0, 255), 1, 8, 0);
#endif

    int min_y = src->height / 2 - r1.height / 2;
    int max_x = src->width / 4;

    if (r1.y > min_y && r1.x < max_x) {
        ret |= TPL_1_NORMAL;
        printf("%d %d ", r1.x, r1.y);
    }

    min_y = src->height / 2 - r2.height / 2;

    if (r2.y > min_y && r2.x < max_x) {
        ret |= TPL_2_NORMAL;
        printf("%d %d ", r2.x, r2.y);
    }

    if (ret == (TPL_1_NORMAL | TPL_2_NORMAL)) {
        if (r1.y > r2.y) {
            ret = TPL_1_NORMAL;
        } else {
            ret = TPL_2_NORMAL;
        }
    }

    return ret;
}

int templatesMatching2(IplImage *src, IplImage *templ1, IplImage *templ2)
{

    int ret = 0;

    if ((ret = templateValidate(src, templ1, templ2)) == 0) {
        skewRotate(src, src, cvPoint2D32f(src->width / 2, src->height / 2), 180.0);
        ret = templateValidate(src, templ1, templ2);
    }

#ifdef DEBUG
//    debug(src, "src", "templ", NULL);
//    debug(templ1, "templ1", "templ", NULL);
//    debug(templ2, "templ2", "templ", NULL);
#endif

    return ret;
}

void templCreateBinMask(IplImage *src)
{

}
