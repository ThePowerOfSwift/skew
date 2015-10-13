#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cv.h>
#include <highgui.h>
#include <leptonica/allheaders.h>

int countDebugImages = 0;

debugImage *debugImages[MAX_DEBUG_IMAGES];

void debug(IplImage *src, char *imageName, char *moduleName, int drawHist)
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

    CV_CALL( img->title = (char *)cvAlloc(strlen(imageName) +
                       strlen(moduleName) +
                       strlen(DEBUG_IMAGENAME_SEPARATOR)) );

    strcpy(img->title, moduleName);
    strcat(img->title, DEBUG_IMAGENAME_SEPARATOR);
    strcat(img->title, imageName);

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

    if ((img->drawHist = drawHist) != NULL) {
        CV_CALL( img->titleHist[0] = (char *)cvAlloc(strlen(img->title) +
                                        strlen(DEBUG_HIST_WINDOW_TITLE) +
                                        strlen(DEBUG_FILENAME_SEPARATOR) + 1) );
        strcpy(img->titleHist[0], img->title);
        strcat(img->titleHist[0], DEBUG_FILENAME_SEPARATOR);
        strcat(img->titleHist[0], DEBUG_HIST_WINDOW_TITLE);
    }

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
        cvNamedWindow(img->title, CV_WINDOW_NORMAL);
        CV_ASSERT( img->image = cvLoadImage(img->filename, CV_LOAD_IMAGE_COLOR) );
        cvShowImage(img->title, img->image);

        if (img->drawHist != NULL) {
            debugHist *hist;
            int bins = 256;
            float range[] = {0, 255};
            float *ranges[] = { range };
            CvHistogram *histogram = cvCreateHist(1, &bins, CV_HIST_ARRAY, ranges, 1);
            IplImage *r = cvCreateImage(cvGetSize(img->image), 8, 1);
            IplImage *g = cvCreateImage(cvGetSize(img->image), 8, 1);
            IplImage *b = cvCreateImage(cvGetSize(img->image), 8, 1);

            CV_CALL( hist = (debugHist *)cvAlloc(sizeof(debugHist)) );

            cvSplit(img->image, b, g, r, NULL);

            // red
            cvCalcHist(&r, histogram, 0, NULL);
            hist->r = drawHistogram(histogram, 1, 1);
            cvClearHist(histogram);
            CV_CALL (img->titleHist[1] = (char *)cvAlloc(strlen(img->titleHist[0]) +
                                                strlen(DEBUG_HIST_WINDOW_TITLE_RED) + 1) );
            strcpy(img->titleHist[1], img->titleHist[0]);
            strcat(img->titleHist[1], DEBUG_HIST_WINDOW_TITLE_RED);

            // green
            cvCalcHist(&g, histogram, 0, NULL);
            hist->g = drawHistogram(histogram, 1, 1);
            cvClearHist(histogram);
            CV_CALL (img->titleHist[2] = (char *)cvAlloc(strlen(img->titleHist[0]) +
                                                strlen(DEBUG_HIST_WINDOW_TITLE_RED) + 1) );
            strcpy(img->titleHist[2], img->titleHist[0]);
            strcat(img->titleHist[2], DEBUG_HIST_WINDOW_TITLE_GREEN);

            // blue
            cvCalcHist(&g, histogram, 0, NULL);
            hist->b = drawHistogram(histogram, 1, 1);
            cvClearHist(histogram);
            CV_CALL (img->titleHist[3] = (char *)cvAlloc(strlen(img->titleHist[0]) +
                                                strlen(DEBUG_HIST_WINDOW_TITLE_RED) + 1) );
            strcpy(img->titleHist[3], img->titleHist[0]);
            strcat(img->titleHist[3], DEBUG_HIST_WINDOW_TITLE_BLUE);

            cvNamedWindow(img->titleHist[1], CV_WINDOW_NORMAL);
            cvShowImage(img->titleHist[1], hist->r);

            cvNamedWindow(img->titleHist[2], CV_WINDOW_NORMAL);
            cvShowImage(img->titleHist[2], hist->r);

            cvNamedWindow(img->titleHist[3], CV_WINDOW_NORMAL);
            cvShowImage(img->titleHist[3], hist->r);

            img->hist = hist;

            cvReleaseImage(&r);
            cvReleaseImage(&g);
            cvReleaseImage(&g);
            cvReleaseHist(&histogram);
        }
    }

    while (1) {
        if (cvWaitKey(100) == 27) {
            break;
        }
    }

    for (int i = 0; i < countDebugImages; i++ ) {
        img = debugImages[i];
        cvDestroyWindow(img->title);
        cvReleaseImage(&img->image);
        if (img->drawHist != NULL) {
            for(int i = 0; i < 3; i++) {
                cvFree(&img->titleHist[i]);
            }
            cvReleaseImage(&img->hist->b);
            cvReleaseImage(&img->hist->g);
            cvReleaseImage(&img->hist->r);
            cvFree(&img->hist);

        }
        cvFree(&img->filename);
        cvFree(&img->title);
        cvFree(&img);
    }
    __END__;
}

IplImage *drawHistogram(CvHistogram *histogram, float scaleX, float scaleY)
{
    float max = 0;
    cvGetMinMaxHistValue(histogram, 0, &max, NULL, NULL);
    IplImage *img = cvCreateImage(cvSize(256 * scaleX, 64 * scaleY), 8, 1);
    cvZero(img);

    for (int i = 0; i < 255; i++) {
        float value = cvQueryHistValue_1D(histogram, i);
        float next  = cvQueryHistValue_1D(histogram, i+1);
        CvPoint points[5] = {cvPoint(i*scaleX, 64 * scaleY),
                             cvPoint(i * scaleX + scaleX, 64 * scaleY),
                             cvPoint(i * scaleX + scaleX, (64 - next * 64 / max) * scaleY),
                             cvPoint(i * scaleX, (64 - value * 64 / max) * scaleY)};
        points[4] = points[0];
        cvFillConvexPoly(img, points, 5, cvScalar(255, 0, 0, 0), 8, 0);
    }

    return img;
}

//PIX *IplImage2PIX(IplImage *src)
//{
//    IplImage *gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//    cvCvtColor(src, gray, CV_BGR2GRAY);
//    PIX *pixs = pixCreate(gray->width, gray->height, gray->depth);

//    for (int i = 0; i < gray->height; i++) {
//        uchar *ptr = (uchar*) (gray->imageData + i * gray->widthStep);
//        for (int j = 0; j < gray->width; j++) {
//            pixSetPixel(pixs, j, i, (l_uint32)ptr[j]);
//        }
//    }

//#ifdef DEBUG
////    pixDisplay(pixs, 20, 20);
//#endif
//    cvReleaseImage(&gray);

//    return pixs;
//}
