TARGET_DIR=libs/desktop
#MAKEFLAGS = -j8

GCC = g++

OPENCV_INCLUDE=/usr/local/include
OPENCV_LIB_PATH=/usr/local/lib

OBJS = $(TARGET_DIR)/image.o $(TARGET_DIR)/feature_extractor.o $(TARGET_DIR)/image_matching.o $(TARGET_DIR)/image_stream.o $(TARGET_DIR)/sequence.o $(TARGET_DIR)/sequence_matching.o $(TARGET_DIR)/sequence_stream.o $(TARGET_DIR)/vocabulary_tree.o $(TARGET_DIR)/PREVIeW.o $(TARGET_DIR)/demo.o
INCS = -I3rd_party/Eigen -I3rd_party/tbb/include/desktop -Ijni/headers -I$(OPENCV_INCLUDE)
LIBS_PATHS = -L$(OPENCV_LIB_PATH) -L3rd_party/tbb/lib/desktop/intel64/gcc4.4

COMPILER_FLAGS = -std=c++11 -O3 -fPIC -Xlinker -fexceptions -ffunction-sections -funwind-tables \
  -fstack-protector -no-canonical-prefixes\
  -fpermissive -fexceptions -fomit-frame-pointer -fno-strict-aliasing \
  -finline-limit=64 -D__DESKTOP__ -Wa,--noexecstack

LINKER_FLAGS = -fPIC

LIBS = -lopencv_core -lopencv_nonfree -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_flann -ltbb

$(TARGET_DIR)/%.o: jni/src/%.cpp
	@echo '==============================='
	@echo 'Building file: $<'
	@echo 'Invoking: '$(GCC)
	$(GCC) $(COMPILER_FLAGS) $(INCS) --compile -x c++ -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo '==============================='

$(TARGET_DIR)/%.o: jni/src/structures/%.cpp
	@echo '==============================='
	@echo 'Building file: $<'
	@echo 'Invoking: '$(GCC)
	$(GCC) $(COMPILER_FLAGS) $(INCS) --compile -x c++ -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo '==============================='

$(TARGET_DIR)/demo: $(OBJS)
	@echo '==============================='
	@echo 'Creating target: $@'
	@echo 'Invoking: linker'
	$(GCC) $(LINKER_FLAGS) $(LIBS_PATHS) $(INCS) $+ -o $@ $(LIBS)
	@echo 'Finished building target'
	@echo '==============================='

lib:
	@echo '==============================='
	@echo 'Creating dependencies for library'
	make
	@echo '==============================='
	@echo '==============================='
	@echo 'Creating target library: libPREVIeW.so'
	@echo 'Invoking: linker'
	$(GCC) -shared $(LINKER_FLAGS) $(LIBS_PATHS) $(INCS) $+ -o $(TARGET_DIR)/libPREVIeW.so $(OBJS) $(LIBS)
	@echo 'Finished building target library'
	@echo '==============================='

all: clean $(OBJS) $(TARGET_DIR)/demo

clean:
	rm -f $(TARGET_DIR)/*.o $(TARGET_DIR)/demo $(TARGET_DIR)/libPREVIeW.so
