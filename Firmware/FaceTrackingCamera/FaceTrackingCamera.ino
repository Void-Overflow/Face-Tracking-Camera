
#include <Stepper.h>

#define servoPin 9
Stepper stepper = Stepper(32, 2, 4, 3, 5); // Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

int NoReceptionElapsed = 0;
int reconnectionAttemptElapsed = 0;
bool isRunning = false;

int x, y;
int servoPos = 90;
bool servoDir = false;

void writeServo(int angle)
{
  // Convert the angle to microseconds (us)
  // Servo range is typically from 1000us (0 degrees) to 2000us (180 degrees)
  int pulseWidth = map(angle, 0, 180, 1000, 2000);

  // Generate the PWM signal
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(servoPin, LOW);
  delay(25 - pulseWidth / 1000); // Ensure a 20ms period
}

void idle()
{
  if (servoPos >= 180)
    servoDir = true;
  if (servoPos <= 0)
    servoDir = false;

  if (servoDir == false)
  {
    servoPos++;
    stepper.step(2);
  }
  else
  {
    servoPos--;
    stepper.step(-2);
  }

  writeServo(servoPos);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(servoPin, OUTPUT);

  stepper.setSpeed(500);

  while (!Serial) // wait for serial port to connect
  {
    idle();
  }
  servoPos = 90;

  delay(250);
  Serial.println("Connected to Face Tracking Camera Receiver!"); // message sent for c++ program to identify RPi
}

void loop()
{
  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');

    x = data.substring(0, data.indexOf("|")).toInt();
    y = data.substring(data.indexOf("|") + 1).toInt();

    Serial.println("Device Received Coordinates (" + String(x) + "," + String(y) + ")");
    NoReceptionElapsed = millis();
    isRunning = true;
  }
  else if (!Serial.available() && millis() - NoReceptionElapsed >= 2500) // if not connected to device for at least 2.5 seconds
  {
    isRunning = false;
    digitalWrite(LED_BUILTIN, LOW);

    idle();
    if (millis() - reconnectionAttemptElapsed >= 500)
    {
      Serial.println("Connected to Face Tracking Camera Receiver!"); // message sent for c++ program to identify RPi
      reconnectionAttemptElapsed = millis();
    }
  }
  else if (millis() - NoReceptionElapsed < 2500) // if it has been connected in the previous 2.5 seconds
    digitalWrite(LED_BUILTIN, HIGH);

  if (isRunning == true)
  {
    if (x > 40 || x < -40)
      stepper.step((x > 50) ? 2 : -2);

    if (y > 60)
    {
      servoPos++;
      if (servoPos > 180)
        servoPos = 180;
    }
    else if (y < -60)
    {
      servoPos--;
      if (servoPos < 0)
        servoPos = 0;
    }
    writeServo(servoPos);
  }
}
