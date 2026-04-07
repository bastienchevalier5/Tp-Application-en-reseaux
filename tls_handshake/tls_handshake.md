# Handshake TLS (Simulation simplifiée)

## Objectif
Ce document explique en détail le handshake TLS simulé par `tls_handshake.cpp`.

## Contexte
Le handshake TLS permet d’établir une connexion chiffrée entre un client et un serveur.
La simulation est volontairement simplifiée pour mettre en évidence les messages principaux.

## État initial
Les deux côtés commencent dans l’état `IDLE`.

## Étape 1 : ClientHello
Fonction : `TLSClient::sendClientHello()`

- Le client envoie un message `ClientHello`.
- Ce message inclut généralement :
  - les versions TLS supportées,
  - la liste des suites de chiffrement,
  - un nombre aléatoire (`random`).
- Le client passe à l’état `CLIENT_HELLO_SENT`.
- Message affiché :
  - `Client ──► Serveur  ClientHello (cipher suites, random)`

Signification : le client annonce ses préférences et démarre le handshake.

## Étape 2 : ServerHello + Certificate
Fonction : `TLSServer::receiveClientHello(const std::string& message)`

- Le serveur reçoit le `ClientHello`.
- Il répond avec :
  - `ServerHello` : choix de la suite de chiffrement et un nombre aléatoire serveur.
  - `Certificate` : le certificat du serveur pour prouver son identité.
- Le serveur passe à l’état `SERVER_HELLO_SENT`.
- Messages affichés :
  - `Serveur ──► Client  ServerHello (choix de chiffre, random)`
  - `Serveur ──► Client  Certificate (certificat du serveur)`

Signification : le serveur choisit les paramètres de sécurité et s’authentifie.

## Étape 3 : ClientKeyExchange
Fonction : `TLSClient::sendClientKeyExchange()`

- Le client commence la phase de génération de clé.
- Il envoie `ClientKeyExchange` contenant une valeur chiffrée qui permet de dériver la clé de session.
- Dans la simulation, l’état est forcé à `CERTIFICATE_SENT` après réception du certificat pour simplifier le flux.
- Le client passe à l’état `CLIENT_KEY_EXCHANGED`.
- Message affiché :
  - `Client ──► Serveur  ClientKeyExchange (pre-master secret)`

Signification : le client transmet la clé secrète partagée nécessaire pour générer les clés de chiffrement.

## Étape 4 : ChangeCipherSpec + Finished
Fonction : `TLSServer::receiveClientKeyExchange(const std::string& message)`

- Le serveur reçoit le `ClientKeyExchange`.
- Il envoie ensuite :
  - `ChangeCipherSpec` : indique que les messages suivants seront chiffrés.
  - `Finished` : message final confirmant que le handshake a réussi.
- Le serveur passe à l’état `ESTABLISHED`.
- Messages affichés :
  - `Serveur ──► Client  ChangeCipherSpec`
  - `Serveur ──► Client  Finished`

Signification : le serveur termine le handshake et bascule vers les communications chiffrées.

## Résultat
Après ces étapes :

- `Client` et `Serveur` sont en état `ESTABLISHED`.
- Le canal est prêt à échanger des données chiffrées.

## Simplifications de la simulation

Cette version ne couvre pas :

- la vérification détaillée du certificat,
- la génération réelle de clés symétriques,
- les messages `ServerHelloDone`, `Finished` côté client,
- la dérivation exacte des secrets.

Elle illustre cependant le flux général de l’établissement TLS :

1. le client propose,
2. le serveur répond et s’authentifie,
3. le client partage le secret,
4. le serveur confirme le chiffrement.
