//============================================================================
// Name        : image_stream.cpp
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
#include "opencv2/features2d/features2d.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "defines.h"

#include "structures/vocabulary_tree.h"
#include "structures/feature_extractor.h"
#include "structures/image.h"
#include "structures/sequence.h"
#include "structures/image_stream.h"
#include "structures/image_matching.h"
#include "structures/sequence_matching.h"
#include "structures/sequence_stream.h"

// Shared Timer
#include "profiler.h"
extern Profiler_Engine profiler;

#include "utils.h"


Image_Stream_Engine::Image_Stream_Engine(Vocabulary_Tree_Engine *Vocabulary_tree_in)
{
	Vocabulary_tree = Vocabulary_tree_in;

	if(!feature_extractor.initialize())
	{
		_validFeatures = false;
		allocated = false;
		return;
	}

	_validFeatures = true;
	allocated = true;
}

bool Image_Stream_Engine::add_new_image(cv::Mat &inputImage, unsigned int imageGlobalID_in, cv::Mat &input_UI_img)
{
	unsigned int imageID = stream.size();
	Image_Engine *newImage = new Image_Engine(imageID, inputImage, feature_extractor, Vocabulary_tree, imageGlobalID_in, input_UI_img);

	if(!newImage->success)
	{
		PRINTF_PLATFORM("Dropping image %u.\n", imageID);
		newImage->deallocate_less();
		return false;
	}

	PRINTF_PLATFORM("                            Adding image %u\n", imageID);
	stream.push_back(newImage);

	return true;
}

void Image_Stream_Engine::deallocate()
{
	if(allocated)
	{
		for(unsigned int i = 0; i < stream.size(); i++)
		{
			stream.at(i)->deallocate();
			delete stream.at(i);
		}

		stream.clear();
		inverse_indexing.clear();
		feature_extractor.deallocate();
		allocated = false;
	}
}

Image_Stream_Engine::~Image_Stream_Engine()
{
	if(allocated)
		deallocate();
}
