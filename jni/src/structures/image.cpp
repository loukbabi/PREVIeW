//============================================================================
// Name        : image.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <stack>
#include <fstream>
#include <iomanip>
#include <vector>
#include <queue>

#ifdef __ANDROID_UI__
	#include <android/log.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "defines.h"

#include "structures/vocabulary_tree.h"
#include "structures/feature_extractor.h"
#include "structures/image.h"

// Shared Timer
#include "tictoc.h"
#include "profiler.h"
extern Profiler_Engine profiler;

extern bool endProcessByUser;


bool storeFeatureDescriptors(cv::Mat descriptors, const char* fileName)
{
	char filePath[128];
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);

	std::ofstream myfile;
	myfile.open(filePath);

	if(myfile.is_open())
		myfile << descriptors << "\n";
	else
	{
		PRINTF_PLATFORM("Could not create file: %s", filePath);
		return false;
	}
	myfile.close();

	PRINTF_PLATFORM("Feature descriptors are stored.\n");
	return true;
}


bool Image_Engine::extractImageFeatures(Feature_Extractor_Engine& feature_extractor, cv::Mat &inputImage)
{
//ToDo: Keep the same buffer for reading images (maybe input_UI_img)

P_TIC("DetectORB");
	feature_extractor.detector->detect(Image, keypoints);
	cv::KeyPointsFilter::retainBest(keypoints, NUMBER_OF_FEATURES_FROM_EACH_IMAGE);
P_TOC("DetectORB");

	if(keypoints.size() < MIN_FEATURES_PER_IMAGE_THRESHOLD)
	{
		keypoints.clear();
		PRINTF_PLATFORM("Rejecting Image %d since insufficient number of local features were found.\n", imageID);
		return false;
	}

P_TIC("ComputeORB");	
	feature_extractor.descriptor->compute(inputImage, keypoints, descriptors);
	numberOfFeatures = descriptors.rows;
P_TOC("ComputeORB");

	return true;
}

bool Image_Engine::image2descriptor(Feature_Extractor_Engine& feature_extractor, cv::Mat &input_UI_img, cv::Mat &inputImage)
{
	bool flag = extractImageFeatures(feature_extractor, inputImage);

	if(!flag)
	{
		CV_IMG_RELEASE(Image);
		return false;
	}


	Vocabulary_tree->treeTraversing(this);

	#ifdef __ANDROID__
		#ifdef __ANDROID_UI__
			P_TIC("DrawKeyPoints");
				Image.copyTo(input_UI_img);
				cv::drawKeypoints(Image, keypoints, input_UI_img, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG );
				cv::resize(Image, Image, cv::Size(100, 100), 0, 0, cv::INTER_NEAREST );
			P_TOC("DrawKeyPoints");
		#endif
	#else
		P_TIC("DrawKeyPoints");
		Image.copyTo(input_UI_img);
		cv::drawKeypoints(Image, keypoints, input_UI_img, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG );
		P_TOC("DrawKeyPoints");
	#endif

	CV_IMG_RELEASE(Image);

	return true;
}

bool Image_Engine::readImgFromMemory(std::string &imageName)
{
	Image = cv::imread(imageName, CV_LOAD_IMAGE_GRAYSCALE);

	if(! Image.data )  // Check for invalid input
	{
		PRINTF_PLATFORM("Could not open Image: %s .\n", imageName.c_str());
		return false;
	}
	else
		return true;
}

Image_Engine::Image_Engine(unsigned int imageID_in, cv::Mat &inputImage, Feature_Extractor_Engine& feature_extractor,
							Vocabulary_Tree_Engine* Vocabulary_tree_in, unsigned int imageGlobalID_in, cv::Mat &input_UI_img)
{
	Vocabulary_tree = Vocabulary_tree_in;
	image_descriptor_size = Vocabulary_tree->imgDecrSize;
	imageGlobalID = imageGlobalID_in;

	unit_image_descriptor_sp.resize(image_descriptor_size, 1);
	unit_image_descriptor_sp.reserve(NUMBER_OF_FEATURES_FROM_EACH_IMAGE);
	

	imageID = imageID_in;

	if (inputImage.channels() == 1)
		inputImage.copyTo(Image);
	else
		cv::cvtColor(inputImage, Image, CV_BGR2GRAY);

	allocated = true;

	if(!image2descriptor(feature_extractor, input_UI_img, inputImage))
	{
		success = false;
		return;
	}

	success = true;
	return;
}

void Image_Engine::deallocate()
{
	if(allocated)
	{
		unit_image_descriptor_sp.setZero();
		unit_image_descriptor_sp.resize(0,0);

		Image.release();
		descriptors.release();
		keypoints.clear();

		allocated = false;
	}
}
void Image_Engine::deallocate_less()
{
	if(allocated)
	{
		Image.release();
		allocated = false;
	}
}

Image_Engine::~Image_Engine()
{
	if(allocated)
		deallocate();
}
