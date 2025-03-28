#include "SensorManager.h"
#include <Wire.h>
#include <SPI.h>
#include <cmath>  // Para fabs() y otras funciones matemáticas
#include <DallasTemperature.h>
#include "MAX31865.h"
#include "sensor_types.h"
#include "config.h"
#include <Preferences.h>
#include "config_manager.h"
#include "debug.h"
#include "utilities.h"

// Eliminadas las inclusiones de "ADS124S08.h" y "AdcUtilities.h"
#include "sensors/NtcManager.h"
#include "sensors/PHSensor.h"
#include "sensors/ConductivitySensor.h"
#include "sensors/HDS10Sensor.h"
// Se eliminó la variable externa ADC

// Inclusión de nuevos sensores
#include "sensors/RTDSensor.h"
#include "sensors/SHT30Sensor.h"
#include "sensors/DS18B20Sensor.h"

// -------------------------------------------------------------------------------------
// Métodos de la clase SensorManager
// -------------------------------------------------------------------------------------

void SensorManager::beginSensors(const std::vector<SensorConfig>& enabledNormalSensors) {
    // Encender alimentación 3.3V
    powerManager.power3V3On();
    
    // Inicializar RTD y configurarlo
    rtd.begin();
    {
        bool vBias = true;
        bool autoConvert = true;
        bool oneShot = false;
        bool threeWire = false;
        uint8_t faultCycle = 0; // MAX31865_FAULT_DETECTION_NONE
        bool faultClear = true;
        bool filter50Hz = true;
        uint16_t lowTh = 0x0000;
        uint16_t highTh = 0x7fff;
        rtd.configure(vBias, autoConvert, oneShot, threeWire, faultCycle, faultClear, filter50Hz, lowTh, highTh);
    }

    // Verificar si hay algún sensor DS18B20
    bool ds18b20SensorEnabled = false;
    for (const auto& sensor : enabledNormalSensors) {
        if (sensor.type == DS18B20 && sensor.enable) {
            ds18b20SensorEnabled = true;
            break;
        }
    }

    // Inicializar DS18B20 solo si está habilitado en la configuración
    if (ds18b20SensorEnabled) {
        // TIEMPO ejecución ≈ 65 ms
        // Inicializar DS18B20
        dallasTemp.begin();
        dallasTemp.requestTemperatures();
        DEBUG_PRINTLN("DS18B20 inicializado");
    }

    // Configurar los pines analógicos para cada sensor
    // NTC100K 0 - Pin A0
    pinMode(NTC100K_0_PIN, INPUT);
    // NTC100K 1 - Pin A3
    pinMode(NTC100K_1_PIN, INPUT);
    // NTC10K - Pin A4
    pinMode(NTC10K_PIN, INPUT);
    // PH Sensor - Pin A5
    pinMode(PH_SENSOR_PIN, INPUT);
    // Conductivity Sensor - Pin A6
    pinMode(COND_SENSOR_PIN, INPUT);
    // HDS10 Sensor - Pin A7
    pinMode(HDS10_SENSOR_PIN, INPUT);
    // Battery Sensor - Pin A8
    pinMode(BATTERY_SENSOR_PIN, INPUT);
    // Soil Humidity Sensor
    pinMode(SOILH_SENSOR_PIN, INPUT);

    // Configurar ADC interno
    analogReadResolution(12); // Resolución de 12 bits (0-4095)
    analogSetAttenuation(ADC_11db); // Atenuación para medir hasta 3.3V
}

SensorReading SensorManager::getSensorReading(const SensorConfig &cfg) {
    SensorReading reading;
    strncpy(reading.sensorId, cfg.sensorId, sizeof(reading.sensorId) - 1);
    reading.sensorId[sizeof(reading.sensorId) - 1] = '\0';
    reading.type = cfg.type;
    reading.value = NAN;

    readSensorValue(cfg, reading);

    return reading;
}

/**
 * @brief Lógica principal para leer el valor de cada sensor normal (no Modbus) según su tipo.
 */
float SensorManager::readSensorValue(const SensorConfig &cfg, SensorReading &reading) {
    switch (cfg.type) {
        case N100K:
            // Usar NtcManager para obtener la temperatura
            reading.value = NtcManager::readNtc100kTemperature(cfg.configKey);
            break;

        case N10K:
            // Usar NtcManager para obtener la temperatura del NTC de 10k
            reading.value = NtcManager::readNtc10kTemperature();
            break;
            
        case HDS10:
            // Leer sensor HDS10 y obtener el porcentaje de humedad
            reading.value = HDS10Sensor::read();
            break;
            
        case PH:
            // Leer sensor de pH y obtener valor
            reading.value = PHSensor::read();
            break;

        case COND:
            // Leer sensor de conductividad y obtener valor en ppm
            reading.value = ConductivitySensor::read();
            break;
        
        case SOILH:
            {
                // Leer el valor del pin analógico
                int adcValue = analogRead(SOILH_SENSOR_PIN);
                
                // Convertir el valor ADC a voltaje (0-3.3V con resolución de 12 bits)
                float voltage = adcValue * (3.3f / 4095.0f);
                
                // Verificar si el voltaje está en rango válido
                if (voltage <= 0.0f || voltage >= 3.3f) {
                    reading.value = NAN;
                } else {
                    // Convertir el voltaje a porcentaje (0V = 0%, 3.3V = 100%)
                    reading.value = (voltage / 3.3f) * 100.0f;
                }
            }
            break;

        case RTD:
            reading.value = RTDSensor::read();
            break;

        case DS18B20:
            reading.value = DS18B20Sensor::read();
            break;

        case SHT30:
        {
            float tmp = 0.0f, hum = 0.0f;
            SHT30Sensor::read(tmp, hum);
            reading.subValues.clear();
            
            // Agregar temperatura como primer valor [0]
            {
                SubValue sT; 
                sT.value = tmp;
                reading.subValues.push_back(sT);
            }
            
            // Agregar humedad como segundo valor [1]
            {
                SubValue sH; 
                sH.value = hum;
                reading.subValues.push_back(sH);
            }
            
            // Asignar el valor principal como NAN si alguno de los valores falló
            reading.value = (isnan(tmp) || isnan(hum)) ? NAN : tmp;
        }
        break;

        default:
            reading.value = NAN;
            break;
    }
    return reading.value;
}

ModbusSensorReading SensorManager::getModbusSensorReading(const ModbusSensorConfig& cfg) {
    ModbusSensorReading reading;
    
    // Copiar el ID del sensor
    strlcpy(reading.sensorId, cfg.sensorId, sizeof(reading.sensorId));
    reading.type = cfg.type;
    
    // Leer sensor según su tipo
    switch (cfg.type) {
        case ENV4:
            reading = ModbusSensorManager::readEnvSensor(cfg);
            break;
        // Añadir casos para otros tipos de sensores Modbus
        default:
            DEBUG_PRINTLN("Tipo de sensor Modbus no soportado");
            break;
    }
    
    return reading;
}

void SensorManager::getAllSensorReadings(std::vector<SensorReading>& normalReadings,
                                        std::vector<ModbusSensorReading>& modbusReadings,
                                        const std::vector<SensorConfig>& enabledNormalSensors,
                                        const std::vector<ModbusSensorConfig>& enabledModbusSensors) {
    // Reservar espacio para los vectores
    normalReadings.reserve(enabledNormalSensors.size());
    modbusReadings.reserve(enabledModbusSensors.size());
    
    // Leer sensores normales
    for (const auto &sensor : enabledNormalSensors) {
        normalReadings.push_back(getSensorReading(sensor));
    }
    
    // Si hay sensores Modbus, inicializar comunicación, leerlos y finalizar
    if (!enabledModbusSensors.empty()) {
        // Determinar el tiempo máximo de estabilización necesario
        uint32_t maxStabilizationTime = 0;
        
        // Revisar cada sensor habilitado para encontrar el tiempo máximo
        for (const auto &sensor : enabledModbusSensors) {
            uint32_t sensorStabilizationTime = 0;
            
            // Obtener el tiempo de estabilización según el tipo de sensor
            switch (sensor.type) {
                case ENV4:
                    sensorStabilizationTime = MODBUS_ENV4_STABILIZATION_TIME;
                    break;
                // Añadir casos para otros tipos de sensores Modbus con sus respectivos tiempos
                default:
                    sensorStabilizationTime = 500; // Tiempo predeterminado si no se especifica
                    break;
            }
            
            // Actualizar el tiempo máximo si este sensor necesita más tiempo
            if (sensorStabilizationTime > maxStabilizationTime) {
                maxStabilizationTime = sensorStabilizationTime;
            }
        }
        
        // Encender alimentación de 12V para sensores Modbus
        powerManager.power12VOn();
        DEBUG_PRINTF("Esperando %u ms para estabilización de sensores Modbus\n", maxStabilizationTime);
        delay(maxStabilizationTime);
        
        // Inicializar comunicación Modbus antes de comenzar las mediciones
        ModbusSensorManager::beginModbus();
        
        // Leer todos los sensores Modbus
        for (const auto &sensor : enabledModbusSensors) {
            modbusReadings.push_back(getModbusSensorReading(sensor));
        }
        
        // Finalizar comunicación Modbus después de completar todas las lecturas
        ModbusSensorManager::endModbus();
        
        // Apagar alimentación de 12V después de completar las lecturas
        powerManager.power12VOff();
    }
}
