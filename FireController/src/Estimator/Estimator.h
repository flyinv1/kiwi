#include <ADC.h>
#include <Arduino.h>
#include <HX711.h>
#include <IntervalTimer.h>
#include <PacketSerial.h>

#include "../KiwiGPIO.h"
// #include "../Libraries/ADC/ADC.h"

#ifndef KIWI_ESTIMATOR
#define KIWI_ESTIMATOR

#define ADC_BUFFER_LENGTH 128

class Estimator {

public:
    typedef enum {
        mode_standby,
        mode_sample,
        num_modes,
    } ModeType;

    typedef struct {
        ModeType mode;
        void (Estimator::*method)(void);
    } StateMachineType;

    StateMachineType StateMachine[num_modes] = {
        { mode_standby, &Estimator::sm_standby },
        { mode_sample, &Estimator::sm_sample }
    };

    enum TRANSITIONS {
        num_transitions = 2,
    };

    typedef struct {
        ModeType prev;
        ModeType next;
        void (Estimator::*method)(void);
    } StateMachineTransition;

    StateMachineTransition TransitionTable[num_transitions] = {
        { mode_standby, mode_sample, &Estimator::sm_standby_to_sample },
        { mode_sample, mode_standby, &Estimator::sm_sample_to_standby }
    };

    ModeType mode = mode_standby;

    ADC* adc = new ADC();

    IntervalTimer timer;

    // only one instance of Estimator, use a singleton for easy isr
    static Estimator* estimator;

    Estimator();

    void init();

    void main();

    void setState(ModeType new_mode);

private:
    // the estimator produces a real time estimate of thrust, chamber pressure,
    // and injector pressure regardless of the mode, as well as fusing the two to provide a robust
    // estimate of engine performance.
    float chamber_pressure;
    float injector_pressure;
    float thrust;

    // keep track of time elapsed over each loop
    int dt;
    int t_last;

    // typedef struct {
    //     int16_t x_offset;
    //     int16_t y_offset;
    //     int16_t slope;
    // } PressureCalibration;

    // const PressureCalibration pressure_0_cal = {
    //     0,
    //     0,
    //     0,
    // };

    // const PressureCalibration pressure_1_cal = {
    //     0,
    //     0,
    //     0
    // };

    // // write buffer for pressure transmitter ADC data
    uint8_t _pressure_index_0 = 0;
    uint8_t _pressure_index_1 = 0;
    uint16_t _pressure_buffer_0[ADC_BUFFER_LENGTH];
    uint16_t _pressure_buffer_1[ADC_BUFFER_LENGTH];

    int sampleInterval = 1;

    HX711 lc_propellant;
    HX711 lc_thrust;

    // // load cell adjustments (https://github.com/bogde/HX711)
    // const long LOADCELL_OFFSET = 50682624;
    // const long LOADCELL_DIVIDER = 5895655;

    void sm_standby(void);

    void sm_sample(void);

    void sm_standby_to_sample(void);

    void sm_sample_to_standby(void);

    static void adc_timer_callback(void);

    static void adc_isr(void);
};

#endif