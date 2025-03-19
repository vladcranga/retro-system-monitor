#pragma once
#include <string>
#include <vector>
#include <chrono>

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
        // Boolean to check if we have valid CPU measurements
        bool has_valid_measurement = false;
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
    
    // Configure update intervals (in milliseconds)
    void setUpdateIntervals(unsigned int cpu, unsigned int memory, 
                           unsigned int cpuTemp, unsigned int battery, 
                           unsigned int processes);

private:
    // Update methods
    void updateCPU();
    void updateMemory();
    void updateCPUTemp();
    void updateBattery();
    void updateProcesses();
    void updateDistroInfo();

    // Check if update is needed based on elapsed time
    bool shouldUpdate(std::chrono::time_point<std::chrono::steady_clock>& lastUpdate, 
                     unsigned int interval);

    // Member variables
    CPUStats cpu_stats;
    CPUStats cpu_stats_prev;
    MemoryStats mem_stats;
    float cpu_temp;
    BatteryStats battery_stats;
    std::vector<std::string> top_processes;
    DistroInfo distro_info;
    
    // Last update timestamps
    std::chrono::time_point<std::chrono::steady_clock> last_cpu_update;
    std::chrono::time_point<std::chrono::steady_clock> last_memory_update;
    std::chrono::time_point<std::chrono::steady_clock> last_cpu_temp_update;
    std::chrono::time_point<std::chrono::steady_clock> last_battery_update;
    std::chrono::time_point<std::chrono::steady_clock> last_processes_update;
    
    // Update intervals in milliseconds
    unsigned int cpu_update_interval = 1000;      // 1 second
    unsigned int memory_update_interval = 3000;   // 3 seconds
    unsigned int cpu_temp_update_interval = 5000; // 5 seconds
    unsigned int battery_update_interval = 30000; // 30 seconds
    unsigned int processes_update_interval = 3000; // 3 seconds
};