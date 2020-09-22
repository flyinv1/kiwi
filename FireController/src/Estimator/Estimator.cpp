#include "Estimator.h"

#include <ADC.h>
#include <Arduino.h>
#include <IntervalTimer.h>
#include <NBHX711.h>
// #include "../Libraries/HX711/HX711.h"

Estimator::Estimator()
{
    clock = StateClock();
}

Estimator::~Estimator()
{
}

void Estimator::init()
{
    // Estimator::estimator = this;

    /* 
        Initialize ADC pressure measurement. 
        Pressure measuremnts are updated at fixed time intervals via timer interrupts. Data is shifted in to the estimator's pressure buffers if available.
    */

    // enable analog pins for input
    pinMode(pin_pressure_0, INPUT);
    pinMode(pin_pressure_1, INPUT);
    analogReadResolution(12);

    // adc allows hardware averaging
    // adc->adc0->setAveraging(16);
    // adc->adc0->setAveraging(16);

    // // use high speeds for lower impedences
    // // use low speeds for high impedences
    // adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);
    // adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
    // adc->adc0->setResolution(10);

    // // set interrupt for adc0 when sample is ready
    // adc->adc0->enableInterrupts(adc_isr);

    // initialize load cell amps
    lc_thrust.begin();
    lc_propellant.begin();

    lc_thrust.setScale(LC_THRUST_SCALE);
    lc_propellant.setScale(LC_PROPELLANT_SCALE);

    // Enable timer ISR's for adc sampling
    // timer.begin(adc_timer_callback, adc_sample_interval);

    // Start the estimator clock
    // clock.start();
}

void Estimator::main(void)
{
    // clock.tick();
    if (should_sample) {
        // retrieve reading average over loop
        // float _p_adc0 = sample_pressure_adc(pressure_buffer_0, pressure_index_0);
        // float _p_adc1 = sample_pressure_adc(pressure_buffer_1, pressure_index_1);
        float _p_adc0 = float(analogRead(pin_pressure_0));
        float _p_adc1 = float(analogRead(pin_pressure_1));

        // convert readings to pressure
        float _p0 = compute_pressure(_p_adc0);
        float _p1 = compute_pressure(_p_adc1);

        if (pressureMode == THROTTLE) {
            p_upstream = _p0;
            p_downstream = _p1;
            p_chamber = 0;
        } else {
            p_upstream = 0;
            p_downstream = _p0;
            p_chamber = _p1;
        }

        if (lc_thrust.update()) {
            thrust = lc_thrust.getUnits();
        }

        if (lc_propellant.update()) {
            // Offset the bottle mass -> this can't be tared before or during a run since the nitrous bottle will be filled
            // Assume that the scale has been tared when assembled (i.e. under load from the top plate)
            m_propellant = lc_propellant.getUnits() - LC_PROPELLANT_BOTTLE_MASS - LC_PROPELLANT_EMPTY_MASS;
        }
    }
}

void Estimator::begin()
{
    clock.advance();
    should_sample = true;
}

void Estimator::stop()
{
    clock.advance();
    should_sample = false;
}

void Estimator::tareThrustCell()
{
    lc_thrust.tare();
}

void Estimator::setPressureMode(PressureMode mode)
{
    pressureMode = mode;
}

float Estimator::getUpstreamPressure()
{
    return p_upstream;
}

float Estimator::getDownstreamPressure()
{
    return p_downstream;
}

float Estimator::getChamberPressure()
{
    return p_chamber;
}

float Estimator::getThrust()
{
    return thrust;
}

float Estimator::getPropellantMass()
{
    return m_propellant;
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
    // return p;
    float _voltage = (p / ADC_FULL_SCALE) * ADC_REF_VOLTAGE;
    float _current = _voltage / ADC_REF_RESISTANCE * 1000;
    float _pressure = (_current - ADC_I_MIN) / ADC_I_MAX * ADC_I_SCALE;
    return _pressure;
}

void Estimator::adc_timer_callback(void)
{
    // estimator->adc->adc0->startSingleRead(pin_pressure_0);
    // estimator->adc->adc0->startSingleRead(pin_pressure_1);
}

void Estimator::adc_isr(void)
{
    // get pin that triggered interrupt
    // and save data to output buffer
    // uint8_t pin = ADC::sc1a2channelADC0[ADC1_HC0 & 0x1f];
    // if (pin == pin_pressure_0) {
    //     uint16_t adc_val = estimator->adc->adc0->readSingle();
    //     if (estimator->pressure_index_0 < ADC_BUFFER_LENGTH) {
    //         estimator->pressure_buffer_0[estimator->pressure_index_0++] = adc_val;
    //     } else {
    //         // Clear the buffer and write from beginning
    //         // This should be a circular buffer <- low priority since loop is fast
    //         estimator->pressure_index_0 = 0;
    //         estimator->pressure_buffer_0[estimator->pressure_index_0++] = adc_val;
    //     }
    // } else if (pin == pin_pressure_1) {
    //     uint16_t adc_val = estimator->adc->adc0->readSingle();
    //     if (estimator->pressure_index_1 < ADC_BUFFER_LENGTH) {
    //         estimator->pressure_buffer_1[estimator->pressure_index_1++] = adc_val;
    //     } else {
    //         estimator->pressure_index_1 = 0;
    //         estimator->pressure_buffer_1[estimator->pressure_index_1++] = adc_val;
    //     }
    // } else {
    //     // flush the adc
    //     estimator->adc->readSingle();
    // }

    // if (estimator->adc->adc0->adcWasInUse) {
    //     // restore ADC config, and restart conversion
    //     estimator->adc->adc0->loadConfig(&estimator->adc->adc0->adc_config);
    //     // avoid a conversion started by this isr to repeat itself
    //     estimator->adc->adc0->adcWasInUse = false;
    // }
}