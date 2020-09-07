#ifndef KIWI_GPIO
#define KIWI_GPIO

enum KIWI_IO {
    pin_pressure_0 = 18,        // Pressure transmitter 0 ADC
    pin_pressure_1 = 19,        // Pressure transmitter 1 ADC
    pin_lc_sck = 23,            // HX711 serial clock
    pin_lc_thrust_sda = 21,     // Thrust HX711 data
    pin_lc_propellant_sda = 22, // Propellant HX711 data
    pin_igniter_sdn = 5,        // Igniter shutdown (HIGH = OFF)
    pin_igniter_ctr = 7,        // Igniter control signal (analog 0-5V)
    pin_throttle_tx = 17,       // Throttle controller serial TX
    pin_throttle_rx = 16,       // Throttle controller serial RX
    pin_run_valve = 20,         // Run valve digital output (HIGH = OPEN)
};

#endif