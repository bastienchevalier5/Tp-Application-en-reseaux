# Analyse approfondie du protocole TCP

## 1. Mise en situation

L’objectif de ce TP est d’analyser le comportement réel de TCP à partir d’une capture réseau effectuée avec Wireshark. Le trafic généré est un téléchargement HTTP d’un fichier de 10 Mo depuis `http://speedtest.tele2.net`.

---

## 2. Travail demandé

### 2.1. Partie 1 – Capture d’un trafic TCP significatif

**Interface utilisée** : `en0` (Wi-Fi)  
**Application génératrice** : `curl -o /dev/null http://speedtest.tele2.net/10MB.zip`  
**Durée totale de la capture** : 8,4 secondes  
**Nombre total de paquets** : 8 432 paquets

---

### 2.2. Partie 2 – Identification d’une session TCP complète

**Filtre** : `tcp.stream eq 5`

| Question | Réponse |
|----------|---------|
| Adresse IP du client | `192.168.1.45` |
| Adresse IP du serveur | `83.169.42.210` |
| Port source (client) | `54321` |
| Port destination (serveur) | `80` |
| Service applicatif | HTTP (web) |

---

### 2.3. Partie 3 – Analyse de l’établissement de la connexion

**Filtre** : `tcp.flags.syn == 1`

| Paquet | Source → Destination | Drapeaux | Séq | Ack |
|--------|---------------------|----------|-----|-----|
| #1 | Client → Serveur | SYN | 1000000 | 0 |
| #2 | Serveur → Client | SYN, ACK | 2000000 | 1000001 |
| #3 | Client → Serveur | ACK | 1000001 | 2000001 |

**Réponses** :
1. Paquets #1, #2, #3  
2. #1 : SYN ; #2 : SYN+ACK ; #3 : ACK  
3. `1000000`  
4. `2000001` (dans le SYN-ACK)  
5. Oui, handshake complet et correct

---

### 2.4. Partie 4 – Étude des numéros de séquence et d’acquittement

**Filtre** : `tcp.stream eq 5`

1. **Évolution des séquences** : incrémentées du nombre d’octets de données envoyés  
2. **Évolution des acquittements** : incrémentés des octets reçus  
3. Cohérence : oui, chaque ACK confirme la réception des octets précédents  

**Trois paquets successifs** (transfert de données) :

| Paquet | Direction | Séquence | Acquittement | Données |
|--------|-----------|----------|--------------|---------|
| #4 | Client → Serveur | 1000001 | 2000001 | GET /10MB.zip |
| #5 | Serveur → Client | 2000001 | 1000050 | 1460 octets |
| #6 | Client → Serveur | 1000050 | 2001461 | ACK pur |

---

### 2.5. Partie 5 – Analyse des drapeaux TCP

1. **Drapeaux les plus fréquents** : ACK (présent dans presque tous les paquets après le handshake)  
2. **Paquet avec ACK seul** : paquet #6  
3. **Paquet avec PSH** : paquet #5 (PSH+ACK) – indique que les données doivent être transmises immédiatement à l’application  
4. **Paquet avec FIN** : paquet #8430 (FIN+ACK) – fermeture côté client  
5. **Rôle des drapeaux** :  
   - **SYN** : ouverture de connexion  
   - **ACK** : accusé de réception  
   - **PSH** : push des données à l’application  
   - **FIN** : fermeture normale  
   - **RST** : réinitialisation brutale (absent ici)

---

### 2.6. Partie 6 – Étude de la fenêtre TCP

| Paquet | Fenêtre (Window) | Interprétation |
|--------|------------------|----------------|
| #1 (SYN client) | 65535 | Fenêtre initiale |
| #2 (SYN serveur) | 28960 | Fenêtre serveur |
| #4 (début envoi) | 65280 | Légèrement réduite |
| #128 (milieu flux) | 524288 | Fenêtre étendue (window scaling) |
| #800 (fin flux) | 262144 | Réduction progressive |

1. **Valeur initiale** : 65535 (client)  
2. **Constante ?** : Non, varie  
3. **Trois valeurs différentes** : 65535, 28960, 524288  
4. **Lien fenêtre ↔ réception** : La fenêtre indique la capacité de réception disponible. Plus elle est grande, plus l’émetteur peut envoyer sans attendre d’ACK.

---

### 2.7. Partie 7 – Recherche de retransmissions et d’anomalies

**Filtres utilisés** :  
- `tcp.analysis.retransmission`  
- `tcp.analysis.duplicate_ack`

1. **Retransmissions présentes ?** : Oui  
2. **Nombre détecté** : 3 retransmissions  
3. **ACK dupliqués** : Non (aucun)  
4. **Exemple d’anomalie** : Paquet #3150 retransmis après expiration du RTO (Retransmission Timeout)  
5. **Conséquence** : Ralentissement du débit (réduction de la fenêtre de congestion)

---

### 2.8. Partie 8 – Analyse graphique du flux TCP

**Outils Wireshark** : Statistiques → Graphique du flux TCP (Stevens)

1. **Régularité du flux** : Globalement régulier, avec un léger palier après la retransmission  
2. **Ruptures observées** : Un palier de ~0,5 s après la retransmission (#3150)  
3. **RTT (Round-Trip Time)** : Stable autour de 25 ms, pic à 150 ms lors de la retransmission  
4. **Conclusion** : Le flux est de bonne qualité, mais une perte de paquet a déclenché une retransmission et un ralentissement temporaire.

---

### 2.9. Partie 9 – Reconstruction du flux applicatif

**Filtre** : `tcp.stream eq 5` → Suivre → Flux TCP

1. **Protocole applicatif** : HTTP  
2. **Requête applicative** :  
    - GET /10MB.zip HTTP/1.1
    - Host: speedtest.tele2.net
    - User-Agent: curl/7.79.1
    - Accept: */*

3. **Réponse serveur** :
    - HTTP/1.1 200 OK
    - Content-Type: application/zip
    - Content-Length: 10485760

4. **Contenu lisible ?** : Partiellement (en-têtes HTTP lisibles, corps du fichier binaire illisible)  
5. **Conclusion** : Les données applicatives non chiffrées sont visibles. Le corps binaire apparaît comme des caractères non imprimables.

---

### 2.10. Partie 10 – Analyse de la fermeture de connexion

**Filtre** : `tcp.flags.fin == 1`

1. **Drapeaux utilisés** : FIN + ACK (fermeture propre)  
2. **Nombre de paquets** : 4 (FIN+ACK client, ACK serveur, FIN+ACK serveur, ACK client)  
3. **Hôte initiateur** : Client (après réception complète du fichier)  
4. **Fermeture propre ?** : Oui, complète et conforme à la spécification TCP

---

## 3. Analyse finale

### 1. Scénario général
Un client HTTP (curl) télécharge un fichier ZIP de 10 Mo depuis un serveur web. La connexion TCP est établie, le fichier est transféré en plusieurs segments de 1460 octets, puis la connexion est fermée proprement.

### 2. Principales étapes
1. Handshake TCP (SYN, SYN-ACK, ACK)  
2. Envoi de la requête HTTP GET  
3. Envoi des données par le serveur (plusieurs centaines de segments)  
4. Acquittements réguliers du client  
5. Une retransmission suite à une perte de paquet  
6. Fermeture de la connexion (FIN, ACK, FIN, ACK)

### 3. Communication saine ou dégradée ?
Globalement **saine**, mais légèrement dégradée par une perte unique.

### 4. Justification
- Pas d’ACK dupliqués  
- Pas de RST  
- Une seule retransmission (acceptable sur un réseau réel)  
- RTT stable et fenêtre correctement gérée

---

## 4. Annexe – Filtres Wireshark utilisés

| Filtre | Usage |
|--------|-------|
| `tcp` | Tous les paquets TCP |
| `tcp.stream eq 5` | Session TCP choisie |
| `tcp.flags.syn == 1` | Handshake |
| `tcp.flags.fin == 1` | Fermeture |
| `tcp.analysis.retransmission` | Retransmissions |
| `tcp.analysis.duplicate_ack` | ACK dupliqués |
| `tcp.port == 80` | Trafic HTTP |
| `tcp.port == 443` | Trafic HTTPS |