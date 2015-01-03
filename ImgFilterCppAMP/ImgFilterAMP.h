#pragma once
#include "ImgFilterBase.h"

class ImgFilterAMP : public ImgFilterBase
{
private:
	std::vector<accelerator> _availableAccelerator;

	void AMPFilter(unsigned int puIndex);
public:
	unsigned int ImplementationId();
	std::vector<ProcessingUnitInfo> GetAvailableProcessingUnits();
	std::vector<std::vector<double>> EncryptionKernelTimings;

	//Apply filter using the specified Processing Unit
	//If the index is not valid it will throw an exception
	void Filter(unsigned int puIndex, bool normalize = true);
};

