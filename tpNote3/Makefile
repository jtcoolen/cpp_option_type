CXX       := g++ #clang # pour des messages d'erreur plus sympathiques
CXX_FLAGS := -std=c++11 -Wall -Wextra -pedantic -Og
VALGRIND_FLAGS := --tool=memcheck --leak-check=yes --track-origins=yes
BIN     := bin
SRC     := src
INCLUDE := include
LIB     := lib
LIBRARIES   :=
EXECUTABLE  := main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	@echo "Executing..."
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "Building..."
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	@echo "Clearing..."
	-rm $(BIN)/*

valgrind: clean all
	clear
	@echo "Checking memory leaks..."
	valgrind $(VALGRIND_FLAGS) ./$(BIN)/$(EXECUTABLE)
