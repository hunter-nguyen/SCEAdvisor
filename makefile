# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable and object files
TARGET = web_server
OBJS = web_server.o system_info.o

# Rule to compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -lws2_32 -lpdh

# Clean to remove generated files
clean:
	echo "Cleaning up"
	rm -f $(OBJS) $(TARGET)