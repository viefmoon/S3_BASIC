#include "sensors/BatterySensor.h"
#include "config.h"

/**
 * @brief Lee el voltaje de la batería
 * 
 * @return float Voltaje de la batería en voltios, o NAN si hay error
 */
float BatterySensor::readVoltage() {
    // Leer el valor del pin analógico para la batería
    int adcValue = analogRead(BATTERY_SENSOR_PIN);
    
    // Convertir el valor ADC a voltaje (0-3.3V con resolución de 12 bits)
    float voltage = adcValue * (3.3f / 4095.0f);
    
    // Comprobar si el voltaje es válido
    if (isnan(voltage) || voltage <= 0.0f || voltage >= 3.3f) {
        return NAN;
    }

    // Calcular el voltaje real de la batería
    float batteryVoltage = calculateBatteryVoltage(voltage);
    return batteryVoltage;
}

/**
 * @brief Calcula el voltaje real de la batería a partir de la lectura del ADC
 * 
 * En config.h, las constantes están definidas como:
 * const double R1 = 470000.0;  // Resistencia conectada a GND
 * const double R2 = 1500000.0; // Resistencia conectada a la batería
 * 
 * @param adcVoltage Voltaje medido por el ADC
 * @return float Voltaje real de la batería
 */
float BatterySensor::calculateBatteryVoltage(float adcVoltage) {
    // Usando las constantes definidas en config.h
    // El voltaje de la batería se calcula como:
    // V_bat = V_adc * (R1 + R2) / R1
    return adcVoltage * ((R1 + R2) / R1);
} 