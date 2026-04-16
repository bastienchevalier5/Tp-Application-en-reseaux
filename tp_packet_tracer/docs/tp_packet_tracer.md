# TP : Système distribué et tolérance aux fautes

## Questions

### 1. Qu’est-ce qu’un système distribué ?

Un **système** distribué est un ensemble de machines ou de composants logiciels, reliés par un réseau, qui coopèrent pour fournir un service unique perçu comme cohérent par les utilisateurs.[file:1]

### 2. Pourquoi utiliser plusieurs serveurs ?

Plusieurs serveurs permettent d’assurer la continuité du service en cas de panne, de répartir la charge entre plusieurs machines et de faciliter la maintenance sans interrompre totalement le service.[file:1]

### 3. Qu’est-ce qu’un SPOF (Single Point Of Failure) ?

Un SPOF est un composant unique dont la panne provoque l’arrêt complet du service ou du système (exemple : un seul switch ou un seul serveur sans redondance).[file:1]

### 4. Pourquoi la redondance améliore la cybersécurité ?

La redondance améliore la cybersécurité car elle augmente la disponibilité du service même en cas d’attaque ou d’incident, en limitant l’impact d’une panne ou d’un équipement compromis.[file:1]

### 5. Que se passe-t-il si un switch tombe en panne ?

Grâce à la redondance (deux switchs reliés entre eux et au routeur), le trafic peut passer par l’autre switch et le service web reste accessible tant qu’un chemin fonctionnel existe.[file:1]

### 6. Quel est le lien entre haute disponibilité et cybersécurité ?

La haute disponibilité et la cybersécurité partagent l’objectif de maintenir la disponibilité du service ; une architecture redondante limite l’impact des pannes et des attaques sur la continuité de service.[file:1]

## Bonus

Ajouter un troisième serveur et un deuxième lien entre les switchs rend le réseau plus robuste et augmente la tolérance aux fautes en réduisant encore les points de défaillance uniques.[file:1]