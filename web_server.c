#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "system_info.h"

#define PORT 8005
#define BUFFER_SIZE 1024

// Handle a client's request
void handle_client(SOCKET client_socket)
{
    unsigned long long total_virtual_memory_avail = get_virtual_memory();
    unsigned long long virtual_memory_used = get_virtual_memory_usage();
    unsigned long long virtual_memory_process = get_virtual_memory_process();
    unsigned long long total_ram = get_ram();
    unsigned long long ram_used = get_ram_usage();
    unsigned long long ram_process = get_ram_from_process();
    double percent_cpu = percent_cpu_usage();
    double percent_cpu_process_usage = percent_cpu_process();

    char response[BUFFER_SIZE];
    // Define the HTTP response
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<!DOCTYPE HTML>"
             "<html>"
             "<head><title>System Information</title></head>"
             "<body>"
             "<p> total_virtual_memory_avail %llu\n </p>"
             "<p> virtual_memory_used %llu\n </p>"
             "<p> virtual_memory_process %llu\n </p>"
             "<p> total_ram %llu\n </p>"
             "<p> ram_used %llu\n </p>"
             "<p> ram_process %llu\n </p>"
             "<p> percent_cpu %.2f\n </p>"
             "<p> percent_cpu_process_usage %.2f\n </p>"
             "</body>"
             "</html>",
             total_virtual_memory_avail, virtual_memory_used, virtual_memory_process,
             total_ram, ram_used, ram_process, percent_cpu, percent_cpu_process_usage);

    // Send HTTP response to client,
    if (send(client_socket, response, strlen(response), 0) == SOCKET_ERROR)
    {
        fprintf(stderr, "send failed: %d\n", WSAGetLastError());
    }
}

int main()
{
    WSADATA wsa_data;                            // store Windows Sockets APi data
    SOCKET server_socket, client_socket;         // store server and client socket descriptors
    struct sockaddr_in server_addr, client_addr; // structures to store server and client information
    int client_addr_len = sizeof(client_addr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        fprintf(stderr, "WSAStartup Failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // Domain, Type, Protocol
    if (server_socket == INVALID_SOCKET)
    {
        fprintf(stderr, "Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set socket option to reuse address
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) == SOCKET_ERROR)
    {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Configure the server address structure
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available network interface
    server_addr.sin_port = htons(PORT);       // Convert port number to network byte order

    // Bind the socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        fprintf(stderr, "Listen failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    // Loop to accept incoming connections
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET)
        {
            fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
            continue; // Continue to accept other connections if an error occurs
        }

        // Handle the client's request
        handle_client(client_socket);

        Sleep(5000);
    }

    // Close the server socket
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
