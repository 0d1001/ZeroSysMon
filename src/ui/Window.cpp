#include "Window.h"
#include <iostream>
#include <windows.h>
#include <algorithm>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

Window::Window()
    : m_window(nullptr)
    , m_clearColor(0.04f, 0.05f, 0.05f, 1.0f)
    , m_shouldClose(false)
    , m_width(0)
    , m_height(0)
{
}

Window::~Window()
{
    Shutdown();
}

bool Window::Initialize(const char* title, int width, int height)
{
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!m_window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return false;
    }

    HWND hwnd = glfwGetWin32Window(m_window);
    if (hwnd) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    m_fontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/Unbounded-Regular.ttf", 14.0f);

    m_fontLarge = io.Fonts->AddFontFromFileTTF("resources/fonts/Unbounded-Bold.ttf", 28.0f);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void Window::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::ProcessEvents()
{
    if (!m_window) return;
    
    glfwPollEvents();
    
    if (glfwWindowShouldClose(m_window)) {
        m_shouldClose = true;
    }

    glfwGetWindowSize(m_window, &m_width, &m_height);
}

void Window::Render(const SystemState& state)
{
    if (!m_window) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 8.0f;
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    
    // ========== Interface ==========
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    
    ImGui::Begin("ZeroSysMon", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    ImGui::SetCursorPos(ImVec2(io.DisplaySize.x - 150, 10));
    static bool alwaysOnTop = true;
    if (ImGui::Checkbox("Always on Top", &alwaysOnTop)) {
        HWND hwnd = glfwGetWin32Window(m_window);
        if (hwnd) {
            SetWindowPos(hwnd, 
                        alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                        0, 0, 0, 0, 
                        SWP_NOMOVE | SWP_NOSIZE);
        }
    }

    float margin = 40.0f;
    float blockSpacing = 30.0f;

    float minBlockWidth = 400.0f;
    float minBlockHeight = 250.0f;
    
    // CPU BLOCK
    ImVec2 cpuBlockSize(std::max((m_width / 2 - margin - blockSpacing / 2), minBlockWidth), std::max((m_height / 2 - margin - blockSpacing / 2), minBlockHeight));
    ImVec2 cpuPosition(
        margin,
        margin
    );
    
    RenderCPU(state, cpuBlockSize, cpuPosition);

    // RAM BLOCK
    ImVec2 ramBlockSize(std::max((m_width / 2 - margin - blockSpacing / 2), minBlockWidth), std::max((m_height / 2 - margin - blockSpacing / 2), minBlockHeight));
    ImVec2 ramPosition(
        cpuBlockSize.x + margin + blockSpacing,
        margin
    );
    
    RenderRAM(state, ramBlockSize, ramPosition);

    // GPU BLOCK
    ImVec2 gpuBlockSize(std::max((m_width / 2 - margin - blockSpacing / 2), minBlockWidth), std::max((m_height / 2 - margin - blockSpacing / 2), minBlockHeight));
    ImVec2 gpuPosition(
        margin,
        cpuBlockSize.y + margin + blockSpacing
    );
    
    RenderGPU(state, gpuBlockSize, gpuPosition);

    ImGui::Dummy(ImVec2(0, 0));
    
    ImGui::End();
    // =================================
    
    // Render ImGui
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(m_window);
}

void Window::RenderCPU(const SystemState& state, const ImVec2& blockSize, const ImVec2& position)
{
    ImGui::SetCursorPos(position);
    
    ImGui::BeginChild("CPU Block", ImVec2(blockSize.x, blockSize.y / 2), true);
    
    // Header
    ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "CPU");
    ImGui::Separator();
    ImGui::Spacing();
    
    // CPU NAME
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "CPU:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%s", state.cpuName.c_str());
    
    // Core Count
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Cores:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%d", state.cpuCores);
    
    // Threads Count
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Threads:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%d", state.cpuThreads);

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Frequency:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%.2f GHz", state.cpuFrequencyGHz);
    
    ImGui::EndChild();

    float bottomHeight = blockSize.y / 2 - 10;
    float leftWidthPercent = 0.7f;
    float rightWidthPercent = 0.3f;

    ImGui::SetCursorPos(ImVec2(position.x, position.y + blockSize.y / 2 + 10));

    ImGui::BeginChild("CPU Graph", ImVec2(blockSize.x * leftWidthPercent - 5, bottomHeight), true);
    RenderCPUGraph(state, ImVec2(blockSize.x * leftWidthPercent - 5, bottomHeight));
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(position.x + blockSize.x * (1 - rightWidthPercent) + 5, position.y + blockSize.y / 2 + 10));

    ImGui::BeginChild("CPU Percent", ImVec2(blockSize.x * rightWidthPercent - 5, bottomHeight), true);
    RenderCPUPercent(state, ImVec2(blockSize.x * rightWidthPercent - 5, bottomHeight));
    ImGui::EndChild();
}

void Window::RenderCPUGraph(const SystemState& state, const ImVec2& blockSize)
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(35.0f/255.0f, 35.0f/255.0f, 35.0f/255.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(60.0f/255.0f, 221.0f/255.0f, 167.0f/255.0f, 1.0f));
    
    ImGui::PlotLines(
        "##CPU_GRAPH",
        state.cpuPercent,
        32,
        0,
        nullptr,
        0.0f,
        100.0f,
        ImVec2(blockSize.x - 20, blockSize.y - 30)
    );
    
    ImGui::PopStyleColor(2);
}

void Window::RenderCPUPercent(const SystemState& state, const ImVec2& blockSize)
{
    float currentPercent = state.cpuPercent[31];
    
    char percentText[16];
    snprintf(percentText, sizeof(percentText), "%.1f %%", currentPercent);
    ImVec2 percentSize = ImGui::CalcTextSize(percentText);
    
    float fontSize = 24.0f;
    float scale = fontSize / ImGui::GetFontSize();
    
    float centerX = (blockSize.x - percentSize.x * scale) / 2.0f;
    ImGui::SetCursorPosX(centerX);
    
    ImGui::SetCursorPosY((blockSize.y - percentSize.y * scale) / 2.0f);
    
    ImGui::PushFont(m_fontLarge);
    ImGui::TextColored(
        ImVec4(60.0f/255.0f, 221.0f/255.0f, 167.0f/255.0f, 1.0f),
        "%s", percentText
    );
    ImGui::PopFont();
}

void Window::RenderRAM(const SystemState& state, const ImVec2& blockSize, const ImVec2& position)
{
    ImGui::SetCursorPos(position);
    
    ImGui::BeginChild("RAM Block", ImVec2(blockSize.x, blockSize.y / 2), true);
    
    // Header
    ImGui::TextColored(ImVec4(170.0f/255.0f, 110.0f/255.0f, 255.0f/255.0f, 1.0f), "RAM");
    ImGui::Separator();
    ImGui::Spacing();
    
    // RAM COUNT
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Used:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%llu / %llu MB", state.ramUsedGB, state.ramTotalGB);
    
    ImGui::EndChild();

    float bottomHeight = blockSize.y / 2 - 10;
    float leftWidthPercent = 0.7f;
    float rightWidthPercent = 0.3f;

    ImGui::SetCursorPos(ImVec2(position.x, position.y + blockSize.y / 2 + 10));

    ImGui::BeginChild("RAM Graph", ImVec2(blockSize.x * leftWidthPercent - 5, bottomHeight), true);
    RenderRAMGraph(state, ImVec2(blockSize.x * leftWidthPercent - 5, bottomHeight));
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(position.x + blockSize.x * (1 - rightWidthPercent) + 5, position.y + blockSize.y / 2 + 10));

    ImGui::BeginChild("RAM Percent", ImVec2(blockSize.x * rightWidthPercent - 5, bottomHeight), true);
    RenderRAMPercent(state, ImVec2(blockSize.x * rightWidthPercent - 5, bottomHeight));
    ImGui::EndChild();
}

void Window::RenderRAMGraph(const SystemState& state, const ImVec2& blockSize)
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(35.0f/255.0f, 35.0f/255.0f, 35.0f/255.0f, 1.0f));
    
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(160.0f/255.0f, 100.0f/255.0f, 255.0f/255.0f, 1.0f));
    
    ImGui::PlotLines(
        "##RAM_GRAPH",
        state.ramPercent,
        32,
        0,
        nullptr,
        0.0f,
        100.0f,
        ImVec2(blockSize.x - 20, blockSize.y - 30)
    );
    
    ImGui::PopStyleColor(2);
}

void Window::RenderRAMPercent(const SystemState& state, const ImVec2& blockSize)
{
    float currentPercent = state.ramPercent[31];
    
    char percentText[16];
    snprintf(percentText, sizeof(percentText), "%.1f %%", currentPercent);
    ImVec2 percentSize = ImGui::CalcTextSize(percentText);
    
    float fontSize = 24.0f;
    float scale = fontSize / ImGui::GetFontSize();
    
    float centerX = (blockSize.x - percentSize.x * scale) / 2.0f;
    ImGui::SetCursorPosX(centerX);
    
    ImGui::SetCursorPosY((blockSize.y - percentSize.y * scale) / 2.0f);
    
    ImGui::PushFont(m_fontLarge);
    ImGui::TextColored(
        ImVec4(160.0f/255.0f, 100.0f/255.0f, 255.0f/255.0f, 1.0f),
        "%s", percentText
    );
    ImGui::PopFont();
}

void Window::RenderGPU(const SystemState& state, const ImVec2& blockSize, const ImVec2& position)
{
    ImGui::SetCursorPos(position);
    
    ImGui::BeginChild("GPU Block", ImVec2(blockSize.x, blockSize.y / 2), true);
    
    // Header
    ImGui::TextColored(ImVec4(1.0f, 0.549f, 0.196f, 1.0f), "GPU");
    ImGui::Separator();
    ImGui::Spacing();
    
    // GPU NAME
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "GPU:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%s", state.gpuName);
    
    // VRAM Count
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Used:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%llu / %llu GB", state.gpuMemoryUsedMB, state.gpuMemoryTotalMB);
    
    ImGui::EndChild();

    float bottomHeight = blockSize.y / 2 - 10;
    float leftWidthPercent = 0.7f;
    float rightWidthPercent = 0.3f;

    ImGui::SetCursorPos(ImVec2(position.x, position.y + blockSize.y / 2 + 10));

    ImGui::BeginChild("GPU Graph", ImVec2(blockSize.x * leftWidthPercent - 5, bottomHeight), true);
    RenderGPUGraph(state, ImVec2(blockSize.x * leftWidthPercent - 5, bottomHeight));
    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(position.x + blockSize.x * (1 - rightWidthPercent) + 5, position.y + blockSize.y / 2 + 10));

    ImGui::BeginChild("GPU Percent", ImVec2(blockSize.x * rightWidthPercent - 5, bottomHeight), true);
    RenderGPUPercent(state, ImVec2(blockSize.x * rightWidthPercent - 5, bottomHeight));
    ImGui::EndChild();
}

void Window::RenderGPUGraph(const SystemState& state, const ImVec2& blockSize)
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(35.0f/255.0f, 35.0f/255.0f, 35.0f/255.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.549f, 0.196f, 1.0f));
    
    ImGui::PlotLines(
        "##GPU_GRAPH",
        state.gpuPercent,
        32,
        0,
        nullptr,
        0.0f,
        100.0f,
        ImVec2(blockSize.x - 20, blockSize.y - 30)
    );
    
    ImGui::PopStyleColor(2);
}

void Window::RenderGPUPercent(const SystemState& state, const ImVec2& blockSize)
{
    float currentPercent = state.gpuPercent[31];
    
    char percentText[16];
    snprintf(percentText, sizeof(percentText), "%.1f %%", currentPercent);
    ImVec2 percentSize = ImGui::CalcTextSize(percentText);
    
    float fontSize = 24.0f;
    float scale = fontSize / ImGui::GetFontSize();
    
    float centerX = (blockSize.x - percentSize.x * scale) / 2.0f;
    ImGui::SetCursorPosX(centerX);
    
    ImGui::SetCursorPosY((blockSize.y - percentSize.y * scale) / 2.0f);
    
    ImGui::PushFont(m_fontLarge);
    ImGui::TextColored(
        ImVec4(1.0f, 0.549f, 0.196f, 1.0f),
        "%s", percentText
    );
    ImGui::PopFont();
}


bool Window::ShouldClose() const
{
    return m_shouldClose;
}