# TP TCP Sliding Window

## Partie 1 — Observation du sliding window dans Wireshark

**Données :**
- Taille d'un segment TCP : **1460 octets**
- RTT moyen : **40 ms**
- Taille de la fenêtre TCP : **8760 octets**

### Questions & Réponses

**1. Combien de segments TCP peuvent être envoyés sans attendre d'acquittement ?**

> Nombre de segments = Taille de fenêtre ÷ Taille d'un segment
> = 8760 ÷ 1460 = **6 segments**

**2. Combien d'octets peuvent être envoyés sans attendre d'ACK ?**

> La taille de la fenêtre correspond exactement au nombre d'octets pouvant transiter sans ACK.
> **8760 octets**

**3. Combien d'acquittements sont nécessaires pour transmettre 43 800 octets ?**

> Nombre total de segments = 43 800 ÷ 1460 = **30 segments**
> Nombre d'ACK = 30 ÷ 6 = **5 acquittements**
> (un ACK est envoyé après chaque fenêtre complète reçue)

**4. Quelle est la relation entre la taille de la fenêtre et le nombre de segments envoyés ?**

> La fenêtre TCP détermine directement combien de segments peuvent être "en vol" simultanément. Plus la fenêtre est grande, plus on peut envoyer de segments sans attendre d'ACK, ce qui améliore l'utilisation du lien et donc le débit.

---

## Partie 2 — Numéros de séquence

**Numéros de séquence observés :**
```
1000 → 2460 → 3920 → 5380 → 6840
```

### Questions & Réponses

**1. Quelle est la taille d'un segment TCP dans cet exemple ?**

> Différence entre deux numéros consécutifs = 2460 − 1000 = **1460 octets**

**2. Combien d'octets sont transmis entre le premier et le dernier segment ?**

> Du numéro 1000 au numéro 6840, on a 4 intervalles de 1460 octets.
> 4 × 1460 = **5840 octets transmis**

**3. Quel sera le prochain numéro de séquence attendu ?**

> 6840 + 1460 = **8300**

**4. Que représente le numéro de séquence dans TCP ?**

> Le numéro de séquence représente le **numéro du premier octet** du segment dans le flux de données. Il permet au récepteur de reconstituer les données dans l'ordre et de détecter les doublons ou les pertes.

---

## Partie 3 — Calcul du débit théorique

**Données :**
- Taille de fenêtre : **64 KB**
- RTT : **50 ms**

**Formule :**
```
Débit = Taille_Fenêtre / RTT
```

### Questions & Réponses

**1. Convertir la taille de fenêtre en octets.**

> 64 KB = 64 × 1024 = **65 536 octets**

**2. Convertir le RTT en secondes.**

> 50 ms = 50 / 1000 = **0,05 s**

**3. Calculer le débit maximal théorique en octets par seconde.**

> Débit = 65 536 / 0,05 = **1 310 720 octets/s**

**4. Convertir le résultat en Ko/s.**

> 1 310 720 / 1024 = **1280 Ko/s**

**5. Convertir le résultat en Mb/s.**

> 1 310 720 × 8 = 10 485 760 bits/s = **≈ 10,49 Mb/s**

---

## Partie 4 — Influence du RTT

**Données :**
- Taille de fenêtre fixe : **65 535 octets**
- Formule : `Débit = 65 535 / RTT`

### Questions & Réponses

**1. Calculer le débit maximal si le RTT est de 20 ms.**

> Débit = 65 535 / 0,020 = **3 276 750 octets/s ≈ 3199,95 Ko/s ≈ 26,21 Mb/s**

**2. Calculer le débit maximal si le RTT est de 100 ms.**

> Débit = 65 535 / 0,100 = **655 350 octets/s ≈ 640 Ko/s ≈ 5,24 Mb/s**

**3. Calculer le débit maximal si le RTT est de 200 ms.**

> Débit = 65 535 / 0,200 = **327 675 octets/s ≈ 320 Ko/s ≈ 2,62 Mb/s**

**4. Que peut-on conclure concernant l'influence du RTT sur les performances TCP ?**

> Le RTT est **inversement proportionnel** au débit. Doubler le RTT divise le débit par deux. Un RTT élevé (latence importante, ex. liaison satellite) pénalise fortement les performances TCP même si la bande passante physique est grande. C'est une limite fondamentale du protocole TCP avec fenêtre fixe.

| RTT | Débit max |
|-----|-----------|
| 20 ms | ≈ 26,21 Mb/s |
| 100 ms | ≈ 5,24 Mb/s |
| 200 ms | ≈ 2,62 Mb/s |

---

## Partie 5 — Fenêtre glissante

**Données :**
- Taille de segment : **1000 octets**
- Taille de fenêtre : **5000 octets**

### Questions & Réponses

**1. Combien de segments peuvent être envoyés avant réception d'un ACK ?**

> 5000 / 1000 = **5 segments**

**2. Quels sont les numéros de séquence envoyés si le premier numéro est 0 ?**

> - Segment 1 : Seq = **0**
> - Segment 2 : Seq = **1000**
> - Segment 3 : Seq = **2000**
> - Segment 4 : Seq = **3000**
> - Segment 5 : Seq = **4000**

**3. Quel numéro d'acquittement sera envoyé après réception complète des données ?**

> ACK = dernier octet reçu + 1 = 4000 + 1000 = **ACK = 5000**
> (Le numéro d'ACK indique le prochain octet attendu.)

**4. Que devient la fenêtre après réception de l'ACK ?**

> La fenêtre **glisse** vers l'avant : l'émetteur peut envoyer 5 nouveaux segments à partir du numéro de séquence 5000. Le mécanisme de sliding window permet ainsi un flux continu de données sans bloquer l'émetteur.

---

## Partie 6 — Analyse d'une capture Wireshark

**Extrait de capture :**
```
Seq = 0
Seq = 1460
Seq = 2920
Seq = 4380
Ack = 5840
Window Size = 5840
```

### Questions & Réponses

**1. Combien de segments ont été envoyés ?**

> 4 segments (Seq = 0, 1460, 2920, 4380)

**2. Combien d'octets ont été transmis ?**

> 4 × 1460 = **5840 octets**

**3. Pourquoi l'ACK indique 5840 ?**

> L'ACK = 5840 signifie que le récepteur a bien reçu tous les octets jusqu'à 5839 et **attend le prochain octet à partir de 5840**. C'est la confirmation de la réception correcte des 4 segments.

**4. Quelle est la taille de la fenêtre annoncée par le récepteur ?**

> **5840 octets** (Window Size = 5840), ce qui correspond exactement aux 4 segments reçus.

**5. Que signifie une diminution de la taille de la fenêtre ?**

> Une fenêtre qui rétrécit indique que le **tampon de réception du destinataire se remplit** (le récepteur traite les données moins vite qu'il ne les reçoit). Si la fenêtre atteint 0, l'émetteur doit s'arrêter d'envoyer jusqu'à nouvel ordre. C'est le mécanisme de **contrôle de flux** TCP.

---

## Partie 7 — Débit réel

**Données :**
- Taille de fenêtre : **12 000 octets**
- RTT : **60 ms**

### Questions & Réponses

**1. Calculer le débit maximal théorique.**

> Débit théorique = 12 000 / 0,060 = **200 000 octets/s = 195,3 Ko/s ≈ 1,6 Mb/s**

**2. Si seulement 8000 octets sont transmis par RTT, quel est le débit réel ?**

> Débit réel = 8000 / 0,060 = **133 333 octets/s ≈ 130,2 Ko/s ≈ 1,07 Mb/s**

**3. Comparer débit réel et débit théorique.**

> Le débit réel représente environ **66,7 %** du débit théorique. La fenêtre n'est pas pleinement utilisée.

**4. Proposer une explication possible à la différence observée.**

> Plusieurs causes possibles :
> - **Congestion réseau** : TCP a réduit sa fenêtre via le contrôle de congestion (slow start, AIMD)
> - **Tampon récepteur insuffisant** : le récepteur a annoncé une fenêtre réduite
> - **Pertes de paquets** : des retransmissions ont réduit le débit effectif
> - **Limitation applicative** : l'application ne génère pas assez de données à envoyer

---

## Partie 8 — Synthèse

### Questions & Réponses

**1. Quel est le rôle principal du sliding window ?**

> Le sliding window permet d'envoyer **plusieurs segments en parallèle** sans attendre un ACK pour chacun. Il optimise l'utilisation du lien réseau en évitant les temps morts liés à la latence (RTT).

**2. Pourquoi TCP n'envoie-t-il pas les données une par une ?**

> Envoyer un segment et attendre son ACK avant d'en envoyer un autre (stop-and-wait) est très inefficace : pendant le RTT, le lien est inutilisé. Avec une fenêtre de N segments, on peut utiliser le lien N fois plus efficacement.

**3. Quel est l'impact d'une petite fenêtre TCP ?**

> Une petite fenêtre limite le nombre de segments en transit. Le débit maximal est réduit car l'émetteur est souvent en attente d'ACK. Cela est particulièrement pénalisant sur des liens à fort RTT.

**4. Quel est l'impact d'un RTT élevé ?**

> Un RTT élevé réduit directement le débit (Débit = Fenêtre / RTT). Pour maintenir un bon débit, il faut augmenter la taille de la fenêtre proportionnellement au RTT (notion de **BDP** : Bandwidth-Delay Product).

**5. Pourquoi le mécanisme de sliding window améliore-t-il les performances ?**

> Il permet de **"remplir le tuyau"** réseau en ayant constamment des données en transit. Sans ce mécanisme, chaque RTT ne permettrait d'envoyer qu'un seul segment. Avec une fenêtre de W octets, le débit théorique est multiplié par W / (taille d'un segment), ce qui peut représenter un gain très significatif.

---

*TP réalisé dans le cadre du module RSX102 — Technologies pour les applications en réseau*
