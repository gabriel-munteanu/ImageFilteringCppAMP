#include <iostream>

#include "Tests.h"


void TestImgFilterCPU()
{
	std::cout << "Testing Image Filtering CPU implementation\r\n";

	ImgFilterCPU imgFilterCpu;
	bitmap_image image("input.bmp");

	if (!image)
	{
		std::cerr << "TestImgFilterCPU - Could not open input file input.bmp";
		return;
	}

	int filter[3][3] = {
		{ 0, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 0 }
	};

	imgFilterCpu.SetFilter((int*)filter, 3);
	//a little hacking so we don't need to create a new image, just overwrite this image data and save it with another name
	imgFilterCpu.SetImage((unsigned char*)image.data(), image.width(), image.height());

	imgFilterCpu.Filter(0);//sequential
	image.save_image("TestOutput/1.TestImgFilterCPU-Sequential.bmp");

	//filter the filtered image but with this filter we should not affect the image
	imgFilterCpu.Filter(1);//parallel
	image.save_image("TestOutput/1.TestImgFilterCPU-Parallel.bmp");

}

void TestImgFilterAMP()
{
	std::cout << "Testing Image Filtering AMP implementation\r\n";

	ImgFilterCPU imgFilterAMP;
	bitmap_image image("input.bmp");

	if (!image)
	{
		std::cerr << "TestImgFilterAMP - Could not open input file input.bmp";
		return;
	}

	int filter[3][3] = {
		{ 0, 0, 0 },
		{ 0, 1, 3 },
		{ -7, 0, 0 }
	};

	imgFilterAMP.SetFilter((int*)filter, 3);
	//a little hacking so we don't need to create a new image, just overwrite this image data and save it with another name
	imgFilterAMP.SetImage((unsigned char*)image.data(), image.width(), image.height());


	imgFilterAMP.Filter(1);
	image.save_image("TestOutput/1.TestImgFilterAMP.bmp");
}