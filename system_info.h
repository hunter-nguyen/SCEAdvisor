#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#ifdef _WIN32
unsigned long long get_virtual_memory();
unsigned long long get_virtual_memory_usage();
unsigned long long get_virtual_memory_process();
unsigned long long get_ram();
unsigned long long get_ram_usage();
unsigned long long get_ram_from_process();
double percent_cpu_usage();
double percent_cpu_process();
void handle_client(int client_socket);

#elif __linux__
#include <sys/types.h>
#include <sys/sysinfo.h>
unsigned long long linux_get_total_virtual_memory();
unsigned long long linux_get_virtual_memory_used();
unsigned long long linux_get_virtual_memory_process();
unsigned long long linux_get_total_ram();
unsigned long long linux_get_total_ram_usage();
double linux_percent_cpu_usage();
double linux_percent_cpu_usage_process();

#endif

#endif