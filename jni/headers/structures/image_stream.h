//============================================================================
// Name        : image_stream.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef IMAGE_STREAM_H
#define IMAGE_STREAM_H

class Image_Stream_Engine{

private:
	Vocabulary_Tree_Engine *Vocabulary_tree;

	bool _validFeatures;

public:
	std::vector<Image_Engine*> stream;

	std::vector<int> inverse_indexing;
	Feature_Extractor_Engine feature_extractor;
	bool allocated;

	inline bool validFeatures() const{		return _validFeatures;		}

	bool add_new_image(cv::Mat &inputImage, unsigned int imageGlobalID_in, cv::Mat &input_UI_img);

	void deallocate();
	Image_Stream_Engine(Vocabulary_Tree_Engine *Vocabulary_tree_in);
	~Image_Stream_Engine();

};

#endif // IMAGE_STREAM_H
