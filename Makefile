SOURCES := $(shell find . -name "*.cpp")

TARGET = app

all:
	@echo "Compiling..."
	g++ -Ofast -march=native -flto -funroll-loops -ffast-math -I. $(SOURCES) glad.c -o $(TARGET) -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc
	@echo "Compiling finished"
	./$(TARGET)
	
clean:
	rm -f $(TARGET).exe