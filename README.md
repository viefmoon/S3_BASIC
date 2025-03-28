# S3_BASIC

Firmware para dispositivo basado en ESP32-S3 con capacidades de sensores múltiples y comunicación LoRaWAN.

## Características

- Soporte para múltiples tipos de sensores (DS18B20, RTD, SHT30, etc.)
- Comunicación LoRaWAN para envío de datos
- Modo de configuración mediante BLE
- Gestión de energía y ciclos de sueño para bajo consumo
- Almacenamiento de configuración en memoria no volátil

## Requisitos

- PlatformIO
- ESP32-S3
- Bibliotecas requeridas (ver platformio.ini)

## Estructura del Proyecto

- `/src`: Código fuente principal
- `/include`: Archivos de cabecera
- `/lib`: Bibliotecas

## Instrucciones de Compilación

1. Clone el repositorio
2. Abra el proyecto en PlatformIO
3. Compile y cargue el firmware en el dispositivo 