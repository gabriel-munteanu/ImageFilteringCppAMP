#include <iostream>

#include "ImgFilterCPU.h"
#include "Tests\Tests.h"
#include "Tests\Benchmark.h"


void list_all_accelerators()
{
	std::vector<accelerator> accs = accelerator::get_all();
	for (int i = 0; i < accs.size(); i++) {
		std::wcout << accs[i].description << "\n";
		std::wcout << accs[i].device_path << "\n";
		std::wcout <<"mem: "<< accs[i].dedicated_memory / 1024 << "MB\n";
		std::wcout << (accs[i].supports_cpu_shared_memory ?
			"CPU shared memory: true" : "CPU shared memory: false") << "\n";
		std::wcout << (accs[i].supports_double_precision ?
			"double precision: true" : "double precision: false") << "\n";
		std::wcout << (accs[i].supports_limited_double_precision ?
			"limited double precision: true" : "limited double precision: false") << "\n\n";
	}
}

int main() {
	try{
		Benchmark::HardPerformanceTest();
	}
	catch (std::exception &ex) {
		std::cout << ex.what() << "\n";
	}
	
	std::cout << "\r\nOK";
	getchar();
	return 0;
}