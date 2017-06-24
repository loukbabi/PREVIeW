//============================================================================
// Name        : storeImageDescriptors.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iomanip>

#include "defines.h"

inline bool storeImageDescriptorSimilarityMatrix(Image_Stream_Engine &image_stream, const char* fileName)
{

	char filePath[64];
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);

	std::ofstream myfile;
	myfile.open(filePath);

	if(myfile.is_open())
	{
		Eigen::MatrixXf sim_matrix;
		sim_matrix = Eigen::MatrixXf::Zero(image_stream.stream.size(), image_stream.stream.size());

		float float_problemSize = float(image_stream.stream.size());

		for(unsigned int i = 0; i < image_stream.stream.size(); i++)
		{
			PRINTF_PLATFORM("Storing Scores progress: %f%%\n", (float(i) / float_problemSize) * 100);
			for(unsigned int j = 0; j < image_stream.stream.size(); j++)
			{
				if( j <= i)
				{
					float tempDist = L1score(image_stream.stream.at(i)->unit_image_descriptor_sp, image_stream.stream.at(j)->unit_image_descriptor_sp);
					sim_matrix(j,i) = tempDist;
				}
			}
		}

		myfile << image_stream.stream.size() << " " << image_stream.stream.size() << "\n";
		myfile << std::setprecision(PRINTING_PRECISION) << sim_matrix;
		myfile << "\n";
	}
	else
	{
		PRINTF_PLATFORM("Could not create file: %s", filePath);
		return false;
	}


	myfile.close();

	return true;
}

inline bool storeSequenceDescriptorSimilarityMatrix(Sequence_Stream_Engine &sequence_stream, const char* fileName)
{

	char filePath[64];
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);

	std::ofstream myfile;
	myfile.open(filePath);

	if(myfile.is_open())
	{
		Eigen::MatrixXf sim_matrix;
		sim_matrix = Eigen::MatrixXf::Zero(sequence_stream.stream.size(), sequence_stream.stream.size());

		float float_problemSize = float(sequence_stream.stream.size());

		for(unsigned int i = 0; i < sequence_stream.stream.size(); i++)
		{
			PRINTF_PLATFORM("Storing Scores progress: %f%%\n", (float(i) / float_problemSize) * 100);
			for(unsigned int j = 0; j < sequence_stream.stream.size(); j++)
			{
				if( j <= i)
				{
					float tempDist = L1score(sequence_stream.stream.at(i)->unit_sequence_descriptor_sp, sequence_stream.stream.at(j)->unit_sequence_descriptor_sp);
					sim_matrix(j,i) = tempDist;
				}
			}
		}

		myfile << sequence_stream.stream.size() << " " << sequence_stream.stream.size() << "\n";
		myfile << std::setprecision(PRINTING_PRECISION) << sim_matrix;
		myfile << "\n";
	}
	else
	{
		PRINTF_PLATFORM("Could not create file: %s", filePath);
		return false;
	}


	myfile.close();

	return true;
}

inline bool storeSequenceDescriptorSimilarityMatrix(Eigen::Array<float, Eigen::Dynamic, Eigen::Dynamic> &similarityMatrix, unsigned int lastSequenceID, const char* fileName)
{
	lastSequenceID++;
	char filePath[64];
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);

	std::ofstream myfile;
	myfile.open(filePath);

	if(myfile.is_open())
	{
		myfile << lastSequenceID << " " << lastSequenceID << "\n";
		myfile << std::setprecision(PRINTING_PRECISION) << similarityMatrix.block(0, 0, lastSequenceID, lastSequenceID);
		myfile << "\n";
	}
	else
	{
		PRINTF_PLATFORM("Could not create file: %s", filePath);
		return false;
	}
	PRINTF_PLATFORM("Similarity Matrix was stored successfully!\n");

	myfile.close();

	return true;
}

inline bool storeImageMatchList(Image_Matching_Engine &image_matcher, unsigned int imageStreamSize, const char* fileName)
{
	char filePath[64];
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);
	std::ofstream myfile;
	myfile.open(filePath);

	if(!myfile.is_open())
	{
		PRINTF_PLATFORM("Could not create file \"%s\".\n", filePath);
		return false;
	}


	myfile << imageStreamSize << " " << image_matcher.match_list.size() << "\n";
	if(image_matcher.match_list.size() != 0)
	{
		int size = int(image_matcher.match_list.size()) - 1;
		for (int i = 0; i <= size; i++)
		{
			int imgId1 = image_matcher.match_list[i]->pair(0)->imageID;
			int imgId2 = image_matcher.match_list[i]->pair(1)->imageID;
			myfile <<imgId1<<" "<<imgId2;
			if(i != size)
				myfile << "\n";
		}
	}

	myfile.close();

	PRINTF_PLATFORM("Image matching list was stored successfully!\n");
	return true;
}

inline bool storeSeqMatchList(Sequence_Matching_Engine &sequence_matcher, unsigned int seqStreamSize, const char* fileName)
{
	char filePath[64];
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);
	std::ofstream myfile;
	myfile.open(filePath);

	if(!myfile.is_open())
	{
		PRINTF_PLATFORM("Could not create file \"%s\".\n", filePath);
		return false;
	}

	myfile << seqStreamSize << " " << seqStreamSize << "\n";
	unsigned int size = int(sequence_matcher.match_list.size()) - 1;
	for (unsigned int i = 0; i <= size; i++)
	{
		int seqId1 = sequence_matcher.match_list[i]->pair(0)->sequenceID;
		int seqId2 = sequence_matcher.match_list[i]->pair(1)->sequenceID;
		myfile <<seqId1<<" "<<seqId2;
		if(i != size)
		{
			myfile << "\n";
		}
	}


	myfile.close();
	return true;
}

