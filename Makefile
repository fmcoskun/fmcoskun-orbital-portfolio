# ============================================================================
# FMCOSKUN STUDIO — 3D INTERACTIVE DESKTOP PORTFOLIO (Makefile)
# Usage: mingw32-make        (build executable)
#        mingw32-make run    (build and launch)
#        mingw32-make clean  (remove build artifacts)
# ============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
SRC      := main.cpp

ifeq ($(OS),Windows_NT)
    TARGET  := fmcoskun_studio.exe
    LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm
    RM_CMD  := del /Q
else
    TARGET  := fmcoskun_studio
    LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    RM_CMD  := rm -f
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM_CMD) $(TARGET)

