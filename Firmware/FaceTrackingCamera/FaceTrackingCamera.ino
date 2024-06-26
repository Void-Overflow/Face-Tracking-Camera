int NoReceptionElapsed = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  while (!Serial)
  {
  } // wait for serial port to connect
  delay(250);
  Serial.println("Connected to Face Tracking Camera Receiver!");
}
void loop()
{
  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    digitalWrite(LED_BUILTIN, LOW);

    int x = data.substring(0, data.indexOf("|")).toInt();
    int y = data.substring(data.indexOf("|") + 1).toInt();

    Serial.println("Device Received Coordinates (" + String(x) + "," + String(y) + ")");
    NoReceptionElapsed = millis();
  }
  else if (!Serial.available() && millis() - NoReceptionElapsed >= 2500) // if not connected to device for at least 2.5 seconds
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    Serial.println("Connected to Face Tracking Camera Receiver!");
  }
  else if (millis() - NoReceptionElapsed < 2500) // if it has been connected in the previous 2.5 seconds
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
