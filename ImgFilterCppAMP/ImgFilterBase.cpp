#include "ImgFilterBase.h"


ImgFilterBase::ImgFilterBase()
{
	_data = NULL;
	_filter = NULL;
	_imgHeight = 0;
	_imgWidth = 0;
	_filterRank = 0;
}

void ImgFilterBase::SetImage(unsigned char *data, unsigned int width, unsigned int height)
{
	_data = (Pixel*)data;
	_imgHeight = height;
	_imgWidth = width;
}

void ImgFilterBase::SetFilter(int *filter, unsigned int filterRank)
{
	if (filterRank < 3 || filterRank % 2 == 0)
		throw new std::exception("Invalid filter rank");

	_filterRank = filterRank;
	_filter = new int[filterRank*filterRank];
	memcpy(_filter, filter, filterRank*filterRank*sizeof(unsigned int));
	
	_filterNorm = 0;
	for (unsigned int i = 0; i < filterRank*filterRank; i++)
		_filterNorm += _filter[i];
}

unsigned int ImgFilterBase::FixIndex(int index, unsigned int limit) restrict(amp, cpu)
{
	if (index < 0)
		return 0;
	if (index >= limit)
		return limit - 1;

	return index;
}

unsigned int ImgFilterBase::FixColorComponentValue(int value) restrict(amp, cpu)
{
	if (value < 0)
		return 0;

	if (value > 255)
		return 255;

	return value;
}

ImgFilterBase::~ImgFilterBase()
{
	delete[] _filter;
}