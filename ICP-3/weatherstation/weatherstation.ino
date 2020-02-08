/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>
#include <HttpClient.h>
#include <Bridge.h>
#include <LiquidCrystal_I2C.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
const int buzzer = 10;

//Temp vars
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

//Light vars
int light;

//barometer vars
Adafruit_BMP280 bmp; // I2C

//Dust vars
int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;


String temp_msg = "http://api.thingspeak.com/update?key=39A4LSNDUISGO0UJ&field1="; //field specific
String humidity_msg = "http://api.thingspeak.com/update?key=39A4LSNDUISGO0UJ&field2="; //field specific
String bar_msg = "http://api.thingspeak.com/update?key=39A4LSNDUISGO0UJ&field3=";
String light_msg = "http://api.thingspeak.com/update?key=39A4LSNDUISGO0UJ&field4=";
String dust_msg = "http://api.thingspeak.com/update?key=39A4LSNDUISGO0UJ&field5=";

HttpClient client; // For talking with the thingspeak api
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Bridge.begin();

//  LCD
  lcd.begin();
  lcd.setBacklight((uint8_t)1);
  lcd.print("Hello!"); 
  delay(100);
  lcd.setCursor(0,1);
  lcd.print("Connecting...");

  pinMode(buzzer, OUTPUT);
  
  temp_hum_setup();
  light_setup();
  dust_setup();
  barometer_setup();
  
}

void loop()
{
  temp_hum_loop();
  light_loop();
  dust_loop();
  barometer_loop();
  update_all();
  lcd_out();
}

void updateTemp(){
  String request = "";
  request += temp_msg;
  request += String(temp);
  Serial.println(request);
  client.get(request);
}

void updateHumidity(){
  String request = "";
  request += humidity_msg;
  request += String(hum);
  Serial.println(request);
  client.get(request);
}

void updateBarometer(){
  String request = "";
  request += bar_msg;
  request += String(bmp.readPressure());
  Serial.println(request);
  client.get(request);
}


void updateLight(){
  String request = "";
  request += light_msg;
  request += String(light);
  Serial.println(request);
  client.get(request);
}

void updateDust(){
  String request = "";
  request += dust_msg;
  request += String(concentration);
  Serial.println(request);
  client.get(request);
}

void temp_hum_setup()
{
  Serial.begin(9600);
  dht.begin();
}

void temp_hum_loop() {
    delay(500);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
//    updateHumidity();
    temp= dht.readTemperature();
//    updateTemp();
    
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    delay(500); //Delay 2 sec.
}

void barometer_setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  
  while(!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    }
}


void barometer_loop() {
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    
    Serial.println();

//    updateBarometer();
    
    delay(2000);
}

void light_setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
}

void light_loop()
{
  light=analogRead(A0);
  Serial.println("Light = " + String(light));
  delay(500);
  if(light < 100){
    play_sound();
  }
}

void dust_setup(){
  Serial.begin(9600);
  pinMode(8,INPUT);
  starttime = millis(); 
}

void dust_loop() {
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
    Serial.print("Concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();

//    updateDust();
  }
}

void lcd_out() {

  lcd.backlight();
//  start:
//  error=0;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(hum);
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" %, Temp: ");
  lcd.print(temp);
  lcd.print(" Celsius");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Pressure = "));
  lcd.print(bmp.readPressure());
  lcd.print(" Pa");
  delay(1000);


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Approx altitude = "));
  lcd.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
  lcd.print(" m");
  delay(1000);


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Light = " + String(light));
  delay(1000);


  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Concentration = ");
  lcd.setCursor(0,1);
  lcd.print(concentration);
  lcd.print(" pcs/0.01cf");
  delay(1000);

}

void update_all(){
  String request = "http://api.thingspeak.com/update?key=39A4LSNDUISGO0UJ&field1=" + String(temp)+"&field2=" +String(hum) + "&field3=" + String(bmp.readPressure()) + "&field4=" + String(light)+ "&field6="+ String(concentration);
  client.get(request);
} 

void play_sound(){
 
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec
  
}
