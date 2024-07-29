# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lws2_32 -lpdh

# Target executable and object files
SRCS = web_server.c system_info.c
OBJS = $(SRCS:.c=.o)

TARGET = web_server

all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
# Clean to remove generated files
clean:
	echo "Cleaning up"
	rm -f $(OBJS) $(TARGET)