#include "PowerManager.h"

PowerManager::PowerManager() {
    // Constructor sin dependencias externas
}

void PowerManager::begin() {
    // Configurar pines como salidas
    pinMode(POWER_3V3_PIN, OUTPUT);
    pinMode(POWER_12V_PIN, OUTPUT);
    
    // Asegurar que todas las fuentes están apagadas al inicio
    allPowerOff();
}

void PowerManager::power3V3On() {
    digitalWrite(POWER_3V3_PIN, LOW);
    delay(POWER_STABILIZE_DELAY);
}

void PowerManager::power3V3Off() {
    digitalWrite(POWER_3V3_PIN, HIGH);
}

void PowerManager::power12VOn() {
    digitalWrite(POWER_12V_PIN, HIGH);
    delay(POWER_STABILIZE_DELAY);
}

void PowerManager::power12VOff() {
    digitalWrite(POWER_12V_PIN, LOW);
}

void PowerManager::allPowerOff() {
    power3V3Off();
    power12VOff();
}