# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Targets
TARGET_LINUX = linux_web_server
TARGET_WINDOWS = windows_web_server

# Source files for each platform
SRCS_LINUX = linux_web_server.c linux_system_info.c
SRCS_WINDOWS = windows_web_server.c windows_system_info.c

# Object files for each platform
OBJS_LINUX = $(SRCS_LINUX:.c=.o)
OBJS_WINDOWS = $(SRCS_WINDOWS:.c=.o)

# Include directory for headers
INCLUDE_DIR = .

# Linux build rules
$(TARGET_LINUX): $(OBJS_LINUX)
	$(CC) $(CFLAGS) -o $(TARGET_LINUX) $(OBJS_LINUX)

# Rule for compiling .c files to .o files on Linux
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Windows build rules (with Windows-specific libraries)
$(TARGET_WINDOWS): $(OBJS_WINDOWS)
	$(CC) $(CFLAGS) -o $(TARGET_WINDOWS) $(OBJS_WINDOWS) -lws2_32 -lpdh

# Rule for compiling .c files to .o files on Windows
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@ -D_WINDOWS

# Clean up
clean:
	echo "Cleaning up"
	rm -f $(OBJS_LINUX) $(TARGET_LINUX) $(OBJS_WINDOWS) $(TARGET_WINDOWS)
