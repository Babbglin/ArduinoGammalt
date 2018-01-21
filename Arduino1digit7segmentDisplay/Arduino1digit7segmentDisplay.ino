int c = 12;
int d = 11;

int e = 10;

int g = 9;

int f = 8;

int a = 7;

int b = 6;

int chrono = 1000;

void zero(){

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

digitalWrite(c, HIGH);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(f, HIGH);

digitalWrite(g, LOW);

}

void one(){

digitalWrite(c, HIGH);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(g, LOW);

digitalWrite(f, LOW);

digitalWrite(a, LOW);

digitalWrite(b, HIGH);

}

void two(){

digitalWrite(c, LOW);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(g, HIGH);

digitalWrite(f, LOW);

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

}

void three(){

digitalWrite(c, HIGH);

digitalWrite(d, HIGH);

digitalWrite(e, LOW);

digitalWrite(g, HIGH);

digitalWrite(f, LOW);

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

}

void four(){

digitalWrite(c, HIGH);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(g, HIGH);

digitalWrite(f, HIGH);

digitalWrite(a, LOW);

digitalWrite(b, HIGH);

}

void five(){

digitalWrite(a, HIGH);

digitalWrite(b, LOW);

digitalWrite(c, HIGH);

digitalWrite(d, HIGH);

digitalWrite(e, LOW);

digitalWrite(f, HIGH);

digitalWrite(g, HIGH);

}

void six(){

digitalWrite(a, LOW);

digitalWrite(b, LOW);

digitalWrite(c, HIGH);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(f, HIGH);

digitalWrite(g, HIGH);

}

void seven(){

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

digitalWrite(c, HIGH);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(f, LOW);

digitalWrite(g, LOW);

}

void eight(){

digitalWrite(c, HIGH);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(g, HIGH);

digitalWrite(f, HIGH);

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

}

void nine(){

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

digitalWrite(c, HIGH);

digitalWrite(d, HIGH);

digitalWrite(e, LOW);

digitalWrite(f, HIGH);

digitalWrite(g, HIGH);

}

void setup() {

pinMode(c, OUTPUT);

pinMode(d, OUTPUT);

pinMode(e, OUTPUT);

pinMode(g, OUTPUT);

pinMode(f, OUTPUT);

pinMode(a, OUTPUT);

pinMode(b, OUTPUT);

}

void loop() {

zero();

delay(chrono);

one();

delay(chrono);

two();

delay(chrono);

three();

delay(chrono);

four();

delay(chrono);

five();

delay(chrono);

six();

delay(chrono);

seven();

delay(chrono);

eight();

delay(chrono);

nine();

delay(chrono);

}
