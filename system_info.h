#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

unsigned long long get_virtual_memory();
unsigned long long get_virtual_memory_usage();
unsigned long long get_virtual_memory_process();
unsigned long long get_ram();
unsigned long long get_ram_usage();
unsigned long long get_ram_from_process();
double percent_cpu_usage();
double percent_cpu_process();

#endif