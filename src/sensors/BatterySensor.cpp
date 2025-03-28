#include "sensors/BatterySensor.h"

#include "ADS124S08.h"
#include "AdcUtilities.h"
extern ADS124S08 ADC;

/**
 * @brief Lee el voltaje de la batería
 * 
 * @return float Voltaje de la batería en voltios, o NAN si hay error
 */
float BatterySensor::readVoltage() {
    // Configurar el multiplexor para leer AIN9 con referencia a AINCOM (tierra)
    uint8_t muxConfig = ADS_P_AIN9 | ADS_N_AINCOM;
    
    // Asegurarse de que el ADC esté despierto
    ADC.sendCommand(WAKE_OPCODE_MASK);
    
    // Leer voltaje diferencial entre AIN9 y COMMON
    float voltage = AdcUtilities::measureAdcDifferential(muxConfig);

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