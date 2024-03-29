#pragma once
#include <windows.h>
//Example from http://msdn.microsoft.com/en-us/library/windows/desktop/ms683194(v=vs.85).aspx


class WindowsHelper {
private:

typedef BOOL(WINAPI *LPFN_GLPI)(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	PDWORD);

// Helper function to count set bits in the processor mask.
static DWORD CountSetBits(ULONG_PTR bitMask) {
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
	DWORD i;

	for (i = 0; i <= LSHIFT; i++) {
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}

public:

static unsigned int GetAvailableLogicalProcessors() {
	LPFN_GLPI glpi;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	DWORD returnLength = 0;
	unsigned int logicalProcessorCount = 0;

	glpi = (LPFN_GLPI)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation");

	DWORD rc = glpi(buffer, &returnLength);// after the first call we know how much space we need
	buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)new char[returnLength];
	rc = glpi(buffer, &returnLength);

	for (unsigned int i = 0; i < returnLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)
		if (buffer[i].Relationship == RelationProcessorCore)
			// A hyper threaded core supplies more than one logical processor.
			logicalProcessorCount += CountSetBits(buffer[i].ProcessorMask);

	delete[] buffer;
	return logicalProcessorCount;
}

//this will return time difference in milliseconds
static double ElapsedTime(const __int64 &start, const __int64 &end) {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return (double(end) - double(start)) * 1000.0 / double(freq.QuadPart);
}

};