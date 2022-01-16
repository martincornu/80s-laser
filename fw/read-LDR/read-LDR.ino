#define LDRpin A0 // pin where we connected the LDR and the resistor

int LDRValue = 0;     // result of reading the analog pin

void setup() {
  Serial.begin(9600); // sets serial port for communication
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  LDRValue = analogRead(LDRpin); // read the value from the LDR
  Serial.println(LDRValue);      // print the value to the serial port
  delay(500);                    // wait a little
}
