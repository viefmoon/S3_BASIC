#ifndef NTC_MANAGER_H
#define NTC_MANAGER_H

class NtcManager {
public:
    /**
     * Lee la temperatura del sensor NTC100K
     * @param configKey Identificador de la configuración del sensor (0 o 1)
     * @return Temperatura en grados Celsius, o NAN si hay error
     */
    static double readNtc100kTemperature(const char* configKey);
    
    /**
     * Lee la temperatura del sensor NTC10K
     * @return Temperatura en grados Celsius, o NAN si hay error
     */
    static double readNtc10kTemperature();

private:
    /**
     * Calcula los coeficientes A, B, C para la ecuación de Steinhart-Hart
     * basado en tres pares de puntos temperatura-resistencia
     */
    static void calculateSteinhartHartCoeffs(double T1, double R1,
                                          double T2, double R2,
                                          double T3, double R3,
                                          double &A, double &B, double &C);
    
    /**
     * Calcula la temperatura usando la ecuación de Steinhart-Hart
     * @param resistance La resistencia del termistor en ohms
     * @param A,B,C Los coeficientes de Steinhart-Hart
     * @return Temperatura en grados Celsius
     */
    static double steinhartHartTemperature(double resistance, double A, double B, double C);
    
    /**
     * Calcula la resistencia del NTC basado en un divisor de voltaje
     * @param voltage Voltaje medido en el punto medio del divisor
     * @param vRef Voltaje de referencia
     * @param rFixed Resistencia fija del divisor
     * @param ntcTop true si el NTC está conectado a Vref, false si está conectado a GND
     * @return Resistencia del NTC en ohms, o -1 si hay error
     */
    static double computeNtcResistanceFromVoltageDivider(double voltage, double vRef, double rFixed, bool ntcTop);
};

#endif // NTC_MANAGER_H 