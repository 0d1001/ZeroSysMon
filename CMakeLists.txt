cmake_minimum_required(VERSION 3.10)
project(ZeroSysMon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_DIR "imgui")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(ZeroSysMon 
    src/main.cpp
    src/core/CpuCollector.cpp
    src/core/RamCollector.cpp
    src/core/GpuCollector.cpp
    src/ui/Window.cpp
    ${IMGUI_SOURCES})

target_include_directories(ZeroSysMon PRIVATE 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

find_package(glfw3 REQUIRED)

target_link_libraries(ZeroSysMon glfw)

if(WIN32)
    target_link_libraries(ZeroSysMon opengl32 dxgi) 
endif()

if(MSVC)
    target_compile_options(ZeroSysMon PRIVATE /W3)
else()
    target_compile_options(ZeroSysMon PRIVATE -Wall -Wno-unused-parameter)
endif()