LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_ROOT:=/Users/amith/Downloads/OpenCV-android-sdk
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCV_ROOT}/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := cubedraw

LOCAL_SRC_FILES := nativeCubeDraw.cpp
LOCAL_LDLIBS += -llog -landroid -lEGL -lGLESv2
LOCAL_LDLIBS    += -lOpenMAXAL -lmediandk
LOCAL_LDLIBS    += -landroid
LOCAL_CFLAGS    += -UNDEBUG


include $(BUILD_SHARED_LIBRARY)