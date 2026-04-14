#include <cstdio>
#include <chrono>
#include <thread>

#include "core/SystemState.h"
#include "core/CpuCollector.h"
#include "core/RamCollector.h"
#include "ui/Window.h"

int main() {
    Window window;
    if (!window.Initialize("ZeroSysMon", 1000, 600)) {
        return -1;
    }

    SystemState state = {};
    CpuCollector cpuCollector;
    RamCollector ramCollector;
    cpuCollector.Initialize(state);
    
    auto lastUpdate = std::chrono::steady_clock::now();

    while (!window.ShouldClose()) {
        window.ProcessEvents();
        
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() >= 1) {
            cpuCollector.Update(state);
            ramCollector.Update(state);
            lastUpdate = now;
        }
        
        window.Render(state);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}