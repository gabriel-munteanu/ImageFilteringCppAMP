#pragma once
#include "ImgFilterBase.h"

class ImgFilterCPU : public ImgFilterBase
{

private:
	Pixel *_resultData;
	//Apply the filter to the pixel specified by the coordinates
	void FilterPixel(unsigned int line, unsigned int column);
	void FilterPixelComponent(unsigned int line, unsigned int column, unsigned int offset);

	//This function is static so we can create new threads with it. It will work with the instance variables
	static void FilterLines(ImgFilterCPU *instance, unsigned long startLine, unsigned long concurrentThreads);

	void PrepareForFilter();
	void EndFilter();

	void SequentialFilter();
	void ParallelFilter();
public:
	unsigned int ImplementationId();
	std::vector<ProcessingUnitInfo> GetAvailableProcessingUnits();

	//Apply filter using the specified Processing Unit
	//If the index is not valid it will throw an exception
	void Filter(unsigned int puIndex, bool normalize = true);
};

