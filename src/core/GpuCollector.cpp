#include "GpuCollector.h"
#include <cstdio>
#include <string>
#include <algorithm>

GpuCollector::GpuCollector() : m_adapter(nullptr) {}

GpuCollector::~GpuCollector() {
    if (m_adapter) m_adapter->Release();
}

bool GpuCollector::Initialize(SystemState& state)
{
    IDXGIFactory1* factory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
    if (FAILED(hr)) {
        printf("DXGI: Failed to create factory\n");
        return false;
    }
    
    IDXGIAdapter1* adapter1 = nullptr;
    hr = factory->EnumAdapters1(0, &adapter1);
    if (FAILED(hr)) {
        printf("DXGI: No adapter found\n");
        factory->Release();
        return false;
    }
    
    hr = adapter1->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&m_adapter);
    if (FAILED(hr)) {
        printf("DXGI: IDXGIAdapter3 not supported (old Windows?)\n");
        m_adapter = nullptr;
    }
    
    DXGI_ADAPTER_DESC1 desc;
    hr = adapter1->GetDesc1(&desc);
    
    if (SUCCEEDED(hr)) {
        WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, 
                            state.gpuName, sizeof(state.gpuName), 
                            nullptr, nullptr);
        
        state.gpuMemoryTotalMB = desc.DedicatedVideoMemory / (1024 * 1024);
        
        printf("DXGI: GPU = %s, Memory = %llu MB\n", 
               state.gpuName, state.gpuMemoryTotalMB);
    }
    
    m_gpuType = GetGPUType(state.gpuName, state);
    
    adapter1->Release();
    factory->Release();
    
    return true;
}

void GpuCollector::Update(SystemState& state)
{
    float percent = GetGPULoad(state);
    
    for (int i = 0; i < 31; i++) {
        state.gpuPercent[i] = state.gpuPercent[i + 1];
    }
    state.gpuPercent[31] = percent;
}

float GpuCollector::GetGPULoad(SystemState& state)
{
    if (!m_adapter) return 0.0f;
    
    DXGI_QUERY_VIDEO_MEMORY_INFO info;
    HRESULT hr = m_adapter->QueryVideoMemoryInfo(
        0,
        DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
        &info
    );
    
    if (SUCCEEDED(hr)) {
        state.gpuMemoryUsedMB = info.CurrentUsage / (1024 * 1024);
        
        if (state.gpuMemoryTotalMB > 0) {
            return (float)state.gpuMemoryUsedMB / state.gpuMemoryTotalMB * 100.0f;
        }
    }
    return 0.0f;
}

GPUType GpuCollector::GetGPUType(const char* gpuName, SystemState& state) {
    std::string name(gpuName);
    std::string lower = name;
    for (char& c : lower) c = tolower(c);
    
    GPUType gpuType = GPU_UNKNOWN;
    
    if (lower.find("nvidia") != std::string::npos) {
        gpuType = GPU_NVIDIA;
    }
    else if (lower.find("amd") != std::string::npos || 
             lower.find("radeon") != std::string::npos) {
        gpuType = GPU_AMD;
    }
    else if (lower.find("intel") != std::string::npos) {
        if (lower.find("uhd") != std::string::npos || 
            lower.find("hd graphics") != std::string::npos ||
            lower.find("iris") != std::string::npos) {
            gpuType = GPU_INTEGRATED;
        } else {
            gpuType = GPU_INTEL;
        }
    }
    
    if (state.gpuMemoryTotalMB < 128) {
        gpuType = GPU_INTEGRATED;
    }
    
    return gpuType;
}