#include <thread>

#include "ImgFilterCPU.h"

unsigned int ImgFilterCPU::ImplementationId() { return 1; }

std::vector<ProcessingUnitInfo> ImgFilterCPU::GetAvailableProcessingUnits() {
	std::vector<ProcessingUnitInfo> pusInfo;
	pusInfo.push_back(ProcessingUnitInfo(std::string("CPU Sequential implementation"), this, 0, 0, false));
	pusInfo.push_back(ProcessingUnitInfo(std::string("CPU Parallel implementation"), this, 1, 0, false));

	return pusInfo;
}

void ImgFilterCPU::Filter(unsigned int puIndex, bool normalize)
{
	if (puIndex != 0 && puIndex != 1)
		throw std::exception("Invalid Processing Unit index");
	if (_data == NULL)
		throw std::exception("No image to filter");

	_normalize = normalize;
	PrepareForFilter();

	if (puIndex == 0)
		SequentialFilter();

	if (puIndex == 1)
		ParallelFilter();

	EndFilter();
}

void ImgFilterCPU::PrepareForFilter()
{
	_resultData = new Pixel[_imgWidth * _imgHeight];
}

void ImgFilterCPU::EndFilter()
{
	memcpy(_data, _resultData, _imgHeight*_imgWidth*sizeof(Pixel));
	delete[] _resultData;
}

void ImgFilterCPU::SequentialFilter()
{
	FilterLines(this, 0, 1); //current thread must filter all lines starting with 0
}

void ImgFilterCPU::ParallelFilter()
{
	std::vector<std::thread> threads;
	unsigned int procCount = WindowsHelper::GetAvailableLogicalProcessors();

	for (unsigned int i = 0; i < procCount; i++)
		threads.push_back(std::thread(FilterLines, this, i, procCount));

	for (auto &th : threads) {
		th.join();
	}
}

void ImgFilterCPU::FilterLines(ImgFilterCPU *instance, unsigned long startLine, unsigned long concurrentThreads)
{
	unsigned int height = instance->_imgHeight,
		width = instance->_imgWidth;
	unsigned int line = startLine;

	while (line < height)
	{
		for (int column = 0; column < width; column++)
			instance->FilterPixel(line, column);

		line += concurrentThreads;
	}
}

void ImgFilterCPU::FilterPixel(unsigned int line, unsigned int column)
{
	FilterPixelComponent(line, column, 0);//R
	FilterPixelComponent(line, column, 1);//G
	FilterPixelComponent(line, column, 2);//B
}

void ImgFilterCPU::FilterPixelComponent(unsigned int line, unsigned int column, unsigned int offset)
{
	int accumulator = 0;
	for (int lineImg = FltrBckOffset(line), lineFltr = 0; lineFltr < _filterRank; lineImg++, lineFltr++)
		for (int colImg = FltrBckOffset(column), colFltr = 0; colFltr < _filterRank; colImg++, colFltr++)
		{
			Pixel *pixAddr = _data + (FixIndex(lineImg, _imgHeight) * _imgWidth + FixIndex(colImg, _imgWidth));
			int filterValue = _filter[lineFltr*_filterRank + colFltr];

			accumulator += filterValue * *((char*)pixAddr + offset);
		}
	if (_normalize && _filterNorm != 0)
		accumulator /= _filterNorm;

	Pixel *outPixAddr = _resultData + (line *_imgWidth + column);
	*((char*)outPixAddr + offset) = (char)FixColorComponentValue(accumulator);
}