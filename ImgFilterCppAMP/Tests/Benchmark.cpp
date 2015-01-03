#include <fstream>
#include <iostream>
#include "Benchmark.h"
#include "..\ImgFilterAMP.h"
#include "..\ImgFilterCPU.h"
#include "..\Helpers\WindowsHelpers.h"

std::vector<std::string> Benchmark::_testImages;

void Benchmark::GenerateTestValues()
{
	char fileName[128];
	for (int i = 1; i <= 16; i++) {
		sprintf(fileName, "InputBenchmark/%i.bmp", i);
		_testImages.push_back(std::string(fileName));
	}
}

void Benchmark::HardPerformanceTest() {

	ImgFilterBase *implementations[] = { new ImgFilterAMP(), new ImgFilterCPU() };
	int implementationsCount = sizeof(implementations) / sizeof(implementations[0]);

	std::vector<ProcessingUnitInfo> pusInfo;
	std::vector<std::vector<double>> pusTimings;// each PU have a vector with timings


	for (int i = 0; i < implementationsCount; i++) {
		auto implPus = implementations[i]->GetAvailableProcessingUnits();
		pusInfo.insert(pusInfo.end(), implPus.begin(), implPus.end());
	}

	//add all necessary vectors
	for (unsigned int i = 0; i < pusInfo.size(); i++)
		pusTimings.push_back(std::vector<double>());

	int filter[3][3] = {
		{ 0, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 0 }
	};

	//for each test run all PUs, this way we don't stress to much one PU at once
	for (unsigned int testIndex = 0; testIndex < _testImages.size(); testIndex++) {
		bitmap_image *image = new bitmap_image("input.bmp");
		std::cout << "# " << image->size() / 1024 / 1024.0 << "MB\n";

		for (unsigned int puIndex = 0; puIndex < pusInfo.size(); puIndex++) {
			LARGE_INTEGER tStart, tEnd;			
			
			pusInfo[puIndex].implementation->SetFilter((int*)filter, 3);
			pusInfo[puIndex].implementation->SetImage((unsigned char*)image->data(), image->width(), image->height());
			std::cout << pusInfo[puIndex].name << "\n";

			//TODO: Create Windows Helpers method using below method of getting the current 'time'. We should make this thing portable 
			QueryPerformanceCounter(&tStart);
			pusInfo[puIndex].implementation->Filter(pusInfo[puIndex].processingUnitId);
			QueryPerformanceCounter(&tEnd);

			pusTimings[puIndex].push_back(WindowsHelper::ElapsedTime(tStart.QuadPart, tEnd.QuadPart));			
		}

		delete image;
	}

	for (int i = 0; i < implementationsCount; i++)
		delete implementations[i];

	ExportHardBenchmarkData(pusInfo, pusTimings);
	std::cout << "HardPerformanceTest finished.\r\n";
}

void Benchmark::ExportHardBenchmarkData(std::vector<ProcessingUnitInfo> pusInfo, std::vector<std::vector<double>> timings) {
	std::ofstream fout;
	time_t rawtime;
	tm timeinfo;
	char filename[100];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(filename, 100, "BenchmarkResults-%Y-%m-%d_%H-%M-%S.csv", &timeinfo);//2001-08-29_23-50-15

	fout.open(filename);

	//the first line will contain the memory values used for tests
	fout << ",,,";//this is an 'empty PU description'
	for (auto memValue : _testImages) {
		if (memValue < 2 << 19)//if memValue unde 1MB then write KB, else MB
			fout << memValue / 1024 << "KB, ";
		else
			fout << memValue / 1024 / 1024 << "MB, ";
	}
	fout << "\n";

	for (unsigned int puIndex = 0; puIndex < pusInfo.size(); puIndex++) {
		auto pu = pusInfo[puIndex];

		fout << pu.name << "," << pu.isGPU << "," << pu.availableMemory / 1024 << ",";//display availabelMemory in MB
		for (auto time : timings[puIndex])
			fout << time / 1000 << ",";//display elapsed time in seconds
		fout << "\n";

		if (pu.isGPU) {
			fout << "Kernel Execution Time,,,";
			for (auto time : ((ImgFilterAMP*)pu.implementation)->EncryptionKernelTimings[pu.processingUnitId])
				fout << time / 1000 << ",";//display elapsed time in seconds
			fout << "\n";
		}
	}

	fout.close();
}
