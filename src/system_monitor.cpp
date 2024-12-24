#include "../include/system_monitor.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <dirent.h>

SystemMonitor::SystemMonitor() {
    update();
}

void SystemMonitor::update() {
    updateCPU();
    updateMemory();
    updateCPUTemp();
    updateBattery();
    updateProcesses();
}

void SystemMonitor::updateCPU() {
    std::ifstream stat_file("/proc/stat");
    std::string line;
    
    if (std::getline(stat_file, line)) {
        std::istringstream ss(line);
        std::string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        
        ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        
        cpu_stats_prev = cpu_stats;
        cpu_stats.user = user;
        cpu_stats.nice = nice;
        cpu_stats.system = system;
        cpu_stats.idle = idle;
        
        // Skip first calculation
        if (cpu_stats_prev.user != 0) {
            unsigned long long prev_idle = cpu_stats_prev.idle;
            unsigned long long prev_total = cpu_stats_prev.user + cpu_stats_prev.nice + 
                                          cpu_stats_prev.system + cpu_stats_prev.idle;
            unsigned long long curr_idle = idle;
            unsigned long long curr_total = user + nice + system + idle;
            
            unsigned long long total_diff = curr_total - prev_total;
            unsigned long long idle_diff = curr_idle - prev_idle;
            
            cpu_stats.usage_percent = ((total_diff - idle_diff) * 100.0) / total_diff;
        }
    }
}

void SystemMonitor::updateMemory() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    unsigned long total = 0, free = 0, buffers = 0, cached = 0;
    
    while (std::getline(meminfo, line)) {
        if (line.substr(0, 9) == "MemTotal:")
            sscanf(line.c_str(), "MemTotal: %lu kB", &total);
        else if (line.substr(0, 8) == "MemFree:")
            sscanf(line.c_str(), "MemFree: %lu kB", &free);
        else if (line.substr(0, 8) == "Buffers:")
            sscanf(line.c_str(), "Buffers: %lu kB", &buffers);
        else if (line.substr(0, 7) == "Cached:")
            sscanf(line.c_str(), "Cached: %lu kB", &cached);
    }
    
    mem_stats.total = total;
    mem_stats.free = free + buffers + cached;
    mem_stats.used = total - mem_stats.free;
    mem_stats.usage_percent = (mem_stats.used * 100.0) / total;
}

void SystemMonitor::updateCPUTemp() {
    std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
    int temp;
    if (temp_file >> temp) {
        // Convert from millidegrees to degrees
        cpu_temp = temp / 1000.0;  
    }
}

void SystemMonitor::updateBattery() {
    battery_stats.present = false;
    battery_stats.percentage = 0.0;
    battery_stats.charging = false;
    battery_stats.status = "No Battery";

    // Try to read battery info from /sys/class/power_supply/BAT0/
    std::ifstream status_file("/sys/class/power_supply/BAT0/status");
    std::ifstream capacity_file("/sys/class/power_supply/BAT0/capacity");

    if (status_file.good() && capacity_file.good()) {
        battery_stats.present = true;
        
        // Read battery percentage
        std::string capacity;
        if (std::getline(capacity_file, capacity)) {
            battery_stats.percentage = std::stof(capacity);
        }

        // Read charging status
        std::string status;
        if (std::getline(status_file, status)) {
            battery_stats.status = status;
            battery_stats.charging = (status == "Charging");
        }
    }
}

void SystemMonitor::updateProcesses() {
    std::vector<std::string> processes;
    DIR* dir = opendir("/proc");
    struct dirent* ent;
    
    if (dir != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            // Only process directories (PIDs)
            if (std::isdigit(ent->d_name[0])) {
                std::string cmd_path = "/proc/" + std::string(ent->d_name) + "/comm";
                std::ifstream cmd_file(cmd_path);
                std::string cmd_name;
                if (std::getline(cmd_file, cmd_name)) {
                    processes.push_back(cmd_name);
                }
            }
        }
        closedir(dir);
    }
    
    std::sort(processes.begin(), processes.end());
    top_processes = std::vector<std::string>(processes.begin(), 
                                           processes.begin() + std::min(size_t(5), processes.size()));
}

std::vector<std::string> SystemMonitor::getTopProcesses(int count) const {
    return std::vector<std::string>(top_processes.begin(),
                                   top_processes.begin() + std::min(count, (int)top_processes.size()));
}