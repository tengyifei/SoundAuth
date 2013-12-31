LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := nativedft
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := nativedft.cpp
LOCAL_LDLIBS    := -llog 

include $(BUILD_SHARED_LIBRARY)
