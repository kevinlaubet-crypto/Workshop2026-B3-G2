# Veille climatique multi-zones du vaisseau

**Workshop national EPSI Bachelor 3 · Horizon 2080 · Septembre 2026**
Pilier 2 : FoodTech & AgriTech spatiale (sous-projet SpaceFarm)

Un boîtier autonome qui surveille la température et l'humidité d'une zone du vaisseau et alerte l'équipage sur place quand les valeurs sortent des seuils de cette zone. Aucun réseau n'est nécessaire.

## Ce que fait le prototype

- Mesure la température et l'humidité toutes les 2 secondes.
- Compare les mesures aux seuils de la zone active (SERRE ou CABINE).
- Alerte sur place : écran LCD, LED rouge et buzzer.
- Bascule sur une sonde de température de secours si le capteur principal tombe en panne, et l'affiche (mode dégradé).
- Change de zone par un contact entre la broche RX et la masse.

## Zones et seuils

| Zone   | Température | Humidité  |
|--------|-------------|-----------|
| SERRE  | 18 à 28 °C  | 50 à 80 % |
| CABINE | 19 à 26 °C  | 30 à 60 % |

Pour ajouter une zone (LABO, SOUTE…), il suffit d'ajouter une ligne dans le tableau `zones[]` du code.

## Matériel et câblage

| Composant                          | Broche de l'ESP8266                              |
|------------------------------------|--------------------------------------------------|
| Capteur DHT11 (broche S)           | D4                                               |
| Module thermistance (sortie AO)    | A0                                               |
| Écran LCD 1602, mode 4 bits        | RS D1 · E D2 · DB4 D5 · DB5 D6 · DB6 D7 · DB7 D3 |
| LED rouge + résistance 220 Ω       | D0 (allumée à l'état bas)                        |
| Buzzer actif                       | D8                                               |
| Fil de changement de zone          | RX (à toucher à la masse)                        |

Carte : NodeMCU ESP8266 (CH340). Une breadboard partage les rails 3,3 V, 5 V et masse.

## Installer et téléverser

1. Installer l'IDE Arduino.
2. Ajouter les cartes ESP8266 : *Fichier → Préférences → URL de gestionnaire de cartes* :
   `http://arduino.esp8266.com/stable/package_esp8266com_index.json`,
   puis *Outils → Carte → Gestionnaire de cartes* → installer **esp8266**.
3. Installer les bibliothèques (*Croquis → Inclure une bibliothèque → Gérer les bibliothèques*) :
   **LiquidCrystal** et **DHT sensor library** (Adafruit).
4. Ouvrir `serre_poc/serre_poc.ino`, choisir la carte **NodeMCU 1.0 (ESP-12E Module)** et le bon port, puis téléverser.
5. Moniteur série à **115200 bauds** pour voir la trace des mesures.

## Structure du dépôt

```
README.md               cette page
serre_poc/
  serre_poc.ino         code du prototype
```
