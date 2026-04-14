#include "CpuCollector.h"
#include "windows.h"
#include <cstdint>
#include <cstdlib>

CpuCollector::CpuCollector() { }

CpuCollector::~CpuCollector() { }

bool CpuCollector::Initialize(SystemState& state)
{
    state.cpuPercent[31] = GetCurrentUsage();
    state.cpuName = GetProcessorName();
    state.cpuCores = GetCoreCount();
    state.cpuThreads = GetThreadCount();
    state.cpuFrequencyGHz = GetFrequency();
    
    return true;
}

void CpuCollector::Update(SystemState& state)
{
    for (int i = 0; i < 31; i++)
    {
        state.cpuPercent[i] = state.cpuPercent[i + 1];
    }
    state.cpuPercent[31] = GetCurrentUsage();
}

std::string CpuCollector::GetProcessorName() {
    HKEY hKey;
    std::string cpuName = "Unknown";
    
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        char buffer[512] = {0};
        DWORD bufferSize = sizeof(buffer);
        DWORD type = REG_SZ;
        
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr,
                             &type, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            cpuName = buffer;
            printf("CPU Name: [%s]\n", cpuName.c_str());
        } else {
            printf("RegQueryValueExA failed, error: %ld\n", GetLastError());
        }
        
        RegCloseKey(hKey);
    } else {
        printf("RegOpenKeyExA failed, error: %ld\n", GetLastError());
    }
    
    return cpuName;
}

int CpuCollector::GetCoreCount() {
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize);
    
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        return -1; 
    }
    
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = 
        (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(bufferSize);
    
    if (!buffer) {
        return -1;
    }
    
    if (!GetLogicalProcessorInformation(buffer, &bufferSize)) {
        free(buffer);
        return -1;  // ERROR
    }
    
    int coreCount = 0;
    DWORD entries = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
    
    for (DWORD i = 0; i < entries; i++) {
        if (buffer[i].Relationship == RelationProcessorCore) {
            coreCount++;
        }
    }
    
    free(buffer);
    return coreCount;
}

int CpuCollector::GetThreadCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    return sysinfo.dwNumberOfProcessors;
}

float CpuCollector::GetFrequency()
{
    HKEY hKey;
    DWORD mhz = 0;
    DWORD size = sizeof(mhz);
    
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExA(hKey, "~MHz", NULL, NULL, (LPBYTE)&mhz, &size);
        RegCloseKey(hKey);
    }
    
    return mhz / 1000.0f;  // MHz → GHz
}

float CpuCollector::GetCurrentUsage()
{
    static FILETIME idleTimePrev = {}, kernelTimePrev = {}, userTimePrev = {};

    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) return 0.0;

    auto toUInt64 = [](FILETIME ft) {
        return ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        };

    uint64_t idleDiff = toUInt64(idleTime) - toUInt64(idleTimePrev);
    uint64_t kernelDiff = toUInt64(kernelTime) - toUInt64(kernelTimePrev);
    uint64_t userDiff = toUInt64(userTime) - toUInt64(userTimePrev);

    idleTimePrev = idleTime;
    kernelTimePrev = kernelTime;
    userTimePrev = userTime;

    uint64_t total = kernelDiff + userDiff;
    return total ? (1.0 - (float)idleDiff / total) * 100.0 : 0.0;
}

