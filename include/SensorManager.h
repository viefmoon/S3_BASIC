#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "sensor_types.h"
#include <ESP32Time.h>
#include "PowerManager.h"
#include "MAX31865.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "SHT31.h"
#include "ModbusSensorManager.h"
#include "sensors/NtcManager.h"

// Variables y objetos globales declarados en main.cpp
extern ESP32Time rtc;
extern PowerManager powerManager;
extern SPIClass spi;
extern SPISettings spiAdcSettings;
extern SPISettings spiRtdSettings;
extern MAX31865_RTD rtd;
extern OneWire oneWire;
extern DallasTemperature dallasTemp;
extern SHT31 sht30Sensor;

/**
 * @brief Clase que maneja la inicialización y lecturas de todos los sensores
 *        incluyendo sensores normales y Modbus.
 */
class SensorManager {
  public:
    // Inicializa pines, periféricos (ADC, RTD, etc.), OneWire, etc.
    static void beginSensors(const std::vector<SensorConfig>& enabledNormalSensors);

    // Devuelve la lectura (o lecturas) de un sensor NO-Modbus según su configuración.
    static SensorReading getSensorReading(const SensorConfig& cfg);
    
    // Devuelve la lectura de un sensor Modbus según su configuración
    static ModbusSensorReading getModbusSensorReading(const ModbusSensorConfig& cfg);
    
    // Obtiene todas las lecturas de sensores (normales y Modbus) habilitados
    static void getAllSensorReadings(std::vector<SensorReading>& normalReadings,
                                    std::vector<ModbusSensorReading>& modbusReadings,
                                    const std::vector<SensorConfig>& enabledNormalSensors,
                                    const std::vector<ModbusSensorConfig>& enabledModbusSensors);

  private:
    // Métodos de lectura internos
    static float readSensorValue(const SensorConfig &cfg, SensorReading &reading);
};

#endif // SENSOR_MANAGER_H
