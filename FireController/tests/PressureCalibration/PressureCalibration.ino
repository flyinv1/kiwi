#include <ADC.h>

int run_pin = 20;

int adc_0_pin = 19;
int adc_1_pin = 18;

//ADC* adc = new ADC();

long sample_timer = 0;
int sample_interval = 5; // ms
int samples = 10;
int ct = 0;

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

//    adc->adc0->setAveraging(16);
//
//    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_HIGH_SPEED);
//    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);
//    adc->adc0->setResolution(12);

    sample_timer = millis();
}

void loop()
{
    if (millis() - sample_timer > sample_interval) {
        p0 += analogRead(adc_0_pin);
        p1 += analogRead(adc_1_pin);
        if (ct++ > samples) {
          Serial.print((float)p0 / samples);
          Serial.print(' ');
          Serial.println((float)p1 / samples);
          p0 = 0;
          p1 = 0;
          ct = 0;
        }
        sample_timer = millis();
    }

    while (Serial.available()) {
        Serial.read();
        digitalWrite(run_pin, !open);
        open = !open;
    }
    delay(1);
}
