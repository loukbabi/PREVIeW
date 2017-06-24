//============================================================================
// Name        : image.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef IMAGE_H
#define IMAGE_H

class Image_Engine{

private:
	Vocabulary_Tree_Engine* Vocabulary_tree;

public:
	int image_descriptor_size;

	Eigen::SparseMatrix<float> unit_image_descriptor_sp;

	cv::Mat Image;
	bool success;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	int numberOfFeatures;
	unsigned int imageID;
	unsigned int NumOfVisualWords;
	unsigned int imageGlobalID;

	bool allocated;

	bool extractImageFeatures(Feature_Extractor_Engine& feature_extractor, cv::Mat &inputImage);

	bool image2descriptor(Feature_Extractor_Engine& feature_extractor, cv::Mat &input_UI_img, cv::Mat &inputImage);

	bool readImgFromMemory(std::string &imageName);

	void calculateOpticalFlow(Image_Engine *prev_img);

	Image_Engine(unsigned int imageID_in, cv::Mat &inputImage, Feature_Extractor_Engine& feature_extractor,
							Vocabulary_Tree_Engine* Vocabulary_tree_in, unsigned int imageGlobalID_in, cv::Mat &input_UI_img);


	~Image_Engine();
	void deallocate();
	void deallocate_less();
};

#endif // IMAGE_H
