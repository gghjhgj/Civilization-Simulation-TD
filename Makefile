SOURCES := $(shell find . -name "*.cpp")
TARGET = app
CXX = g++
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc
INCLUDE = -I.

release: CXXFLAGS = -Ofast -march=native -flto -funroll-loops -ffast-math
release: build
	@echo "Build RELEASE gotowy."

debug: CXXFLAGS = -g -O0 -Wall -Wextra
debug: build
	@echo "Build DEBUG gotowy."

build:
	@echo "Kompilowanie w trybie..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) glad.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET).exe