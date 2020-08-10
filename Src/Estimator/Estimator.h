#include <Arduino.h>
#include <IntervalTimer.h>

// #include "../Libraries/ADC/ADC.h"
// #include "../Libraries/HX711/HX711.h"

#ifndef KIWI_ESTIMATOR
#define KIWI_ESTIMATOR

class Estimator {

    // enum typedef {
    //     mode_pressure,
    //     mode_thrust,
    //     mode_fused_thrust,
    //     num_modes
    // } ModeType;

    // typedef struct {
    //     ModeType mode;
    //     void (*method)(void);
    // } StateMachineType;

    // StateMachineType StateMachine[] = {
    //     { mode_pressure, sm_pressure },
    //     { mode_thrust, sm_thrust },
    //     { mode_fused_thrust, sm_fused_thrust }
    // };

public:
    // ADC* adc = new ADC();
    // uint8_t pressure_buffer_size = 128;

    // Estimator();

    // void init();

    // void main();

private:
    // // pressure transmitter pins
    // const int _pressure_pin_0 = 0;
    // const int _pressure_pin_1 = 0;

    // // HX711 load cell amplifier pins
    // const int _lc_propellant_pin_sda = 0;
    // const int _lc_propellant_pin_sck = 0;
    // const int _lc_thrust_pin_sda = 0;
    // const int _lc_thrust_pin_sck = 0;

    // // write buffer for pressure transmitter ADC data
    // uint8_t _pressure_index_0 = 0;
    // uint8_t _pressure_index_1 = 0;
    // uint16_t _pressure_buffer_0[_pressure_buffer_size];
    // uint16_t _pressure_buffer_1[_pressure_buffer_size];

    // HX711 lc_propellant;
    // HX711 lc_thrust;

    // // load cell adjustments (https://github.com/bogde/HX711)
    // const long LOADCELL_OFFSET = 50682624;
    // const long LOADCELL_DIVIDER = 5895655;

    // void sm_pressure(void);

    // void sm_thrust(void);

    // void sm_fused_thrust(void);

    // void adc_isr(void);
};

#endif