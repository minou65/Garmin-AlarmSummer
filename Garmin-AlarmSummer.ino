/*
  Projekt:   Garmin-AlarmSummer.ino
  Autor:     andy
  Erstellt:  22.08.2025

  Beschreibung:
    Dieser Sketch realisiert eine Alarm- und Quittierungslogik für ein Arduino-basiertes System.
    Ein externer Alarm (GarminAlert) wird optisch (LED) und akustisch (Piezo-Buzzer) signalisiert.
    Die Quittierung erfolgt über einen Taster (InputButton).

  Verhalten:
    - Liegt ein Alarm an (GarminAlert LOW) und wurde noch nicht quittiert:
        * LED und Summer blinken im Sekundentakt.
    - Wird der Taster gedrückt (Quittierung):
        * Summer bleibt aus, LED leuchtet dauerhaft.
    - Ist kein Alarm mehr aktiv (GarminAlert HIGH):
        * LED und Summer sind aus, Quittierung wird zurückgesetzt.

  Technische Hinweise:
    - Beide Eingänge werden per Software entprellt (mittels Neotimer).
    - Die NeoTimer-Library steuert das Blinken und Piepen im Sekundentakt.
    - Die Eingänge sind mit INPUT_PULLUP konfiguriert, d.h. sie sind im Ruhezustand HIGH und werden bei Aktivierung auf LOW gezogen.

  Pinbelegung:
    - PIN_GARMIN_ALERT   (Eingang):  Alarm-Signal (LOW = aktiv, HIGH = inaktiv)
    - PIN_INPUT_BUTTON   (Eingang):  Quittiertaster (LOW = gedrückt, HIGH = offen)
    - PIN_LED_BUTTON     (Ausgang):  LED-Anzeige
    - PIN_PIEZO_BUZZER   (Ausgang):  Piezo-Buzzer
*/

#include "neotimer.h"

// Pin-Definitionen
const int PIN_GARMIN_ALERT = 2;     // Eingang: GarminAlert
const int PIN_INPUT_BUTTON = 3;     // Eingang: InputButton
const int PIN_LED_BUTTON = 4;    // Ausgang: LEDButton
const int PIN_PIEZO_BUZZER = 5;    // Ausgang: PiezoBuzzer

Neotimer secondTimer(1000); // 1 Sekunde
Neotimer alertDebouncer(50);
Neotimer buttonDebouncer(50);

bool acknowledged = false; // Quittierungs-Status global

void setup() {
    pinMode(PIN_GARMIN_ALERT, INPUT_PULLUP);
    pinMode(PIN_INPUT_BUTTON, INPUT_PULLUP);
	pinMode(PIN_LED_BUTTON, OUTPUT);
	pinMode(PIN_PIEZO_BUZZER, OUTPUT);

	digitalWrite(PIN_LED_BUTTON, LOW);
	digitalWrite(PIN_PIEZO_BUZZER, LOW);
}

void loop() {

    alertDebouncer.debounce((digitalRead(PIN_GARMIN_ALERT) == LOW));
    buttonDebouncer.debounce((digitalRead(PIN_INPUT_BUTTON) == LOW));

    if (alertDebouncer.getDebouncedState()) {
        // Quittierung merken, sobald der Button gedrückt wird
        if (buttonDebouncer.getDebouncedState()) {
            acknowledged = true;
        }

        if (acknowledged) {
            // Summer aus, LED dauerhaft an
            digitalWrite(PIN_PIEZO_BUZZER, LOW);
            digitalWrite(PIN_LED_BUTTON, HIGH);
        } else {
            // Summer im Sekundentakt, LED blinkt im Sekundentakt
            if (secondTimer.repeat()) {
                digitalWrite(PIN_LED_BUTTON, !digitalRead(PIN_LED_BUTTON));
                digitalWrite(PIN_PIEZO_BUZZER, !digitalRead(PIN_PIEZO_BUZZER));
            }
        }
    } else {
        // Alles zurücksetzen
        digitalWrite(PIN_LED_BUTTON, LOW);
        digitalWrite(PIN_PIEZO_BUZZER, LOW);
        secondTimer.restart();
        acknowledged = false;
    }
}
