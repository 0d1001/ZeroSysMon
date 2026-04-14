#pragma once

#include "SystemState.h"

class RamCollector {
public:
    RamCollector();
    ~RamCollector();
    
    void Update(SystemState& state);
};