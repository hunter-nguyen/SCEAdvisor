## Prerequisites

Before you begin, ensure you have the following installed on your machine:

### General
- **Make**: A build automation tool that automatically builds executable programs and libraries from source code
- **GCC**: The GNU Compiler Collection, which includes the C compiler

### Windows
- **MinGW** or **Visual Studio**: For compiling Windows applications
- **Windows SDK**: Includes necessary libraries like `ws2_32` and `pdh`

### Docker
- **Docker**: A platform for developing, shipping, and running applications in containers
- **Docker Compose**: A tool for defining and running multi-container Docker applications
---
## Compiling
### Linux
To build for Linux, run:
```make```

### Windows
To build for Windows, run:
```make windows```

## Running
### Linux
After running, to start the server with Linux, run:
```./linux_web_server```

### Windows
After building, to start the server with Windows, run:
```./windows_web_server```

## Cleaning Up
To clean up all build files for both Linux and Windows, run:
```make clean```

## Docker
To build and run using Docker, run:
```cd docker
docker-compose up```
