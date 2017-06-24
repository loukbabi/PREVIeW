//============================================================================
// Name        : vocabulary_tree.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef VOCABULARY_TREE_H
#define VOCABULARY_TREE_H

class Image_Engine;
class Image_Matching_Engine;
class Feature_Ptr;

class VT_node{

public:
	int nodeID;
	bool isLeafNode;
	int directIndexing_ID;
	unsigned char descriptor[DESCR_SIZE_BYTE];
	int imageDescriptorID;
	bool allocated;
	float weight;

	std::vector<VT_node *> childrenNodes;
	std::vector<int> childrenNodesIDs;
	VT_node *parentNode;
	int parentNodeID;

	VT_node(int nodeID_in, float weight_in, int K_in, unsigned char *descriptor_in, int *imageDescriptorIDGrouing, bool is_root = false);

	void deallocate();
	~VT_node();
};

class Vocabulary_Tree_Engine{

private:

	int scoringType, weightingType;
	int numOFnodes;

	std::vector<VT_node *> NodeID_2_VTptr;
	std::vector<VT_node> VT;

	int imageDescriptorIDGrouing;
	bool allocated;


	Eigen::VectorXf image_descriptor_temp;
	std::vector<int> nonZeroDescriptorIDs_img;
	std::vector< Feature_Ptr* > Feature_Ptr_vector;

public:

	Eigen::VectorXf sequence_descriptor_temp;

	#ifdef RETAIN_HIGH_OCCURANCES_ONLY
		Eigen::VectorXi	sequence_word_occurances;
	#endif
	std::vector<int> nonZeroDescriptorIDs_seq;
	std::vector<cv::Scalar> colorVector;

	unsigned int NumOfVisualWordsTempG;
	int imgDecrSize;
	int K, L;
	Eigen::VectorXf weights_vector;

	void add_New_VT_Node(std::string &Node_Line_1, std::string &Node_Line_2);
	void assigneWordIdToLeafNode(std::string &nodes2words_line);

	void addNodeDependenciesBack(int parent_nodeID_in, bool is_root = false);

	void addNodeDependenciesFront();

	bool read_VT_file(const char * fileName);

	bool read_VT_properties(const char *fileName);

	bool read_VT_Nodes(const char * fileName);

	void treeTraversing(Image_Engine *image);

	void deallocate();
	~Vocabulary_Tree_Engine();
};

#endif //VOCABULARY_TREE_H
