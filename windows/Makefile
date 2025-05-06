CXX = g++
WINFLAGS = -mwindows -municode
LIBFLAGS = -lsetupapi
SRC = main.cpp
EXE = a.exe
LOG_FILE = keylogger.log

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(WINFLAGS) $(SRC) $(LIBFLAGS) -o $(EXE)

clean:
	rm $(EXE) $(LOG_FILE)

run: $(EXE)
	./$(EXE)
