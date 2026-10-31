# ============================================================================
# FMCOSKUN STUDIO — 3D INTERACTIVE DESKTOP PORTFOLIO (Makefile)
# Usage: mingw32-make        (build executable)
#        mingw32-make run    (build and launch)
#        mingw32-make test   (build and run JSON parser tests)
#        mingw32-make clean  (remove build artifacts)
# ============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
SRC      := main.cpp

ifeq ($(OS),Windows_NT)
    TARGET  := fmcoskun_studio.exe
    TEST_BIN := json_test.exe
    LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm
    RM_CMD  := del /Q
else
    TARGET  := fmcoskun_studio
    TEST_BIN := json_test
    LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    RM_CMD  := rm -f
endif

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(SRC) $(wildcard include/*.hpp)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_BIN)
	./$(TEST_BIN) .

$(TEST_BIN): tests/json_test.cpp $(wildcard include/*.hpp)
	$(CXX) $(CXXFLAGS) -Iinclude tests/json_test.cpp -o $(TEST_BIN) $(LDFLAGS)

clean:
	$(RM_CMD) $(TARGET) $(TEST_BIN)

