#include "GpuCollector.h"
#include "windows.h"
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <dxgi.h>
#include <pdh.h>

GpuCollector::GpuCollector() { }

GpuCollector::~GpuCollector() { }

void GpuCollector::Update(SystemState& state)
{
    GetGPULoadViaPDH(state);
    GetGPUInfoViaDXGI(state);
}

bool GpuCollector::GetGPUInfoViaDXGI(SystemState& state)
{
    IDXGIFactory1* factory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
    if (FAILED(hr)) {
        printf("DXGI: Failed to create factory\n");
        return false;
    }
    
    IDXGIAdapter1* adapter = nullptr;
    hr = factory->EnumAdapters1(0, &adapter);
    
    if (FAILED(hr)) {
        printf("DXGI: No adapter found\n");
        factory->Release();
        return false;
    }
    
    DXGI_ADAPTER_DESC1 desc;
    hr = adapter->GetDesc1(&desc);
    
    if (SUCCEEDED(hr)) {
        WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, 
                            state.gpuName, sizeof(state.gpuName), 
                            nullptr, nullptr);
        
        state.gpuMemoryTotalMB = desc.DedicatedVideoMemory / (1024 * 1024);
        
        printf("DXGI: GPU = %s, Memory = %llu MB\n", 
               state.gpuName, state.gpuMemoryTotalMB);
    }
    
    adapter->Release();
    factory->Release();
    
    return true;
}

bool GpuCollector::GetGPULoadViaPDH(SystemState& state) {
    return true;
}