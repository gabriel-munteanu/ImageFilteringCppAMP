#pragma once

#include <vector>
#include <string>
#include <amp.h>

#include "Helpers\WindowsHelpers.h"

using namespace concurrency;

#define PIXEL_SIZE 3 //bytes

#define FltrBckOffset(index, fltrRank) (index - (fltrRank/2))

struct ProcessingUnitInfo;
struct Pixel
{
	unsigned char R, G, B;
};

class ImgFilterBase
{
private:

protected:
	Pixel *_data;
	unsigned int _imgWidth, _imgHeight;
	int *_filter, _filterNorm;
	unsigned int _filterRank;
	bool _normalize;

	ImgFilterBase();
	//Giving an index value and it's limit, this function will make sure the index is inside the matrix
	static unsigned int FixIndex(int index, unsigned int limit) restrict(amp, cpu)
	{
		if (index < 0)
			return 0;
		if (index >= limit)
			return limit - 1;

		return index;
	}
	//After we compute the color component value we need to make sure that it's value is between 0 and 255
	static unsigned int FixColorComponentValue(int value) restrict(amp, cpu)
	{
		if (value < 0)
			return 0;

		if (value > 255)
			return 255;

		return value;
	}

public:
	~ImgFilterBase();
	virtual unsigned int ImplementationId() = 0;
	virtual std::vector<ProcessingUnitInfo> GetAvailableProcessingUnits() = 0;

	//The data will be overwrite by filter process
	void SetImage(unsigned char *data, unsigned int width, unsigned int height);

	//Set the filter that will be applied to the image. 
	//The rank of the matrix must be an odd value and greater or equal to 3
	void SetFilter(int *filter, unsigned int filterRank);

	//Apply filter using the specified Processing Unit
	//If the index is not valid it will throw an exception
	virtual void Filter(unsigned int puIndex, bool normalize = true) = 0;
};

struct ProcessingUnitInfo {
	std::string name;
	//if is set to 0 then we are working with the RAM memory and the CPU as PU
	// if non 0 the values is in KB
	unsigned long availableMemory;
	bool isGPU;
	ImgFilterBase *implementation;
	unsigned int processingUnitId;

	ProcessingUnitInfo(std::string pName, ImgFilterBase* pImplementation, unsigned int pProcessingUnitId, unsigned long pAvailalbeMemory, bool pIsGPU) {
		name = pName;
		implementation = pImplementation;
		processingUnitId = pProcessingUnitId;
		availableMemory = pAvailalbeMemory;
		isGPU = pIsGPU;
	}
};

