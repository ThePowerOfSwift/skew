#include "skew.h"
#include "contours.h"
#include "utils.h"
#include "errno.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

extern IplImage *morph_temp;

int main(int argc, char *argv[])
{
    IplImage *image;

    CV_FUNCNAME("main");

    __BEGIN__;

    CV_ASSERT(image = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR));

    __END__;

//    error = cvGetErrStatus();
//    errorMessage = 0;
//    if (error) {
//        errorMessage = cvErrorStr(error);_
//    } else {
//        error = errno;                   // needs #include <cerrno>
//        errorMessage = strerror(error);  //       #include <cstring>
//    }
//    CV_ASSERT(origin);
//    skewInit();
//    contoursInit();

//    skew(origin, 1);
//    contoursOutline(origin, 1);

//    debug_run();

//    skewEnd();
//    contoursEnd();

//    cvNamedWindow("original", CV_WINDOW_NORMAL);
//    cvNamedWindow("binary", CV_WINDOW_NORMAL);
//    cvNamedWindow("contours", CV_WINDOW_NORMAL);


//    IplImage *image, *gray, *bin, *dst, *rgb;

//    CvMemStorage *storage = cvCreateMemStorage(0);
//    CvSeq *contours;

//    image = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);

//    cvRectangle(image, cvPoint(0, 0), cvPoint(image->width - 5, image->height - 5), cvScalar(255, 255, 255, 0), 5, 8, 0);

//    gray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
//    bin = cvCreateImage(cvGetSize(image),  IPL_DEPTH_8U, 1);
//    rgb = cvCreateImage(cvGetSize(image),  IPL_DEPTH_8U, 3);
//    dst = cvCloneImage(image);

////    t = cvCreateImage(cvSize(image->width, image->height) , IPL_DEPTH_8U, 1);

//    cvCvtColor(image, gray, CV_RGB2GRAY);
//    cvAdaptiveThreshold(gray, bin, 250, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, 1);

//    cvCvtColor(bin, rgb, CV_GRAY2RGB);
//    MORPH(rgb, rgb, CV_MOP_OPEN, 3, 1);

//    cvCvtColor(rgb, gray, CV_RGB2GRAY);
//    cvThreshold(gray, bin, 195, 255, CV_THRESH_BINARY);

//    cvCvtColor(bin, rgb, CV_GRAY2RGB);
//    morph(rgb, rgb, CV_MOP_CLOSE, 9, 1);

//    cvCvtColor(rgb, bin, CV_RGB2GRAY);
//    cvShowImage("binary", bin);

//    int contoursCount = cvFindContours(bin, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0 ));

//    for(CvSeq* seq0 = contours; seq0!=0; seq0 = seq0->h_next){
//        cvDrawContours(dst, seq0, CV_RGB(255, 216, 0), CV_RGB(0, 0, 250), 0, 1, 8, cvPoint(0, 0)); // рисуем контур
//        rect = cvBoundingRect(seq0, 0);
//        cvRectangle(dst,
//                    cvPoint(rect.x, rect.y),
//                    cvPoint(rect.x+rect.width, rect.y+rect.height),
//                    cvScalar(0, 0, 255, 0),
//                    2, 8, 0);
//    }

//    // показываем картинки
//    cvShowImage("original", image);
////    cvShowImage("binary", bin);
//    cvShowImage("contours", dst);
////    cvShowImage("morphology", morphology);

//    // ждём нажатия клавиши
//    cvWaitKey(0);

//    // освобождаем ресурсы
//    cvReleaseMemStorage(&storage);
//    cvReleaseImage(&image);
//    cvReleaseImage(&gray);
//    cvReleaseImage(&bin);
//    cvReleaseImage(&dst);
//    cvReleaseImage(&t);
//    // удаляем окна
//    cvDestroyAllWindows();
//    return 0;
//    int error;
//    const char * errorMessage;
//exit:
////    if(!cvSaveImage(name, gray)) {
//          error = cvGetErrStatus();
//          errorMessage = 0;
//          if (error) {
//              errorMessage = cvErrorStr(error);
//          } else {
//              error = errno;                   // needs #include <cerrno>
//              errorMessage = strerror(error);  //       #include <cstring>
//          }
////          std::cout << errorMessage << std::endl;
//          // with "echo shell_exec("./opencv '$newName'");" in php
////      }
//            exit(EXIT_FAILURE);
}

