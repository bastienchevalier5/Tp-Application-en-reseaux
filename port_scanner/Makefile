# Makefile for Port Scanner

CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2
LDFLAGS :=

# Détection du système d'exploitation
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Windows_NT)
    LDFLAGS += -lws2_32
    EXECUTABLE := port_scanner.exe
else
    EXECUTABLE := port_scanner
endif

TARGET := $(EXECUTABLE)
SOURCES := port_scanner.cpp
OBJECTS := $(SOURCES:.cpp=.o)

# Cibles par défaut
.PHONY: all build clean help

all: build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

help:
	@echo "Port Scanner Makefile"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  all     - Compile le port scanner (par défaut)"
	@echo "  build   - Compile le port scanner"
	@echo "  clean   - Supprime les fichiers compilés"
	@echo "  help    - Affiche cette aide"
	@echo ""
	@echo "Utilisation:"
	@echo "  make              # Compile"
	@echo "  make clean        # Nettoie"
	@echo "  make run          # Compile et exécute"

run: build
	./$(TARGET) 127.0.0.1 1 1024 200

run-help: build
	./$(TARGET)

.PHONY: run run-help
