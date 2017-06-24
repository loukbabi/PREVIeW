//============================================================================
// Name        : utils.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

inline bool writeDatasetNum(int datasetId, const char* fileName){

	char filePath[64];
	std::ofstream myfile;
	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);

	myfile.open(filePath);
	if(!myfile.is_open())
	{
		PRINTF_PLATFORM("Could not create %s.\n", filePath);
		return false;
	}

	myfile << datasetId;

	myfile.close();

	sprintf(filePath, "%s/%s.%s", OUTPUT_METADATA_PATH, fileName, OUTPUT_METADATA_FORMAT);

	myfile.open(filePath);
	if(!myfile.is_open())
	{
		PRINTF_PLATFORM("Could not create %s.\n", filePath);
		return false;
	}

	myfile << datasetId;

	myfile.close();
	return true;
}

inline void str2ucharLess2Most(std::string &token_8bit, unsigned char *tempBite)
{
	unsigned char tempBite2 = 0;
	for(int i = 0; i < int(token_8bit.length()); i++)
	{
		int j = token_8bit.length() - i - 1;

		if(token_8bit[j] == '1')
		{
			tempBite2 += (1<<i);
		}
	}

	*tempBite = tempBite2;
}

inline void str2ucharMost2Less(std::string &token_8bit, unsigned char *tempBite)
{
	unsigned char tempBite2 = 0;
	for(unsigned int i = 0; i < token_8bit.length(); i++)
	{
		if(token_8bit[i] == '1')
		{
			tempBite2 += (1<<i);
		}
	}

	*tempBite = tempBite2;
}

inline unsigned int hammingDistance(unsigned char *descriptorPtr, unsigned char *tempNodeDescriptorPtr, int descriptorSize_bytes)
{
	unsigned int distance = 0;

	for(int i = 0; i < descriptorSize_bytes; i++)
	{
		unsigned char val = descriptorPtr[i] ^ tempNodeDescriptorPtr[i];
		while(val) // Brian Kernighan algorithm
		{
			distance++;
			val &= val - 1;
		}
	}

	return distance;
}

inline unsigned int HammingDistSparce(Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic> temp1, Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic> temp2)
{

	unsigned int sum = 0;
	for(int i = 0; i < temp1.rows(); i++)
	{
		sum += temp1(i) == temp2(i);
	}
	return sum;
}

inline float L1score(Eigen::SparseMatrix<float> &unit_image_descriptor1, Eigen::SparseMatrix<float> &unit_image_descriptor2)
{
	return ( 1 - ( 0.5*(unit_image_descriptor1 - unit_image_descriptor2).norm() ) );
}

inline bool readFolderContent(std::vector<std::string> &out,const std::string &directory)
{
	std::vector<std::string> temp;

	DIR *dir;
	class dirent *ent;
	class stat st;

	dir = opendir(directory.c_str());
	if(dir == NULL)
	{
		PRINTF_PLATFORM("Directory \"%s\" could not be opened.\n", directory.c_str());
		return false;
	}

	#ifdef INPUT_IMAGE_PREFIX
		std::string imageNamePrefix = INPUT_IMAGE_PREFIX;
		int imageNamePrefix_length = imageNamePrefix.length();
	#endif

	#ifdef INPUT_IMAGE_ENDFIX
		std::string imageNameEndfix = INPUT_IMAGE_ENDFIX;
		std::string fileType = INPUT_IMAGE_FORMAT;
		int imageNameEndfix_length = imageNameEndfix.length();
		int totalImageNameEndfix_length = imageNameEndfix_length + fileType.length() + 1;
	#endif

	while ((ent = readdir(dir)) != NULL) {
		const std::string file_name = ent->d_name;

		if (file_name[0] == '.')
			continue;

		if (file_name.substr(file_name.length()-3, 3) != INPUT_IMAGE_FORMAT)
			continue;

		#ifdef INPUT_IMAGE_PREFIX
			if (file_name.substr(0, imageNamePrefix_length) != INPUT_IMAGE_PREFIX)
				continue;
		#endif

		#ifdef INPUT_IMAGE_ENDFIX
			if (file_name.substr(file_name.size() - totalImageNameEndfix_length, imageNameEndfix_length) != INPUT_IMAGE_ENDFIX)
				continue;
		#endif
		temp.push_back(file_name);
	}
	closedir(dir);

	std::sort(temp.begin(), temp.end());

	for(unsigned int i = 0; i < temp.size(); i++)
	{

		const std::string full_file_name = directory + "/" + temp.at(i);

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		out.push_back(full_file_name);
	}

	temp.clear();

	if(out.size() == 0)
	{
		PRINTF_PLATFORM("No image of type \"%s\" files where found.\n", INPUT_IMAGE_FORMAT);
		return false;
	}

	return true;
}

#endif // UTILS_H
