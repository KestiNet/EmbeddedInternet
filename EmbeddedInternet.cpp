/*
 *TIES536 Sulautettu Internet
 *Harjoitus 6
 *Tekijä: Esa Kesti
 */

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <HardwareSerial.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;


WiFiMulti WiFiMulti;
HTTPClient ask;

const char* ssid     = "OurNetwork 2.4"; //Wifi SSID
const char* password = "PASSWORD REMOVED"; //Wifi Password
const char* apiKeyIn = "apiKeyIn removed";      // API KEY IN
const char* apiKeyIn2 = "apiKeyIn removed";
const unsigned int writeInterval = 1800000;   // write interval (in ms)

// ASKSENSORS API host config
const char* host = "api.asksensors.com";  // API host name
const int httpPort = 80;      // port

//Määritellään I/O pinnin numero ja alustetaan sensorValue arvo
const int sensorPin = 34;
int sensorValue = 0;
//Määritellään I/O pinnin numero ja LED pinnin numero
byte sensorPin2 = 26;
byte redPin = 4;


void setup()
{
// uusi serial yhteys TEMT6000 sensoria varten, säädetään nopeus ja viive
  Serial1.begin(115200);
  delay(1800000);

  Määrittää PIR sensorin signaalin sisääntulon ja ulosmenon
  pinMode(sensorPin2, INPUT);
  pinMode(redPin, OUTPUT);
  Serial2.begin(115200);

  // open serial
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect ESP32 to AskSensors.");
  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//Määrittää BME280 sensorin nopeuden ja alustaa sensorin
  Serial.begin(115200);
  while(!Serial); // time to get serial running
  Serial.println("BME280 test");
  bme.begin(0x76);

 }
//Aliohjelma PIR sensorin toimintaa varten, lukee digitaalisen signaalin ja kirjoittaa muuttujaan state
void tunnistus(){
  byte state = digitalRead(sensorPin2);
  digitalWrite(redPin,state);
  delay(1800000);
}

void loop(){

  // Use WiFiClient class to create TCP connections
  WiFiClient client;


if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {

//Lähettää AskSensor sensor1:selle mittausdataa
  String url = "http://api.asksensors.com/write/";
  url += apiKeyIn;
  url += "?module1=";
  url += bme.readTemperature();
  url += "&module2=";
  url += bme.readTemperature();
  url += "&module3=";
  url += bme.readHumidity();
  url += "&module4=";
  url += bme.readPressure() / 100.0F;

  //Lähettää AskSensor sensor2:selle mittausdataa
  String url2 = "http://api.asksensors.com/write/";
  url2 += apiKeyIn2;
  url2 += "?module1=";
  url2 += digitalRead(sensorPin2);
  url2 += "&module2=";
  url2 += digitalRead(sensorPin2);
  url2 += "&module3=";
  url2 += analogRead(sensorPin);

  Serial.print("********** requesting URL: ");
  Serial.println(url);
   // send data
   ask.begin(url); //Specify the URL

Serial.println(url2);
   // send data
   ask.begin(url2); //Specify the URL

    //Check for the returning code
    int httpCode = ask.GET();

    if (httpCode > 0) {

        String payload = ask.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      } else {
      Serial.println("Error on HTTP request");
    }

     ask.end(); //End
    Serial.println("********** End ");
    Serial.println("*****************************************************");

  }

  client.stop();  // stop client

  delay(writeInterval);    // delay
}
