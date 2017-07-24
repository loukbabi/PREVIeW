//============================================================================
// Name        : defines.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifndef DEFINES_H
#define DEFINES_H


// =============== Input Parameters =============== //
#ifdef __DESKTOP__
	#define INPUT_METADATA_PATH "Input/Metadata"
	#define INPUT_IMAGE_PATH "Input/Image_test"
	#define OUTPUT_METADATA_PATH "Output/Metadata"
#elif __ANDROID__
	#define INPUT_METADATA_PATH "//data/data/LCD/Input/Metadata"
	#define INPUT_IMAGE_PATH "//data/data/LCD/Input/Image_test"
	#define OUTPUT_METADATA_PATH "//data/data/LCD/Output/Metadata"
#endif

#define VOCABULARY_TREE_FILE_NAME "ORB_K10L6"
#define VOCABULARY_TREE_FILE_FORMAT "voc"

#define INPUT_IMAGE_FORMAT  "png"
#define INPUT_IMAGE_PREFIX "SVS_T_"
//  #define INPUT_IMAGE_ENDFIX ""


#define MAX_VW_SEQUENCE_SIZE 10000
#define SEQ_SEP_TIME_WINDOW 5
#define SEQ_SEP_MIN_SEQUENCE_SIZE 10
#define NEW_OLD_RATIO 0.75
#define MIN_SEQUENCE_SIZE std::max(SEQ_SEP_TIME_WINDOW, SEQ_SEP_MIN_SEQUENCE_SIZE)
#define MAX_NUMBER_OF_IMAGES 100000
#define MAX_NUMBER_OF_SEQUENCES (MAX_NUMBER_OF_IMAGES / 10.0f)
// ================================================ //

// =============== Output Parameters =============== //
#define OUTPUT_METADATA_FORMAT "txt"
#define PRINTING_PRECISION 5
// ================================================= //

// =============== Execution Properties =============== //
#define DETECTOR_TYPE "ORB"
#define DESCRIPTOR_TYPE "ORB"
#define DESCR_SIZE 256
#define DESCR_SIZE_BYTE (DESCR_SIZE >> 3)
#define NUMBER_OF_FEATURES_FROM_EACH_IMAGE 300
#define MIN_FEATURES_PER_IMAGE_THRESHOLD 150
#define TEMPORAL_CONSISTENCY_TOLERANCE 0.7
#define ESTIMATION_FOR_NUMBER_OF_FEATURES_FROM_EACH_SECUENCE (NUMBER_OF_FEATURES_FROM_EACH_IMAGE * 10)

#define INVERSE_INDEXING_IMAGES_PER_WORD_ESTIMATION 50
#define INVERSE_INDEXING_SEQUENCES_PER_WORD_ESTIMATION (INVERSE_INDEXING_IMAGES_PER_WORD_ESTIMATION * 2)

// #define PROFILER							// Enable Profiler
#define WITH_TBB_FOR_PIPELINING   	

#define USE_FILETERING
#define USE_NORMALIZATION

#ifdef USE_FILETERING
	#define KERNEL_SIZE 3
	#define FILTER_KERNEL   	 2.3088f, -0.5663f, -1.8762f,\
   								-0.4084f,  2.1938f, -0.7538f,\
   								-1.8333f, -0.3420f,  2.1512f
#else
	#define KERNEL_SIZE 1
	#define FILTER_KERNEL 1
#endif
#define SCORE_THRESHOLD_SEQ 3.5f

#define MULTIPLE_SEQUENCE_MATCHING
#define MULTISET_UNION_BASED_SVWV_CREATION

#define RETAIN_HIGH_OCCURANCES_ONLY // Keep only visual words in the sequence descriptors with more than OCCURANCES_THRESHOLD occurances
#ifdef RETAIN_HIGH_OCCURANCES_ONLY
	#define OCCURANCES_THRESHOLD 1	   	   	   	   	   	   
#endif

#define SCORE_THRESHOLD_IMG 0.25f

#define PREVENT_LOOP_DETECTION_TIME_LAG_IMGS 30
#define PREVENT_LOOP_FOR_CLOSE_IN_TIME_SEQS 3
#define PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL (int(floor(KERNEL_SIZE / 2.0f)))
#define PREVENT_LOOP_DETECTION_TIME_LAG_SEQS std::max(PREVENT_LOOP_FOR_CLOSE_IN_TIME_SEQS, PREVENT_LOOP_FIRST_N_CHECKS_FOR_SEQS_DUE_TO_KERNEL)


#define IMAGE_VISUALIZATION_DELAY 10
#define NOT_LEAF_NODE_VALUE -10
#define UNINITIALIZED_VWV_VALUE -1.0f
#define ESTIMATED_IMAGE_MEMBERS 300
// ==================================================== //




#include "defines_helper.h"


#endif // DEFINES_H
