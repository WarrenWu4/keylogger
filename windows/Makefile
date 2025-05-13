CXX = g++
CXXFLAGS = -Iinclude
WINFLAGS = -mwindows -municode
LIBFLAGS = -lsetupapi
SRC = main.cpp src/*.cpp 
EXE = a.exe
LOG_FILE = keylogger.log

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(WINFLAGS) $(LIBFLAGS) -o $(EXE)

clean:
	rm $(EXE) $(LOG_FILE)

run: $(EXE)
	./$(EXE)
