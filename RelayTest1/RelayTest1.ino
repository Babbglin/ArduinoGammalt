int Relay = 7;
int stateRelay = LOW;

 
void setup() {
  pinMode(Relay, OUTPUT);
}
 
void loop() {
 digitalWrite(Relay,LOW); // Turns ON Relays 1
Serial.println("Light ON");
delay(2000); // Wait 2 seconds
digitalWrite(Relay,HIGH); // Turns Relay Off
Serial.println("Light OFF");
delay(2000);
}
