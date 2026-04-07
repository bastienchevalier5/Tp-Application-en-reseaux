# Makefile for WebSocket Project

.PHONY: all install clean help run dev

# Variables
NODE := node
NPM := npm
PORT ?= 3000

all: install

install:
	$(NPM) install

clean:
	rm -rf node_modules
	rm -f package-lock.json

help:
	@echo "WebSocket Project Makefile"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  all       - Installe les dépendances (par défaut)"
	@echo "  install   - Installe les dépendances npm"
	@echo "  run       - Démarre le serveur WebSocket"
	@echo "  dev       - Démarre en mode développement"
	@echo "  clean     - Supprime node_modules et package-lock.json"
	@echo "  help      - Affiche cette aide"
	@echo ""
	@echo "Utilisation:"
	@echo "  make                    # Installe les dépendances"
	@echo "  make run                # Lance le serveur (PORT=3000 par défaut)"
	@echo "  make run PORT=8080      # Lance sur le port 8080"
	@echo "  make clean              # Nettoie"

run: install
	$(NODE) index.js

dev: install
	$(NPM) run dev

outdated:
	$(NPM) outdated

update:
	$(NPM) update

list:
	$(NPM) list
