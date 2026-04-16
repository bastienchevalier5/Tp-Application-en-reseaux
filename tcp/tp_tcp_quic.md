# TP Comparaison TCP vs QUIC (HTTP/3)

## 1. Capture HTTP sur TCP

**Filtre utilisé** : `tcp.port == 443`  
**Site** : `https://google.fr`

> Simulation de capture : échange HTTP/2 sur TLS (TCP).

### Questions

1. **Combien de paquets TCP sont observés ?**  
   → Environ 145 paquets TCP (dont ACK, PSH, SYN, FIN).

2. **Adresse IP du serveur**  
   → `142.250.185.35` (Google)

3. **Port TCP utilisé**  
   → 443 (HTTPS)

4. **Nombre de connexions TCP visibles**  
   → 1 connexion persistante (HTTP/2)

5. **Handshake TCP visible ?**  
   → Oui (paquets #1, #2, #3)

---

## 2. Analyse du handshake TCP

**Filtre** : `tcp.flags.syn == 1`

| Paquet | Source | Drapeaux |
|--------|--------|----------|
| #1     | Client | SYN      |
| #2     | Serveur| SYN-ACK  |
| #3     | Client | ACK      |

### Questions

1. **Nombre de paquets du handshake TCP** → 3  
2. **Drapeaux activés** → SYN, SYN+ACK, ACK  
3. **Numéro de séquence initial du client** → 123456789 (exemple)  
4. **Acquittement envoyé par le serveur** → 123456790 (SYN-ACK)  
5. **Temps d'établissement** → ~15 ms

---

## 3. Analyse du transport HTTP sur TCP

**Filtre** : `tls or http`

1. **Protocole applicatif** → HTTP/2 (via TLS)  
2. **Requête applicative** → `GET /` (visible après déchiffrement)  
3. **Réponse serveur** → `200 OK`  
4. **Contenu lisible ?** → Non, chiffré (TLS)  
5. **Trafic chiffré ?** → Oui (TLS 1.3)

---

## 4. Capture HTTP/3 sur QUIC

**Filtre** : `udp.port == 443`  
**Site** : `https://google.fr` (support HTTP/3)

> Simulation : HTTP/3 sur QUIC.

### Questions

1. **Paquets UDP observés** → ~120 paquets  
2. **Port UDP utilisé** → 443  
3. **Paquets QUIC identifiés ?** → Oui (protocole QUIC dans Wireshark)  
4. **Nombre de flux QUIC** → 5 flux (1 initial, 3 data, 1 control)  
5. **Adresse IP serveur** → `142.250.185.35` (identique)

---

## 5. Analyse du handshake QUIC

**Filtre** : `quic`

1. **Paquets nécessaires** → 2 (Initial + Handshake)  
2. **Handshake similaire à TCP ?** → Non, intégré au chiffrement  
3. **Chiffrement dès le début ?** → Oui (paquets chiffrés immédiatement)  
4. **Infos applicatives en clair ?** → Très peu (DCID, SCID, version)  
5. **Différence principale** → Pas de handshake séparé ; crypto + transport combinés

---

## 6. Comparaison des protocoles de transport

| Critère | TCP | QUIC |
|---------|-----|------|
| Protocole sous-jacent | TCP | UDP |
| Rapidité d'établissement | 3 paquets | 2 paquets |
| Paquets nécessaires | +1 RTT | 0‑RTT possible |
| Intégration chiffrement | TLS ajouté | Intégré nativement |
| Analyse Wireshark | Facile (décodage TLS possible) | Plus difficile (chiffré par défaut) |

---

## 7. Analyse des performances observables

1. **Connexion plus courte avec QUIC** → Oui (0‑RTT)  
2. **Nombre total de paquets** → QUIC légèrement inférieur  
3. **Retransmissions TCP** → 2 observées (perte simulée)  
4. **Pertes UDP** → 0 (mais QUIC gère ses propres retransmissions)  
5. **Protocole le plus efficace** → QUIC (moins de RTT)

---

## 8. Analyse du chiffrement

1. **Contenu TCP lisible ?** → Non (TLS)  
2. **Contenu QUIC lisible ?** → Non (chiffré dès le début)  
3. **Protection la plus rapide** → QUIC (dès le 1er paquet)  
4. **Visibilité contenu** → TCP : visible après déchiffrement ; QUIC : plus opaque

---

## 9. Synthèse technique

1. **Différences principales**  
   - TCP : fiable, séquentiel, TLS ajouté  
   - QUIC : fiable sur UDP, chiffrement intégré, multiplexage sans blocage

2. **Protocole le plus moderne** → QUIC

3. **Le plus performant** → QUIC (0‑RTT, moins de latence)

4. **Le plus sécurisé** → QUIC (chiffrement obligatoire)

5. **TCP reste pertinent pour**  
   - Réseaux filtrant UDP  
   - Infrastructures anciennes  
   - Debugging réseau simple

6. **QUIC préférable pour**  
   - Web moderne (HTTP/3)  
   - Vidéo, mobile, réseaux instables  
   - Réduction de latence

---

## 10. Filtres Wireshark utilisés

| Filtre | Usage |
|--------|-------|
| `tcp` | Trafic TCP |
| `udp` | Trafic UDP |
| `quic` | Trafic QUIC |
| `tcp.port == 443` | HTTPS/TCP |
| `udp.port == 443` | HTTP/3/QUIC |
| `tls` | Handshake TLS |
| `http` | HTTP non chiffré |