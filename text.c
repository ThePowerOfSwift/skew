#include "utils.h"
#include "text.h"
#include <cv.h>
#include <highgui.h>
#include <tesseract/capi.h>
#include <leptonica/allheaders.h>
#include <stdio.h>
#include "contours.h"

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

    int *fx = calloc(photo->width / 2, sizeof(int));
    for (int j = photo->width / 2, max = photo->height / 2; j > 0; j--) {
        for (int i = photo->height -1; i > max; i--) {
        ptr = (uchar*) (photo->imageData + i * photo->widthStep);
            if (ptr[j] == 0x00) {
                fx[j]++;
            }
        }
    }

    int maxval = 0;
    int minval = photo->height / 2;
    for (int i = photo->width / 2 - 1; i > 0; i--) {
        printf("x = %d %d\n", i, fx[i]);
        maxval = CV_IMAX(maxval, fx[i]);
        minval = CV_IMIN(minval, fx[i]);
   }

   int dx = 5;
   int minlocx;
   int maxlocx;
   for (int i = photo->width / 2 - 1; i > 0; i--) {
       if (fx[i] == maxval) {
           for (int j = 0, left = i; j < dx; j++, left--) {
               if (left <= 0) break;
               if (fx[left] > minval) {
                   minlocx = left;
                   j = 0;
               }
           }

           for (int j = 0, right = i; j < dx; j++, right++) {
               if (right >= photo->width / 2 -1) break;
               if (fx[right] > minval) {
                   maxlocx = right;
                   j = 0;
               }
           }
           break;
       }
   }

    printf("max: %d\n", maxval);
    printf("minloc: %d\n", minlocx);
    printf("maxloc: %d\n", maxlocx);

    int *fy = calloc(photo->height, sizeof(int));
    for (int i = photo->height -1, max = photo->height / 2; i > max; i--) {
        ptr = (uchar*) (photo->imageData + i * photo->widthStep);
        for (int j = maxlocx; j > minlocx; j--) {
            if (ptr[j] == 0x00) {
                fy[i]++;
            }
        }
    }

    maxval = 0;
    minval = photo->height / 2;
    for (int i = photo->height -1, max = photo->height / 2; i > max; i--) {
        printf("y = %d %d\n", i, fy[i]);
        maxval = CV_IMAX(maxval, fy[i]);
//        minval = CV_IMIN(min, fy[i]);
    }

    printf("max: %d\n", maxval);
    printf("minloc: %d\n", minlocx);
    printf("maxloc: %d\n", maxlocx);

//    int x = 0, y = 0;
//    int maxX = src->width / 2;
//    int maxY = src->height - cvRound(src->height * 0.1);
//    int offset = 100;

//    for (int i = src->height / 2; i < maxY; i++) {
//        ptr = (uchar*) (photo->imageData + i * photo->widthStep);
//        for (int j = offset; j < maxX; j++) {
//            if (ptr[j] == 0x00) {
//                if (j >= x) {
//                    x = j;
//                }
//                if (j < (x - 100)) {
//                    y = i;
//                }
//            }
//        }
//    }

//    maxY = y;
//    maxX = x;
//    int minY = src->height / 2;

//    for (int j = maxX; j > offset; j--) {
//        for (int i = maxY; i > minY; i--) {
//            ptr = (uchar *)(photo->imageData + i * photo->widthStep);
//            if (ptr[j] == 0x00) {
//                if (i < y) {
//                    y = i;
//                }
//            }
//        }
//    }

//    CvRect rect = cvRect(0, y, maxX, maxY - y);

    CvRect rect = cvRect(0, 1, 1, 1);

    if (clear != NULL) {
        cvRectangleR(src, rect, cvScalar(255, 255, 255, 0), CV_FILLED, 8, 0);
    }

#ifdef DEBUG
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(src, rgb, CV_GRAY2BGR);
    cvRectangleR(rgb, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
    cvLine(photo, cvPoint(minlocx, rgb->height / 2), cvPoint(maxlocx, rgb->height / 2), cvScalar(0, 0, 255, 0), 2, 8, 0);
    debug(rgb, "photoRect", "text", NULL);
    cvReleaseImage(&rgb);
#endif
#ifdef DEBUG
    debug(photo, "photo", "text", NULL);
#endif
    cvReleaseImage(&photo);

    return &rect;
}

void textDrawLines4(IplImage *src, int lheight)
{
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(src, rgb, CV_GRAY2BGR);
    uchar *ptr;
    int dy = 200;
    int dist1 = 0;
    int minheight = 10;

    for (int x = src->width / 2; x < src->width; x++) {
        for (int y = src->height; y > src->height - 200; y--) {
            ptr = (uchar *)(src->imageData + y * src->widthStep);
            if (ptr[x] == 0x00) {
                int height = 0;
                for (int i = y; ptr[x] == 0x00; i--, height++) {
                    ptr = (uchar *)(src->imageData + i * src->widthStep);
                }
                if (height > minheight) {

                }
            }
        }
    }
}

void textDrawLines3(IplImage *src, int lheight)
{
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(src, rgb, CV_GRAY2BGR);

    uchar *ptr, *pix;
    int dx = 20;

    for (int y = src->height-1; y > 0; y--) {
        ptr = (uchar *)(src->imageData + y * src->widthStep);
        for (int x = src->width - 1; x > 0; x--) {
            if (ptr[x] == 0x00) {
                int width = 1;
                for (;x > 0 && ptr[x] == 0x00; x--) {
                    width++;
                }
                if (width > dx) {
                    int j = width + x, jmin = j - dx;
                    int height = 0;
                    for (; j > jmin; j--) {
                        int i = y;
                        pix = (uchar *)(src->imageData + i * src->widthStep);
                        int h = 0;
                        for (; i > 0 && pix[j] == 0x00; i--) {
                            pix = (uchar *)(src->imageData + i * src->widthStep);
                            h++;
                            height = CV_IMAX(height, h);

                        }
                        i = y+1;
                        pix = (uchar *)(src->imageData + i * src->widthStep);
                        for (; i < src->height && pix[j] == 0x00; i++) {
                            pix = (uchar *)(src->imageData + i * src->widthStep);
                            h++;
                            height = CV_IMAX(height, h);
                        }
                    }

                    if (height > 0 && height < (lheight / 2)) {
                        for (j = width + x; j > x; j--) {
                            int i = y;
                            pix = (uchar *)(src->imageData + i * src->widthStep);
//                            int h = 0;
                            for (; pix[j] == 0x00; i--) {
//                                pix = (uchar *)(src->imageData + i * src->widthStep);
//                                h++;
//                                height = CV_IMAX(height, h);
                                pix[j] = 0xff;
                            }
                            i = y+1;
                            pix = (uchar *)(src->imageData + i * src->widthStep);
                            for (; i < src->height && pix[j] == 0x00; i++) {
                                pix = (uchar *)(src->imageData + i * src->widthStep);
//                                h++;
//                                y++;
//                                height = CV_IMAX(height, h);
                                pix[j] = 0xff;
                            }
                        }
//                        cvRectangleR(rgb, cvRect(x, y-height, width, height), cvScalar(0, 0, 255, 0), CV_FILLED, 8, 0);
//                        cvRectangleR(src, cvRect(x, y-height, width, height), cvScalar(255, 255, 255, 0), CV_FILLED, 8, 0);
                    }
                }
            }
        }
    }
#ifdef DEBUG
    debug(rgb, "line3", "text", NULL);
#endif
}

int textGetMaxLineHeight(IplImage *src, int sx, int sy, int xmin, int ymin)
{
    uchar *ptr;
    int max = 0;
    sx = sx == 0 ? src->width - 100 : sx;
    xmin = xmin == 0 ? src->width / 2 : xmin;
    sy = sy == 0 ? src->height - 1 : sy;
    ymin = ymin == 0 ? src->height / 2 - 100 : ymin;

    for (int x = sx; x > xmin; x--) {
        max = 0;
        for (int y = sy, lheight = 0; y > ymin; y--) {
            ptr = (uchar *) (src->imageData + y * src->widthStep);
            if (ptr[x] == 0x00) {
                lheight++;
                max = CV_IMAX(max, lheight);
            } else {
                lheight = 0;
            }
        }
    }

    return max;
}

void textDrawLines2(IplImage *src)
{
    uchar *ptr;
    int dx = 30;

    for (int x = src->width; x > 0; x--) {
        for (int y = src->height; y > 0; y--) {
            ptr = (uchar*) (src->imageData + y * src->widthStep);
            if (ptr[x] == 0x00) {
                int lwidth = 0;
                for (int j = 0; j < dx; j++) {
                    int lx = x + lwidth + j;
                    if (lx >= src->width) break;
                    if (ptr[lx] == 0x00) {
                        lwidth += j;
                        j = 0;
                    }
                }
                if (lwidth != 0) {
                    cvLine(src, cvPoint(x, y), cvPoint(x + lwidth, y), cvScalar(0, 0, 0, 0), 1, 8, 0);
                }
            }
        }
    }
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

    if (abs(ra->y - rb->y) < 5) return 0;

    return CV_CMP(ra->y, rb->y);
}


CvRect textGetLineRect(IplImage *src, CvPoint pt)
{
    int step = src->widthStep/sizeof(uchar);
    uchar* data = (uchar *)src->imageData;

    CvRect rect = cvRect(pt.x, pt.y, 0, 0);

    CvPoint e = cvPoint(pt.x, pt.y);
    CvPoint s = cvPoint(0, 0);

    CvPoint min = cvPoint(src->width, src->height);
    CvPoint max = cvPoint(0, 0);

    int x = pt.x;
    int y = pt.y;

    uchar b = data[(y + 1) * step + x];
    uchar bl = data[(y + 1) * step + x - 1];
    uchar l = data[y * step + x - 1];
    uchar tl = data[(y - 1) * step + x - 1];
    uchar t = data[(y - 1) * step + x];
    uchar tr = data[(y - 1) * step + x + 1];
    uchar r = data[y * step + x + 1];
    uchar br = data[(y + 1) * step + x + 1];

    x = pt.x;
    y = pt.y;

    while (s.x != e.x || s.y != e.y) {
        if ((b = data[(y + 1) * step + x]) == 0xff) {
            while ((l = data[y * step + x - 1]) == 0x00 && (bl = data[(y + 1) * step + x - 1]) == 0xff) {         // left
                x--;
                if (x == e.x && y == e.y) goto end;
            }
        }

        while((bl = data[(y + 1) * step + x - 1]) == 0x00 && (b = data[(y + 1) * step + x]) == 0xff) {        // bottom-left
            y++;
            x--;
            if (x == e.x && y == e.y) goto end;
        }
        min.x = CV_IMIN(min.x, x);

        if ((b = data[(y + 1) * step + x]) == 0x00) {
            if ((r = data[y * step + x + 1]) == 0xff) {
                while ((b = data[(y + 1) * step + x]) == 0x00 && (br = data[(y + 1) * step + x + 1]) == 0xff) {       // bottom
                    y++;
                    if (x == e.x && y == e.y) goto end;
                }
            }
        }
        max.y = CV_IMAX(max.y, y);

        if ((b = data[(y + 1) * step + x]) == 0x00) {
            while ((br = data[(y + 1) * step + x + 1]) == 0x00 && (r = data[y * step + x + 1]) == 0xff) {         // bottom-right
                y++;
                x++;
                if (x == e.x && y == e.y) goto end;
            }
        }
        max.y = CV_IMAX(max.y, y);

        if ((t = data[(y - 1) * step + x]) == 0xff) {
            while ((r = data[y * step + x + 1]) == 0x00 && (tr = data[(y - 1) * step + x + 1]) == 0xff) {         // right
                x++;
                if (x == e.x && y == e.y) goto end;
            }
        }
        max.x = CV_IMAX(max.x, x);

        if ((b = data[(y + 1) * step + x]) == 0x00) {
            while ((tr = data[(y - 1) * step + x + 1]) == 0x00 && (t = data[(y - 1) * step + x]) == 0xff) {       // top-right
                y--;
                x++;
                if (x == e.x && y == e.y) goto end;
            }
        }
        max.x = CV_IMAX(max.x, x);

        if ((l = data[y * step + x - 1]) == 0xff) {
            while ((t = data[(y - 1) * step + x]) == 0x00 && (tl = data[(y - 1) * step + x - 1]) == 0xff) {      // top
                y--;
                if (x == e.x && y == e.y) goto end;
            }
        }
        if ((b = data[(y + 1) * step + x]) == 0x00) {
            while ((tl = data[(y - 1) * step + x - 1]) == 0x00 && (l = data[y * step + x - 1]) == 0xff) {       // top-left
                y--;
                x--;
                if (x == e.x && y == e.y) goto end;
            }
        }
        min.y = CV_IMIN(min.y, y);
        min.x = CV_IMIN(min.x, x);

        end:;

        s.x = x;
        s.y = y;
    }

    rect.x = min.x;
    rect.y = min.y;

    rect.height = max.y - min.y;
    rect.width = max.x - min.x;

    rect.height = rect.height <= 0 ? 1 : rect.height + 1;
    rect.width  = rect.width <= 0 ? 1 : rect.width + 1;

    return rect;
}

void textClearLine(IplImage *src, CvRect line)
{
    uchar *ptr;

    int x = line.x;
    int y = line.y - line.height / 2;
    int ymax = line.y;
    int xmax = line.x + line.width;

    for (; y < ymax; y++) {
        ptr = (uchar*) (src->imageData + y * src->widthStep);
        for (; x < xmax; x++) {
            if (ptr[x] == 0x00) {
                CvRect rect = textGetLineRect(src, cvPoint(x, y));
                cvRectangleR(src, rect, cvScalar(255, 255, 255, 0), CV_FILLED, 8, 0);
//                debug(src, "clear", "text", NULL);
            }
        }
    }
}

void textClear(IplImage *src)
{
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    IplImage *img = cvCloneImage(src);
    cvCvtColor(src, rgb, CV_GRAY2BGR);

    uchar *ptr;

    for (int y = src->height - 50; y > 100; y--) {
        ptr = (uchar*) (img->imageData + y * img->widthStep);
        int x = src->width - 10;
        for (; x > 10; x--) {
            if (ptr[x] == 0x00) {
                CvRect rect = textGetLineRect(img, cvPoint(x, y));
                x = rect.x;
                cvRectangleR(img, rect, cvScalar(255, 255, 255, 0), CV_FILLED, 8, 0);
                textClearLine(src, rect);

                cvRectangleR(rgb, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
//                debug(rgb, "clear", "text", NULL);
//                return;
            }
        }
    }
    debug(rgb, "clear", "text", NULL);
    debug(img, "clearVal", "text", NULL);
}

int textCriteriaMinWidth(int width)
{
    static w = 0;
    int d = 20; // default;

    if (width != 0) w = width;
    if (w == 0) w = d;

    return w;
}

int textCriteriaMinHeight(int height)
{
    static h = 0;
    int d = 5; // default

    if (height != 0) h = height;
    if (h == 0) h = d;

    return h;
}

int textCriteria(CvRect rect)
{
    int c1 = CV_CMP(rect.height, textCriteriaMinHeight(0));
    int c2 = CV_CMP(rect.width, textCriteriaMinWidth(0));

    return CV_CMP(c1, c2);
}

void textClearLables(IplImage *src, CvRect **rects, size_t *size)
{
    IplImage *rgb = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(src, rgb, CV_GRAY2BGR);

    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq *contours, *c;
    size_t s;
    int i, ret;

    cvFindContours(src, storage, &contours,
                   sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

    s = contoursGetSize(contours);
    assert(s > 0);
    *size = s;

    *rects = calloc(s, sizeof(CvRect));
    assert((*rects) != NULL);

    for (i = s - 1, c = contours; c != NULL; c = c->h_next, i--) {
        CvRect rect = cvBoundingRect(c, 0);
        memcpy(&(*rects)[i], &rect, sizeof(CvRect));
    }

    qsort(*rects, s, sizeof(CvRect), _contoursCmpXCallback);
    qsort(*rects, s, sizeof(CvRect), _contoursCmpYCallback);

    for (i = s - 1; i > 0; i--) {
        CvRect rect = (*rects)[i];
        if ((ret = textCriteria(rect)) == 0) {

            CvRect clear = cvRect(rect.x, rect.y - rect.height / 2,
                                  rect.width, rect.height);

            for (int j = i - 1; j > 0; j--) {
                CvRect label = (*rects)[j];
               if ((ret = textCriteria(label)) == 0) {
                    if ((ret = intersectRect(label, clear)) == 1) {
                        int offset = label.x - rect.x;

                        if (abs(offset) < 5) break;

                        cvRectangleR(rgb, label, cvScalar(0, 0, 255, 0), 1, 8, 0);
                        cvRectangleR(src, label, cvScalar(255, 255, 255, 0), 1, 8, 0);
                        memset(&(*rects)[j], 0, sizeof(CvRect));
                    }
                }
            }
        }
    }

    debug(rgb, "clearlabel", "text", NULL);

    cvReleaseMemStorage(&storage);
}

void textTest(IplImage *src)
{
    IplImage *bin, *filtered, *lines;

    filtered = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    float data[] = {-0.1, 0.2, -0.1,
                     0.2,  1,   0.2,
                    -0.1, 0.2, -0.1};

    CvMat kernel = cvMat(3, 3, CV_32FC1, data);
    cvFilter2D(src, filtered, &kernel, cvPoint(-1, -1));

#ifdef DEBUG
    debug(filtered, "filtered", "text", NULL);
#endif

    cvCvtColor(filtered, bin, CV_BGR2GRAY);
    cvThreshold(bin, bin, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif

    lines = cvCloneImage(bin);
    textDrawLines2(lines);

    CvRect *rects;
    size_t size;
    textClearLables(lines, &rects, &size);

    free(rects);
    cvReleaseImage(&bin);
    cvReleaseImage(&filtered);
    cvReleaseImage(&lines);
}

void _textTest(IplImage *src)
{
    IplConvKernel *element;
    IplImage *bin, *filtered, *lines;

    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq *contours;


    filtered = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//    lines = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);


    float data[] = {-0.1, 0.2, -0.1,
                     0.2,  1,   0.2,
                    -0.1, 0.2, -0.1};

    CvMat kernel = cvMat(3, 3, CV_32FC1, data);
    cvFilter2D(src, filtered, &kernel, cvPoint(-1, -1));

#ifdef DEBUG
    debug(filtered, "filtered", "text", NULL);
#endif

    cvCvtColor(filtered, bin, CV_BGR2GRAY);
    cvThreshold(bin, bin, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif

    lines = cvCloneImage(bin);
    textDrawLines2(lines);

//    CvRect r = textGetLineRect(lines, cvPoint(950, 961));
//    cvRectangleR(src, r, cvScalar(255, 0, 0, 0), 1, 8, 0);
    int border = 5;
    cvRectangle(lines, cvPoint(0, 0), cvPoint(src->width - border, src->height - border),
                cvScalar(255, 255, 255, 0), border, 8, 0);
    textClear(lines);

#ifdef DEBUG
    debug(lines, "linesDrawed", "text", NULL);
#endif

    element = cvCreateStructuringElementEx(2, 11, 1, 5, CV_SHAPE_ELLIPSE, NULL);
    cvDilate(lines, lines, element, 1);
    cvReleaseStructuringElement(&element);
    int lheight = textGetMaxLineHeight(lines, 0, 0 ,0 ,0);
    textDrawLines3(lines, lheight);
    textDrawLines3(lines, lheight);

#ifdef DEBUG
    debug(lines, "linesResult", "text", NULL);
#endif

    cvFindContours(lines, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

    int size = 0;
    for (CvSeq *c = contours; c != NULL; c = c->h_next)size++;
    CvRect *strings = calloc(size, sizeof(CvRect));

    size = 0;
    uchar *ptr;

    for (CvSeq *c = contours; c != NULL; c = c->h_next) {
        CvRect rect = cvBoundingRect(c, 0);
        memcpy(&strings[size++], &rect, sizeof(CvRect));
    }

    qsort(strings, size, sizeof(CvRect), _textStringsCmpXCallback);
    qsort(strings, size, sizeof(CvRect), _textStringsCmpYCallback);

    CvRect line = strings[size - 1];

    CvRect *str = calloc(size, sizeof(CvRect));
    int s = 0;

    for (int i = size; i > 0; i--) {
        CvRect rect = strings[i];

        if (rect.height < lheight / 2) continue;
        if (rect.height > lheight * 2) continue;

        for (int y = rect.y; y > 0; y--) {
            ptr = (uchar *) (bin->imageData + y * bin->widthStep);
            uchar pix;
            for (int x = rect.x, max = rect.x + rect.width; x < max ;x++) {
                pix = ptr[x];
                if (pix == 0x00) {
                    rect.y--;
                    rect.height++;
                    break;
                }
            }
            if (pix != 0x00) break;
        }

        for (int y = rect.y + rect.height; y < bin->height; y++) {
            ptr = (uchar *) (bin->imageData + y * bin->widthStep);
            uchar pix;
            for (int x = rect.x, max = rect.x + rect.width; x < max; x++) {
                pix = ptr[x];
                if (pix == 0x00) {
                    rect.height++;
                    break;
                }
            }
            if (pix != 0x00) break;
        }

        for (int y = rect.y, max = rect.y + rect.height; y < max; y++) {
            ptr = (uchar *) (bin->imageData + y * bin->widthStep);
            for (int x = rect.x - 1; ptr[x] == 0x00; x--) {
                rect.x--;
                rect.width++;
            }
        }

        for (int y = rect.y, max = rect.y + rect.height; y < max; y++) {
            ptr = (uchar *) (bin->imageData + y * bin->widthStep);
            for (int x = rect.x + rect.width; ptr[x] == 0x00; x++) {
                rect.width++;
            }
        }

        memcpy(&str[s++], &rect, sizeof(CvRect));
//        printf("s%d: x = %d y = %d\n", i, rect.x, rect.y);
//        cvRectangleR(src, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
    }
    free(strings);

//    strings = realloc(s, size);

    line = str[0];
    CvRect line1 = str[1];

    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1, 1, 0, 1, 8);
    char buf[3];

    for (int i = 0; i < s; i++) {
        CvRect rect = str[i];
        if (rect.height < line.height / 2) continue;
        if (rect.height > line.height * 2) continue;

        printf("\ns%d: %d %d %d %d", i, rect.x, rect.y, rect.width, rect.height);
        itoa(i, buf);
        cvPutText(src, buf, cvPoint(rect.x - 10, rect.y), &font, cvScalar(255, 0, 0, 0));
        cvRectangleR(src, rect, cvScalar(0, 0, 255, 0), 1, 8, 0);
    }


    free(str);

#ifdef DEBUG
    debug(src, "src", "text", NULL);
#endif

    cvReleaseImage(&bin);
    cvReleaseImage(&lines);
    cvReleaseImage(&filtered);
}

void _testMorp(IplImage *src)
{
    IplConvKernel *element;
    IplImage *bin, *gray, *filtered, *lines;

    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq *contours;

    filtered = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
    bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    lines = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    float data[] = {-0.1, 0.2, -0.1,
                     0.2,  1,   0.2,
                    -0.1, 0.2, -0.1};

    CvMat kernel = cvMat(3, 3, CV_32FC1, data);
    cvFilter2D(src, filtered, &kernel, cvPoint(-1, -1));

//#ifdef DEBUG
//    debug(filtered, "filtered", "text", NULL);
//#endif

    cvCvtColor(src, gray, CV_BGR2GRAY);
    cvThreshold(gray, bin, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    cvReleaseImage(&gray);

#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif

    CvRect *photo = textGetPhoto(bin, 1);

    element = cvCreateStructuringElementEx(2, 7, 1, 5, CV_SHAPE_ELLIPSE, NULL);
    cvDilate(bin, lines, element, 1);
    cvReleaseStructuringElement(&element);


#ifdef DEBUG
    IplImage *d = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
    cvCvtColor(bin, d, CV_GRAY2BGR);
#endif

    textDrawLines2(bin);

#ifdef DEBUG
    debug(d, "binLines", "text", NULL);
    cvZero(d);
#endif

    textDrawLines2(lines);

#ifdef DEBUG
    debug(d, "dilateLines", "text", NULL);
#endif


#ifdef DEBUG
    cvReleaseImage(&d);
#endif

    cvReleaseImage(&filtered);
    cvReleaseImage(&bin);
    cvReleaseImage(&lines);
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
    cvThreshold(bin, bin, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);

#ifdef DEBUG
    debug(bin, "bin", "text", NULL);
#endif
    CvRect *photo = textGetPhoto(bin, 1);

    textDrawLines2(bin);

    element = cvCreateStructuringElementEx(2, 11, 1, 5, CV_SHAPE_ELLIPSE, NULL);
    cvDilate(bin, lines, element, 1);
    cvReleaseStructuringElement(&element);

    int lheight = textGetMaxLineHeight(lines, 0, 0, 0, 0);
    textDrawLines3(lines, lheight);

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

    cvCvtColor(bin, src, CV_GRAY2BGR);

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
#ifdef ZZZ
void textGetResult(IplImage *src, TessBaseAPI *handle)
{
    IplImage *fg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *bin = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *bg = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *res = cvCreateImage(cvGetSize(src), IPL_DEPTH_32S, 1);
    IplConvKernel *kern;
    CvMemStorage *storage = cvCreateMemStorage(0);
//    CvSeq *contours, *maskCont = getMaskContours(storage);

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
#endif
