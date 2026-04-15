#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include "../core/SystemState.h"

class Window {
public:
    Window();
    ~Window();
    
    bool Initialize(const char* title, int width, int height);
    
    void Shutdown();
    
    void ProcessEvents();
    
    void Render(const SystemState& state);
    
    bool ShouldClose() const;
    
private:
    GLFWwindow* m_window;

    ImVec4 m_clearColor;

    ImFont* m_fontDefault;
    ImFont* m_fontLarge;
    
    bool m_shouldClose;
    
    int m_width;
    int m_height;

    void RenderCPU(const SystemState& state, const ImVec2& blockSize, const ImVec2& position);
    void RenderCPUGraph(const SystemState& state, const ImVec2& blockSize);
    void RenderCPUPercent(const SystemState& state, const ImVec2& blockSize);

    void RenderGPU(const SystemState& state, const ImVec2& blockSize, const ImVec2& position);
    void RenderGPUGraph(const SystemState& state, const ImVec2& blockSize);
    void RenderGPUPercent(const SystemState& state, const ImVec2& blockSize);

    void RenderRAM(const SystemState& state, const ImVec2& blockSize, const ImVec2& position);
    void RenderRAMGraph(const SystemState& state, const ImVec2& blockSize);
    void RenderRAMPercent(const SystemState& state, const ImVec2& blockSize);
};