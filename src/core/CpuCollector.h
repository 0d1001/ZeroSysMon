#pragma once

#include "SystemState.h"
#include <string>

class CpuCollector
{
    public:
        CpuCollector();
        ~CpuCollector();

        bool Initialize(SystemState& state);
        void Update(SystemState& state);
        std::string GetProcessorName();
        int GetCoreCount();
        int GetThreadCount();
        float GetFrequency(); //GHz
        float GetCurrentUsage();
};