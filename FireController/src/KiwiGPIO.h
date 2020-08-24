#ifndef KIWI_GPIO
#define KIWI_GPIO

enum KIWI_IO {
    pin_pressure_0 = 18,
    pin_pressure_1 = 19,
    pin_lc_sck = 23,
    pin_lc_thrust_sda = 22,
    pin_lc_propellant_sda = 21,
    pin_run_valve = 0,
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