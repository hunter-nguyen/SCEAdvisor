
#include "system_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/sysinfo.h>

unsigned long long linux_get_total_virtual_memory()
{
    struct sysinfo info;
    if (sysinfo(&info) != 0)
    {
        perror("sysinfo");
        return 0;
    }
    return info.totalram * info.mem_unit;
}

unsigned long long linux_get_virtual_memory_used()
{
    struct sysinfo info;
    if (sysinfo(&info) != 0)
    {
        perror("sysinfo");
        return 0;
    }
    return (info.totalram - info.freeram) * info.mem_unit;
}

unsigned long long linux_get_virtual_memory_process()
{
    FILE *file = fopen("/proc/self/status", "r");

    if (!file)
    {
        perror("fopen");
        return 0;
    }
    unsigned long long vms = 0; // Virtual memory size
    char line[256];             // Store each line read from the file

    // Read the file line by line
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "VmSize:", 7) == 0)
        {
            sscanf(line + 7, "%llu", &vms);
            fclose(file);
            return vms * 1024; // Convert from kB to Bytes
        }
    }

    fclose(file);
    return 0;
}

unsigned long long linux_get_total_ram()
{
    struct sysinfo info;
    if (sysinfo(&info) == 0)
    {
        return info.totalram;
    }
    return 0;
}

unsigned long long linux_get_total_ram_usage()
{
    struct sysinfo info;
    if (sysinfo(&info) == 0)
    {
        return info.totalram - info.freeram;
    }
    return 0;
}

double linux_percent_cpu_usage()
{
    static unsigned long long last_total_time = 0;
    static unsigned long long last_idle_time = 0;

    FILE *fp;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total_time, idle_time;
    unsigned long long total_diff, idle_diff;

    // Read CPU times from /proc/stat
    fp = fopen("/proc/stat", "r");
    if (!fp)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8)
    {
        fclose(fp);
        perror("fscanf");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    total_time = user + nice + system + idle + iowait + irq + softirq + steal;
    idle_time = idle + iowait;

    // Calculate differences
    total_diff = total_time - last_total_time;
    idle_diff = idle_time - last_idle_time;

    // Update last times
    last_total_time = total_time;
    last_idle_time = idle_time;

    if (total_diff == 0)
    {
        return 0.0;
    }

    // Calculate CPU usage percentage
    double cpu_usage = (1.0 - (double)idle_diff / total_diff) * 100.0;
    return cpu_usage;
}

double linux_percent_cpu_usage_process()
{
    static unsigned long long last_total_time = 0;
    static unsigned long long last_process_time = 0;

    FILE *file;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total_time, process_total_time;
    unsigned long long total_diff, process_time_diff;

    // Read CPU times from /proc/stat
    file = fopen("/proc/stat", "r");
    if (!file)
    {
        perror("fopen /proc/stat");
        exit(EXIT_FAILURE);
    }
    if (fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8)
    {
        fclose(file);
        perror("fscanf /proc/stat");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    total_time = user + nice + system + idle + iowait + irq + softirq + steal;

    // Read process CPU times from /proc/self/stat
    file = fopen("/proc/self/stat", "r");
    if (!file)
    {
        perror("fopen /proc/self/stat");
        exit(EXIT_FAILURE);
    }
    unsigned long long process_utime, process_stime, process_cutime, process_cstime;
    if (fscanf(file, "%*d %*s %*c %*d %*d %llu %llu %llu %llu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d",
               &process_utime, &process_stime, &process_cutime, &process_cstime) != 4)
    {
        fclose(file);
        perror("fscanf /proc/self/stat");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    process_total_time = process_utime + process_stime + process_cutime + process_cstime;

    // Calculate differences
    total_diff = total_time - last_total_time;
    process_time_diff = process_total_time - last_process_time;

    // Update last times
    last_total_time = total_time;
    last_process_time = process_total_time;

    if (total_diff == 0)
    {
        return 0.0;
    }

    // Calculate process CPU usage percentage
    double process_cpu_usage = ((double)process_time_diff / total_diff) * 100.0;
    return process_cpu_usage;
}
