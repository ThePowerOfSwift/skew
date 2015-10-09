#include "utils.h"

#include "text.h"
#include <cv.h>
#include <highgui.h>

#include <tesseract/capi.h>
#include <leptonica/allheaders.h>

#include <stdio.h>

CvRect *textGetPhoto(IplImage *src, int clear)
{
    IplImage *photo;
    IplConvKernel *element;
    uchar *ptr;

    photo = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    element = cvCreateStructuringElementEx(9, 9, 5, 5, CV_SHAPE_ELLIPSE, NULL);
    cvDilate(src, photo, element, 1);
    cvErode(photo, photo, element, 1);
    cvReleaseStructuringElement(&element);

#ifdef DEBUG
    debug(photo, "photo", "text", NULL);
#endif

    int x = 0, y = 0;
    int maxX = src->width / 2;
    int maxY = src->height - cvRound(src->height * 0.1);
    int offset = 100;

    for (int i = src->height / 2; i < maxY; i++) {
        ptr = (uchar*) (photo->imageData + i * photo->widthStep);
        for (int j = offset; j < maxX; j++) {
            if (ptr[j] == 0x00) {
                if (j >= x) {
                    x = j;
                }
                if (j < (x - 100)) {
                    y = i;
                }
            }
        }
    }

    maxY = y;
    maxX = x;
    int minY = src->height / 2;

    for (int j = maxX; j > offset; j--) {
        for (int i = maxY; i > minY; i--) {
            ptr = (uchar *)(photo->imageData + i * photo->widthStep);
            if (ptr[j] == 0x00) {
                if (i < y) {
                    y = i;
                }
            }
        }
    }

    CvRect rect = cvRect(0, y, maxX, maxY - y);

    if (clear != NULL) {
        cvRectangleR(src, rect, cvScalar(255, 255, 255, 0), CV_FILLED, 8, 0);
    }

#ifdef DEBUG
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(src, rgb, CV_GRAY2BGR);
    cvRectangleR(rgb, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
    debug(rgb, "photoRect", "text", NULL);
    cvReleaseImage(&rgb);
#endif

    cvReleaseImage(&photo);

    return &rect;
}

void textDrawLines(IplImage *src)
{
    assert(cvGetElemType(src) == CV_8UC1);
#ifdef DEBUG
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(src, rgb, CV_GRAY2BGR);
#endif
    uchar *ptr;

    int height = 4,
        dx = 40,
        line_y,
        line_x,
        line_width;

    for (int x = src->width; x > 0; x--) {
        for (int y = src->height; y > 0; y--) {
            ptr = (uchar*) (src->imageData + y * src->widthStep);
            if (ptr[x] == 0x00) {
                line_y = y - height;
                line_x = x;
                line_width = 0;
                ptr = (uchar*) (src->imageData + y * src->widthStep);
//                for (line_width = 0; ptr[line_x - line_width] == 0x00; line_width++) {};

//                if (line_width >= max_width) {
//                    for (int i = 0; i < line_width; i++) {
//                        ptr[line_x - i] == (float)0xff;
//                    }
//                }

                for (int j = 0; j < dx; j++) {
                    if (ptr[line_x--] == 0x00) {
                        j = 0;
                    }
                }

                if ((x - line_x) > dx+1) {
#ifdef DEBUG
                        cvLine(rgb, cvPoint(x, y), cvPoint(line_x + dx, y), cvScalar(0, 0, 255, 0), 1, 8, 0);
#endif
                        cvLine(src, cvPoint(x, y), cvPoint(line_x + dx, y), cvScalar(0, 0, 0, 0), 1, 8, 0);
                }
            }
        }
    }

#ifdef DEBUG
    debug(rgb, "lines", "text", NULL);
#endif
}

CvSeq *getMaskContours(CvMemStorage *storage)
{
    CvSeq *contours;
    IplImage *mask = cvLoadImage("/home/andrei/img/pas/templ1/mask.png", CV_LOAD_IMAGE_GRAYSCALE);
    IplImage *mask2 = cvLoadImage("/home/andrei/img/pas/templ1/mask.png", CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* dst = cvCreateImage(cvGetSize(mask2), IPL_DEPTH_32F, 1);

    cvCornerHarris(mask, dst, 3, 3, 0.04);
#ifdef DEBUG
    debug(mask, "mask", "text", NULL);
#endif
#ifdef DEBUG
    debug(dst, "mask2", "text", NULL);
#endif
    cvFindContours(mask, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

//    cvClearMemStorage(storage);
    return contours;
}
int testHuMoments(CvHuMoments *m1, CvHuMoments *m2)
{
    int round = 100;
    if (m1->hu1 == 0 || m2->hu1 == 0) {
        return 99;
    }
    int mu1 = cvRound(m1->hu1 * round) - cvRound(m2->hu1 * round);
    int mu2 = cvRound(m1->hu2 * round) - cvRound(m2->hu2 * round);

    if (abs(mu1) < 15 && abs(mu2) < 15) {
          return 0;
    }
    return abs(mu1) + abs(mu2);
}

CvSeq *fillContours(CvSeq *contours, CvSeq *fill_contours, IplImage *dst, CvMemStorage *storage)
{
    CvSeq *seq = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvContour), storage);
    double match;
    int result;
//    for (CvSeq *mask = fill_contours; mask != NULL; mask = mask->h_next) {
//        for (CvSeq *contour = contours; contour != NULL; contour = contour->h_next) {
//            match = cvMatchShapes(mask, contour, CV_CONTOURS_MATCH_I3, 0);
//            if (match == 0) {
//                CvRect rect = cvBoundingRect(contour, 0);
//                cvRectangleR(dst, rect, cvScalar(255, 255, 255, 0), 2, 8, 0);
////                cvDrawContours(dst, contour, cvScalar(255, 255, 255, 0), cvScalar(0, 255, 0, 0), -1, 1, 8, cvPoint(0,0));
//                printf("%f\n", match);
//                cvSeqPush(seq, contour);
////                break;
//            }
//        }
//    }

    for (CvSeq *contour = contours; contour != NULL; contour = contour->h_next) {
        CvMoments moments;
        CvHuMoments huMoments;
        cvMoments(contour, &moments, 0);

        cvGetHuMoments(&moments,&huMoments);
        for (CvSeq *mask = fill_contours; mask != NULL; mask = mask->h_next) {
            CvMoments maskMoments;
            CvHuMoments huMomentsMask;
            cvMoments(mask, &maskMoments, 0);

            cvGetHuMoments(&maskMoments,&huMomentsMask);
            match = 1;
//              if (cvRound(huMoments.hu1) == cvRound(huMomentsMask.hu1) && cvRound(huMoments.hu2) == cvRound(huMomentsMask.hu2)) {
//              if (huMoments.hu1 == huMomentsMask.hu1 && huMoments.hu2 == huMomentsMask.hu2) {
//              if (cvRound(huMoments.hu1 * 1000) == cvRound(huMomentsMask.hu1* 1000) && cvRound(huMoments.hu2* 1000) == cvRound(huMomentsMask.hu2* 1000)) {
//            if (cvRound(huMoments.hu1 * 10) == cvRound(huMomentsMask.hu1* 10)) {

            if ((result = testHuMoments(&huMoments, &huMomentsMask)) == 0) {
                CvRect rect = cvBoundingRect(contour, 0);
//                result = testHuMoments(&huMoments, &huMomentsMask);
                printf("%f %f : %f %f = %d \n", huMoments.hu1, huMoments.hu2, huMomentsMask.hu1, huMomentsMask.hu2, result);
                cvRectangleR(dst, rect, cvScalar(255, 255, 255, 0), 2, 8, 0);
            }
//            printf("%d %d : %d %d \n", cvRound(huMoments.hu1 * 1000), cvRound(huMoments.hu2* 1000), cvRound(huMomentsMask.hu1* 1000), cvRound(huMomentsMask.hu2* 1000));

//            match = cvMatchShapes(&huMomentsMask, &huMoments, CV_CONTOURS_MATCH_I3, 0);
//            if (match == 0) {
//                CvRect rect = cvBoundingRect(contour, 0);
//                cvRectangleR(dst, rect, cvScalar(255, 255, 255, 0), 2, 8, 0);
////                cvDrawContours(dst, contour, cvScalar(255, 255, 255, 0), cvScalar(0, 255, 0, 0), -1, 1, 8, cvPoint(0,0));
//                printf("%f\n", match);
//                cvSeqPush(seq, contour);

//            }
        }
//        break;
    }

    return seq;
}

int _textStringsCmpXCallback(const void *a, const void *b)
{
    const CvRect *ra = (const CvRect *)a;
    const CvRect *rb = (const CvRect *)b;

    return CV_CMP(ra->x, rb->x);
}

int _textStringsCmpYCallback(const void *a, const void *b)
{
    const CvRect *ra = (const CvRect *)a;
    const CvRect *rb = (const CvRect *)b;

    return CV_CMP(ra->y, rb->y);
}


void testMorp(IplImage *src)
{
    IplConvKernel *element;
    IplImage *bin, *t1, *filtered, *lines;

    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq *contours;


    filtered = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    t1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    lines = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);


    float data[] = {-0.1, 0.2, -0.1,
                     0.2,  1,   0.2,
                    -0.1, 0.2, -0.1};

    CvMat kernel = cvMat(3, 3, CV_32FC1, data);
    cvFilter2D(src, filtered, &kernel, cvPoint(-1, -1));

#ifdef DEBUG
//    debug(filtered, "filtered", "text", NULL);
#endif

    cvCvtColor(filtered, bin, CV_BGR2GRAY);
    cvThreshold(bin, t1, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    cvReleaseImage(&bin);
    bin = t1;


#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif
    CvRect *photo = textGetPhoto(bin, 1);

    element = cvCreateStructuringElementEx(2, 11, 1, 5, CV_SHAPE_ELLIPSE, NULL);
    cvDilate(bin, lines, element, 1);
    cvReleaseStructuringElement(&element);

    textDrawLines(lines);

#ifdef DEBUG
    debug(lines, "lines2", "text", NULL);
#endif
#ifdef DEBUG
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(bin, rgb, CV_GRAY2BGR);
#endif

    cvFindContours(lines, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

    printf("contours heights => \n");
    uchar *ptr;
    int dx = 30;
    int dy = 5;
    int pad = 3;


    int rc = 0;
    int rs = 0;
    for (CvSeq *c = contours; c != NULL; c = c->h_next)rs++;
    CvRect *rects = calloc(rs, sizeof(CvRect) );

    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
        CvRect rect = cvBoundingRect(c, 0);

        if (rect.height > 6) {
            for (int i = 1; i < dy; i++) {
                int y = rect.y - i;
                if (y <= 0) break;
                ptr = (uchar*) (bin->imageData + y * bin->widthStep);
                for (int j = 0; j < dx; j++) {
                    int x = rect.x - j;
                    if (x <= 0) break;
                    if (ptr[x] == 0x00) {
                        rect.x = x;
                        j = 1;
                    }
                }
                rect.y--;
            }

            int height = 0;
            for (int i = 1; i < dy; i++) {
                int y = rect.y + height + i;
                if(y >= src->height) break;
                ptr = (uchar*) (bin->imageData + y * bin->widthStep);
                int width = 0;
                for (int j = 1; j < dx; j++) {
                    int x = rect.x + width + j;
                    if(x >= src->width) break;
                    if (ptr[x] == 0x00) {
                        width += j;
                        j = 1;
                    }
                }
                if (width > rect.width) {
                    rect.width = width;
                }
                if (width != 0) {
                    height += i;
                    if (height > rect.height) {
                        rect.height = height;
                    }
                    i = 1;
                }
            }


//            rect.y -= pad;
//            rect.x -= pad;
//            rect.width += pad * 2;
//            rect.height += pad * 2;

            memcpy(&rects[rc++], &rect, sizeof(CvRect));
            printf("rect: x = %d y = %d h = %d w = %d\n", rect.x, rect.y, rect.height, rect.width);

        }
    }
    CvRect *strings = realloc(rects, sizeof(CvRect) * rc);
    if (!strings) {
        perror ("realloc");
        exit(EXIT_FAILURE);
    }
    qsort(strings, rc, sizeof(CvRect), _textStringsCmpXCallback);
    qsort(strings, rc, sizeof(CvRect), _textStringsCmpYCallback);


    for (int i = 0; i < rc; i++) {
//        cvRectangleR(src, strings[i], cvScalar(0, 0, 255, 0), 1, 8, 0);
        printf("string: x = %d y = %d h = %d w = %d\n", strings[i].x, strings[i].y, strings[i].height, strings[i].width);
    }

    CvRect * sieve = calloc(rc, sizeof(CvRect));
    int sc = 0;

    for (int i = 0; i < rc; i++) {
        CvRect rect;
        memcpy(&rect, &strings[i], sizeof(CvRect));
        for (int j = i + 1; j <= rc; j++) {
            CvRect s = strings[j];

            if (abs(rect.y - s.y) < 5 && abs(rect.x - s.x) < 5) {
                rect.x = CV_IMIN(rect.x, s.x);
                rect.y = CV_IMIN(rect.y, s.y);
                rect.width = CV_IMAX(rect.width, s.width);
                rect.height = CV_IMAX(rect.height, s.height);
            } else {
                memcpy(&sieve[sc++], &rect, sizeof(CvRect));
                i = j - 1;
                break;
            }
        }
    }

    for (int i = 0; i < sc; i++) {
        cvRectangleR(src, sieve[i], cvScalar(0, 0, 255, 0), 1, 8, 0);
        printf("sieve: x = %d y = %d h = %d w = %d\n", sieve[i].x, sieve[i].y, sieve[i].height, sieve[i].width);
    }

    free(strings);
    free(sieve);
#ifdef DEBUG
    debug(src, "src", "text", NULL);
#endif
    cvReleaseImage(&filtered);
    cvReleaseImage(&bin);
    cvReleaseImage(&lines);

}

void textGetResult(IplImage *src, TessBaseAPI *handle)
{
    IplImage *fg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *bg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *res = cvCreateImage(cvGetSize(src), IPL_DEPTH_32S, 1);
    IplConvKernel *kern;
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contours, *maskCont = getMaskContours(storage);

#ifdef TEXT
    PIX *pixs = IplImage2PIX(src);
    char *text;
#endif
    cvCvtColor(src, bin, CV_BGR2GRAY);
    cvThreshold(bin, bin, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif

    cvErode(bin, fg, NULL, 2);

#ifdef DEBUG
//    debug(fg, "fg", "text", NULL);
#endif
    kern = cvCreateStructuringElementEx(2, 9, 1, 5, CV_SHAPE_ELLIPSE, NULL);
    IplImage *temp = cvCreateImage(cvGetSize(bin), IPL_DEPTH_8U, 1);
    cvMorphologyEx(bin, bg, temp, kern, CV_MOP_CLOSE, 1);
    cvReleaseStructuringElement(&kern);

    kern = cvCreateStructuringElementEx(11, 2, 5, 0, CV_SHAPE_ELLIPSE, NULL);
    cvErode(bg, bg, kern, 3);
    cvReleaseStructuringElement(&kern);

#ifdef DEBUG
    debug(bg, "bg", "text", NULL);
#endif
    cvFindContours(bg, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
//    fillContours(contours, maskCont, src, storage);
#ifdef DEBUG
    debug(bg, "bg1", "text", NULL);
#endif

#ifdef TEXT
    TessBaseAPISetImage2(handle, pixs);
#endif
    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
        CvRect rect = cvBoundingRect(c, 0);

#ifdef TEXT
        TessBaseAPISetRectangle(handle, rect.x, rect.y, rect.width, rect.height);
        text = TessBaseAPIGetUTF8Text(handle);
        printf("%s", text);
        TessDeleteText(text);
#else
        cvRectangleR(src, rect, cvScalar(0, 0, 255, 0), 2, 8, 0);
        cvDrawContours(src, c, cvScalar(0, 0, 255, 0), cvScalar(0, 255, 0, 0), -1, 1, 8, cvPoint(0,0));
#endif
    }

#ifdef DEBUG
    debug(src, "src", "text", NULL);
#endif

#ifdef TEXT
    pixDestroy(&pixs);
#endif

    cvReleaseImage(&bin);
    cvReleaseImage(&fg);
    cvReleaseImage(&bg);
//    IplImage *gray, *sobel, *bin, *temp;
//    IplConvKernel *kern;
//    CvMemStorage *storage = cvCreateMemStorage(0);
//    CvSeq *contours;
//    int ret;

//#ifdef TEXT
//    PIX *pixs = IplImage2PIX(src);
//    char *text;
//#endif

//    gray = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U, 1);
//    sobel = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U, 1);
//    cvCvtColor(src, gray, CV_BGR2GRAY);

//    cvSmooth(gray, gray, CV_GAUSSIAN, 9, 0, 0, 0);
//#ifdef DEBUG
//    debug(gray, "gray", "text", NULL);
//#endif

//    cvSobel(gray, sobel, 1, 0, 3);
////    cvCanny(gray, sobel, 0, 200, 5);
//#ifdef DEBUG
//    debug(sobel, "sobel", "text", NULL);
//#endif
//    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//    cvThreshold(sobel, bin, 100, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

//#ifdef DEBUG
//    debug(bin, "bin", "text", NULL);
//#endif

//    kern = cvCreateStructuringElementEx(22, 4, 6, 3, CV_SHAPE_ELLIPSE, NULL);
//    temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//    cvMorphologyEx(bin, bin, temp, kern, CV_MOP_CLOSE, 1);
//    cvReleaseStructuringElement(&kern);

////    kern = cvCreateStructuringElementEx(9, 1, 2, 0, CV_SHAPE_ELLIPSE, NULL);
//////    temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//////    cvMorphologyEx(bin, bin, temp, kern, CV_MOP_CLOSE, 1);
////    cvDilate(bin, bin, kern, 2);
////    cvReleaseStructuringElement(&kern);



//    cvFindContours(bin, storage,
//                              &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));


//    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
//        c = cvApproxPoly(c, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 5, 1);
//        CvRect rect = cvBoundingRect(c, 0);

////        cvRectangleR(src, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
//        cvDrawContours(src, c, cvScalar(0, 0, 255, 0), cvScalar(0, 255, 0, 0), -1, 1, 8, cvPoint(0,0));
//    }
//#ifdef TEXT
//    TessBaseAPISetImage2(handle, pixs);
//    ret = TessBaseAPIRecognize(handle, NULL);
//    text = TessBaseAPIGetUTF8Text(handle);
//    fputs(text, stdout);
//    pixDestroy(&pixs);
//    TessDeleteText(text);
//#endif

    /////////////////////////////////////////////////
//    IplImage *bin, *mask;x

//    bin = cvCreateImage(cvGetSize(templ), IPL_DEPTH_8U, 1);
//    mask = cvCreateImage(cvGetSize(templ), IPL_DEPTH_8U, 1);

//    cvCvtColor(templ, bin, CV_RGB2GRAY);
//    cvAdaptiveThreshold(bin, mask, 250, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5);

//#ifdef DEBUG
//    debug(mask, "mask", "templ", NULL);
//#endif

//    IplImage *rgb, *small, *grad, *temp, *bin, *connect;
//    IplConvKernel *kern;
//    CvMemStorage *storage = cvCreateMemStorage(0);
//    CvSeq *contours;
//    rgb = cvCreateImage(cvSize(templ->width / 2 + 1, templ->height / 2 +1), templ->depth, templ->nChannels);
//    cvPyrDown(templ, rgb, CV_GAUSSIAN_5x5);

//    small = cvCreateImage(cvGetSize(rgb), IPL_DEPTH_8U, 1);

//    cvCvtColor(rgb, small, CV_BGR2GRAY);

//    grad = cvCreateImage(cvGetSize(small), small->depth, small->nChannels);
//    kern = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_ELLIPSE, NULL);
//    temp = cvCreateImage(cvSize(templ->width, templ->height), IPL_DEPTH_8U, 1);
//    cvMorphologyEx(small, grad, temp, kern, CV_MOP_GRADIENT, 1);
//    cvReleaseStructuringElement(&kern);
////    cvRelease();

//    bin = cvCreateImage(cvGetSize(small), small->depth, small->nChannels);
//    cvThreshold(small, bin, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);

//    connect = cvCreateImage(cvGetSize(bin), bin->depth, bin->nChannels);
//    kern = cvCreateStructuringElementEx(9, 1, 0, 0, CV_SHAPE_RECT, NULL);
//    cvMorphologyEx(bin, connect, temp, kern, CV_MOP_CLOSE, 1);
//    cvReleaseStructuringElement(&kern);

//    cvFindContours(connect, storage,
//                              &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));


//    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
//        CvRect rect = cvBoundingRect(c, 0);
//        cvRectangleR(small, rect, cvScalar(0, 0, 255, 0), 2, 8, 0);
//    }
//    #ifdef DEBUG
//        debug(src, "mask", "templ", NULL);
//    #endif
}
