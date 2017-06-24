//============================================================================
// Name        : image_matching.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef IMAGE_MATCHING_H
#define IMAGE_MATCHING_H

class Image_Match{

public:
	Eigen::Matrix<Image_Engine*, 2, 1> pair;
	float matchingScore;

	Image_Match(Image_Engine *image1, Image_Engine* image2, float matchingScore_in)
	{
		pair << image1,
				image2;

		matchingScore = matchingScore_in;
	}

	~Image_Match()
	{

	}
};

class Image_Matching_Engine{

public:
	std::vector<Image_Match*> match_list;

	std::vector< std::vector<Image_Engine*> > inverseIndexingList;
	Vocabulary_Tree_Engine* Vocabulary_tree;
	unsigned char needToBeChecked[MAX_NUMBER_OF_IMAGES];
	bool allocated;

	Image_Matching_Engine(Vocabulary_Tree_Engine* Vocabulary_tree_in);

	void updateInverseIndexing_withTimeLag(std::vector<Image_Engine*> &stream);

	bool findMatchANDupdateInverseIndexing_withTimeLag(std::vector<Image_Engine*> &stream);
	bool findMatch(Image_Engine *lastImage);
	bool findImageMembersPairs(Sequence_Engine *lastSequence, Sequence_Engine* matchedSequence, float maxSimScore);

	~Image_Matching_Engine();
	void deallocate();
};

#endif // IMAGE_MATCHING_H
