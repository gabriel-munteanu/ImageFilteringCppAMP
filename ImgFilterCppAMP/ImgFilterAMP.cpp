#include "ImgFilterAMP.h"

#include <atlconv.h>
#include <atlbase.h>

#include "Helpers\AMPCharHelpers.h"

unsigned int ImgFilterAMP::ImplementationId() { return 0; }

std::vector<ProcessingUnitInfo> ImgFilterAMP::GetAvailableProcessingUnits() {
	std::vector<ProcessingUnitInfo> pusInfo;

	auto accs = accelerator::get_all();
	for (auto &acc : accs)
		//exclude CPU accelerator because it cannot be used for computation
		//exclude Software Emulated Accelerator because it is very very slow and it's purpose is for debugging only
		if (acc.device_path != accelerator::cpu_accelerator && acc.device_path != accelerator::direct3d_ref)
			_availableAccelerator.push_back(acc);

	//remove WARP because is causing problems in HardBenchmarkTests
	_availableAccelerator.pop_back();

	unsigned int i = 0;
	for (auto &a : _availableAccelerator) {
		pusInfo.push_back(ProcessingUnitInfo(std::string(CW2A(a.description.c_str())), this, i++, a.dedicated_memory, !a.is_emulated));
		EncryptionKernelTimings.push_back(std::vector<double>());
	}

	return pusInfo;
}

struct ConstFilterData
{
	unsigned int imgWidth, imgHeight;
	unsigned int filterRank, filterNorm, normalize;
	int filter[9][9];
};

void ImgFilterAMP::AMPFilter(unsigned int puIndex)
{
	unsigned long dataLength = _imgWidth * _imgHeight * PIXEL_SIZE;
	accelerator_view accView = _availableAccelerator[puIndex].create_view(queuing_mode::queuing_mode_immediate);
	extent<2> acc_ext(_imgHeight, _imgWidth);

	ConstFilterData constData = { _imgWidth, _imgHeight, _filterRank, _filterNorm, _normalize ? 1 : 0 };
	for (int i = 0; i < _filterRank; i++)
		for (int j = 0; j < _filterRank; j++)
			constData.filter[i][j] = _filter[i * _filterRank + j];

	//allocate memory on GPU and copy data to it
	array<unsigned int> acc_data((dataLength / 4), accView);
	array<unsigned int> acc_res_data((dataLength / 4), accView);//result data
	copy((unsigned int*)_data, acc_data);
	accView.wait();

	LARGE_INTEGER tStart, tEnd;
	QueryPerformanceCounter(&tStart);

#define CharLinearIndex(x,y,offset) ((x * constData.imgWidth + y) * PIXEL_SIZE + offset)
	//compute
	parallel_for_each(accView, acc_ext, [constData, &acc_data, &acc_res_data](index<2> idx) restrict(amp) {
		int line = idx[0], column = idx[1];

		for (int offset = 0; offset <= 2; offset++)//0=R, 1=G, 2=B
		{
			int accumulator = 0;
			for (int lineImg = FltrBckOffset(line, constData.filterRank), lineFltr = 0; lineFltr < constData.filterRank; lineImg++, lineFltr++)
				for (int colImg = FltrBckOffset(column, constData.filterRank), colFltr = 0; colFltr < constData.filterRank; colImg++, colFltr++)
				{
					int filterValue = constData.filter[lineFltr][colFltr];

					accumulator += filterValue * read_uchar(acc_data,
						CharLinearIndex(FixIndex(lineImg, constData.imgHeight), FixIndex(colImg, constData.imgWidth), offset));
				}
			if (constData.normalize && constData.filterNorm != 0)
				accumulator /= constData.filterNorm;

			write_uchar(acc_res_data,
				CharLinearIndex(FixIndex(line, constData.imgHeight), FixIndex(column, constData.imgWidth), offset), FixColorComponentValue(accumulator));
		}
	});
	accView.wait();

	QueryPerformanceCounter(&tEnd);
	EncryptionKernelTimings[puIndex].push_back(WindowsHelper::ElapsedTime(tStart.QuadPart, tEnd.QuadPart));
	copy(acc_res_data, (unsigned int*)_data);
}

void ImgFilterAMP::Filter(unsigned int puIndex, bool normalize)
{
	if (puIndex < 0 || puIndex >= _availableAccelerator.size())
		throw std::exception("Invalid Processing Unit index");
	if (_data == NULL)
		throw std::exception("No image to filter");

	_normalize = normalize;

	AMPFilter(puIndex);
}

