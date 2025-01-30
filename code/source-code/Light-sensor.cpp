//from https://how2electronics.com/how-to-use-ldr-sensor-module-with-arduino/
// Define pin numbers
const int ledPin = 2;      // LED connected to digital pin 8
const int ldrPin = A0;     // LDR connected to analog pin A0
 
void setup() 
{
  pinMode(ledPin, OUTPUT); // Set the LED pin as output
  Serial.begin(9600);      // Start serial communication at 9600 bps
}
 
void loop() 
{
  int ldrValue = analogRead(ldrPin);  // Read the value from the LDR
  Serial.println(ldrValue);           // Print the value to the serial monitor
 
  // Assuming a lower value means more light
  if (ldrValue > 600) 
  {   
    digitalWrite(ledPin, HIGH);  // Turn the LED on
  } 
  else 
  {
    digitalWrite(ledPin, LOW);   // Turn the LED off
  }
 
  delay(500);  // Wait for half a second
}
