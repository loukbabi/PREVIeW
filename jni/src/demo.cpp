//============================================================================
// Name        : demo.cpp
// Author      : Loukas Bampis
// Version     : 1.0
// Copyright   : See Licence.txt
// Project     : PREVIeW - Place Recognition with unifiEd sequence VIsual Words.
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stack>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "structures/PREVIeW.h"

int main()
{
	PREVIeW PREVIeW_engine;
	
	if(!PREVIeW_engine.initialize()) return -1;

	PREVIeW_engine.deploy_fromMemory();
	PREVIeW_engine.finalize();

	if(!PREVIeW_engine.storeOutputMetadata()) return -1;

	PREVIeW_engine.deallocate();

	return 0;
}