

CXX = clang++
CXX_STANDARD = c++17
# CXX_WARNINGS = -Wall -Wextra -Wpedantic -Werror
CXX_WARNINGS = -Wall

CXXFLAGS = -std=$(CXX_STANDARD) $(CXX_WARNINGS)
CPPFLAGS = -I $(INCLUDE_DIR)

# compiler call build
# CXX_COMPILER_CALL = $(CXX) $(CXXFLAGS) $(CPPFLAGS)

all: build run

build:
	$(CXX) $(CXXFLAGS) src/*.cpp \
	-L/opt/homebrew/lib \
	-lSDL2 \
	-llua5.4 \
	-I/opt/homebrew/include \
	-D_THREAD_SAFE \
	-o gameengine

run:
	./gameengine

clean:
	rm gameengine

.PHONY: all build run clean