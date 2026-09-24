// POC Horizon 2080 - Veille climatique multi-zones - NodeMCU ESP8266
// Un seul boitier, plusieurs zones du vaisseau : chaque zone a ses propres seuils.
// Si le DHT11 tombe en panne, la sonde analogique prend le relais sur la temperature.

#include <LiquidCrystal.h>
#include <DHT.h>

// ---- Broches (voir cablage) ----
const int PIN_DHT    = D4;   // DHT11, broche S
const int PIN_LED    = D0;   // LED rouge : allumee quand D0 = LOW
const int PIN_BUZZER = D8;   // buzzer, broche I/O
const int PIN_SONDE  = A0;   // sonde de secours (module rouge), broche AO
const int PIN_BOUTON = D9;   // = RX : fil touche a la masse = zone suivante

const bool INVERSER_SONDE = true;

// ---- Une zone = un nom et quatre seuils (temp min, temp max, hum min, hum max) ----
struct Zone {
  const char* nom;
  float tMin, tMax, hMin, hMax;
};

Zone zones[] = {
  {"SERRE ", 18, 28, 50, 80},   // les cultures : chaud et humide
  {"CABINE", 19, 26, 30, 60}    // l'equipage : confort et pas de condensation
};
const int NB_ZONES = sizeof(zones) / sizeof(zones[0]);  // calcule automatiquement
int zone = 0;

LiquidCrystal lcd(D1, D2, D5, D6, D7, D3);  // RS, E, DB4, DB5, DB6, DB7
DHT dht(PIN_DHT, DHT11);

float temp = NAN, hum = NAN, temp2 = NAN;
bool secours = false;
unsigned long derniereLecture = 0, dernierAppui = 0;

// Convertit la sonde analogique (thermistance CTN 10 kOhm) en degres Celsius
float lireSonde() {
  int brut = analogRead(PIN_SONDE);
  if (brut <= 0 || brut >= 1023) return NAN;
  float r = INVERSER_SONDE ? 10000.0 * (1023.0 - brut) / brut
                           : 10000.0 * brut / (1023.0 - brut);
  return 1.0 / (1.0 / 298.15 + log(r / 10000.0) / 3950.0) - 273.15;  // loi Beta
}

// Un seul message, du plus grave au moins grave
const char* etat() {
  Zone z = zones[zone];
  if (isnan(temp))    return "CAPTEUR HS";
  if (temp > z.tMax)  return "TROP CHAUD";
  if (temp < z.tMin)  return "TROP FROID";
  if (isnan(hum))     return "MODE SECOURS";
  if (hum > z.hMax)   return "TROP HUMIDE";
  if (hum < z.hMin)   return "TROP SEC";
  return "OK";
}

void afficher() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("%s %2.0fC", zones[zone].nom, temp);
  if (isnan(hum)) lcd.print(" --%");
  else            lcd.printf(" %2.0f%%", hum);
  lcd.setCursor(0, 1);
  lcd.print(etat());
  if (secours) lcd.print(" *");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BOUTON, INPUT_PULLUP);
  digitalWrite(PIN_LED, HIGH);     // LED eteinte
  digitalWrite(PIN_BUZZER, LOW);   // buzzer muet
  lcd.begin(16, 2);
  lcd.print("Demarrage...");
  dht.begin();
}

void loop() {
  // 1. Changer de zone : fil de RX touche a la masse
  if (digitalRead(PIN_BOUTON) == LOW && millis() - dernierAppui > 500) {
    dernierAppui = millis();
    zone = (zone + 1) % NB_ZONES;
    afficher();
  }

  // 2. Mesurer toutes les 2 secondes
  if (millis() - derniereLecture >= 2000) {
    derniereLecture = millis();
    temp = dht.readTemperature();
    hum  = dht.readHumidity();
    temp2 = lireSonde();

    // Capteur principal muet : on bascule sur la sonde de secours
    secours = false;
    if (isnan(temp) && !isnan(temp2)) {
      temp = temp2;
      secours = true;
    }

    afficher();
    Serial.printf("[%s] T=%.1f H=%.0f secours=%d etat=%s\n",
                  zones[zone].nom, temp, hum, secours, etat());
  }

  // 3. Alerte : LED des que ce n'est pas OK, buzzer seulement si une valeur sort des seuils
  const char* e = etat();
  bool alerte = strcmp(e, "OK") != 0;
  bool grave  = alerte && strcmp(e, "MODE SECOURS") != 0;
  digitalWrite(PIN_LED, alerte ? LOW : HIGH);
  digitalWrite(PIN_BUZZER, (grave && (millis() / 300) % 2 == 0) ? HIGH : LOW);
}
