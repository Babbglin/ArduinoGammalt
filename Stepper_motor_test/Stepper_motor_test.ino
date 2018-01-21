#include <StepperMotor.h>

String incomingByte;
StepperMotor motor(8,9,10,11);

void setup(){
 Serial.begin(9600);
 motor.setStepDuration(1);
}

void loop(){
 Serial.print("Enter rotation in ms (minus to change direction)");
  if (Serial.available() > 0) {
    incomingByte = Serial.readString(); // read the incoming byte:
  }
 //4075 = 1 varv vad jag sett hittils
 //motor.step verkar vara minsta 100 sen valfritt värde. T.ex 136 går bra osv
  
 Serial.println();
 Serial.print("Current roration value is ");
 Serial.println(incomingByte);
 
 motor.step(incomingByte.toInt());
 delay(2000);
}
