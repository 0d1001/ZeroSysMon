#pragma once

#include <cstdint>
#include <string>

struct SystemState
{
    // CPU
    std::string cpuName;
    float cpuPercent[32] = {0};
    float cpuFrequencyGHz;
    int cpuCores;
    int cpuThreads;
    
    // RAM
    uint64_t ramTotalGB;
    uint64_t ramUsedGB;
    float ramPercent[32] = {0};
    // GPU
    char gpuName[128];
    float gpuPercent;
    uint64_t gpuMemoryTotalMB;
    uint64_t gpuMemoryUsedMB;
};