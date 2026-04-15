#pragma once

#include "SystemState.h"

class GpuCollector {
public:
    GpuCollector();
    ~GpuCollector();
    
    void Update(SystemState& state);
    
private:
    void* m_pdhQuery;
    void* m_pdhCounter;
    
    bool GetGPUInfoViaDXGI(SystemState& state);
    bool GetGPULoadViaPDH(SystemState& state);
};