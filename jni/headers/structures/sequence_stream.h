//============================================================================
// Name        : sequence_stream.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef SEQUENCE_STREAM_H
#define SEQUENCE_STREAM_H

class Sequence_Stream_Engine{

private:
	Vocabulary_Tree_Engine *Vocabulary_tree;
	float previous_numberOfVisualWords;

public:
	std::vector<Sequence_Engine*> stream;
	Sequence_Engine *currentSequence;

	#ifdef _DEBUG_
		std::vector<float> endingSeqThres;
		std::vector<float> endingSeqThres2;
	#endif

	bool isFirstSequence, allocated;

	Sequence_Stream_Engine(Vocabulary_Tree_Engine *Vocabulary_tree_in);
	void add_new_image(Image_Engine *newImage);
	bool createNewSequenceIfisTime();
	bool isTimeToCreateNewSequence();
	void createNewSequence();
	void endCurrentSequence();



	void deallocate();
	~Sequence_Stream_Engine();
};


#endif // SEQUENCE_STREAM_H
