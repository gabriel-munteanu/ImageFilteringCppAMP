#pragma once
#include <vector>
#include "..\ImgFilterBase.h"
#include "../Utils/bitmap_image.hpp"

class Benchmark {

private:
	static std::vector<std::string> _testImages;
	static void ExportHardBenchmarkData(std::vector<ProcessingUnitInfo>, std::vector<std::vector<double>>);
	static void GenerateTestValues();

public:
	//Test all Processing Units against all memory test values
	static void HardPerformanceTest();	
};

