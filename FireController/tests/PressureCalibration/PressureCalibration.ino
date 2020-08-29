#include <ADC.h>

#define ADC_V_REF 3.292
#define ADC_MAX 1024

int run_pin = 20;

int adc_0_pin = 19;
int adc_1_pin = 18;

ADC* adc = new ADC();

long sample_timer = 0;
int sample_interval = 100; // us

long p0;
long p1;

bool open = false;

void setup()
{
    Serial.begin(115200);

    pinMode(run_pin, OUTPUT);
    digitalWrite(run_pin, LOW);

    pinMode(adc_0_pin, INPUT);
    pinMode(adc_1_pin, INPUT);

    adc->adc0->setAveraging(16);
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_HIGH_SPEED);
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
    adc->adc0->setResolution(12);

    analogReadRes(10);
    analogReadAveraging(10);

    sample_timer = micros();
}

void loop()
{
    if (micros() - sample_timer > sample_interval) {
        p0 = adc->adc0->analogRead(adc_0_pin);
        p1 = adc->adc0->analogRead(adc_1_pin);
//        p0 = analogRead(adc_0_pin);
//        p1 = analogRead(adc_1_pin);
        Serial.print((float)p0);
        Serial.print(' ');
        Serial.print((float)p1);
        Serial.print(' ');
        Serial.print(current(p0));
        Serial.print(' ');
        Serial.print(current(p1));
        Serial.print(' ');
        Serial.print(pressure(p0));
        Serial.print(' ');
        Serial.print(pressure(p1));
        Serial.println();
        sample_timer = micros();
    }

    while (Serial.available()) {
        Serial.read();
        digitalWrite(run_pin, !open);
        open = !open;
    }
}

float pressure(long p) {
  float v = (float)p / ADC_MAX * ADC_V_REF;
  float i = (v) / 124 * 1000;
  return (i - 4) / 16 * 1000;
}

float current(long reading) {
  float v = (float)reading / ADC_MAX * ADC_V_REF;
  return v / 124 * 1000;
}
