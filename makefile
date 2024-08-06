# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

# Targets
TARGET_LINUX = linux_web_server
TARGET_WINDOWS = windows_web_server

# Source files for each platform
SRCS_LINUX = linux/linux_web_server.c linux/linux_system_info.c
SRCS_WINDOWS = windows/windows_web_server.c windows/windows_system_info.c

# Object files for each platform
OBJS_LINUX = $(SRCS_LINUX:.c=.o)
OBJS_WINDOWS = $(SRCS_WINDOWS:.c=.o)

# Include directory for headers
INCLUDE_DIR = .

# Default target
all: $(TARGET_LINUX)

# Linux build rules
$(TARGET_LINUX): $(OBJS_LINUX)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $^ $(LDFLAGS)

# Rule for compiling .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Windows build rules
$(TARGET_WINDOWS): $(OBJS_WINDOWS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $^ -lws2_32 -lpdh -D_WINDOWS

# Clean up
clean:
	@echo "Cleaning up"
	@rm -f $(OBJS_LINUX) $(TARGET_LINUX) $(OBJS_WINDOWS) $(TARGET_WINDOWS)
	@rm -f *.o linux/*.o windows/*.o

# Windows target
windows: $(TARGET_WINDOWS)
