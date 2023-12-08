
EXECUTABLE_NAME = gameengine
SRCS = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)

CXX = clang++
CXX_STANDARD = c++17
CXX_WARNINGS = -Wall -Wfatal-errors -Wshadow -Wnon-virtual-dtor -Wcast-align
# -Wfatal-errors : 첫 번째 오류가 발생하면 즉시 컴파일을 중단. 에러가 너무 많을 시 유용
# -Wshadow : 지역 변수가 전역 변수를 가리는 경우 경고
# -Wnon-virtual-dtor : 가상 소멸자가 아닌 클래스의 소멸자를 가상 소멸자로 선언하지 않은 경우 경고
# -Wcast-align : 잘못된 메모리 정렬로 인한 캐스팅을 경고


CXX_STD_WARNINGS = -std=$(CXX_STANDARD) $(CXX_WARNINGS)

# Linker Flags, Libraries (ld = linker)
LDFLAGS = -L/opt/homebrew/lib 
LDLIBS = \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-lSDL2_mixer \
	-llua5.4 \
	-lspdlog \
	-lfmt
INCLUDE_PATHS = \
	-I/opt/homebrew/include\
	-I"./libs/"\
	-I"./src/"\


CXX_COMPILER_CALL = $(CXX) $(CXX_STD_WARNINGS) \
	$(SRCS) \
	$(LDFLAGS)\
	$(LDLIBS)\
	$(INCLUDE_PATHS) \
	-D_THREAD_SAFE \
	-o $(EXECUTABLE_NAME)

all: build run

build:
	$(CXX_COMPILER_CALL)

verbose:
	$(CXX_COMPILER_CALL) -v

debug:
	$(CXX_COMPILER_CALL) -g

videoCheck:
	$(CXX) $(CXX_STD_WARNINGS) _videoCheck.cpp \
	$(LDFLAGS)\
	$(LDLIBS)\
	$(INCLUDE_PATHS) \
	-D_THREAD_SAFE \
	-o videoCheck \
	&& ./videoCheck

compile_commands.json:
	rm compile_commands.json && bear -- make build

run:
	./$(EXECUTABLE_NAME)

clean:
	rm -f $(EXECUTABLE_NAME)

.PHONY: all build run clean verbose videoCheck compile_commands.json