int switchState = 0;
void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, INPUT);
}

void loop(){
  switchState = digitalRead(2);
  if(switchState == LOW){
  // the button is not pressed

  digitalWrite(3, HIGH); //green LED
  digitalWrite(4, LOW); //Red LED
  digitalWrite(5, LOW); //red LED
  }

  else{
    while(true){
      //Red
    digitalWrite(5, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    delay(2000);

    //Green
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    delay(1000);

    //Yellow
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(3, LOW);
    delay(250);
    }
  }
 }
