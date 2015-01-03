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
	if (filterRank < 3 || filterRank % 2 == 0 || filterRank > 9)
		throw new std::exception("Invalid filter rank");

	_filterRank = filterRank;
	_filter = new int[filterRank*filterRank];
	memcpy(_filter, filter, filterRank*filterRank*sizeof(unsigned int));
	
	_filterNorm = 0;
	for (unsigned int i = 0; i < filterRank*filterRank; i++)
		_filterNorm += _filter[i];
}

ImgFilterBase::~ImgFilterBase()
{
	delete[] _filter;
}