#include <RoboClaw.h>

RoboClaw motor = RoboClaw(&Serial4, 10000);

int encoder_value = 0;
int motor_address = 0x80;
int motor_baud = 460800;

float position = 0;
int t = 0;

void setup()
{
    Serial.begin(115200);
    motor.begin(motor_baud);
    char v[48];
    motor.ReadVersion(motor_address, v);
    Serial.write(v);
    encoder_value = motor.ReadEncM1(motor_address);
    motor.SetEncM1(motor_address, 660);
    motor.ReadEncM1(motor_address);
    t = millis();
    // motor.SpeedAccelDeccelPositionM1(motor_address, 1000, 500, 1000, 0, 1);
    pinMode(20, OUTPUT);
    digitalWrite(20, HIGH);
}

void loop()
{
    if (Serial.available()) {
        char c;
        c = Serial.read();
        if (c == 'w') {
            position += 50;
            motor.SpeedAccelDeccelPositionM1(motor_address, 1000, 500, 1000, position, 1);
        } else if (c == 's') {
            position -= 50;
            motor.SpeedAccelDeccelPositionM1(motor_address, 1000, 500, 1000, position, 1);
        } else if (c == 'q') {
            motor.SpeedAccelDeccelPositionM1(motor_address, 0, 0, 0, 0, 1);
        } else {
            Serial.write(c);
        }
        Serial.print(position);
    }
    encoder_value = motor.ReadEncM1(motor_address);
    if (millis() - t > 10) {
        // Serial.print("encoder: ");
        Serial.print(encoder_value);
        Serial.print(" ");
        int16_t current1;
        int16_t current2;
        motor.ReadCurrents(motor_address, current1, current2);
        // Serial.print("current: ");
        Serial.print(current1);
        // Serial.print(" ");
        Serial.println();
        t = millis();
    }
}
