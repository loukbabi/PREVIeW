//============================================================================
// Name        : sequence_matching.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef SEQUENCE_MATCHING_H
#define SEQUENCE_MATCHING_H

class Sequence_Engine;

class Sequence_Match{

public:
	Eigen::Matrix<Sequence_Engine*, 2, 1> pair;
	float matchingScore;

	Sequence_Match(Sequence_Engine *image1, Sequence_Engine* image2, float matchingScore_in)
	{
		pair << image1,
				image2;

		matchingScore = matchingScore_in;
	}

	~Sequence_Match()
	{

	}
};

class Sequence_Matching_Engine{

public:
	std::vector<Sequence_Match*> match_list;

	std::vector< std::vector<Sequence_Engine*> > inverseIndexingList;
	Vocabulary_Tree_Engine* Vocabulary_tree;
	unsigned char needToBeChecked[MAX_NUMBER_OF_IMAGES];
	bool allocated;
	Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic> similarityMatrix;

	Eigen::Array<float, KERNEL_SIZE, KERNEL_SIZE> filterKernel;
	Image_Matching_Engine *image_matcher;

	Sequence_Matching_Engine(Vocabulary_Tree_Engine* Vocabulary_tree_in, Image_Matching_Engine *image_matcher);

	void updateInverseIndexing_withTimeLag(std::vector<Sequence_Engine*> &stream);

	bool findMatchANDupdateInverseIndexing_withTimeLag(std::vector<Sequence_Engine*> &stream, cv::Mat &output_UI_img);
	bool findMatch(std::vector<Sequence_Engine*> &stream, cv::Mat &output_UI_img);

	float filteredL1score(Sequence_Engine* lastSequence, Sequence_Engine *tempSequence, std::vector<Sequence_Engine*> &stream);

	~Sequence_Matching_Engine();

	void deallocate();

};

#endif // SEQUENCE_MATCHING_H
