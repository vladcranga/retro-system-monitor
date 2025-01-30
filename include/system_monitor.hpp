#pragma once
#include <string>
#include <vector>

// Encapsulate the system monitoring functionality
class SystemMonitor {
public:
    struct CPUStats {
        // Percentage of CPU usage
        float usage_percent;
        // CPU times in user/nice/system/idle space
        unsigned long long user;
        unsigned long long nice;
        unsigned long long system;
        unsigned long long idle;
    };

    struct MemoryStats {
        // Percentage of memory usage
        float usage_percent;
        // Total/used/free amount of memory in KB
        unsigned long total;
        unsigned long used;
        unsigned long free;
    };

    struct BatteryStats {
        // Whether the battery is present/charging
        bool present;
        bool charging;
        // Percentage of battery level
        float percentage;
        // Battery status (e.g., "Charging")
        std::string status;
    };

    struct DistroInfo {
        // Distro name (may include version)
        std::string name;
    };

    // Constructor
    SystemMonitor();

    // Update the system stats
    void update();

    // Getter methods
    CPUStats getCPUStats() const { return cpu_stats; }
    MemoryStats getMemoryStats() const { return mem_stats; }
    float getCPUTemp() const { return cpu_temp; }
    BatteryStats getBatteryStats() const { return battery_stats; }
    std::vector<std::string> getTopProcesses(int count = 5) const;
    DistroInfo getDistroInfo() const { return distro_info; }

private:
    // Update methods
    void updateCPU();
    void updateMemory();
    void updateCPUTemp();
    void updateBattery();
    void updateProcesses();
    void updateDistroInfo();

    // Variable declarations
    CPUStats cpu_stats;
    CPUStats cpu_stats_prev;
    MemoryStats mem_stats;
    float cpu_temp;
    BatteryStats battery_stats;
    std::vector<std::string> top_processes;
    DistroInfo distro_info;
};