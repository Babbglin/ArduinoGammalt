int c = 12;
int d = 11;

int e = 10;

int g = 9;

int f = 8;

int a = 7;

int b = 6;

int chrono = 1000;

void zero(){

digitalWrite(a, LOW);

digitalWrite(b, LOW);

digitalWrite(c, LOW);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(f, LOW);

digitalWrite(g, HIGH);

}

void one(){

digitalWrite(c, LOW);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(g, HIGH);

digitalWrite(f, HIGH);

digitalWrite(a, HIGH);

digitalWrite(b, LOW);

}

void two(){

digitalWrite(c, HIGH);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(g, LOW);

digitalWrite(f, HIGH);

digitalWrite(a, LOW);

digitalWrite(b, LOW);

}

void three(){

digitalWrite(c, LOW);

digitalWrite(d, LOW);

digitalWrite(e, HIGH);

digitalWrite(g, LOW);

digitalWrite(f, HIGH);

digitalWrite(a, LOW);

digitalWrite(b, LOW);

}

void four(){

digitalWrite(c, LOW);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(g, LOW);

digitalWrite(f, LOW);

digitalWrite(a, HIGH);

digitalWrite(b, LOW);

}

void five(){

digitalWrite(a, LOW);

digitalWrite(b, HIGH);

digitalWrite(c, LOW);

digitalWrite(d, LOW);

digitalWrite(e, HIGH);

digitalWrite(f, LOW);

digitalWrite(g, LOW);

}

void six(){

digitalWrite(a, HIGH);

digitalWrite(b, HIGH);

digitalWrite(c, LOW);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(f, LOW);

digitalWrite(g, LOW);

}

void seven(){

digitalWrite(a, LOW);

digitalWrite(b, LOW);

digitalWrite(c, LOW);

digitalWrite(d, HIGH);

digitalWrite(e, HIGH);

digitalWrite(f, HIGH);

digitalWrite(g, HIGH);

}

void eight(){

digitalWrite(c, LOW);

digitalWrite(d, LOW);

digitalWrite(e, LOW);

digitalWrite(g, LOW);

digitalWrite(f, LOW);

digitalWrite(a, LOW);

digitalWrite(b, LOW);

}

void nine(){

digitalWrite(a, LOW);

digitalWrite(b, LOW);

digitalWrite(c, LOW);

digitalWrite(d, LOW);

digitalWrite(e, HIGH);

digitalWrite(f, LOW);

digitalWrite(g, LOW);

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
