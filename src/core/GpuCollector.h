#pragma once

#include "SystemState.h"
#include <windows.h>
#include <dxgi.h>
#include <dxgi1_4.h>

enum GPUType {
    GPU_UNKNOWN,
    GPU_NVIDIA,
    GPU_AMD,
    GPU_INTEL,
    GPU_INTEGRATED
};

class GpuCollector {
public:
    GpuCollector();
    ~GpuCollector();
    
    bool Initialize(SystemState& state);
    void Update(SystemState& state);
    
private:
    float GetGPULoad(SystemState& state);
    GPUType GetGPUType(const char* gpuName, SystemState& state);
    
    IDXGIAdapter3* m_adapter;
    GPUType m_gpuType;
};