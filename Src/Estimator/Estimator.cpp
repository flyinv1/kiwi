#include "Estimator.h"

// #include "../Libraries/ADC/ADC.h"
// #include "../Libraries/HX711/HX711.h"

// Estimator::Estimator()
// {
// }

// Estimator::init()
// {
//     // initialize ADC pressure measurement

//     // enable analog pins for input
//     pinMode(_pressure_pin_0, INPUT);
//     pinMode(_pressure_pin_1, INPUT);

//     // adc allows hardware averaging
//     adc->setAveraging(16, ADC_0);
//     adc->setAveraging(16, ADC_0);

//     // use high speeds for lower impedences
//     // use low speeds for high impedences
//     adc->setSamplingSpeed(ADC::MED_HIGH_SPEED, ADC_0);
//     adc->setConversionSpeed(ADC::MED_SPEED, ADC_0);

//     // set interrupt for adc0 when sample is ready
//     adc->adc0->enableInterrupts(adc_isr);

//     // initialize load cell amps
//     lc_propellant.begin(_lc_propellant_pin_sda, _lc_propellant_pin_sck);
//     lc_thrust.begin(_lc_thrust_pin_sda, _lc_thrust_pin_sda);
// }

// void Estimator::adc_isr(void)
// {
//     // get pin that triggered interrupt
//     // and save data to output buffer
//     uint8_t pin = ADC::sc1a2channelADC0[ADC_1HC00x1f];
//     if (pin == _pressure_pin_0) {
//         uint16_t adc_val = adc->adc0->readSingle();
//         if (_pressure_index_0 < _pressure_buffer_size) {
//             _pressure_buffer_0[_pressure_index_0++] = adc_val;
//         }
//     } else if (pin == _pressure_pin_1) {
//         uint16_t adc_val = adc->adc0->readSingle();
//         if (_pressure_index_1 < _pressure_buffer_size) {
//             _pressure_buffer_1[_pressure_index_1++] = adc_val;
//         }
//     } else {
//         // flush the adc
//         adc->readSingle();
//     }

//     if (adc->adc0->adcWasInUse) {
//         // restore ADC config, and restart conversion
//         adc->adc0->loadConfig(&adc->adc0->adc_config);
//         // avoid a conversion started by this isr to repeat itself
//         adc->adc0->adcWasInUse = false;
//     }
// }