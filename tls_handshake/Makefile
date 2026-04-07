# Makefile for TLS Handshake

CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2
LDFLAGS :=

# Détection du système d'exploitation
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Windows_NT)
    LDFLAGS += -lws2_32
    EXECUTABLE := tls_handshake.exe
else
    EXECUTABLE := tls_handshake
endif

TARGET := $(EXECUTABLE)
SOURCES := tls_handshake.cpp
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
	@echo "TLS Handshake Makefile"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  all     - Compile le programme TLS handshake (par défaut)"
	@echo "  build   - Compile le programme TLS handshake"
	@echo "  clean   - Supprime les fichiers compilés"
	@echo "  help    - Affiche cette aide"
	@echo ""
	@echo "Utilisation:"
	@echo "  make              # Compile"
	@echo "  make clean        # Nettoie"
	@echo "  make run          # Compile et exécute"

run: build
	./$(TARGET)

.PHONY: run
