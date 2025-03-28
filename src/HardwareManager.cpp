/*******************************************************************************************
 * Archivo: src/HardwareManager.cpp
 * Descripción: Implementación de la gestión de hardware del sistema.
 *******************************************************************************************/

#include "HardwareManager.h"
#include "debug.h"
// time execution < 10 ms
bool HardwareManager::initHardware(PowerManager& powerManager, SHT31& sht30Sensor, SPIClass& spi, const std::vector<SensorConfig>& enabledNormalSensors) {
    // Configurar GPIO one wire con pull-up
    pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
    
    // Inicializar I2C con pines definidos
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    // Inicializar SPI con pines definidos
    spi.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);

    // Verificar si hay algún sensor SHT30
    bool sht30SensorEnabled = false;
    for (const auto& sensor : enabledNormalSensors) {
        if (sensor.type == SHT30 && sensor.enable) {
            sht30SensorEnabled = true;
            break;
        }
    }

    // Inicializar SHT30 solo si está habilitado en la configuración
    if (sht30SensorEnabled) {
        //Inicializar SHT30 para reset y dummy lectura
        sht30Sensor.begin();
        sht30Sensor.reset();
    }
    
    // Inicializar los pines de selección SPI (SS)
    initializeSPISSPins();
    
    // Inicializar PowerManager para control de energía
    powerManager.begin();
    
    return true;
}

void HardwareManager::initializeSPISSPins() {
    // Inicializar SS del LORA conectado directamente
    pinMode(LORA_NSS_PIN, OUTPUT);
    digitalWrite(LORA_NSS_PIN, HIGH);

    // Inicializar SS de PT100 como pin nativo
    pinMode(PT100_CS_PIN, OUTPUT);
    digitalWrite(PT100_CS_PIN, HIGH);

    // Inicializar SS del ADC como pin nativo
    pinMode(ADS124S08_CS_PIN, OUTPUT);
    digitalWrite(ADS124S08_CS_PIN, HIGH);
}