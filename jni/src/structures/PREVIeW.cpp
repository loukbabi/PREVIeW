//============================================================================
// Name        : PREVIeW.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stack>

#ifdef __ANDROID_UI__
	#include <android/log.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "structures/PREVIeW.h"

std::stack<timespec> tic_stack;
bool endProcessByUser = false;
Profiler_Engine profiler;

bool PREVIeW::initialize(const char *VT_file_name, const char *Input_img_path)
{
	createUI();
	if(!readMetadata(VT_file_name, Input_img_path))
	{
		// Destroing display window
		CV_DESTROYWINDOW("UI");
		return false;
	}
	return true;
}

cv::Mat* PREVIeW::createUI()
{
	// Creating display window
	CV_NAMEDWINDOW("UI");

	UI_img = cv::Mat(480, 640 + 150, CV_8UC1, cv::Scalar(0));
	cv::putText(UI_img, "Initliazing Application.", cv::Point(30, 180), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255), 2);
	cv::putText(UI_img, "Loading Vocabulary Tree.", cv::Point(5, 220), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255), 2);
	CV_IMSHOW("UI", UI_img);
	return &UI_img;
}

bool PREVIeW::readMetadata(const char *VT_file_name, const char *Input_img_path)
{
	allocated = true;
	experiment_executed = false;

	// Initialize Profiler
	#ifdef PROFILER
		labels = new std::vector<std::string>{"ReadImgFromMemory", 
							"DetectORB", "ComputeORB", "DrawKeyPoints",
							"TreeTraversal", "FormSVWVs", "FormIVWVs", "InverseIndexingUpdate", 
							"FindSequenceMatch", "FindImageMatch", "TOTAL"};
	#endif

	P_INIT(labels, 10);

	acquiredImagesCount = 0;
	totalImgCount = 0;

	// Read the Vocabulary Tree from file
	Vocabulary_tree = new Vocabulary_Tree_Engine;

	if(VT_file_name == NULL)
	{
		if(!Vocabulary_tree->read_VT_file(VOCABULARY_TREE_FILE_NAME)) return false;
	}
	else
	{
		if(!Vocabulary_tree->read_VT_file(VT_file_name)) return false;
	}

	// Declaring image stream engine
	image_stream = new Image_Stream_Engine( Vocabulary_tree );
	if(!image_stream->validFeatures()) return false;

	// Declaring image matching engine
	image_matcher = new Image_Matching_Engine( Vocabulary_tree );

	// Declaring sequence stream engine
	sequence_stream = new Sequence_Stream_Engine( Vocabulary_tree );

	// Declaring sequence matching engine
	sequence_matcher = new Sequence_Matching_Engine( Vocabulary_tree, image_matcher);
	
	#ifdef READ_IMAGES_FROM_MEMORY
		// Reading input images' names
		PRINTF_PLATFORM("=======================================\n");
		PRINTF_PLATFORM("   Reading input images' names...\n");
		if(Input_img_path == NULL)
		{
			if(!readFolderContent(imageNames, INPUT_IMAGE_PATH)) return false;
		}
		else
		{
			if(!readFolderContent(imageNames, Input_img_path)) return false;
		}
		PRINTF_PLATFORM("=======================================\n");
	#else
		if(Input_img_path != NULL)
		{
			if(!readFolderContent(imageNames, Input_img_path)) return false;
		}
	#endif

	return true;
}

void PREVIeW::allocate_UI_img(cv::Mat &inputImage)
{
	#ifdef __ANDROID__
		UI_img = cv::Mat(inputImage.rows, inputImage.cols + 150, CV_8UC1, cv::Scalar(0));
		input_UI_img = UI_img(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
		output_UI_img = UI_img(cv::Rect(inputImage.cols + 50, 50, 100, 100));
	#else
		UI_img = cv::Mat(inputImage.rows, 2*inputImage.cols + 50, CV_8UC1, cv::Scalar(0));
		input_UI_img = UI_img(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
		output_UI_img = UI_img(cv::Rect(inputImage.cols + 50, 0, inputImage.cols, inputImage.rows));
	#endif
}

bool PREVIeW::deploy_fromMemory()
{
	// Starting Input Stream
	PRINTF_PLATFORM("=======================================\n");
	PRINTF_PLATFORM("   Starting Input Stream...\n");
	for(unsigned int i = 0; i < imageNames.size(); i++)
	{
		P_TIC("ReadImgFromMemory");
			readImg = cv::imread(imageNames[i], CV_LOAD_IMAGE_GRAYSCALE);
			if(! readImg.data )  // Check for invalid input
			{
				PRINTF_PLATFORM("Could not open Image: %s .\n", imageNames[i].c_str());
				return false;
			}
		P_TOC("ReadImgFromMemory");

		P_TIC("TOTAL");
			addImage(readImg);
		P_TOC("TOTAL");

		CV_IMSHOW("UI", UI_img);

	
	}
	PRINTF_PLATFORM("=======================================\n");

	return true;
}

cv::Mat* PREVIeW::addImage(cv::Mat &inputImage)
{
	if(totalImgCount == 0)
		allocate_UI_img(inputImage);

	bool newImageAdded = image_stream->add_new_image( inputImage, totalImgCount++, input_UI_img);
	if(  newImageAdded  )
	{
		P_TIC("FormSVWVs");
		bool flag = sequence_stream->createNewSequenceIfisTime();
		P_TOC("FormSVWVs");

		if(flag)
			TBB_TASK(sequence_matcher->findMatchANDupdateInverseIndexing_withTimeLag(sequence_stream->stream, output_UI_img));
		else
		{
			P_TIC("InverseIndexingUpdate");
			P_TOC("InverseIndexingUpdate");
			P_TIC("FindSequenceMatch");
			P_TOC("FindSequenceMatch");
			P_TIC("FindImageMatch");
			P_TOC("FindImageMatch");
			
		}

		sequence_stream->add_new_image(image_stream->stream.back());

		acquiredImagesCount++;
	}else
	{
		// extra tic-tocs for retaining the same amount of measurements for every timer
		P_TIC("FormSVWVs");
		P_TOC("FormSVWVs");
		P_TIC("InverseIndexingUpdate");
		P_TOC("InverseIndexingUpdate");
		P_TIC("FindSequenceMatch");
		P_TOC("FindSequenceMatch");
		P_TIC("FindImageMatch");
		P_TOC("FindImageMatch");
	}

	return &UI_img;
}

void PREVIeW::finalize()
{
	// End final image sequence
	sequence_stream->endCurrentSequence();
	// If images left in the fanal sequence, then try to find matches
	if(sequence_stream->stream.back()->image_members.size() > 1)
		sequence_matcher->findMatch(sequence_stream->stream, output_UI_img);

	// Note that at least a minimal experiment is performed
	if(sequence_stream->stream.size() > 0)
		experiment_executed = true;
}

bool PREVIeW::storeOutputMetadata()
{
	if(!experiment_executed)
	{
		PRINTF_PLATFORM("Nothing to say. You need to perform an experiment first.\n");
		return false;
	}

	P_SAVEALL(OUTPUT_METADATA_PATH);

	#ifdef _DEBUG_
		PRINTF_PLATFORM("Number of Used images: %u\n", (unsigned int)imageNames.size());
		PRINTF_PLATFORM("Number of Sequence Matches: %u\n", (unsigned int)sequence_matcher->match_list.size());
		PRINTF_PLATFORM("Number of Image Matches: %u\n", (unsigned int)image_matcher->match_list.size());
	#endif

	storeImageMatchList(*image_matcher, image_stream->stream.size(), "imageMatches");
	return true;
}

void PREVIeW::deallocate()
{
	if(allocated)
	{
		// Deallocate Profiler
		#ifdef PROFILER
			labels->clear();
			delete labels;
		#endif

		// Deallocating vocabulary tree engine
		Vocabulary_tree->deallocate();
		delete Vocabulary_tree;

		// Deallocating image stream engine
		image_stream->deallocate();
		delete image_stream;

		// Deallocating image matching engine
		image_matcher->deallocate();
		delete image_matcher;

		// Deallocating sequence stream engine
		sequence_stream->deallocate();
		delete sequence_stream;

		// Deallocating sequence matching engine
		sequence_matcher->deallocate();
		delete sequence_matcher;

		// Deallocating input images' names
		imageNames.clear();

		// Deallocate OpenCV images
		UI_img.release();
		readImg.release();

		// Destroing display window
		CV_DESTROYWINDOW("UI");
		
		allocated = false;
	}
}

PREVIeW::PREVIeW()
{ 
	experiment_executed = false;
	allocated = false;
}

PREVIeW::~PREVIeW()
{
	if(allocated)
		deallocate();
}
