//============================================================================
// Name        : sequence_matching.cpp
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
#include "structures/image.h"
#include "structures/sequence_matching.h"
#include "structures/image_matching.h"
#include "structures/sequence.h"

// Shared Timer
#include "profiler.h"
extern Profiler_Engine profiler;

#include "utils.h"
extern bool endProcessByUser;


Sequence_Matching_Engine::Sequence_Matching_Engine(Vocabulary_Tree_Engine* Vocabulary_tree_in, Image_Matching_Engine *image_matcher_in)
{
	Vocabulary_tree = Vocabulary_tree_in;
	image_matcher = image_matcher_in;
	inverseIndexingList.reserve(Vocabulary_tree->imgDecrSize);

	std::vector<Sequence_Engine*> tempVector;
	for(int i = 0; i < Vocabulary_tree->imgDecrSize; i++)
	{
		inverseIndexingList.push_back(tempVector);
		inverseIndexingList.back().reserve(INVERSE_INDEXING_SEQUENCES_PER_WORD_ESTIMATION);
	}
	memset(needToBeChecked, 0, MAX_NUMBER_OF_IMAGES);


	similarityMatrix = Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic>::Zero(MAX_NUMBER_OF_SEQUENCES, MAX_NUMBER_OF_SEQUENCES);
	filterKernel << FILTER_KERNEL;

	allocated = true;
}

bool Sequence_Matching_Engine::findMatchANDupdateInverseIndexing_withTimeLag(std::vector<Sequence_Engine*> &stream, cv::Mat &output_UI_img)
{
	if( int(stream.size()) > PREVENT_LOOP_DETECTION_TIME_LAG_SEQS + ADDITIONAL_OFFSET_DUE_TO_NORM) //PREVENT_LOOP_DETECTION_TIME_LAG_SEQS
	{
P_TIC("InverseIndexingUpdate");
		updateInverseIndexing_withTimeLag(stream);
P_TOC("InverseIndexingUpdate");
		
		return findMatch(stream, output_UI_img);
	}else
		return false;
}

void Sequence_Matching_Engine::updateInverseIndexing_withTimeLag(std::vector<Sequence_Engine*> &stream)
{
	int inversIndexing_ID2Update = stream.size() - 1 - PREVENT_LOOP_FOR_CLOSE_IN_TIME_SEQS;
	Sequence_Engine* inversIndexing_seq2Update = stream[inversIndexing_ID2Update];

	for (Eigen::SparseMatrix<float>::InnerIterator it(inversIndexing_seq2Update->unit_sequence_descriptor_sp, 0); it; ++it)
		inverseIndexingList[it.index()].push_back(inversIndexing_seq2Update);
}

bool Sequence_Matching_Engine::findMatch(std::vector<Sequence_Engine*> &stream, cv::Mat &output_UI_img)
{
P_TIC("FindSequenceMatch");

	Sequence_Engine* lastSequence = stream.end()[-PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL - 1];

	memset(needToBeChecked + PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL, 1, std::max(0, (int(lastSequence->sequenceID) + 1 - (2 * PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL))) ); // mallon na min mpainei kan stin diadikasia. Ohi opws to ehw twra, na to empodizei to needToBeChecked

	float maxSimScore = -1000.0f;
	Sequence_Engine *matchingSequenceTemp = NULL;

	#ifdef USE_NORMALIZATION
		Sequence_Engine* previousSequence = stream.end()[-PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL - 2];
		float normScore = L2score(lastSequence->unit_sequence_descriptor_sp, previousSequence->unit_sequence_descriptor_sp);
	#endif

	for (Eigen::SparseMatrix<float>::InnerIterator it(lastSequence->unit_sequence_descriptor_sp, 0); it; ++it)
	{
						
		const int nonZeroIndex = it.index();
		for(std::vector<Sequence_Engine*>::iterator stdIt = (inverseIndexingList[nonZeroIndex].begin()), end = inverseIndexingList[nonZeroIndex].end(); stdIt != end; ++stdIt)
		{
			Sequence_Engine *tempSequence = (*stdIt);
			unsigned int sequenceID = tempSequence->sequenceID;

			if(needToBeChecked[sequenceID])
			{
				needToBeChecked[sequenceID] = 0;

				#ifdef USE_FILETERING
					float tempSimScore = filteredL2score(lastSequence, tempSequence, stream);
				#else
					float tempSimScore = L2score(lastSequence->unit_sequence_descriptor_sp, tempSequence->unit_sequence_descriptor_sp);
				#endif
				#ifdef USE_NORMALIZATION
					tempSimScore /= normScore;
				#endif

				if( tempSimScore > SCORE_THRESHOLD_SEQ && tempSimScore > maxSimScore)
				{
			    	maxSimScore = tempSimScore;
					matchingSequenceTemp = tempSequence;		
				}
			}
		}
	}


P_TOC("FindSequenceMatch");

	if(matchingSequenceTemp == NULL)	return false;
	
P_TIC("FindImageMatch");
	bool flag;
	#ifdef MULTIPLE_SEQUENCE_MATCHING
		Sequence_Engine sequenceMatch(0, Vocabulary_tree);


		Sequence_Engine *tempSequence = stream[matchingSequenceTemp->sequenceID - 1];
	    std::copy(tempSequence->image_members.begin(), tempSequence->image_members.end(), 
	    	std::back_inserter(sequenceMatch.image_members));
		std::copy(matchingSequenceTemp->image_members.begin(), matchingSequenceTemp->image_members.end(), 
	    	std::back_inserter(sequenceMatch.image_members));
		tempSequence = stream[matchingSequenceTemp->sequenceID + 1];
	    std::copy(tempSequence->image_members.begin(), tempSequence->image_members.end(), 
	    	std::back_inserter(sequenceMatch.image_members));

		flag = image_matcher->findImageMembersPairs(lastSequence, &sequenceMatch, maxSimScore);

		sequenceMatch.deallocate();
	#else
		flag = image_matcher->findImageMembersPairs(lastSequence, matchingSequenceTemp, maxSimScore);
	#endif
P_TOC("FindImageMatch");

	if(!flag)
		return false;
	
	PRINTF_PLATFORM("Adding Sequence Pair: <%u, %u>.\n", lastSequence->sequenceID, matchingSequenceTemp->sequenceID);

	Sequence_Match *newMatch = new Sequence_Match(lastSequence, matchingSequenceTemp, maxSimScore);
	match_list.push_back(newMatch);

	matchingSequenceTemp->image_members[0]->Image.copyTo(output_UI_img);

	return true;
}

float Sequence_Matching_Engine::filteredL2score(Sequence_Engine* lastSequence, Sequence_Engine *tempSequence, std::vector<Sequence_Engine*> &stream)
{
	int loopStart_j = lastSequence->sequenceID - PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL;
	int loopEnd_j = loopStart_j + KERNEL_SIZE;

	int loopStart_i = tempSequence->sequenceID - PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL;
	int loopEnd_i = loopStart_i + KERNEL_SIZE;

	for(int j = loopStart_j; j < loopEnd_j; j++)																					
		for(int i = loopStart_i; i < loopEnd_i; i++)																				
			if(similarityMatrix(i,j) == 0.0f)																																															
				similarityMatrix(i,j) = L2score(stream[i]->unit_sequence_descriptor_sp, stream[j]->unit_sequence_descriptor_sp);

	float maxValue = similarityMatrix.block<KERNEL_SIZE, KERNEL_SIZE>(loopStart_i, loopStart_j).maxCoeff();

	return ( (similarityMatrix.block<KERNEL_SIZE, KERNEL_SIZE>(loopStart_i, loopStart_j) / maxValue) * filterKernel).sum();
}

Sequence_Matching_Engine::~Sequence_Matching_Engine()
{
	if(allocated)
		deallocate();
}

void Sequence_Matching_Engine::deallocate()
{
	if(allocated)
	{
		for(unsigned int i = 0; i < inverseIndexingList.size(); i++)
			inverseIndexingList[i].clear();

		for(unsigned int i = 0; i < match_list.size(); i++)
			delete match_list[i];

		match_list.clear();

		similarityMatrix.resize(0,0);
		inverseIndexingList.clear();
		allocated = false;
	}
}
