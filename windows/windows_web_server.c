#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "system_info.h"

#define PORT 8005
#define BUFFER_SIZE 2048

void handle_client(SOCKET client_socket)
{
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char content_length[64];

    int received = recv(client_socket, request, sizeof(request), 0);
    if (received <= 0)
    {
        fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
        return;
    }

    request[received] = '\0';

    // Check if the request is for /metrics endpoint (localhost:PORT/metrics)
    if (strstr(request, "GET /metrics HTTP/1.1") != NULL)
    {
        // Define the HTTP response
        snprintf(response, sizeof(response),
                 "# HELP total_virtual_memory_avail Total virtual memory available in bytes\n"
                 "# TYPE total_virtual_memory_avail gauge\n"
                 "total_virtual_memory_avail %llu\n"
                 "# HELP virtual_memory_used Virtual memory used in bytes\n"
                 "# TYPE virtual_memory_used gauge\n"
                 "virtual_memory_used %llu\n"
                 "# HELP virtual_memory_process Virtual memory used by process in bytes\n"
                 "# TYPE virtual_memory_process gauge\n"
                 "virtual_memory_process %llu\n"
                 "# HELP total_ram Total ram available in bytes\n"
                 "# TYPE total_ram gauge\n"
                 "total_ram %llu\n"
                 "# HELP ram_used Total ram used in bytes\n"
                 "# TYPE ram_used gauge\n"
                 "ram_used %llu\n"
                 "# HELP ram_process Total ram used by process in bytes\n"
                 "# TYPE ram_process gauge\n"
                 "ram_process %llu\n"
                 "# HELP percent_cpu Percentage of CPU usage\n"
                 "# TYPE percent_cpu gauge\n"
                 "percent_cpu %.2f\n"
                 "# HELP percent_cpu_process_usage Percentage of CPU usage by process\n"
                 "# TYPE percent_cpu_process_usage gauge\n"
                 "percent_cpu_process_usage %.2f\n",
                 get_virtual_memory(), get_virtual_memory_usage(), get_virtual_memory_process(),
                 get_ram(), get_ram_usage(), get_ram_from_process(),
                 percent_cpu_usage(), percent_cpu_process());
        // Write HTTP headers
        const char *headers = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/plain; version=0.0.4; charset=utf-8\r\n";
        snprintf(content_length, sizeof(content_length), "Content-Length: %zu\r\n\r\n", strlen(response));

        // Send headers and response
        if (send(client_socket, headers, strlen(headers), 0) == SOCKET_ERROR)
        {
            fprintf(stderr, "send failed: %d\n", WSAGetLastError());
        }
        if (send(client_socket, content_length, strlen(content_length), 0) == SOCKET_ERROR)
        {
            fprintf(stderr, "send failed: %d\n", WSAGetLastError());
        }
        if (send(client_socket, response, strlen(response), 0) == SOCKET_ERROR)
        {
            fprintf(stderr, "send failed: %d\n", WSAGetLastError());
        }
    }
    else
    {
        const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        if (send(client_socket, not_found, strlen(not_found), 0) == SOCKET_ERROR)
        {
            fprintf(stderr, "send failed: %d\n", WSAGetLastError());
        }
    }

    // Close the connection
    closesocket(client_socket);
}

int main()
{
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Initialize Winsock for Windows
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // Create a socket for the server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        fprintf(stderr, "Socket creation failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;         // Address family (IPv4)
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    server_addr.sin_port = htons(PORT);       // Port to listen on

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr, "Bind failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        fprintf(stderr, "Listen failed: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Listening on localhost:%d\n", PORT);

    // Main server loop to accept and handle client connections
    // Accept an incoming connection
    while (1)
    {
        client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock >= 0)
        {
            // Handle the client's request
            handle_client(client_sock);
        }
        else
        {
            fprintf(stderr, "Client accept failed: %d\n", WSAGetLastError());
        }
    }

    closesocket(server_fd);
    WSACleanup();

    return 0;
}