#include "Estimator.h"

#include <ADC.h>
#include <Arduino.h>
#include <IntervalTimer.h>
#include <NBHX711.h>
// #include "../Libraries/HX711/HX711.h"

Estimator::Estimator()
{
    // lc_propellant = NBHX711(pin_lc_propellant_sda, pin_lc_sck, HX711_HIST_BUFF, HX711_GAIN);
    // lc_thrust = NBHX711(pin_lc_thrust_sda, pin_lc_sck, HX711_HIST_BUFF, HX711_GAIN);
}

Estimator::~Estimator()
{
}

void Estimator::init()
{
    Estimator::estimator = this;

    /* 
        Initialize ADC pressure measurement. 
        Pressure measuremnts are updated at fixed time intervals via timer interrupts. Data is shifted in to the estimator's pressure buffers if available.
    */

    // enable analog pins for input
    pinMode(pin_pressure_0, INPUT);
    pinMode(pin_pressure_1, INPUT);

    // adc allows hardware averaging
    adc->adc0->setAveraging(16);
    adc->adc0->setAveraging(16);

    // // use high speeds for lower impedences
    // // use low speeds for high impedences
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_HIGH_SPEED);
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
    adc->adc0->setResolution(10);

    // // set interrupt for adc0 when sample is ready
    adc->adc0->enableInterrupts(adc_isr);

    // initialize load cell amps
    lc_thrust.begin();
    lc_propellant.begin();

    lc_thrust.setScale();
    lc_propellant.setScale();

    // start timing loop
    t_last = micros();

    timer.begin(adc_timer_callback, sampleInterval);
}

void Estimator::main(void)
{
    int t = micros();
    dt = t - t_last;

    (this->*StateMachine[mode].method)();

    t_last = t;
}

void Estimator::setState(ModeType new_mode)
{
    if (new_mode < num_modes) {
        for (int i = 0; i < num_transitions; i++) {
            if (TransitionTable[i].prev == mode && TransitionTable[i].next == new_mode) {
                (this->*TransitionTable[i].method)();
                break;
            }
        }
        this->mode = new_mode;
    }
}

void Estimator::getEngineState(EngineState* engineState)
{
}

void Estimator::sm_standby()
{
    // do nothing so far
}

void Estimator::sm_sample()
{
    // Sample pressure ADC buffers
    float _p_adc0 = sample_pressure_adc(_pressure_buffer_0, _pressure_index_0);
    float _p_adc1 = sample_pressure_adc(_pressure_buffer_0, _pressure_index_1);

    // Convert adc readings to pressure values
    float _p0 = compute_pressure(_p_adc0);
    float _p1 = compute_pressure(_p_adc1);

    // Sample load cells
    if (lc_thrust.update()) {
        lc_thrust.getUnits(0);
    }
    if (lc_propellant.update()) {
        lc_propellant.getUnits(0);
    }
}

float Estimator::sample_pressure_adc(uint16_t* buffer, size_t len)
{
    uint16_t p;
    for (uint8_t i = 0; i < len; i++) {
        p += buffer[i];
    }
    return (float)p / (float)len;
}

float Estimator::compute_pressure(float p)
{
    return 0;
}

void Estimator::adc_timer_callback(void)
{
    estimator->adc->adc0->startSingleRead(pin_pressure_0);
    estimator->adc->adc0->startSingleRead(pin_pressure_1);
}

void Estimator::adc_isr(void)
{
    // get pin that triggered interrupt
    // and save data to output buffer
    uint8_t pin = ADC::sc1a2channelADC0[ADC1_HC0 & 0x1f];
    if (pin == pin_pressure_0) {
        uint16_t adc_val = estimator->adc->adc0->readSingle();
        if (estimator->_pressure_index_0 < ADC_BUFFER_LENGTH) {
            estimator->_pressure_buffer_0[estimator->_pressure_index_0++] = adc_val;
        }
    } else if (pin == pin_pressure_1) {
        uint16_t adc_val = estimator->adc->adc0->readSingle();
        if (estimator->_pressure_index_1 < ADC_BUFFER_LENGTH) {
            estimator->_pressure_buffer_1[estimator->_pressure_index_1++] = adc_val;
        }
    } else {
        // flush the adc
        estimator->adc->readSingle();
    }

    if (estimator->adc->adc0->adcWasInUse) {
        // restore ADC config, and restart conversion
        estimator->adc->adc0->loadConfig(&estimator->adc->adc0->adc_config);
        // avoid a conversion started by this isr to repeat itself
        estimator->adc->adc0->adcWasInUse = false;
    }
}