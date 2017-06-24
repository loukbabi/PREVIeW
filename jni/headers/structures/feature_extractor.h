//============================================================================
// Name        : feature_extractor.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

class Feature_Extractor_Engine{

public:
	cv::Ptr<cv::FeatureDetector> detector;
	cv::Ptr<cv::DescriptorExtractor> descriptor;

	bool initialize();
	void deallocate();

};

#endif // FEATURE_EXTRACTOR_H
