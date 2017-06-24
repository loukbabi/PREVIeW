//============================================================================
// Name        : vocabulary_tree.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <bitset>
#include <stack>
#include <bitset>

#ifdef __ANDROID_UI__
	#include <android/log.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "defines.h"
#include "utils.h"
#include "utils_armNeon.h"

#include "structures/feature_extractor.h"
#include "structures/vocabulary_tree.h"
#include "structures/image.h"
#include "structures/sequence.h"
#include "structures/image_stream.h"
#include "structures/image_matching.h"
#include "structures/sequence_stream.h"

// Shared Timer
#include "tictoc.h"
#include "profiler.h"
extern Profiler_Engine profiler;

VT_node::VT_node(int nodeID_in, float weight_in, int K_in, unsigned char *descriptor_in, int *imageDescriptorIDGrouing_in, bool is_root)
{
	allocated = true;
	nodeID = nodeID_in;
	weight = weight_in;
	parentNode = NULL;
	parentNodeID = 0;
	imageDescriptorID = NOT_LEAF_NODE_VALUE;

	isLeafNode = false;

	int imageDescriptorIDGrouing = *imageDescriptorIDGrouing_in;

	childrenNodes.reserve(K_in);
	childrenNodesIDs.reserve(K_in);
	directIndexing_ID = -10;

	if(is_root)
		memset(descriptor, 0, DESCR_SIZE_BYTE);
	else
		memcpy(descriptor, descriptor_in, DESCR_SIZE_BYTE);

	*imageDescriptorIDGrouing_in = imageDescriptorIDGrouing;

}
void VT_node::deallocate()
{
	if(allocated)
	{
		childrenNodes.clear();
		childrenNodesIDs.clear();
		allocated = false;
	}
}
VT_node::~VT_node()
{
	if(allocated)
		deallocate();
}

bool Vocabulary_Tree_Engine::read_VT_file(const char * fileName)
{
	imageDescriptorIDGrouing = 0;

	char file_path[64];
	sprintf(file_path, "%s/%s.%s", INPUT_METADATA_PATH, fileName, VOCABULARY_TREE_FILE_FORMAT);

	if(!read_VT_properties(file_path)) 	return false;
	if(!read_VT_Nodes(file_path)) 		return false;

	allocated = true;

	return true;
}
Vocabulary_Tree_Engine::~Vocabulary_Tree_Engine()
{
	if(allocated)
		deallocate();
}

bool Vocabulary_Tree_Engine::read_VT_properties(const char *fileName)
{
	std::ifstream file(fileName);
	std::string temp_str;

	if(!file){
		PRINTF_PLATFORM("File \"%s\" not found.", fileName);
		return false;
	}

	std::getline(file, temp_str); //%YAML:1.0
	std::getline(file, temp_str); //vocabulary:

	std::getline(file, temp_str); //K:
	K = atoi( temp_str.substr(temp_str.length()-2, 2).c_str() );

	std::getline(file, temp_str); //L: ...:
	L = atoi( temp_str.substr(temp_str.length()-1, 1).c_str() );

	std::getline(file, temp_str); //scoringType: ...
	scoringType =  atoi( temp_str.substr(temp_str.length()-1, 1).c_str() );

	std::getline(file, temp_str); //weightingType: ...
	weightingType =  atoi( temp_str.substr(temp_str.length()-1, 1).c_str() );

	std::getline(file, temp_str); //imgDecrSize: ...
	std::istringstream tokenizer1(temp_str);
	std::string token1;
	std::getline(tokenizer1, token1, ':');
	imgDecrSize = atoi( temp_str.substr(token1.length()+2, temp_str.length()-token1.length()).c_str() );

	std::getline(file, temp_str); //numOFnodes: ...
	std::istringstream tokenizer2(temp_str);
	std::string token2;
	std::getline(tokenizer2, token2, ':');
	numOFnodes =  atoi( temp_str.substr(token2.length()+2, temp_str.length()-token2.length()).c_str() );

	NodeID_2_VTptr.resize(numOFnodes, NULL);
	VT.reserve(numOFnodes);
	weights_vector.resize(imgDecrSize);


	image_descriptor_temp.resize(imgDecrSize);
	nonZeroDescriptorIDs_img.reserve(NUMBER_OF_FEATURES_FROM_EACH_IMAGE + 50); // + 50 just to be safe since openCV's retainBest does not always quaranties that the size will be exactly NUMBER_OF_FEATURES_FROM_EACH_IMAGE. It might be a little bigger.
	Feature_Ptr_vector.reserve(NUMBER_OF_FEATURES_FROM_EACH_IMAGE + 50);
	colorVector.reserve(NUMBER_OF_FEATURES_FROM_EACH_IMAGE + 50);

	sequence_descriptor_temp.resize(imgDecrSize);
	sequence_descriptor_temp.setConstant(UNINITIALIZED_VWV_VALUE);
	nonZeroDescriptorIDs_seq.reserve(ESTIMATION_FOR_NUMBER_OF_FEATURES_FROM_EACH_SECUENCE);

	#ifdef RETAIN_HIGH_OCCURANCES_ONLY
		sequence_word_occurances.resize(imgDecrSize);
		sequence_word_occurances.setConstant(0);
	#endif

	// Printing Tree Properties //
	PRINTF_PLATFORM("-------------------------\n");
	PRINTF_PLATFORM("Reading tree with properties:\n  K = %d\n  L = %d\n  scoringType = %d\n  weightingType = %d\n  imgDecrSize = %d\n  numOFnodes = %d\n", K, L, scoringType, weightingType, imgDecrSize, numOFnodes);
	PRINTF_PLATFORM("-------------------------\n");
	PRINTF_PLATFORM("...\n\n\n");
	// --- //

	file.close();

	return true;
}


void Vocabulary_Tree_Engine::addNodeDependenciesBack(int parent_nodeID_in, bool is_root)
{
	NodeID_2_VTptr[VT.back().nodeID] = &(VT.back());
	if(is_root)
	{
		VT.back().parentNode = &(VT.back());  // look at himself
		VT.back().parentNodeID = 0;			// look at himself
	}
	else
	{
		VT.back().parentNode = NodeID_2_VTptr[parent_nodeID_in];
		VT.back().parentNodeID = parent_nodeID_in;
	}
}

void Vocabulary_Tree_Engine::addNodeDependenciesFront()
{
	VT_node * parent_node_ptr = VT.back().parentNode;

	parent_node_ptr->childrenNodes.push_back(&VT.back());
	parent_node_ptr->childrenNodesIDs.push_back(VT.back().nodeID);
}

void Vocabulary_Tree_Engine::add_New_VT_Node(std::string &Node_Line_1, std::string &Node_Line_2)
{
	int temp_nodeID, temp_parentId;
	float temp_weight;
	unsigned char temp_descriptor[DESCR_SIZE_BYTE];

	std::istringstream tokenizer(Node_Line_1);
	std::string token;
	std::getline(tokenizer, token, ':');
	std::getline(tokenizer, token, ',');
	temp_nodeID = atoi( token.c_str() );


	std::getline(tokenizer, token, ':');
	std::getline(tokenizer, token, ',');
	temp_parentId  = atoi( token.c_str() );

	std::getline(tokenizer, token, ':');
	std::getline(tokenizer, token, ',');
	temp_weight = atof ( token.c_str() );

	tokenizer.str(Node_Line_2);
	std::getline(tokenizer, token, '"');
	std::getline(tokenizer, token, '"');
	std::stringstream tokenizer_descr(token);

	std::vector<std::string> token_descrInv;
	for(int i = 0; i < DESCR_SIZE_BYTE; i++)
	{
		std::string token_8bit;
		tokenizer_descr >> token_8bit;
		token_descrInv.push_back(token_8bit);
	}

	for(int i = 0; i < DESCR_SIZE_BYTE; i++)
	{
		std::string token_8bit;
		token_8bit = token_descrInv[i];

		temp_descriptor[i] = (unsigned char)(atoi(token_8bit.c_str()));

	}

	VT_node temp_vt_node(temp_nodeID, temp_weight, K, temp_descriptor, &imageDescriptorIDGrouing);
	VT.push_back(temp_vt_node);
	temp_vt_node.deallocate();
	addNodeDependenciesBack(temp_parentId);
	addNodeDependenciesFront();
}

void Vocabulary_Tree_Engine::assigneWordIdToLeafNode(std::string &nodes2words_line)
{
	std::istringstream tokenizer(nodes2words_line);
	std::string token;
	std::getline(tokenizer, token, ':');
	std::getline(tokenizer, token, ',');
	int temp_wordID = atoi( token.c_str() );

	std::getline(tokenizer, token, ':');
	std::getline(tokenizer, token, ' ');
	int temp_nodeID = atoi( token.c_str() );

	if(temp_wordID == 0)
	{
		NodeID_2_VTptr[temp_nodeID]->imageDescriptorID = temp_wordID;
		weights_vector(temp_wordID) = NodeID_2_VTptr[temp_nodeID]->weight;
	}
	else
	{
		NodeID_2_VTptr[temp_nodeID]->imageDescriptorID = temp_wordID - 1;
		weights_vector(temp_wordID-1) = NodeID_2_VTptr[temp_nodeID]->weight;
	}


	NodeID_2_VTptr[temp_nodeID]->isLeafNode = true;
}

bool Vocabulary_Tree_Engine::read_VT_Nodes(const char * fileName)
{
	std::ifstream file(fileName);
	std::string temp_str, Node_Line_1, Node_Line_2;
	std::string nodes2words_line;

	PRINTF_PLATFORM("=======================================\n");
	PRINTF_PLATFORM("   Reading Vocabulary from file...\n");
	if(!file){
		PRINTF_PLATFORM("Vocabulary File \"%s\" not found.", fileName);
		return false;
	}

	std::getline(file, temp_str); //%YAML:1.0
	std::getline(file, temp_str); //vocabulary: ...
	std::getline(file, temp_str); //K: ...
	std::getline(file, temp_str); //L: ...
	std::getline(file, temp_str); //scoringType: ...
	std::getline(file, temp_str); //weightingType: ...
	std::getline(file, temp_str); //imgDecrSize: ...
	std::getline(file, temp_str); //numOFnodes: ...
	std::getline(file, temp_str); //nodes: ...

	// Add Root Node
	VT_node root_vt_node(0, 0.0, K, 0, &imageDescriptorIDGrouing, true);
	VT.push_back(root_vt_node);
	root_vt_node.deallocate();
	addNodeDependenciesBack(0, true);

	int endLoop = numOFnodes;
	int endLoop2 = imgDecrSize;

	// Add Rest of Nodes
	for(int i = 1; i < endLoop; i++)
	{
		std::getline(file, Node_Line_1);
		std::getline(file, Node_Line_2);
		add_New_VT_Node(Node_Line_1, Node_Line_2);
	}

	std::getline(file, temp_str); // words:

	for(int i = 0; i < endLoop2; i++)
	{
		std::getline(file, nodes2words_line);
		assigneWordIdToLeafNode(nodes2words_line);
	}

	PRINTF_PLATFORM("=======================================\n");
	file.close();

	return true;
}

void Vocabulary_Tree_Engine::treeTraversing(Image_Engine *image)
{
	image_descriptor_temp.setConstant(UNINITIALIZED_VWV_VALUE);

	P_TIC("TreeTraversal");
	for(int i = 0; i < image->numberOfFeatures; i++)
	{
		unsigned char *descriptorPtr = image->descriptors.row(i).data;

		VT_node *tempNode = &VT.at(0);

		while(!tempNode->isLeafNode) // while we haven't reach the tree's leaf nodes
		{
			unsigned char *tempNodeDescriptorPtr = tempNode->childrenNodes[0]->descriptor;
			unsigned int minDist = hammingDistance(descriptorPtr, tempNodeDescriptorPtr, DESCR_SIZE_BYTE);
			unsigned int minChildID = 0;

			for(unsigned int j = 1; j < tempNode->childrenNodes.size(); j++)
			{
				unsigned char *tempNodeDescriptorPtr = tempNode->childrenNodes[j]->descriptor;

				#ifdef _ARM_NEON_
					unsigned int tempDistance = hammingDistance_armNeon(descriptorPtr, tempNodeDescriptorPtr);
				#else
					unsigned int tempDistance = hammingDistance(descriptorPtr, tempNodeDescriptorPtr, DESCR_SIZE_BYTE);
				#endif

				if(tempDistance < minDist)
				{
					minDist = tempDistance;
					minChildID = j;
				}
			}
			tempNode = tempNode->childrenNodes[minChildID];
		}

		int tempImageDescriptorID = tempNode->imageDescriptorID;

		// Image Descriptor
		float temp_img = image_descriptor_temp(tempImageDescriptorID);
		if(temp_img == UNINITIALIZED_VWV_VALUE)
		{
			temp_img = 0.0f; 
			nonZeroDescriptorIDs_img.push_back(tempImageDescriptorID);
		}
		temp_img += weights_vector(tempImageDescriptorID);
		image_descriptor_temp(tempImageDescriptorID) = temp_img;

		// Sequence Descriptor
		float temp_seq = sequence_descriptor_temp(tempImageDescriptorID);
		if(temp_seq == UNINITIALIZED_VWV_VALUE) // first time seen during current sequence
		{
			temp_seq = 0.0f;
			nonZeroDescriptorIDs_seq.push_back(tempImageDescriptorID);
			#ifdef RETAIN_HIGH_OCCURANCES_ONLY
				sequence_word_occurances(tempImageDescriptorID) = 1;
			#endif
		}
		#ifdef RETAIN_HIGH_OCCURANCES_ONLY
		else
			sequence_word_occurances(tempImageDescriptorID)++;
		#endif

		#ifdef MULTISET_UNION_BASED_SVWV_CREATION
			if(temp_seq <= temp_img) // Union of Multisets
				sequence_descriptor_temp(tempImageDescriptorID) = temp_img;
		#else
			sequence_descriptor_temp(tempImageDescriptorID) = temp_seq + weights_vector(tempImageDescriptorID);
		#endif
	}
	P_TOC("TreeTraversal");

	P_TIC("FormIVWVs");
	unsigned int NumOfVisualWordsTemp = nonZeroDescriptorIDs_img.size();
	image->NumOfVisualWords = NumOfVisualWordsTemp;
	NumOfVisualWordsTempG = NumOfVisualWordsTemp;

	float norm = 0.0f;
	std::sort(nonZeroDescriptorIDs_img.begin(), nonZeroDescriptorIDs_img.end());

	for(unsigned int i = 0; i < NumOfVisualWordsTemp; i++)
	{
		int tempImageDescriptorID = nonZeroDescriptorIDs_img[i];
		float temp = image_descriptor_temp(tempImageDescriptorID);
		image->unit_image_descriptor_sp.insert(tempImageDescriptorID, 0) = temp;		
		norm += temp * temp;
	}

	norm = std::sqrt(norm);
	image->unit_image_descriptor_sp /= norm;
	P_TOC("FormIVWVs");

	nonZeroDescriptorIDs_img.resize(0); // we do not care about calling the destractor nor deallocating
}

void Vocabulary_Tree_Engine::deallocate()
{
	if(allocated)
	{
		for(unsigned int i = 0; i < VT.size(); i++)
			VT.at(i).deallocate();

		image_descriptor_temp.resize(0);
		sequence_descriptor_temp.resize(0);

		#ifdef RETAIN_HIGH_OCCURANCES_ONLY
			sequence_word_occurances.resize(0);
		#endif
				

		weights_vector.resize(0);
		NodeID_2_VTptr.clear();
		VT.clear();

		nonZeroDescriptorIDs_img.clear();
		Feature_Ptr_vector.clear();
		colorVector.clear();

		allocated = false;
	}
}
