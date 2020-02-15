// Starting of Program

int led = 8;
char val;
int light;

void setup()
{

pinMode(led, OUTPUT);  // Digital pin 13 set as output Pin
Serial.begin(9600);
digitalWrite(led, HIGH);
delay(1000);
digitalWrite(led, LOW);
pinMode(A0,INPUT);

}

void loop()
{
  light=analogRead(A0);
//  Serial.println("Light = " + String(light));
  delay(500);
  if( Serial.available()){
    delay(10);
    Serial.print(light);
  }
  
  while (Serial.available() > 0)
  {
  val = Serial.read();
  Serial.println(val);
  }

  if( val == '1') // Forward
    {
     
      digitalWrite(led, HIGH);
    }
  else if(val == '2') // Backward
    {
 
      digitalWrite(led, LOW);
    }

   
}

// End of program
