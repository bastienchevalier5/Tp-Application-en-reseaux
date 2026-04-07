# Handshake TCP (Three-Way Handshake)

## Objectif
Ce document explique en détail le handshake TCP simulé par `handshake.cpp`.

## Contexte
Le handshake TCP sert à établir une connexion fiable entre deux entités : un client et un serveur.
La simulation utilise trois états principaux :

- `CLOSED` : pas de connexion
- `SYN_SENT` / `SYN_RECEIVED` : début du handshake
- `ESTABLISHED` : connexion établie

## Étape 1 : Client envoie SYN
Fonction : `Client::sendSYN()`

- Le client est initialement dans l’état `CLOSED`.
- Il crée un segment TCP avec le drapeau `SYN = true` et un numéro de séquence `seq = isn`.
- Il passe à l’état `SYN_SENT`.
- Message envoyé :
  - `Client ──► Serveur [SYN | seq=100 ack=0]`

Signification : le client demande l’ouverture d’une connexion et envoie son numéro de séquence initial.

## Étape 2 : Serveur reçoit SYN et renvoie SYN-ACK
Fonction : `Server::receiveSYN(const TCPSegment& syn)`

- Le serveur prépare un segment de réponse après avoir reçu le `SYN`.
- Il vérifie que le message contient bien `SYN`.
- Il active `SYN = true` et `ACK = true`.
- Il fixe `seq = isn` du serveur et `ack = syn.seq + 1`.
- Il passe à l’état `SYN_RECEIVED`.
- Message envoyé :
  - `Serveur ──► Client [SYN ACK | seq=300 ack=101]`

Signification : le serveur accepte la demande de connexion et confirme la réception du `SYN` du client.

## Étape 3 : Client reçoit SYN-ACK et envoie ACK
Fonction : `Client::receiveSYNACK(const TCPSegment& synack)`

- Le client vérifie que le serveur a bien envoyé `SYN+ACK`.
- Il vérifie que `synack.ack == isn + 1`.
- Il crée un segment avec `ACK = true`, `seq = isn + 1` et `ack = synack.seq + 1`.
- Il passe à l’état `ESTABLISHED`.
- Message envoyé :
  - `Client ──► Serveur [ACK | seq=101 ack=301]`

Signification : le client confirme que le serveur est prêt et finalise l’établissement de la connexion.

## Étape finale : Serveur confirme le ACK
Fonction : `Server::receiveACK(const TCPSegment& ack_seg)`

- Le serveur vérifie que le segment contient bien `ACK`.
- Il vérifie que `ack_seg.ack == isn + 1` pour le serveur.
- Il passe alors à l’état `ESTABLISHED`.

## Résultat
À la fin du handshake :

- `Client` est en état `ESTABLISHED`
- `Serveur` est en état `ESTABLISHED`

La connexion est maintenant prête pour l’échange de données fiables.

## Notes sur les numéros de séquence

- `seq` est le numéro de séquence du segment envoyé.
- `ack` est le numéro d’octet attendu par le récepteur.
- Après un `SYN`, le prochain numéro attendu est `seq + 1`.

## Pourquoi trois étapes ?

- Le client indique sa volonté de se connecter (`SYN`).
- Le serveur répond et donne sa propre initialisation (`SYN-ACK`).
- Le client confirme la réception et montre qu’il est prêt (`ACK`).

Ce mécanisme assure la synchronisation des numéros de séquence et la fiabilité de la connexion.
