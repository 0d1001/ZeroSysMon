cmake_minimum_required(VERSION 3.10)
project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к ImGui
set(IMGUI_DIR "imgui")

# Файлы ImGui
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Создаем исполняемый файл
add_executable(ImGuiApp 
    src/main.cpp
    src/core/CpuCollector.cpp
    src/ui/Window.cpp
    ${IMGUI_SOURCES})

# Включаем директории
target_include_directories(ImGuiApp PRIVATE 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Находим GLFW (для ucrt64)
find_package(glfw3 REQUIRED)

# Линкуем библиотеки
target_link_libraries(ImGuiApp glfw)

# Для Windows с ucrt64
if(WIN32)
    target_link_libraries(ImGuiApp opengl32)
endif()

# Опции для подавления предупреждений
if(MSVC)
    target_compile_options(ImGuiApp PRIVATE /W3)
else()
    target_compile_options(ImGuiApp PRIVATE -Wall -Wno-unused-parameter)
endif()