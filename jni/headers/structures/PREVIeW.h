// Name        : PREVIeW.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef PREVIEW_H
#define PREVIEW_H

#include "defines.h"

#include "structures/vocabulary_tree.h"
#include "structures/feature_extractor.h"
#include "structures/image.h"
#include "structures/image_stream.h"
#include "structures/sequence_matching.h"
#include "structures/sequence.h"
#include "structures/sequence_stream.h"
#include "structures/image_matching.h"

// Shared Timer
#include "tictoc.h"
#include "profiler.h"

#include "utils.h"

// Optional for include tbb (pipelining)
#ifdef WITH_TBB_FOR_PIPELINING
	#include "tbb/task_group.h"
#endif

class PREVIeW{

private:
bool allocated;
bool experiment_executed;
unsigned int dropedImageCount;

cv::Mat UI_img;
cv::Mat input_UI_img;
cv::Mat output_UI_img;

cv::Mat readImg;

//Profiler
#ifdef PROFILER
	std::vector<std::string> *labels;
#endif

// Used only if reading images from memory	
std::vector<std::string> imageNames;

unsigned int totalImgCount;
unsigned int acquiredImagesCount;

Vocabulary_Tree_Engine *Vocabulary_tree;

Image_Stream_Engine *image_stream;
Image_Matching_Engine *image_matcher;

Sequence_Stream_Engine *sequence_stream;
Sequence_Matching_Engine *sequence_matcher;

TBB_INIT();

// Allocates the Image used for user interface
void allocate_UI_img(cv::Mat &inputImage);

public:

	// Constructor
	PREVIeW();

	// Deconstructor
	~PREVIeW();

	// Read metadata from memory
	bool readMetadata(const char *VT_file_name = NULL, const char *Input_img_path = NULL);

	bool initialize(const char *VT_file_name = NULL, const char *Input_img_path = NULL);

	// Allocate the UI image
	cv::Mat* createUI();

	// Executed if the whole database is read from the memory
	bool deploy_fromMemory();

	// Addes a new image to the map
	cv::Mat* addImage(cv::Mat &inputImage);

	// Finalizeing the input stream (alwase needed in the end)
	void finalize();

	// Store the experiment's metadata
	bool storeOutputMetadata();

	// Deallocating memory
	void deallocate();
};

#endif // PREVIEW_H
