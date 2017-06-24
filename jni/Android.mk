LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libtbb
LOCAL_SRC_FILES := ../3rd_party/tbb/lib/android/armeabi-v7a/libtbb.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=STATIC

include ~/code/libraries/opencv-2.4.13_Android/platforms/build_android_arm/OpenCV.mk

LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
LOCAL_NEON_CFLAGS :=  -mfloat-abi=softfp -mfpu=neon-vfpv4 -mcpu=cortex-a15 -mtune=cortex-a15 -march=armv7-a
LOCAL_CFLAGS= -frtti -ffast-math -Ofast -funroll-loops -fmodulo-sched \
-fmodulo-sched-allow-regmoves -funroll-loops -W -Wall -Wl,--unresolved-symbols=ignore-all\
-fPIC -DPIC -DNDEBUG -Ofast -O3 -funroll-loops -std=c++0x -D__ANDROID__ -D__ANDROID_UI__
LOCAL_CPPFLAGS := -frtti -ffast-math -Ofast -funroll-loops -fmodulo-sched \
-fmodulo-sched-allow-regmoves -funroll-loops -W -Wall -Wl,--unresolved-symbols=ignore-all\
-fPIC -DPIC -DNDEBUG -Ofast -O3 -funroll-loops -std=c++0x -D__ANDROID__ -D__ANDROID_UI__
LOCAL_C_INCLUDES +=			./3rd_party/Eigen \
					./3rd_party/tbb/include/android \
					./jni/headers \
					./jni/src \

LOCAL_SHARED_LIBRARIES := libtbb
					
LOCAL_MODULE    := native_activity
LOCAL_SRC_FILES := src/native.cpp
LOCAL_SRC_FILES := src/structures/PREVIeW.cpp
LOCAL_SRC_FILES += src/structures/feature_extractor.cpp
LOCAL_SRC_FILES += src/structures/image.cpp
LOCAL_SRC_FILES += src/structures/image_matching.cpp
LOCAL_SRC_FILES += src/structures/image_stream.cpp
LOCAL_SRC_FILES += src/structures/sequence.cpp
LOCAL_SRC_FILES += src/structures/sequence_matching.cpp
LOCAL_SRC_FILES += src/structures/sequence_stream.cpp
LOCAL_SRC_FILES += src/structures/vocabulary_tree.cpp
LOCAL_LDLIBS    += -lm -llog -landroid
LOCAL_STATIC_LIBRARIES += android_native_app_glue
                   
LOCAL_PRELINK_MODULE:=true
include $(BUILD_SHARED_LIBRARY)


# #======================================

# include $(CLEAR_VARS)

# OPENCV_INSTALL_MODULES:=on
# OPENCV_LIB_TYPE:=STATIC

# include ~/code/libraries/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk

# LOCAL_ARM_MODE := arm
# LOCAL_ARM_NEON := true
# LOCAL_NEON_CFLAGS :=  -mfloat-abi=softfp -mfpu=neon-vfpv4 -mcpu=cortex-a15 -mtune=cortex-a15 -march=armv7-a
# LOCAL_CFLAGS= -frtti -ffast-math -Ofast -funroll-loops -fmodulo-sched \
# -fmodulo-sched-allow-regmoves -funroll-loops -W -Wall -Wl,--unresolved-symbols=ignore-all\
# -fPIC -DPIC -DNDEBUG -Ofast -O3 -funroll-loops -std=c++0x -D__ANDROID__ -D__ANDROID_UI__
# LOCAL_CPPFLAGS := -frtti -ffast-math -Ofast -funroll-loops -fmodulo-sched \
# -fmodulo-sched-allow-regmoves -funroll-loops -W -Wall -Wl,--unresolved-symbols=ignore-all\
# -fPIC -DPIC -DNDEBUG -Ofast -O3 -funroll-loops -std=c++0x -D__ANDROID__ -D__ANDROID_UI__
# LOCAL_C_INCLUDES +=			./3rd_party/Eigen \
# 					./3rd_party/tbb/include/android \
# 					./jni/headers \
# 					./jni/src \

# LOCAL_SHARED_LIBRARIES := libtbb
					
# LOCAL_MODULE    := demo
# LOCAL_SRC_FILES := src/demo.cpp
# LOCAL_SRC_FILES += src/structures/PREVIeW.cpp
# LOCAL_SRC_FILES += src/structures/feature_extractor.cpp
# LOCAL_SRC_FILES += src/structures/image.cpp
# LOCAL_SRC_FILES += src/structures/image_matching.cpp
# LOCAL_SRC_FILES += src/structures/image_stream.cpp
# LOCAL_SRC_FILES += src/structures/sequence.cpp
# LOCAL_SRC_FILES += src/structures/sequence_matching.cpp
# LOCAL_SRC_FILES += src/structures/sequence_stream.cpp
# LOCAL_SRC_FILES += src/structures/vocabulary_tree.cpp
                   
# LOCAL_PRELINK_MODULE:=true
# include $(BUILD_SHARED_LIBRARY)

# #======================================


OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=STATIC

include ~/code/libraries/opencv-2.4.13_Android/platforms/build_android_arm/OpenCV.mk

LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
LOCAL_NEON_CFLAGS :=  -mfloat-abi=softfp -mfpu=neon-vfpv4 -mcpu=cortex-a15 -mtune=cortex-a15 -march=armv7-a
LOCAL_CFLAGS := -frtti -ffast-math -Ofast -funroll-loops -fmodulo-sched \
-fmodulo-sched-allow-regmoves -funroll-loops -W -Wall -Wl,--unresolved-symbols=ignore-all\
-fPIC -DPIC -DNDEBUG -Ofast -O3 -funroll-loops -std=c++0x -D__ANDROID__
LOCAL_CPPFLAGS := -frtti -ffast-math -Ofast -funroll-loops -fmodulo-sched \
-fmodulo-sched-allow-regmoves -funroll-loops -W -Wall -Wl,--unresolved-symbols=ignore-all\
-fPIC -DPIC -DNDEBUG -Ofast -O3 -funroll-loops -std=c++0x -D__ANDROID__
LOCAL_C_INCLUDES +=			./3rd_party/Eigen \
					./3rd_party/tbb/include/android \
					./jni/headers \
					./jni/src \

LOCAL_SHARED_LIBRARIES := libtbb
					
LOCAL_MODULE    := demo
LOCAL_SRC_FILES := src/demo.cpp
LOCAL_SRC_FILES += src/structures/PREVIeW.cpp
LOCAL_SRC_FILES += src/structures/feature_extractor.cpp
LOCAL_SRC_FILES += src/structures/image.cpp
LOCAL_SRC_FILES += src/structures/image_matching.cpp
LOCAL_SRC_FILES += src/structures/image_stream.cpp
LOCAL_SRC_FILES += src/structures/sequence.cpp
LOCAL_SRC_FILES += src/structures/sequence_matching.cpp
LOCAL_SRC_FILES += src/structures/sequence_stream.cpp
LOCAL_SRC_FILES += src/structures/vocabulary_tree.cpp
                   
LOCAL_PRELINK_MODULE:=true
include $(BUILD_EXECUTABLE)
