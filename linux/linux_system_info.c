#define _GNU_SOURCE
#include "system_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

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

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

double linux_percent_cpu_usage()
{
    struct perf_event_attr pe;
    long long count;
    int fd;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, -1, 0, -1, 0); // -1 for pid means all processes

    // TODO fd always equals -1
    if (fd == -1)
    {
        fprintf(stderr, "Error opening perf event for overall CPU usage\n", stderror(errno));
        return -1;
    }

    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    sleep(1);

    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &count, sizeof(long long));

    close(fd);

    // Convert cycles to a percentage
    double cpu_usage = ((double)count / sysconf(_SC_CLK_TCK)) / sysconf(_SC_NPROCESSORS_ONLN);
    return cpu_usage;
}

double linux_percent_cpu_usage_process()
{
    struct perf_event_attr pe;
    long long count;
    int fd;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0); // 0 for pid means current process

    // TODO: fd always equals -1
    if (fd == -1)
    {
        fprintf(stderr, "Error opening perf event for process CPU usage\n");
        return -1;
    }

    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    sleep(1);

    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &count, sizeof(long long));

    close(fd);

    // Convert cycles to a percentage
    double cpu_usage = ((double)count / sysconf(_SC_CLK_TCK)) / sysconf(_SC_NPROCESSORS_ONLN);
    return cpu_usage;
}