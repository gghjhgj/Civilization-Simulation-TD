SOURCES := $(shell find . -name "*.cpp" -o -name "*.c")
TARGET = app

CXX = g++
CLANG = clang++

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lmimalloc -ltbb12
CLANG_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -ltbb12

INCLUDE = -I.

COMMON_FLAGS = -march=native -flto -Ofast -funroll-loops -ffast-math

CLANG_FLAGS = -O2 -march=native -fno-omit-frame-pointer \
-Wno-c++11-narrowing -g -gcodeview -fuse-ld=lld \
-Wl,-pdb=app.pdb


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
# ASEMBLER
# =========================

assembler: CXXFLAGS = $(filter-out -flto,$(COMMON_FLAGS)) -S -fverbose-asm

assembler:
	@echo "Generowanie asemblera..."
	@for src in $(SOURCES); do \
		echo "Kompilowanie $$src..."; \
		$(CXX) $(CXXFLAGS) $(INCLUDE) $$src -o $${src%.*}.s; \
	done


# =========================
# GCC PGO
# =========================

generate: CXXFLAGS = $(COMMON_FLAGS) -fprofile-generate=$(GCC_PROFILE_DIR)

generate:
	@mkdir -p $(GCC_PROFILE_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) -o $(TARGET).exe $(LIBS)
	@echo "Uruchom app.exe aby wygenerować profile."


use: CXXFLAGS = $(COMMON_FLAGS) -fprofile-use=$(GCC_PROFILE_DIR) -fprofile-correction

use:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) -o $(TARGET).exe $(LIBS)
	@echo "Build GCC PGO gotowy."


# =========================
# NORMAL BUILD
# =========================

build:
	@echo "Kompilowanie..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCES) -o $(TARGET).exe $(LIBS)


# =========================
# CLANG
# =========================

clang:
	@echo "Build CLANG..."

	$(CLANG) $(CLANG_FLAGS) \
	$(INCLUDE) \
	$(SOURCES) \
	-o $(TARGET).exe \
	$(CLANG_LIBS)

	@echo "Build CLANG gotowy."


# =========================
# CLANG PGO
# =========================

clang-generate:
	@mkdir -p profiles/clang

	$(CLANG) $(CLANG_FLAGS) \
	-fprofile-instr-generate=$(CLANG_PROFILE) \
	$(INCLUDE) \
	$(SOURCES) \
	-o $(TARGET).exe \
	$(CLANG_LIBS)

	@echo "Odpal app.exe."


clang-use:
	@echo "Tworzenie profilu LLVM..."

	llvm-profdata merge \
	-output=$(CLANG_PROFDATA) \
	$(CLANG_PROFILE)

	@echo "Budowanie CLANG PGO..."

	$(CLANG) $(CLANG_FLAGS) \
	-fprofile-instr-use=$(CLANG_PROFDATA) \
	$(INCLUDE) \
	$(SOURCES) \
	-o $(TARGET).exe \
	$(CLANG_LIBS)

	@echo "Build CLANG PGO gotowy."


# =========================
# CLEAN
# =========================

clean:
	@echo "Czyszczenie projektu..."
	rm -f $(TARGET).exe
	rm -rf profiles
	rm -f *.pdb
	find . -name "*.s" -type f -delete