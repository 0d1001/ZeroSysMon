#include "RamCollector.h"
#include <windows.h>

RamCollector::RamCollector() {}
RamCollector::~RamCollector() {}

void RamCollector::Update(SystemState& state) {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    if (GlobalMemoryStatusEx(&memInfo)) {
        // Bytes
        uint64_t totalBytes = memInfo.ullTotalPhys;
        uint64_t usedBytes = totalBytes - memInfo.ullAvailPhys;
        
        // To GB (1 GB = 1024^3), 
        state.ramTotalGB = totalBytes / (1024ULL * 1024 * 1024);
        state.ramUsedGB = usedBytes / (1024ULL * 1024 * 1024);

        for (int i = 0; i < 31; i++)
        {
            state.ramPercent[i] = state.ramPercent[i + 1];
        }
        state.ramPercent[31] = (float)usedBytes / totalBytes * 100.0;
    }
}