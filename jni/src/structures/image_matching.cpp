//============================================================================
// Name        : image_matching.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <stdio.h>
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

#include "structures/feature_extractor.h"
#include "structures/vocabulary_tree.h"
#include "structures/sequence.h"
#include "structures/image.h"
#include "structures/image_matching.h"

// Shared Timer
#include "profiler.h"
extern Profiler_Engine profiler;

#include "utils.h"
extern bool endProcessByUser;


Image_Matching_Engine::Image_Matching_Engine(Vocabulary_Tree_Engine* Vocabulary_tree_in)
{
	Vocabulary_tree = Vocabulary_tree_in;
	allocated = true;
}

bool Image_Matching_Engine::findMatchANDupdateInverseIndexing_withTimeLag(std::vector<Image_Engine*> &stream)
{
	if(stream.size() > PREVENT_LOOP_DETECTION_TIME_LAG_IMGS)
	{
		updateInverseIndexing_withTimeLag(stream);
		return findMatch(stream.back());
	}else
		return false;
}

void Image_Matching_Engine::updateInverseIndexing_withTimeLag(std::vector<Image_Engine*> &stream) // Update Inverse Indexing
{
	int inversIndexing_ID2Update = stream.size() - 1 - PREVENT_LOOP_DETECTION_TIME_LAG_IMGS;
	Image_Engine* inversIndexing_img2Update = stream[inversIndexing_ID2Update];

	for (Eigen::SparseMatrix<float>::InnerIterator it(inversIndexing_img2Update->unit_image_descriptor_sp, 0); it; ++it)
		inverseIndexingList[it.index()].push_back(inversIndexing_img2Update);
}

bool Image_Matching_Engine::findMatch(Image_Engine *lastImage)
{
	memset(needToBeChecked, 1, (int(lastImage->imageID) + 1 - PREVENT_LOOP_DETECTION_TIME_LAG_IMGS) );

	float maxSimScore = -1000.0f;
	Image_Engine *mathcingImageTemp = NULL;

	for (Eigen::SparseMatrix<float>::InnerIterator it(lastImage->unit_image_descriptor_sp, 0); it; ++it)
	{
		const int nonZeroIndex = it.index();

		for(std::vector<Image_Engine*>::iterator stdIt = inverseIndexingList[nonZeroIndex].begin(), end = inverseIndexingList[nonZeroIndex].end(); stdIt != end; ++stdIt)
		{
			Image_Engine *tempImage = (*stdIt);
			unsigned int imageID = tempImage->imageID;

			if(needToBeChecked[imageID])
			{
				needToBeChecked[imageID] = 0;
				float tempSimScore = L2score(lastImage->unit_image_descriptor_sp, tempImage->unit_image_descriptor_sp);

				if( (tempSimScore > SCORE_THRESHOLD_IMG) && (tempSimScore > maxSimScore) )
				{
					maxSimScore = tempSimScore;
					mathcingImageTemp = tempImage;
				}
			}
		}
	}

	if(mathcingImageTemp == NULL)	return false;

	CV_IMSHOW("Match Stream", mathcingImageTemp->Image);
	PRINTF_PLATFORM("Adding Image Pair: <%u, %u> .\n", lastImage->imageID, mathcingImageTemp->imageID);
	Image_Match *newMatch = new Image_Match(lastImage, mathcingImageTemp, maxSimScore);
	match_list.push_back(newMatch);

	return true;
}

bool Image_Matching_Engine::findImageMembersPairs(Sequence_Engine *lastSequence, Sequence_Engine* matchedSequence, float maxSimScore)
{
	std::vector<Image_Engine*> &matched_image_members = matchedSequence->image_members;
	std::vector<Image_Engine*> &lastSeq_image_members = lastSequence->image_members;
	int matched_image_members_Size = int(matched_image_members.size());
	int lastSeq_image_members_Size = int(lastSeq_image_members.size());
	std::vector<int> imageMatches;

	int start_matched = 0;

	for(int i = 0; i < lastSeq_image_members_Size; i++)
	{

		float maxImageSim = -10.0f;
		int maxImageDist_ID = 0;
		for(int j = start_matched; j < matched_image_members_Size; j++)
		{
			float tempImageSim = L2score(lastSeq_image_members[i]->unit_image_descriptor_sp, matched_image_members[j]->unit_image_descriptor_sp);
			if( tempImageSim > SCORE_THRESHOLD_IMG && tempImageSim > maxImageSim)
			{
				maxImageSim = tempImageSim;
				maxImageDist_ID = j;
			}
		}
		if(maxImageSim != -10.0f)
		{
			start_matched = maxImageDist_ID;
			imageMatches.push_back(i);
			imageMatches.push_back(maxImageDist_ID);
		}
	}

	if(imageMatches.size() > 0)
	{

		if(imageMatches[1] == imageMatches.back() || imageMatches[0] == imageMatches.end()[-2]) // if all the matches have been done with the same image from the matched_sequence = if that database image hase very few feature points
			return false;

		for(unsigned int i = 0; i < imageMatches.size(); i+=2)
		{
			Image_Match *newMatch = new Image_Match(lastSeq_image_members[imageMatches[i]], matched_image_members[imageMatches[i+1]], maxSimScore);
			match_list.push_back(newMatch);
		}
		return true;
	}
	else
		return false;
}

Image_Matching_Engine::~Image_Matching_Engine()
{
	if(allocated)
		deallocate();
}

void Image_Matching_Engine::deallocate()
{
	if(allocated)
	{
		for(unsigned int i = 0; i < match_list.size(); i++)
			delete match_list[i];

		match_list.clear();
		allocated = false;
	}
}
