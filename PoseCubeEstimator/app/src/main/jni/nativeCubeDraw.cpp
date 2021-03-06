#include <alloca.h>
//
// Created by Amith Lawrence on 11/26/20.
//
#include <jni.h>
#include "nativeCubeDraw.h"
#include <android/log.h>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <android/native_window.h>
#include <android/native_window_jni.h>


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "sampleMSRS", __VA_ARGS__)

using namespace cv;
using namespace std;

// Native method drawNativePose is implemented below
// It gets height,weight of surface, surfaceView, rvec and tvec as inputs
// It draws the cube on top of surface
JNIEXPORT void JNICALL
Java_asu_posecubeestimator_NativeDraw_drawNativePose(JNIEnv *env, jclass type, jint srcWidth,
                                                        jint srcHeight, jobject srcBuffer,
                                                        jobject dstSurface, jfloatArray rvec_,
                                                        jfloatArray tvec_) {

    // Read the rotation and translation vectors
    jfloat *rvec,*tvec;

    if(rvec_!=NULL)
        rvec = env->GetFloatArrayElements(rvec_, NULL);

    if(tvec_!=NULL)
        tvec = env->GetFloatArrayElements(tvec_, NULL);

    // The input image buffer
    uint8_t *srcLumaPtr = reinterpret_cast<uint8_t *>(env->GetDirectBufferAddress(srcBuffer));

    int dstWidth;
    int dstHeight;

    cv::Mat mYuv(srcHeight+srcHeight/2, srcWidth, CV_8UC1, srcLumaPtr); // Getting all channels for display
    cv::Mat mYuvGray(srcHeight,srcWidth,CV_8UC1,srcLumaPtr); // Only getting the luma channel

    ANativeWindow *win = ANativeWindow_fromSurface(env, dstSurface);
    ANativeWindow_acquire(win);

    ANativeWindow_Buffer buf;
    dstWidth = srcHeight;
    dstHeight = srcWidth;

    ANativeWindow_setBuffersGeometry(win, dstWidth, dstHeight, 0 );

    // Acquiring a lock on the SurfaceView to render the processed image
    if (int32_t err = ANativeWindow_lock(win, &buf, NULL)) {
        LOGE("ANativeWindow_lock failed with error code %d\n", err);
        ANativeWindow_unlockAndPost(win);
        ANativeWindow_release(win);
        return ;
    }

    uint8_t *dstLumaPtr = reinterpret_cast<uint8_t *>(buf.bits);

    Mat dstRgba(dstHeight, buf.stride, CV_8UC4,
                dstLumaPtr);
    Mat srcRgba(srcHeight, srcWidth, CV_8UC4);
    Mat flipRgba(dstHeight, dstWidth, CV_8UC4);
    Mat colorRgba(dstHeight, dstWidth, CV_8UC4);


    // convert YUV -> RGBA
    cv::cvtColor(mYuv, colorRgba, COLOR_YUV2RGBA_NV21); //colorRgba is used for display


    if(rvec_!=NULL && tvec_!=NULL){

        // cx, cy -> co-ordinates of the center of the image
        // fx, fy -> co-ordinates of the focal point
        const double cx = mYuvGray.rows/2;
        const double cy = mYuvGray.cols/2;
        const double fx = 1.73 * cx;
        const double fy = 1.73 * cy;

        std::vector<cv::Point3d> pointAxesForCube;
        std::vector<cv::Point2d> pointsImage;
        std::vector<cv::Point2d> pointsImagesRoof;

        int axisLength = 5;

        pointAxesForCube.push_back(cv::Point3d(0,0,0));
        pointAxesForCube.push_back(cv::Point3d(0,axisLength,0));
        pointAxesForCube.push_back(cv::Point3d(axisLength,axisLength,0));
        pointAxesForCube.push_back(cv::Point3d(axisLength,0,0));
        pointAxesForCube.push_back(cv::Point3d(0,0,-axisLength));
        pointAxesForCube.push_back(cv::Point3d(0,axisLength,-axisLength));
        pointAxesForCube.push_back(cv::Point3d(axisLength,axisLength,-axisLength));
        pointAxesForCube.push_back(cv::Point3d(axisLength,0,-axisLength));

        double data[9] = {fx, 0, cx, 0, fy, cy, 0, 0, 1};
        cv::Mat K = cv::Mat(3, 3, CV_64F, data);

        double rotVecdata[3]={double(rvec[0]),double(rvec[1]),double(rvec[2])};
        double transdata[3]={double(tvec[0]),double(tvec[1]),double(tvec[2])};

        cv::Mat rotVec = cv::Mat(3, 1, CV_64F, rotVecdata);
        cv::Mat transVec = cv::Mat(3, 1, CV_64F, transdata);

        cv::projectPoints(pointAxesForCube, rotVec, transVec, K, cv::Mat::zeros(5, 1, CV_64F),
                          pointsImage);

        vector<vector<Point> > pointsImageFloor(1);

        for(int i=0;i<4;i++){
            pointsImageFloor[0].push_back(Point((int)pointsImage[i].x,(int)pointsImage[i].y));
        }
        for(int i=4;i<8;i++){
            pointsImagesRoof.push_back(pointsImage[i]);
        }
        // Drawing the floor of the box
        drawContours(colorRgba, pointsImageFloor, -1, 0, cv::FILLED,LINE_8);

        for(int i=0;i<4;i++){
            line(colorRgba,pointsImage[i],pointsImage[i+4],Scalar(255,223,223),3);
        }
        line(colorRgba,pointsImage[0],pointsImage[1],Scalar(255,223,223),3);
        line(colorRgba,pointsImage[1],pointsImage[2],Scalar(255,223,223),3);
        line(colorRgba,pointsImage[2],pointsImage[3],Scalar(255,223,223),3);
        line(colorRgba,pointsImage[3],pointsImage[0],Scalar(255,223,223),3);
        line(colorRgba,pointsImage[4],pointsImage[5],Scalar(255,223,223),3);
        line(colorRgba,pointsImage[5],pointsImage[6],Scalar(255,233,233),3);
        line(colorRgba,pointsImage[6],pointsImage[7],Scalar(255,233,233),3);
        line(colorRgba,pointsImage[7],pointsImage[4],Scalar(255,233,233),3);

        LOGE("Mid points %f , %f",pointsImage[0].x,pointsImage[0].y);
        LOGE("Color rgba %d , %d",colorRgba.rows,colorRgba.cols);
    }

    cv::transpose(colorRgba, colorRgba);
    cv::flip(colorRgba, colorRgba,1);
    circle(colorRgba,Point(colorRgba.cols/2,colorRgba.rows/2),2,Scalar(255,255,0),1);

    // copy to TextureView surface
    uchar *dbuf;
    uchar *sbuf;
    dbuf = dstRgba.data;
    sbuf = colorRgba.data;
    int i;
    for (i = 0; i < colorRgba.rows; i++) {
        dbuf = dstRgba.data + i * buf.stride * 4;
        memcpy(dbuf, sbuf, colorRgba.cols*4);
        sbuf += colorRgba.cols * 4;
    }

    ANativeWindow_unlockAndPost(win);
    ANativeWindow_release(win);


    if(rvec_!=NULL)
        env->ReleaseFloatArrayElements(rvec_, rvec, 0);

    if(tvec_!=NULL)
        env->ReleaseFloatArrayElements(tvec_, tvec, 0);

    return;
}