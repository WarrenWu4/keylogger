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
endif

# macos
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
    CXXFLAGS = -Wall -Wextra -std=c++17 -framework Cocoa -framework ApplicationServices
    LDFLAGS = 
    SRC = main.cpp
    TARGET = $(APP_NAME)
endif

# windows
ifeq ($(OS),Windows_NT)
    CXX = x86_64-w64-mingw32-g++
    CXXFLAGS = -Wall -Wextra -std=c++17 -mwindows
    LDFLAGS = -lgdi32 -luser32
    SRC = keylogger_tray.cpp
    TARGET = $(APP_NAME).exe
endif

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
