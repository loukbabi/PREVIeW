//============================================================================
// Name        : sequence.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef SEQUENCE_H
#define SEQUENCE_H

class Sequence_Engine{

private:
	Vocabulary_Tree_Engine* Vocabulary_tree;

public:
	Eigen::SparseMatrix<float> unit_sequence_descriptor_sp;
	std::vector<Image_Engine*> image_members;
	unsigned int sequenceID;
	bool allocated;

	Sequence_Engine(unsigned int sequenceID_in = 0, Vocabulary_Tree_Engine* Vocabulary_tree_in = NULL);
	void add_new_Image(Image_Engine* newImage);
	void finalize();
	void deallocate();
	~Sequence_Engine();
};


#endif // SEQUENCE_H
