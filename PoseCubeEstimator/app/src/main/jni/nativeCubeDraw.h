//
// Created by Amith Lawrence on 11/26/20.
//
#include <jni.h>
#ifndef _Included_asu_posecubeestimator_nativeCubeDraw
#define _Included_asu_posecubeestimator_nativeCubeDraw

#ifdef __cplusplus
extern "C" {
#endif

//Native method drawNativePose is implemented below
//It gets height,weight of surface, surfaceView, rvec and tvec as inputs
//It draws the cube on top of surface
JNIEXPORT void JNICALL
Java_drawNative(JNIEnv *env, jclass type, jint width,
                                                    jint height, jobject buffer, jobject surface);

JNIEXPORT void JNICALL
Java_asu_posecubeestimator_NativeDraw_drawNativePose(JNIEnv *env, jclass type, jint width,
                                                        jint height, jobject buffer,
                                                        jobject surface, jfloatArray rvec_,
                                                        jfloatArray tvec_);

#ifdef __cplusplus
}
#endif

#endif //POSECUBEESTIMATOR_NATIVECUBEDRAW_H
