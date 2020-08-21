#ifndef KIWI_GPIO
#define KIWI_GPIO

enum KIWI_IO {
    pin_pressure_0 = 0,
    pin_pressure_1 = 0,
    pin_lc_thrust_sck = 0,
    pin_lc_thrust_sda = 0,
    pin_lc_propellant_sck = 0,
    pin_lc_propellant_sda = 0,
    pin_run_valve = 0,
};

struct TOPICS {
    enum KIWI_API {
        override_shutdown = 0xFF,
        override_purge = 0xFE,

        set_mode = 0x00,
        set_controller_mode = 0x01,
        set_estimator_mode = 0x02,

        controller_run_valve_position = 0x04,
        controller_throttle_valve_position = 0x05,
        controller_igniter_voltage = 0x06,
    };
};

#endif