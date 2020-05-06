
/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>
#include <HttpClient.h>
#include <Bridge.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <stdlib.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//Constants
#define DHTPIN 7          // what pin we're connected to
#define DHTTYPE DHT22     // DHT 22  (AM2302)
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

HttpClient client; // For talking with the thingspeak api
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Variables
int error;
int pulsePin = 1;  // Pulse Sensor purple wire connected to analog pin 1
int blinkPin = 13; // pin to blink led at each beat

int redIndicatorPin = 4;
int greenIndicatorPin = 5;

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;               // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;            // holds the incoming raw data
volatile int IBI = 600;         // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false; // "True" when heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;    // becomes true when Arduino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;       // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse
volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0; // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;  // used to find IBI
volatile int P = 512;                     // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 525;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM


boolean LIGHT_SENSOR_ON = true;
char command;

void setup()
{
    Bridge.begin();
    pinMode(redIndicatorPin, OUTPUT);
    pinMode(greenIndicatorPin, OUTPUT);

    //  LCD
    lcd.begin();
    lcd.setBacklight((uint8_t)1);
    lcd.print("Hello!");
    delay(100);
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");

    temp_hum_setup();
    light_setup();
    dust_setup();
    barometer_setup();

    Serial.begin(9600);

    delay(1000);

    interruptSetup();
}

void loop()
{
    while (Serial.available() > 0){
      command = Serial.read();
      }
    if(command == "1"){
      LIGHT_SENSOR_ON = !LIGHT_SENSOR_ON
    }
    if(LIGHT_SENSOR_ON){
      light_loop();
    }
    temp_hum_loop();
    dust_loop();
    barometer_loop();
    update_all();
    lcd_out();
}

void temp_hum_setup()
{
    dht.begin();
}

void temp_hum_loop()
{
    delay(500);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    delay(500); //Delay 2 sec.
}

void barometer_setup()
{
    Serial.println(F("BMP280 test"));

    while (!bmp.begin())
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    }
}

void barometer_loop()
{
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

    delay(2000);
}

void light_setup()
{
    pinMode(A0, INPUT);
}

void light_loop()
{
    light = analogRead(A0);
    Serial.println("Light = " + String(light));
    delay(500);
    if (light < 100)
    {
        digitalWrite(greenIndicatorPin, HIGH);
        digitalWrite(redIndicatorPin, LOW);
    }
    else if (light > 100)
    {
        digitalWrite(greenIndicatorPin, LOW);
        digitalWrite(redIndicatorPin, HIGH);
        lcd.print("Warning!!!");
    }
    else
    {
        digitalWrite(greenIndicatorPin, LOW);
        digitalWrite(redIndicatorPin, LOW);
    }
}

void dust_setup()
{
    pinMode(8, INPUT);
    starttime = millis();
}

void dust_loop()
{
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
    if ((millis() - starttime) >= sampletime_ms) //if the sampel time = = 30s
    {
        ratio = lowpulseoccupancy / (sampletime_ms * 10.0);
        concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
        Serial.print("Concentration = ");
        Serial.print(concentration);
        Serial.println(" pcs/0.01cf");
        Serial.println("\n");
        lowpulseoccupancy = 0;
        starttime = millis();
    }
}


} // end isr
