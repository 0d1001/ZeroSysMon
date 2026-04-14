#include "Window.h"
#include <iostream>


Window::Window()
    : m_window(nullptr)
    , m_clearColor(0.45f, 0.55f, 0.60f, 1.00f)
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
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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
}

void Window::Render(const SystemState& state)
{
    if (!m_window) return;
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.ChildRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.PopupRounding = 8.0f;
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.95f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.15f, 0.98f);
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
    
    // CPU BLOCK
    ImVec2 cpuBlockSize(400.0f, 170.0f);
    ImVec2 cpuPosition(
        (io.DisplaySize.x - cpuBlockSize.x) * 0.1f,
        (io.DisplaySize.y - cpuBlockSize.y) * 0.2f 
    );
    
    RenderCPU(state, cpuBlockSize, cpuPosition);
    
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
    
    ImGui::BeginChild("CPU Block", blockSize, true);
    
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
    
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Frequency:");
    ImGui::SameLine(120.0f);
    ImGui::Text("%.2f GHz", state.cpuFrequencyGHz);

    ImGui::Separator();
    ImGui::Spacing();
    
    // CPU USAGE
    ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "CPU USAGE:");
    ImGui::ProgressBar((float)state.cpuPercent / 100.0f, ImVec2(blockSize.x - 75, 25));
    ImGui::SameLine(blockSize.x - 55);
    ImGui::Text("%.1f %%", state.cpuPercent);
    
    ImGui::EndChild();
}

bool Window::ShouldClose() const
{
    return m_shouldClose;
}