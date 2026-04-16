# TP Packet Tracer – Système distribué et tolérance aux fautes

Ce dépôt contient la topologie et les configurations minimales utilisées pour le TP « Système distribué et tolérance aux fautes avec Packet Tracer ». Le TP illustre :[file:1]

- la notion de système distribué  
- la redondance d’équipements réseau  
- la tolérance aux pannes  
- la continuité de service  
- le rôle de la redondance dans la cybersécurité[file:1]

## Topologie

Équipements utilisés :[file:1]

- 1 routeur ISR4331  
- 2 switchs 2960  
- 2 serveurs web  
- 2 PC clients[file:1]

Connexions principales :[file:1]

- PC1 → Switch1  
- PC2 → Switch2  
- Serveur1 → Switch1  
- Serveur2 → Switch2  
- Switch1 ↔ Routeur  
- Switch2 ↔ Routeur  
- Switch1 ↔ Switch2[file:1]

## Plan d’adressage

Réseau : `192.168.1.0/24`.[file:1]

- Routeur : `192.168.1.1`  
- Serveur 1 : `192.168.1.10`  
- Serveur 2 : `192.168.1.11`  
- PC1 : `192.168.1.100`  
- PC2 : `192.168.1.101`  
- Masque : `255.255.255.0`  
- Passerelle par défaut (PC/serveurs) : `192.168.1.1`[file:1]

## Contenu du dépôt

- `configs/router/routeur.config` : configuration minimale du routeur (interface LAN 192.168.1.1/24).  
- `configs/switches/switch1.config` : configuration minimale du Switch1 (ports vers PC1, Serveur1, Routeur, lien vers Switch2).  
- `config/switches/switch2.config` : configuration minimale du Switch2 (ports vers PC2, Serveur2, Routeur, lien vers Switch1).  
- `configs/hosts/pc1.config` / `configs/hosts/pc2.config` : paramètres IP à saisir dans Packet Tracer pour les PC.  
- `configs/servers/server1.config` / `configs/servers/server2.config` : paramètres IP et activation du service HTTP sur les serveurs à entrer dans Packet Tracer.  
- `docs/tp-packet-tracer.md` : réponses aux questions théoriques du TP.[file:1]

## Tests à réaliser

1. Depuis PC1, accéder à `http://192.168.1.10` (Serveur 1).  
2. Depuis PC2, accéder à `http://192.168.1.11` (Serveur 2).  
3. Simuler la panne d’un serveur (Power OFF) et vérifier que l’autre serveur reste accessible.  
4. Simuler la coupure d’un lien ou la panne d’un switch et vérifier que le service reste disponible grâce à la redondance.[file:1]