#include "Estimator.h"

#include <ADC.h>
#include <Arduino.h>
#include <IntervalTimer.h>
// #include "../Libraries/HX711/HX711.h"

Estimator::Estimator()
{
}

void Estimator::init()
{
    Estimator::estimator = this;

    // initialize ADC pressure measurement

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
    adc->adc0->setResolution(12);

    // // set interrupt for adc0 when sample is ready
    adc->adc0->enableInterrupts(adc_isr);

    // initialize load cell amps
    lc_propellant.begin(pin_lc_propellant_sda, pin_lc_propellant_sck);
    lc_thrust.begin(pin_lc_thrust_sda, pin_lc_thrust_sck);

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

void Estimator::sm_standby()
{
    // do nothing so far
}

void Estimator::sm_sample()
{

    uint16_t pressure_0;
    uint16_t pressure_1;

    if (_pressure_index_0 > 0 && _pressure_index_1 > 0) {

        // flush read buffer for each set of pressure transmitter data;
        for (uint8_t i = 0; i < _pressure_index_0; i++) {
            pressure_0 += _pressure_buffer_0[i];
        }
        float f_pressure_0 = float(pressure_0) / (_pressure_index_0 + 1);
        _pressure_index_0 = 0;

        for (uint8_t i = 0; i < _pressure_index_1; i++) {
            pressure_1 += _pressure_buffer_1[i];
        }
        float f_pressure_1 = float(pressure_1) / (_pressure_index_1 + 1);
        _pressure_index_1 = 0;
    }
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