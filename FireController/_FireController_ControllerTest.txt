// #include "src/Estimator/Estimator.h"

// Estimator estimator = Estimator();

#include "src/Controller/Controller.h"
#include "src/Target/Target.h"

Controller controller = Controller();
Controller::EngineData data;

long t = 0;
long interval = 0;
int step = 0;

void setup()
{
    Target _targets[120];

    int DT = 1000;
    int START = 45;
    float INTV = 0.25;

    for (int i = 0; i < 60; i++) {
        _targets[i] = Target(DT + i * DT, (START + i * INTV) * TARGET_SCALE);
    }
    for (int i = 0; i < 60; i++) {
        _targets[i + 60] = Target(DT * 60 + DT + i * DT, (START - i * INTV) * TARGET_SCALE);
    }

    Serial.begin(1);
    controller.init();
    controller.setEngineMode(Controller::ENGINE_MODE_COLD);
    controller.setTargets(_targets, 20);
}

void loop()
{
    controller.main();
    if (Serial.available()) {
        Serial.read();
        switch (step) {
        case 0: {
            controller.arm();
            Serial.println("ARMED");
            step++;
        } break;
        case 1: {
            controller.fire();
            Serial.println("FIRING");
            step++;
        } break;
        case 2: {
            controller.abort();
            Serial.println("ABORTING");
            step++;
        } break;
        case 3: {
            Serial.println("RESETTING");
            step = 0;
        } break;
        default:
            break;
        }
        printEngineData();
    }
    controller.getEngineData(&data);
    if (millis() - t > interval && controller.getState() == Controller::state_firing) {
        printEngineData();
        t = millis();
    }
}

void printEngineData()
{
    Serial.print(data.upstream_pressure);
    Serial.print(" ");
    Serial.print(data.downstream_pressure);
    Serial.print(" ");
    Serial.print(data.propellant_mass);
    Serial.print(" ");
    Serial.print(data.thrust);
    Serial.print(" ");
    Serial.print(data.throttle_position);
    Serial.print(" ");
    Serial.print(data.mission_elapsed_time);
    Serial.print(" ");
    Serial.print(data.delta_time);
    Serial.println();
}

uint32_t FreeMem()
{ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t)&stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t)hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}
