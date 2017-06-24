//============================================================================
// Name        : feature_extractor.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <stack>
#include <vector>
#include <queue>

#ifdef __ANDROID_UI__
	#include <android/log.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "structures/feature_extractor.h"

#include "defines.h"

// Shared Timer
#include "profiler.h"

#include "utils.h"
extern Profiler_Engine profiler;

bool Feature_Extractor_Engine::initialize()
{
	cv::initModule_nonfree();
	detector = cv::FeatureDetector::create( DETECTOR_TYPE );
	descriptor = cv::DescriptorExtractor::create( DESCRIPTOR_TYPE );

	if(detector.empty())
	{
		PRINTF_PLATFORM("Can not create requested detector (%s).\n", DETECTOR_TYPE);
		return false;
	}

	if( descriptor.empty() )
	{
		PRINTF_PLATFORM("Can not create requested descriptor extractor (%s).\n", DESCRIPTOR_TYPE);
		return false;
	}

	return true;
}

void Feature_Extractor_Engine::deallocate()
{
	detector.release();
	detector.delete_obj();
	descriptor.release();
	descriptor.delete_obj();
}
