#include <ADC.h>
#include <Arduino.h>
#include <IntervalTimer.h>
#include <NBHX711.h>
#include <PacketSerial.h>

#include "../KiwiGPIO.h"
#include "../StateClock/StateClock.h"

#ifndef KIWI_ESTIMATOR
#define KIWI_ESTIMATOR

#define CALIBRATION_INTERVAL_MS 100
#define CALIBRATION_SAMPLES     20

#define HX711_HIST_BUFF 24
#define HX711_GAIN      64

#define ADC_BUFFER_LENGTH  128
#define ADC_FULL_SCALE     4096
#define ADC_REF_VOLTAGE    3.292f
#define ADC_REF_RESISTANCE 124.0f
#define ADC_I_MIN          4
#define ADC_I_MAX          16
#define ADC_I_SCALE        1000

/*
    these constants are derived experimentally to scale the load cell output to the
    appropriate full scale measurement.
*/
#define LC_THRUST_SCALE           -19347.95f
#define LC_PROPELLANT_SCALE       -102558.6524f
#define LC_PROPELLANT_EMPTY_MASS  1.422f // mass of scale with no load (structure)
#define LC_PROPELLANT_BOTTLE_MASS 6.825f // mass of propellant tank

class Estimator {

public:
    typedef enum {
        THROTTLE,
        CHAMBER
    } PressureMode;

    static Estimator* estimator;

    Estimator();

    ~Estimator();

    void init();

    void main();

    void begin();

    void stop();

    void tareThrustCell();

    void tarePropellantCell();

    float getUpstreamPressure();

    float getDownstreamPressure();

    float getChamberPressure();

    float getThrust();

    float getPropellantMass();

    void setPressureMode(PressureMode mode);

private:
    ADC* adc = new ADC();

    IntervalTimer timer;
    /* 
        the estimator produces a real time estimate of thrust, chamber pressure, and injector pressure regardless of the mode, as well as fusing the two to provide a robust estimate of engine performance.
    */
    float p_upstream;
    float p_downstream;
    float p_chamber;
    float thrust;
    float m_propellant;

    StateClock clock;

    // write buffer for pressure transmitter ADC data
    size_t pressure_index_0 = 0;
    size_t pressure_index_1 = 0;
    uint16_t pressure_buffer_0[ADC_BUFFER_LENGTH];
    uint16_t pressure_buffer_1[ADC_BUFFER_LENGTH];

    NBHX711 lc_propellant { pin_lc_propellant_sda, pin_lc_sck, HX711_HIST_BUFF, HX711_GAIN };
    NBHX711 lc_thrust { pin_lc_thrust_sda, pin_lc_sck, HX711_HIST_BUFF, HX711_GAIN };

    int adc_sample_interval = 1;

    PressureMode pressureMode;

    bool should_sample;

    void sample(void);

    float sample_pressure_adc(uint16_t* buffer, size_t length);

    float compute_pressure(float p);

    static void adc_timer_callback(void);

    static void adc_isr(void);
};

#endif