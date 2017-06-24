//============================================================================
// Name        : utils_armNeon.h
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#ifdef _ARM_NEON_

#ifndef UTILS_ARM_NEON_H
#define UTILS_ARM_NEON_H 

unsigned int hammingDistance_armNeon(unsigned char *descriptorPtr, unsigned char *tempNodeDescriptorPtr)
{
	unsigned int distance;

	asm volatile(
		"vld1.8 {q0}, [%[descriptorPtr]]!;"
		"vld1.8 {q1}, [%[descriptorPtr]];"
		"vld1.8 {q2}, [%[tempNodeDescriptorPtr]]!;"
		"vld1.8 {q3}, [%[tempNodeDescriptorPtr]];"

		"veor.i8 q4, q0, q2;"
		"veor.i8 q5, q1, q3;"

		"vcnt.8 q4, q4;"
		"vcnt.8 q5, q5;"

		"vadd.u8 q4, q4, q5;"

		"vadd.u8 d6, d8, d9;"
		"vpadd.u8 d10, d6, d3;"
		"vpaddl.u8 d20, d10;"
		"vpaddl.u16 d0, d20;"
		"vmov.32 %[distance], d0[0];"

		: [distance] "=r" (distance)
		: [descriptorPtr] "r" (descriptorPtr), [tempNodeDescriptorPtr] "r" (tempNodeDescriptorPtr)
		: "q0", "q1", "q2", "q3", "q4", "q5", "q10"
	);

	return distance;
}

#endif // UTILS_ARM_NEON_H

#endif // _ARM_NEON_