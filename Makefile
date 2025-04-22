APP_NAME = keylogger

# Detect OS
UNAME_S := $(shell uname -s)

# linux
ifeq ($(UNAME_S),Linux)
    CXX = g++
    CXXFLAGS = -Wall -Wextra -std=c++17 `pkg-config --cflags x11`
    LDFLAGS = `pkg-config --libs x11`
    SRC = main.cpp
    TARGET = $(APP_NAME)
	OS:= Linux
else ifeq ($(UNAME_S),Darwin)
    CXX = clang++
    CXXFLAGS = -Wall -Wextra -std=c++17 -framework Cocoa -framework ApplicationServices
    LDFLAGS = 
    SRC = main.cpp
    TARGET = $(APP_NAME)
	OS:= MacOS 
else ifeq ($(OS),Windows_NT)
    CXX = g++ 
    CXXFLAGS = -Wall -Wextra -std=c++17
    LDFLAGS = `` 
    SRC = main.cpp
    TARGET = $(APP_NAME).exe
	OS := Windows 
else
	OS := Unknown
endif

all:
	@echo Detected OS: $(OS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
