#pragma once

#include <vector>
#include <string>

#include "Helpers\WindowsHelpers.h"

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
	unsigned int *_filter, _filterRank;

	ImgFilterBase();
	~ImgFilterBase();
	//Giving an index value and it's limit, this function will make sure the index is inside the matrix
	static unsigned int FixIndex(int index, unsigned int limit) restrict(amp, cpu);

public:
	virtual unsigned int ImplementationId() = 0;
	virtual std::vector<ProcessingUnitInfo> GetAvailableProcessingUnits() = 0;

	//The data will be overwrite by filter process
	void SetData(unsigned char *data, unsigned int width, unsigned int height);

	//Set the filter that will be applied to the image. 
	//The rank of the matrix must be an odd value and greater or equal to 3
	void SetFilter(unsigned int *filter, unsigned int filterRank);

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

