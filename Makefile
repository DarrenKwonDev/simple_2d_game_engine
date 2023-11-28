
CXX = clang++
CXX_STANDARD = c++17
CXX_WARNINGS = -Wall

CXX_COMPILER_FLAGS = -std=$(CXX_STANDARD) $(CXX_WARNINGS)
# CXX_LINKER_FLAGS = -I $(INCLUDE_DIR)

EXCLUDE_FILES = src/_videoCheck.cpp src/_test.cpp

EXECUTABLE_NAME = gameengine

CXX_COMPILER_CALL = $(CXX) $(CXX_COMPILER_FLAGS) src/*.cpp \
	-L/opt/homebrew/lib \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-lSDL2_mixer \
	-llua5.4 \
	-lspdlog \
	-lfmt \
	-I"./libs/" \
	-I/opt/homebrew/include \
	-D_THREAD_SAFE \
	-o $(EXECUTABLE_NAME)

all: build run

build:
	$(CXX_COMPILER_CALL)

verbose:
	$(CXX_COMPILER_CALL) -v

videoCheck:
	$(CXX) $(CXX_COMPILER_FLAGS) _videoCheck.cpp \
	-L/opt/homebrew/lib \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-lSDL2_mixer \
	-I"./libs/" \
	-I/opt/homebrew/include \
	-D_THREAD_SAFE \
	-o videoCheck \
	&& ./videoCheck

run:
	./$(EXECUTABLE_NAME)

clean:
	rm -f $(EXECUTABLE_NAME)

.PHONY: all build run clean verbose videoCheck