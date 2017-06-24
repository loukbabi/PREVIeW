# README #

### Description ###

**PREVIeW - Place Recognition with unifiEd sequence VIsual Words**

* Quick summary: Loop closure detection using two levels of matching with Visual Word Vectors (VWVs). First between sequences (S-VWVs) and then between images (I-VWVs).

### Prerequisites ###

- **OpenCV**: The [OpenCV](http://opencv.org/) library was used for handling images and detecting local feature points. The user can download the library from [here](https://github.com/opencv/opencv/tree/2.4). The library also needs to be built for Android in order to use our algorithm on a mobile device. Tested version: 2.4.13.
- **Eigen**: The [Eigen](http://eigen.tuxfamily.org/) library was used for handling sparse Visual Word Vectors. The library is included in this repository, thus no installation is explicitly required. Nevertheless, in order to achieve the best possible performance, we recommend a complete installation of the library as provided [here](https://bitbucket.org/eigen/eigen/).
- **TBB**: The Threading Building Blocks ([TBB](https://www.threadingbuildingblocks.org/)) library was used for multithreading and pipelining. The library is used only for speeding up the algorithm's execution. It is not explicitly required. Some prebuild library files are already included in this repository. In order to achieve the best possible performance, we recomment the complete installation of the library as provided [here](https://www.threadingbuildingblocks.org/download).
- **Android NDK/SDK**: The Android [NDK](https://developer.android.com/ndk/index.html)/[SDK](https://developer.android.com/studio/index.html) tools are required only for the case of building the mobile version of the algorithm. They can be downloaded [here](https://developer.android.com/ndk/downloads/index.html) and [here](https://developer.android.com/studio/index.html#downloads), respectively.
- **Demo data**: We provide some demo data to test our algorithm. In a terminal, navigate to this workspace and type <sh fetchFiles.sh> (without the "< >"). This will download a pretrained vocabulary tree and some sample images from the [Bicocca 2009-02-25b](http://www.rawseeds.org/rs/datasets/view/6) [1] dataset.

[1] RAWSEEDS (2007-2009) Robotics Advancement through Web-publishing of Sensorial and Elaborated Extensive Data Sets (Project FP6-IST-045144). URL [http://www.rawseeds.org/rs/datasets](http://www.rawseeds.org/rs/datasets).

### Setup ###

- **Desktop Version (Ubuntu)** :
    * In the "Makefile", set the required OpenCV paths for your machine (OPENCV_INCLUDE and OPENCV_LIB_PATH).
    * In a terminal, navigate to this workspace and type <sh buildDesktop.sh> (without the "< >").
    * Execute the application by first navigating into the "Desktop_bin" folder and then typing <./demo> (without the "< >").
    * After the end of execution, output metadata can be found in the "Output/Metadata" folder.
- **Mobile Version (Android). Native C++** :
    * This process requires a rooted Android device.
    * In the "jni/Android.mk", set the required OpenCV path for your machine (include ~/path/to/opencv/library/...../build_android_arm/OpenCV.mk).
    * In the "jni/Application.mk", set the target architecture for your device.
        * Note that the included TBB shared library has been built specifically for "APP_ABI:=armeabi-v7a", "APP_PLATFORM:=android-9" and "NDK_TOOLCHAIN_VERSION:=4.9".
          If you wish to target any other architecture please built your own version of TBB from source (https://www.threadingbuildingblocks.org/download).
    * In a terminal, navigate to this workspace and type <sh buildAndroid.sh> (without the "< >"). This will create an executable named "demo", as well as a library named libnative_activity.so. The library can be used in order to import the application into a UI application (.apk) using the "jni/src/native.cpp" source file.
    * Push the created executable "Android_bin/demo" to the desired device's path via <adb push Android_bin/demo path/to/device> (without the "< >").
    * In another terminal instance connect to device via <adb shell> (without "< >"), get super-user properties <su> (without "< >") and navigate to the folder that you pushed the executable.
    * Execute the native application via <./demo> (without "< >").

### Parameters ###

The file "jni/headers/defines.h" contains all the parameters needed by the application. You can change them as you wish and achieve the best performance for your scenario.
It also contains flags (like "#define PROFILER") to activate or deactivate specific utilities.

### Notes ###

Some ARM-NEON functions are not included due to their reserved rights. The visual vocabulary (vocabulary tree) was generated using the [DBoW2 and DLib](https://github.com/dorian3d) algorithms.

### Who do I talk to? ###

You can find more about me [here](http://robotics.pme.duth.gr/bampis) or contact me via email to "lbampis (at) pme (dot) duth (dot) gr".
