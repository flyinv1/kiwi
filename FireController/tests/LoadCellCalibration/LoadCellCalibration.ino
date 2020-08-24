#include <NBHX711.h>

/*
    This script provides a calibration procedure for HX711 load cell.
    The load cell data is read and averaged over the specified number of samples
    when the return key is pressed.

    The load cell may read at 10 or 80 Hz and providing an averaged sample will take a few seconds.
*/

NBHX711 lc(22, 23, 24, 64);
int samples = 1;

/*
    factor provides the scale calibration factor (converts from <int> to <kg>)
*/
float factor = -102.5586524;

/*
    scale_offset provides the empty scale tare value
*/
long scale_offset = -145967;

/*
    scale_tank_offset provides the nitrous tank + empty scale tare value
*/
long scale_tank_offset = -845864;

/*
    tank_offset provides the nitrous tank tare value
*/
long tank_offset = -699424;

bool running = false;
long avg = 0;
uint8_t ct = 0;

void setup()
{

    Serial.begin(115200);

    lc.begin();
    lc.setOffset(0);
    lc.setScale(1);

    lc.update();

    delay(1000);

    for (int i = 0; i < 24; i++) {
        if (lc.update()) { }
        delay(100);
    }

    //    lc.tare(24);

    Serial.println(lc.getUnits(12));
}

void loop()
{
    if (lc.update()) {
        if (running) {
            avg += lc.getUnits();
            ct++;
            if (ct == samples) {
                running = false;
                Serial.println((float)avg / (float)ct, 2);
                avg = 0;
                ct = 0;
            }
        }
    }
    running = true;
//    if (Serial.available()) {
//        Serial.read();
//        running = true;
//    }
}
