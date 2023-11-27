
CXX = clang++
CXX_STANDARD = c++17
CXX_WARNINGS = -Wall

CXX_COMPILER_FLAGS = -std=$(CXX_STANDARD) $(CXX_WARNINGS)
# CXX_LINKER_FLAGS = -I $(INCLUDE_DIR)

# compiler call build
# CXX_COMPILER_CALL = $(CXX) $(CXX_COMPILER_FLAGS) $(CXX_LINKER_FLAGS)

EXECUTABLE_NAME = gameengine

all: build run

build:
	$(CXX) $(CXX_COMPILER_FLAGS) src/*.cpp \
	-L/opt/homebrew/lib \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-lSDL2_mixer \
	-llua5.4 \
	-I"./libs/" \
	-I/opt/homebrew/include \
	-D_THREAD_SAFE \
	-o $(EXECUTABLE_NAME)

run:
	./$(EXECUTABLE_NAME)

clean:
	rm -f $(EXECUTABLE_NAME)

.PHONY: all build run clean