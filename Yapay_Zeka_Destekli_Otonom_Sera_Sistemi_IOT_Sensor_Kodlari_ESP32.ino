#include <WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SPI.h>
#include <SFE_BMP180.h>
#include <WiFiClient.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

unsigned long previousMillis = 0;
unsigned long interval = 100;
  float shuntvoltage = 0;
  float busvoltage = 0;
  float Akim_mA = 0;
  float Lvoltage = 0;
  float Guc_mW = 0;
  float energy = 0;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

SFE_BMP180 bmp180;
#define DHT_SENSOR_PIN 2
#define DHT_SENSOR_TYPE DHT11



LiquidCrystal lcd(19, 23, 18, 17, 16, 15);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

WiFiClient client;

long myChannelNumber = 1989869;
const char myWriteAPIKey[] = "9KBC51WPR1YOGCTI";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 WiFi.begin("ilyasancar", "ilyasm12323");
dht_sensor.begin();  // initialize the DHT sensor
  lcd.begin(16, 2);
  bmp180.begin();
  
 

  Wire.begin();

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);

 
 while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

  uint32_t currentFrequency;
    
  Serial.println("Hello!");
  
  
  
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) 
    { 
      delay(10); 
      }
  }
  
  //ina219.setCalibration_32V_1A();


  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop() {
  
  //*************** VOLTAJ İNA219**********
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
 
 
  }

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  Akim_mA = ina219.getCurrent_mA();
  Guc_mW = ina219.getPower_mW();
  Lvoltage = busvoltage + (shuntvoltage / 4.2);
  energy = energy + Lvoltage * Akim_mA / 3600;
  
  Serial.print("Bus Voltage:   "); 
  Serial.print(busvoltage); 
  Serial.println(" V");
  Serial.print("Shunt Voltage: "); 
  Serial.print(shuntvoltage); 
  Serial.println(" mV");
  Serial.print("Load Voltage:  "); 
  Serial.print(Lvoltage); 
  Serial.println(" V");
  Serial.print("Current:       "); 
  Serial.print(Akim_mA); 
  Serial.println(" mA");
  Serial.print("Power:         "); 
  Serial.print(Guc_mW); 
  Serial.println(" mW");
  Serial.println("");
delay(3000);  

  
 
  
  //**************BMP180*******************
 
if(!bmp.begin()) {
Serial.print("Failed to read from BMP sensor!!");
while(1);
}

sensors_event_t event;
bmp.getEvent(&event);

Serial.print("Pressure:         ");
Serial.print(event.pressure);
Serial.println(" hPa");

float temperature;
bmp.getTemperature(&temperature);
Serial.print("Temperature:      ");
Serial.print(temperature);
Serial.println(" degrees Celcius ");

//--- extra----you can measure the altitude with the temperature and the air pressure

float seaLevelPressure = 1015;
Serial.print("Altitude:         "); 
Serial.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure)); 
Serial.println(" m");


//***************************DHT11*************************************

  float humidity = dht_sensor.readHumidity();     // read humidity
  float t = dht_sensor.readTemperature();  // read temperature
  int chk = dht_sensor.read(DHT_SENSOR_PIN);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ilyas Sancar ");
  lcd.setCursor(0, 1);
  lcd.print("IZZETGIL");

  delay(2500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Yenilenebilir");
  lcd.setCursor(0, 1);
  lcd.print("Enerji");

  delay(2500);  

  lcd.clear();
  lcd.setCursor(0, 0);  // display position
  lcd.print("Sicaklik:");
  lcd.print(temperature);  // display the temperature
  lcd.print(" C");


  lcd.setCursor(0, 1);  // display position
  lcd.print("Nem:");
  lcd.print(humidity);  // display the humidity
  lcd.print("%");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Basinc:");
  lcd.print(event.pressure);
  lcd.println("hPa");
  
  lcd.setCursor(0, 1);
  lcd.print("Rakim:");
  lcd.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure));
  delay(2000);
  
  //// VOLTAJ AMPER/////
  

 lcd.clear();
  lcd.setCursor(0, 0); 
 lcd.print("Gerilim :");
 lcd.print(Lvoltage);
 lcd.print("V ");

lcd.setCursor(0, 1); 
 lcd.print("Akim :");
 lcd.print(Akim_mA);
 lcd.print("mA ");
 delay(2000); 

 lcd.clear();
  lcd.setCursor(0, 0); 
 lcd.print("Guc :");
 lcd.print(Guc_mW);
 lcd.print("mW ");
delay(100);  


  Serial.print("Nem: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Basinc: ");
  Serial.print(event.pressure);
  Serial.println(" hPa");
  Serial.print("Sicaklik: ");
  Serial.print(temperature);
  Serial.println(" C");
  
   ThingSpeak.setField(1, temperature);
   ThingSpeak.setField(2, event.pressure);
   ThingSpeak.setField(3, Akim_mA);
   ThingSpeak.setField(4, Lvoltage);
   ThingSpeak.setField(5, Guc_mW);
   ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
//ThingSpeak.writeField (myChannelNumber, 1, temperature, myWriteAPIKey);
//ThingSpeak.writeField (myChannelNumber, 2, event.pressure, myWriteAPIKey);  
//ThingSpeak.writeField (myChannelNumber, 3, Akim_mA, myWriteAPIKey);
//ThingSpeak.writeField (myChannelNumber, 4, Lvoltage, myWriteAPIKey);  
//ThingSpeak.writeField (myChannelNumber, 5, Guc_mW, myWriteAPIKey);  
delay(10);
}




  
