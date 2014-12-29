#include "ImgFilterBase.h"


ImgFilterBase::ImgFilterBase()
{
	_data = NULL;
	_filter = NULL;
	_imgHeight = 0;
	_imgWidth = 0;
	_filterRank = 0;
}

void ImgFilterBase::SetData(unsigned char *data, unsigned int width, unsigned int height)
{
	_data = (Pixel*)data;
	_imgHeight = height;
	_imgWidth = width;
}

void ImgFilterBase::SetFilter(unsigned int *filter, unsigned int filterRank)
{
	if (filterRank < 3 || filterRank % 2 == 0)
		throw new std::exception("Invalid filter rank");

	_filter = new unsigned int[filterRank*filterRank];
	memcpy(_filter, filter, filterRank*filterRank*sizeof(unsigned int));
}

unsigned int ImgFilterBase::FixIndex(int index, unsigned int limit) restrict(amp, cpu)
{
	if (index < 0)
		return 0;
	if (index >= limit)
		return limit - 1;

	return index;
}



ImgFilterBase::~ImgFilterBase()
{
	delete[] _filter;
}