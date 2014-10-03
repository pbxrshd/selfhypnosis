// pin assignments
const byte PIN_MA_D = 2; // D2 motorA driver direction
const byte PIN_MA_S = 3; // D3 motorA driver pwm
const byte PIN_MB_D = 4; // D4 motorB driver direction
const byte PIN_MB_S = 5; // D5 motorB driver pwm
const byte PIN_SA_T = 14; // A0 sensorA trigger
const byte PIN_SA_E = 15; // A1 sensorA echo
const byte PIN_SB_T = 16; // A2 sensorB trigger
const byte PIN_SB_E = 17; // A3 sensorB echo

const int SENSOR_MIN = 0;
const int SENSOR_MID = 25;
const int SENSOR_MAX = 50;

const int SPEED_MA_MIN = 70;
const int SPEED_MA_MAX = 255;
const int SPEED_MB_MIN = 30;
const int SPEED_MB_MAX = 255;

const int LOOP_DELAY = 250;

int previousSpeedA = 0;
int previousSpeedB = 0;

void setup()
{
    // Serial.begin(9600);
    // init pins
    pinMode(PIN_MA_D, OUTPUT);
    pinMode(PIN_MB_D, OUTPUT);
    pinMode(PIN_SA_T, OUTPUT);
    pinMode(PIN_SA_E, INPUT);
    pinMode(PIN_SB_T, OUTPUT);
    pinMode(PIN_SB_E, INPUT);
}

void loop()
{
    previousSpeedA = calculateAndApplySpeed(previousSpeedA, PIN_SA_T, PIN_SA_E, PIN_MA_D, PIN_MA_S, SPEED_MA_MIN, SPEED_MA_MAX);
    previousSpeedB = calculateAndApplySpeed(previousSpeedB, PIN_SB_T, PIN_SB_E, PIN_MB_D, PIN_MB_S, SPEED_MB_MIN, SPEED_MB_MAX);
    delay(LOOP_DELAY);
}

// returns the speed that was calculated and applied
int calculateAndApplySpeed(int previousSpeed, byte triggerPin, byte echoPin, byte motorDirectionPin, byte motorPwmPin, int pwmMin, int pwmMax)
{
    //Serial.println("");
    long distance = getDistance(triggerPin, echoPin);
    //Serial.print(previousSpeed);Serial.print(" : ");Serial.print(distance);
    if (distance < SENSOR_MIN || distance > SENSOR_MAX)
    {
        //Serial.print(" OOR");
        return previousSpeed;
    }
    int calculatedSpeed = 0;
    if (distance <= SENSOR_MID)
    {
        calculatedSpeed = map(distance, SENSOR_MIN, SENSOR_MID, pwmMax, pwmMin);
        //Serial.print(" <");Serial.print(calculatedSpeed);
    }
    else
    {
        calculatedSpeed = -map(distance, SENSOR_MID, SENSOR_MAX, pwmMin, pwmMax);
        //Serial.print(" >");Serial.print(calculatedSpeed);
    }
    // if prevSpeed was 0, no need to compensate calcSpeed, just apply. else...
    if (calculatedSpeed != previousSpeed)
    {
        // if new direction is opposite previous, stop motor, to avoid sudden reversing
        if ((previousSpeed < 0 && calculatedSpeed > 0) || (previousSpeed > 0 && calculatedSpeed < 0))   // logical XOR
        {
            calculatedSpeed = 0;
            //Serial.print(" rev ");
        }
        // apply speed and direction
        //Serial.print(" apply "); Serial.print(calculatedSpeed);
        //Serial.print((calculatedSpeed < 0)? "<<" : ">>");
        digitalWrite(motorDirectionPin, (calculatedSpeed < 0));
        analogWrite(motorPwmPin, abs(calculatedSpeed));
    }
    else
    {
        //Serial.print(" no change");
    }
    return calculatedSpeed;
}

long getDistance(byte triggerPin, byte echoPin)
{
    long distance, duration;
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration / 2) / 29.1;
    return distance;
}
