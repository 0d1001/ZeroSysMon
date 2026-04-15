# ZeroSysMon

[![Windows](https://img.shields.io/badge/Platform-Windows-blue)](https://github.com/0d1001/ZeroSysMon)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)

**ZeroSysMon** — лёгкий системный монитор для Windows с графическим интерфейсом. Отображает загрузку CPU, RAM и GPU в реальном времени с графиками истории.

**ZeroSysMon** — lightweight system monitor for Windows with GUI. Displays CPU, RAM and GPU usage in real time with history graphs.

---

## Скриншоты / Screenshots

![ZeroSysMon Screenshot](screenshot.png)

---

## Возможности / Features

| Русский | English |
|---------|---------|
| Мониторинг CPU (имя, ядра, частота, загрузка) | CPU monitoring (name, cores, frequency, usage) |
| Мониторинг RAM (использование памяти, процент) | RAM monitoring (memory usage, percentage) |
| Мониторинг GPU (имя, видеопамять, процент) | GPU monitoring (name, VRAM, percentage) |
| Графики истории (32 точки) | History graphs (32 points) |
| Режим "Поверх всех окон" | Always on Top mode |
| Тёмная тема | Dark theme |
| Кастомные шрифты | Custom fonts |

---

## 🛠️ Технологии / Technologies

- **C++17**
- **ImGui** — графический интерфейс / GUI
- **GLFW** — окно и OpenGL / Window and OpenGL context
- **WinAPI** — сбор системной информации / System information collection
  - `GetSystemTimes` — загрузка CPU / CPU usage
  - `GlobalMemoryStatusEx` — использование RAM / RAM usage
  - `DXGI` — информация о GPU / GPU information
  - `GetLogicalProcessorInformation` — физические ядра / Physical cores
  - Реестр Windows / Windows Registry

---

## Сборка / Build

### Требования / Requirements

- Windows 10/11
- MSYS2 с UCRT64 / MSYS2 with UCRT64
- CMake 3.10+
- Git

### Установка MSYS2 и зависимостей / Install MSYS2 and dependencies

1. Скачай и установи [MSYS2](https://www.msys2.org/) / Download and install [MSYS2](https://www.msys2.org/)
2. Открой **MSYS2 UCRT64** / Open **MSYS2 UCRT64**
3. Выполни команды / Run commands:

```bash
pacman -Syu
pacman -Su
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make