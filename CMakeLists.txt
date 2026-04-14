cmake_minimum_required(VERSION 3.10)
project(ZeroSysMon)

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
add_executable(ZeroSysMon 
    src/main.cpp
    src/core/CpuCollector.cpp
    src/core/RamCollector.cpp
    src/ui/Window.cpp
    ${IMGUI_SOURCES})

# Включаем директории
target_include_directories(ZeroSysMon PRIVATE 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Находим GLFW (для ucrt64)
find_package(glfw3 REQUIRED)

# Линкуем библиотеки
target_link_libraries(ZeroSysMon glfw)

# Для Windows с ucrt64
if(WIN32)
    target_link_libraries(ZeroSysMon opengl32)
endif()

# Опции для подавления предупреждений
if(MSVC)
    target_compile_options(ZeroSysMon PRIVATE /W3)
else()
    target_compile_options(ZeroSysMon PRIVATE -Wall -Wno-unused-parameter)
endif()