CXX = g++
CXXFLAGS = -Iinclude
WINFLAGS = -mwindows -municode
LIBFLAGS = -lsetupapi
SRC = main.cpp src/*.cpp 
EXE = a.exe
LOG_FILES = *.log

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(WINFLAGS) $(LIBFLAGS) -o $(EXE)

clean:
	if exist $(EXE) del /Q $(EXE)
	del /Q $(LOG_FILES)

run: $(EXE)
	./$(EXE)
