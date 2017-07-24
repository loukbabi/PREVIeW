//============================================================================
// Name        : sequence_stream.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
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
#include "structures/image_stream.h"
#include "structures/sequence_matching.h"
#include "structures/sequence.h"
#include "structures/sequence_stream.h"
#include "structures/image_matching.h"

// Shared Timer
#include "profiler.h"
extern Profiler_Engine profiler;

#include "utils.h"

Sequence_Stream_Engine::Sequence_Stream_Engine(Vocabulary_Tree_Engine* Vocabulary_tree_in)
{
	Vocabulary_tree = Vocabulary_tree_in;
	unsigned int sequence_id = stream.size();
	Sequence_Engine *newSequence = new Sequence_Engine(sequence_id, Vocabulary_tree_in);
	isFirstSequence = true;
	allocated = true;
	currentSequence = newSequence;
	previous_numberOfVisualWords = 0.0f;  
}

bool Sequence_Stream_Engine::createNewSequenceIfisTime()
{
	bool createNewSeq_flag = isTimeToCreateNewSequence();

	if(createNewSeq_flag)
	{
		createNewSequence();
		return true;
	}
	return false;
}

bool Sequence_Stream_Engine::isTimeToCreateNewSequence()
{
	if(currentSequence->image_members.size() >= 2)
	{
		float newOld_ratio = (float(Vocabulary_tree->nonZeroDescriptorIDs_seq.size()) - previous_numberOfVisualWords) / float(Vocabulary_tree->NumOfVisualWordsTempG);

		if(currentSequence->image_members.size() > SEQ_SEP_TIME_WINDOW)
		{
			if(currentSequence->image_members.size() > SEQ_SEP_MIN_SEQUENCE_SIZE)
			{
				if( Vocabulary_tree->nonZeroDescriptorIDs_seq.size() > MAX_VW_SEQUENCE_SIZE )
				{
					previous_numberOfVisualWords = 0.0f;
					return true;
				}

				if(newOld_ratio > NEW_OLD_RATIO )
				{
					previous_numberOfVisualWords = 0.0f;
					return true;
				}
			}
		}
	}


	previous_numberOfVisualWords = float(Vocabulary_tree->nonZeroDescriptorIDs_seq.size());
	return false;
}

void Sequence_Stream_Engine::createNewSequence()
{
	if(!isFirstSequence)
		endCurrentSequence();

	Sequence_Engine *newSequence = new Sequence_Engine(stream.size(), Vocabulary_tree);
	currentSequence = newSequence;

	isFirstSequence = false;
}

void Sequence_Stream_Engine::add_new_image(Image_Engine *newImage)
{
	currentSequence->add_new_Image(newImage);
}

void Sequence_Stream_Engine::endCurrentSequence()
{
	currentSequence->finalize();

	stream.push_back(currentSequence);
}

Sequence_Stream_Engine::~Sequence_Stream_Engine()
{
	if(allocated)
		deallocate();
}

void Sequence_Stream_Engine::deallocate()
{
	if(allocated)
	{
		for(unsigned int i = 0; i < stream.size(); i++)
			stream[i]->deallocate();

		delete currentSequence;
		stream.clear();
		allocated = false;
	}
}