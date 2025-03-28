#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include "config.h"

class PowerManager {
private:
    // No más dependencia del expansor de IO

public:
    PowerManager();
    void begin();

    // Métodos de control de energía
    void power3V3On();
    void power3V3Off();
    void power12VOn();
    void power12VOff();
    void allPowerOff();
};

#endif 