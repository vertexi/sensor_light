int ledPin = 9;    // LED connected to digital pin 9

void setup()  { 
  pinMode(ledPin, OUTPUT);   // sets the pin as output 
} 

void loop()  { 
  digitalWrite(ledPin, HIGH);       // sets the digital pin 13 on
  delay(1000);                  // waits for a second
  digitalWrite(ledPin, LOW);        // sets the digital pin 13 off
  delay(1000);                  // waits for a second
}

// void setup()  { 
//   // nothing happens in setup
//   pinMode(ledPin, OUTPUT);   // sets the pin as output 
//   analogFrequency(50);
// } 
// 
// void loop()  { 
//   analogWrite(ledPin, 19);         // 16 one-side 19 neutral 23 another-side
//   delay(3000);
//   analogWrite(ledPin, 16);         // 16 one-side 19 neutral 23 another-side
//   delay(3000);
//   analogWrite(ledPin, 19);         // 16 one-side 19 neutral 23 another-side
//   delay(3000);
//   analogWrite(ledPin, 22);         // 16 one-side 19 neutral 23 another-side
//   delay(3000);
// }
