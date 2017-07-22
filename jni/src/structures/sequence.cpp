//============================================================================
// Name        : sequence.cpp
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
#include "structures/sequence_matching.h"
#include "structures/sequence.h"

// Shared Timer
#include "profiler.h"
extern Profiler_Engine profiler;

extern bool endProcessByUser;

Sequence_Engine::Sequence_Engine(unsigned int sequenceID_in, Vocabulary_Tree_Engine* Vocabulary_tree_in)
{
	sequenceID = sequenceID_in;
	Vocabulary_tree = Vocabulary_tree_in;

	if(Vocabulary_tree_in != NULL)
	{
		unit_sequence_descriptor_sp.resize(Vocabulary_tree_in->imgDecrSize, 1);
		unit_sequence_descriptor_sp.setZero();
	}

	allocated = true;
}

void Sequence_Engine::add_new_Image(Image_Engine* newImage)
{
	image_members.push_back(newImage);
}

void Sequence_Engine::finalize()
{
	float norm = 0.0f;
	unsigned int nonZeroDescriptorIDs_seq_size = Vocabulary_tree->nonZeroDescriptorIDs_seq.size();

	std::sort(Vocabulary_tree->nonZeroDescriptorIDs_seq.begin(), Vocabulary_tree->nonZeroDescriptorIDs_seq.end());
	for(unsigned int i = 0; i < nonZeroDescriptorIDs_seq_size; i++)
	{
		int tempSequenceDescriptorID = Vocabulary_tree->nonZeroDescriptorIDs_seq[i];
		float temp = Vocabulary_tree->sequence_descriptor_temp(tempSequenceDescriptorID);
		Vocabulary_tree->sequence_descriptor_temp(tempSequenceDescriptorID) = UNINITIALIZED_VWV_VALUE;
		#ifdef RETAIN_HIGH_OCCURANCES_ONLY
			int numOfOccurances = Vocabulary_tree->sequence_word_occurances(tempSequenceDescriptorID);
			if (numOfOccurances > OCCURANCES_THRESHOLD)
			{
				unit_sequence_descriptor_sp.insert(tempSequenceDescriptorID, 0) = temp;
				norm += temp * temp;
			}
		#else
			unit_sequence_descriptor_sp.insert(tempSequenceDescriptorID, 0) = temp;
			norm += temp * temp;
		#endif
	}
	unit_sequence_descriptor_sp.makeCompressed(); // optional

	// Normalizing
	norm = std::sqrt(norm);
	unit_sequence_descriptor_sp /= norm;

	Vocabulary_tree->nonZeroDescriptorIDs_seq.resize(0);

	// we do not need to set it to zero, since during a given sequence, 
	// if a new VW is seen -> sequence_word_occurances(i) = 1. 
	// If we see it again -> sequence_word_occurances(i)++. 
	// The VWs seen from previus sequences (sequence_word_occurances(i)!=0) 
	// are not accessed during the current sequence.
	// #ifdef RETAIN_HIGH_OCCURANCES_ONLY
		// Vocabulary_tree->sequence_word_occurances.setConstant(0);
	// #endif

	// Similar to the above
	// Vocabulary_tree->sequence_descriptor_temp.setConstant(UNINITIALIZED_VWV_VALUE);
}

void Sequence_Engine::deallocate()
{
	if(allocated)
	{
		unit_sequence_descriptor_sp.setZero();
		unit_sequence_descriptor_sp.resize(0,0);


		image_members.clear();
		allocated = false;
	}
}

Sequence_Engine::~Sequence_Engine()
{
	if(allocated)
		deallocate();
}
