#ifndef KIWI_GPIO
#define KIWI_GPIO

enum KIWI_IO {
    pin_pressure_0 = 18, // Pressure transmitter 0 ADC
    pin_pressure_1 = 19, // Pressure transmitter 1 ADC
    pin_lc_sck = 23, // HX711 serial clock
    pin_lc_thrust_sda = 21, // Thrust HX711 data
    pin_lc_propellant_sda = 22, // Propellant HX711 data
    pin_igniter_sdn = 5, // Igniter shutdown (HIGH = OFF)
    pin_igniter_ctr = 7, // Igniter control signal (analog 0-5V)
    pin_throttle_tx = 17, // Throttle controller serial TX
    pin_throttle_rx = 16, // Throttle controller serial RX
    pin_run_valve = 20, // Run valve digital output (HIGH = OPEN)
};

struct API {
    enum {
        override_mode = 00,
        override_purge = 01,
        override_shutdown = 02,

        read_controller_status = 10,
        controller_run_valve_position = 11,
        controller_throttle_valve_position = 12,
        controller_igniter_voltage = 13,

        read_estimator_status = 20,
        estimator_calibrate_all = 21,
        estimator_calibrate_thrust = 22,
        estimator_calibrate_propellant = 23,
        estimator_calibrate_pressure_0 = 24,
        estimator_calibrate_pressure_1 = 25,

        read_run_config = 110,
        run_cold_open = 101,
        run_cold_closed = 102,
        run_hot_open = 103,
        run_hot_closed = 104,
        run_shutdown = 110,
    };
};

struct MEM {
    enum {

    };
};

#endif