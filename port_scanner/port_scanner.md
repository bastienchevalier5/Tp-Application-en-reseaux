# Port Scanner

## Description

Le **Port Scanner** est un utilitaire de réseau qui effectue une analyse TCP pour déterminer quels ports sont ouverts sur un hôte cible. Il utilise des sockets non-bloquants avec la fonction `select()` pour tester rapidement une plage de ports avec un timeout configurable.

## Caractéristiques

- **Compatible multi-plateforme** : Fonctionne sur Windows (Winsock2) et les systèmes Unix-like (Linux, macOS)
- **Sockets non-bloquants** : Évite les blocages lors de la connexion à des ports fermés
- **Timeout configurable** : Par défaut 200ms, peut être ajusté selon les besoins
- **Résolution de noms** : Supporte les noms d'hôtes et adresses IP
- **Rapide** : Teste les ports séquentiellement avec un timeout court

## Utilisation

### Syntaxe

```bash
port_scanner <host> [start_port] [end_port] [timeout_ms]
```

### Paramètres

| Paramètre | Description | Défaut | Obligatoire |
|-----------|-------------|--------|-------------|
| `<host>` | Adresse IP ou nom d'hôte cible | N/A | ✅ Oui |
| `start_port` | Numéro du premier port | 1 | Non |
| `end_port` | Numéro du dernier port | 1024 | Non |
| `timeout_ms` | Délai d'expiration en millisecondes | 200 | Non |

### Exemples

```bash
# Scanner les ports 1-1024 sur localhost avec timeout 200ms
./port_scanner 127.0.0.1

# Scanner les ports 80-8080 sur example.com avec timeout 500ms
./port_scanner example.com 80 8080 500

# Scanner les ports 22, 80, 443 sur 192.168.1.1
./port_scanner 192.168.1.1 22 443 200
```

## Fonctionnement technique

### 1. Initialisation des sockets

Le programme initialise d'abord la pile réseau (Winsock2 sur Windows) et valide les paramètres d'entrée.

### 2. Scan de chaque port

Pour chaque port dans la plage :

```
1. Créer un socket TCP
2. Mettre le socket en mode non-bloquant
3. Initier une connexion
4. Attendre la connexion avec timeout via select()
5. Vérifier le code d'erreur avec getsockopt()
6. Fermer le socket
```

### 3. Fonction `scanPort()`

Cette fonction effectue le scan d'un port unique :

- Résout le nom d'hôte en adresse IP avec `getaddrinfo()`
- Tente la connexion en mode non-bloquant
- Utilise `select()` pour monitorer le socket avec timeout
- Retourne `true` si le port est ouvert, `false` sinon

### 4. Fonction `waitForConnect()`

Utilise `select()` pour détecter :

- **writeSet** : Le socket est prêt à écrire (connexion réussie)
- **errorSet** : Une erreur s'est produite
- **timeout** : Le délai d'expiration est atteint

Vérifie ensuite avec `getsockopt(SO_ERROR)` pour confirmer le statut réel.

## Structure du code

### Fonctions principales

| Fonction | Rôle |
|----------|------|
| `main()` | Point d'entrée, parse les arguments, déclenche les scans |
| `scanPort()` | Scanne un port unique et retourne son statut |
| `waitForConnect()` | Utilise select() pour attendre avec timeout |
| `setNonBlocking()` | Configure le socket en mode non-bloquant |
| `closeSocket()` | Ferme un socket (compatible Windows/Unix) |
| `printUsage()` | Affiche l'aide d'utilisation |

### Sections compatibilité

Le code utilise des directives `#ifdef _WIN32` pour gérer les différences entre :

- **Windows** : `SOCKET`, `WSAStartup()`, `closesocket()`, `ioctlsocket()`, `WSAGetLastError()`
- **Unix/Linux** : `int socket`, `close()`, `fcntl()`, `errno`

## Sortie du programme

```
Scan de ports sur 192.168.1.1 de 1 à 1024 (timeout=200 ms)
Port 22 ouvert
Port 80 ouvert
Port 443 ouvert

Ports ouverts détectés : 22, 80, 443
```

Si aucun port n'est ouvert :
```
Aucun port ouvert détecté dans la plage.
```

## Points importants

⚠️ **Limitations et considérations**

- **Ordre séquentiel** : Les ports sont testés l'un après l'autre (pas de parallélisation)
- **Timeout court** : Un timeout très court peut donner des faux négatifs sur réseau lent
- **Permissions** : Nécessite les permissions de base pour créer des sockets
- **Firewall** : Un firewall peut bloquer les connexions, affectant les résultats
- **Légalité** : N'effectuer des scans que sur les systèmes auxquels vous avez accès

## Validation des entrées

Le programme valide automatiquement les paramètres :

```cpp
if (startPort < 1) startPort = 1;                  // Min: 1
if (endPort > 65535) endPort = 65535;              // Max: 65535
if (endPort < startPort) endPort = startPort;      // Correction si inversé
if (timeoutMs < 10) timeoutMs = 10;                // Min: 10ms
```

## Résumé

Le Port Scanner est un outil classique et efficace pour identifier les ports ouverts sur un hôte. Son implémentation utilise les meilleures pratiques de programmation réseau (sockets non-bloquants, select, timeout) et offre une bonne compatibilité multi-plateforme.
