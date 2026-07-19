SOURCES := $(shell find . -name "*.cpp")
TARGET = app

CXX = g++
CLANG = clang++

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc
CLANG_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

INCLUDE = -I.

COMMON_FLAGS = -march=native -flto -Ofast -funroll-loops -ffast-math

CLANG_FLAGS = -O2 -march=native -fno-omit-frame-pointer \
-Wno-c++11-narrowing -g -gcodeview -fuse-ld=lld \
-Wl,-pdb=app.pdb


# Profile folders
GCC_PROFILE_DIR = profiles/gcc
CLANG_PROFILE = profiles/clang/default.profraw
CLANG_PROFDATA = profiles/clang/app.profdata


# =========================
# GCC BUILDS
# =========================

release: CXXFLAGS = $(COMMON_FLAGS)
release: build
	@echo "Build RELEASE gotowy."


profile: CXXFLAGS = -O2 -march=native -g -fno-omit-frame-pointer
profile: build
	@echo "Build PROFILE gotowy."


debug: CXXFLAGS = -g -O0 -Wall -Wextra
debug: build
	@echo "Build DEBUG gotowy."



# =========================
# GCC PGO
# =========================

generate: CXXFLAGS = $(COMMON_FLAGS) -fprofile-generate=$(GCC_PROFILE_DIR)

generate:
	@mkdir -p $(GCC_PROFILE_DIR)
	@echo "Generowanie GCC PGO..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) glad.c -o $(TARGET).exe $(LIBS)
	@echo ""
	@echo "Odpal app.exe i wykonaj testy."
	@echo "Profile zapiszą się w $(GCC_PROFILE_DIR)"


use: CXXFLAGS = $(COMMON_FLAGS) -fprofile-use=$(GCC_PROFILE_DIR) -fprofile-correction

use:
	@echo "Używanie GCC PGO..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) glad.c -o $(TARGET).exe $(LIBS)
	@echo "Build GCC PGO gotowy."



# =========================
# NORMAL BUILD
# =========================

build:
	@echo "Kompilowanie..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) glad.c -o $(TARGET).exe $(LIBS)



# =========================
# CLANG NORMAL
# =========================

clang:
	@echo "Build CLANG..."

	$(CLANG) $(CLANG_FLAGS) \
	$(INCLUDE) \
	$(SOURCES) \
	glad.c \
	-o $(TARGET).exe \
	$(CLANG_LIBS)

	@echo "Build CLANG gotowy."



# =========================
# CLANG PGO
# =========================

clang-generate:
	@mkdir -p profiles/clang

	@echo "Generowanie CLANG PGO..."

	$(CLANG) $(CLANG_FLAGS) \
	-fprofile-instr-generate=$(CLANG_PROFILE) \
	$(INCLUDE) \
	$(SOURCES) \
	glad.c \
	-o $(TARGET).exe \
	$(CLANG_LIBS)

	@echo ""
	@echo "Odpal app.exe aby wygenerować:"
	@echo "$(CLANG_PROFILE)"



clang-use:
	@echo "Tworzenie profilu LLVM..."

	llvm-profdata merge \
	-output=$(CLANG_PROFDATA) \
	$(CLANG_PROFILE)


	@echo "Budowanie CLANG z PGO..."

	$(CLANG) $(CLANG_FLAGS) \
	-fprofile-instr-use=$(CLANG_PROFDATA) \
	$(INCLUDE) \
	$(SOURCES) \
	glad.c \
	-o $(TARGET).exe \
	$(CLANG_LIBS)

	@echo "Build CLANG PGO gotowy."



# =========================
# CLEAN
# =========================

clean:
	rm -f $(TARGET).exe
	rm -rf profiles
	rm -f *.pdb