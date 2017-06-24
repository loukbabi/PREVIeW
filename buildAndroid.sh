rm -f Android/demo libs/armeabi-v7a/demo libs/armeabi-v7a/libPREVIeW.so
ndk-build 2>&1 | grep -v "warning: hidden symbol"
cp libs/armeabi-v7a/demo Android_bin/
