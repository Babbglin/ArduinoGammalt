/*
HC-SR04 Ping distance sensor:
VCC to arduino 5v
GND to arduino GND
Echo to Arduino pin 7
Trig to Arduino pin 8

This sketch originates from Virtualmix: http://goo.gl/kJ8Gl
Has been modified by Winkle ink here: http://winkleink.blogspot.com.au/2012/05/arduino-hc-sr04-ultrasonic-distance.html
And modified further by ScottC here: http://arduinobasics.blogspot.com.au/2012/11/arduinobasics-hc-sr04-ultrasonic-sensor.html
on 10 Nov 2012.
*/


#define echoPin 53 // Echo Pin
#define trigPin 52 // Trigger Pin
#define LEDPin 13 // Onboard LED

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
float duration, distance; // Duration used to calculate distance

void setup() {
	Serial.begin(9600);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
}

void loop() {
	/* The following trigPin/echoPin cycle is used to determine the
	distance of the nearest object by bouncing soundwaves off of it. */
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);

	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);

	/*Calculate the distance (in cm) based on the speed of sound.
	Ljudets hastighet:
	m/s			graderC
	340.276		15
	343.216		20
	346.13		25	
		*/
	/*For example, if the object is 10 cm away from the sensor, 
	and the speed of the sound is 340 m/s or 0.034 cm / µs the sound wave will need to travel about 294 u seconds.
	But what you will get from the Echo pin will be double that number because the sound wave needs to travel forward and bounce backward.
	So in order to get the distance in cm we need to multiply the received travel time value from the echo pin by 0.034 and divide it by 2.*/

	// ljudets hastighet för 25 graderC, -0,55 är kompensationen, för jag anser att den mäter från högtalarens yttersta kant
	distance = duration*0.034613 / 2 - 0.55;

	if (distance >= maximumRange || distance <= minimumRange) {
		/* Send a negative number to computer and Turn LED ON
		to indicate "out of range" */
		Serial.println("-1");
		digitalWrite(LEDPin, HIGH);
	}
	else {
		/* Send the distance to the computer using Serial protocol, and
		turn LED OFF to indicate successful reading. */
		Serial.println(distance);
		digitalWrite(LEDPin, LOW);
	}

	//Delay 50ms before next reading.
	delay(1000);
}