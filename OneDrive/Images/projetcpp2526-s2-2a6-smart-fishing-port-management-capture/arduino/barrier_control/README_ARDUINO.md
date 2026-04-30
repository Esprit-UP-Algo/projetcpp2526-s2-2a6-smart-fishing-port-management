Description

Ce dossier contient deux sketches Arduino pour le scénario barrière (carte UNO).

1) `barrier_input.ino` — Input
- Rôle : détecte la présence d'un bateau à l'aide d'un capteur ultrason HC-SR04.
- Broches : TRIG = D9, ECHO = D10, LED statut = D13
- Sortie : messages série `SHIP_DETECTED:<cm>` ou `NO_SHIP:<cm|TIMEOUT>`

2) `barrier_output.ino` — Output
- Rôle : contrôle une barrière (servo) sur commande série.
- Broches : SERVO = D11, LED statut = D13
- Commandes série : `OPEN`, `CLOSE`, `PING`
- Réponses : `BARRIER:OPEN`, `BARRIER:CLOSED`, `PONG`

Tests et validation

- Charger `barrier_input.ino` sur la UNO, ouvrir le Moniteur Série à 9600 bauds pour voir les détections.
- Charger `barrier_output.ino`, envoyer `OPEN` puis `CLOSE` depuis le Moniteur Série pour vérifier le servo.

Conseils git (exemples)

```bash
git add arduino/barrier_control/barrier_input.ino \
    arduino/barrier_control/barrier_output.ino \
    arduino/barrier_control/README_ARDUINO.md
git commit -m "AA4: ajouter sketches input/output pour la barrière + README"
git push origin capture
```

Remarques

- Utiliser une alimentation externe pour le servo si nécessaire (éviter d'alimenter le servo depuis l'USB si courant élevé).
- Adapter les angles du servo selon la mécanique de la barrière.
