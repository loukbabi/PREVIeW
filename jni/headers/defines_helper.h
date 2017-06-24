//============================================================================
// Name        : defines_helper.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef DEFINES_HELPER_H
#define DEFINES_HELPER_H

#ifdef __ANDROID__
	#define _ARM_NEON_						// ARM NEON Speedups
	#ifdef __ANDROID_UI__
		#define LOG_TAG    "OCV:libnative_activity"
		#define CV_IMG_RELEASE(img)
		#define PRINTF_PLATFORM(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
	#else
		#define CV_IMG_RELEASE(img) img.release()
		#define PRINTF_PLATFORM(...) printf( __VA_ARGS__ )
		#define READ_IMAGES_FROM_MEMORY 	// Reading Images from memory. If desabled, images are expected from camera input
	#endif
#else
	#ifdef __DESKTOP__
		#define CV_IMG_RELEASE(img)
		#define PRINTF_PLATFORM(...) printf( __VA_ARGS__ )
		#define IMAGE_VISUALIZATION 		// Enable Visualization
		#define READ_IMAGES_FROM_MEMORY 	// Reading Images from memory. If desabled, images are expected from camera input
	#endif
#endif

#ifdef IMAGE_VISUALIZATION
	#define CV_NAMEDWINDOW(x) cv::namedWindow( x, cv::WINDOW_AUTOSIZE )
	#define CV_DESTROYWINDOW(x) cv::destroyWindow(x)
	#define CV_IMSHOW(string, img) cv::imshow( string, img ); \
								   int k = cv::waitKey(IMAGE_VISUALIZATION_DELAY); \
								   if(k == 27) endProcessByUser = true //ESC
#else
	#define CV_NAMEDWINDOW(x)
	#define CV_DESTROYWINDOW(x)
	#define CV_IMSHOW(string, img)
#endif		   	

#ifdef TIME
	#define TIC(x) tic()
	#define TOC(printStr, x) PRINTF_PLATFORM(printStr); \
						     toc(x)
#else
	#define TIC(x)
	#define TOC(printStr, x)
#endif

#ifdef WITH_TBB_FOR_PIPELINING
	#define TBB_INIT() tbb::task_group tbb_tasker_g
	#define TBB_TASK(x) tbb_tasker_g.run([&]{x;})
#else
	#define TBB_INIT()
	#define TBB_TASK(x) x
#endif

#ifdef PROFILER
	#define P_INIT(labelVector, printingPrecision) profiler.initialize(labelVector, printingPrecision)
	#define P_DEALLOC() profiler.deallocate();
	#define P_TIC(label) profiler.tic(label);
	#define P_TOC(label) profiler.toc(label);
	#define P_SAVEALL(timersPath) profiler.saveAll(timersPath);
	#define P_SAVE(label, timersPath) profiler.save(label, timersPath);
#else
	#define P_INIT(labelVector, PrintingPrecision)
	#define P_DEALLOC() 
	#define P_TIC(label) 
	#define P_TOC(label) 
	#define P_SAVEALL(timersPath) 
	#define P_SAVE(label, timersPath) 
#endif

#ifdef USE_NORMALIZATION
	#define ADDITIONAL_OFFSET_DUE_TO_NORM 1
#else
	#define ADDITIONAL_OFFSET_DUE_TO_NORM 0
#endif

#endif // DEFINES_HELPER_H
