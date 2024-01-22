#include "ThingSpeak.h"
#include <NewPing.h>
#include <DHT.h> 
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

        LiquidCrystal_I2C lcd(0x27,16,2);


      //**Pin Defination**//
    const int trigPin1 = D7;
    const int echoPin1 = D8;
    const int trigPin2 = D7;
    const int echoPin2 = D8;
    
          #define redled D4
          #define grnled D5
          #define BUZZER D6
          #define DHTPIN 0 
     
        DHT dht(DHTPIN, DHT11);

    unsigned long ch_no = 1553350;
    const char * write_api = "08RQNW3UO118R50V";
             String apiKey = "08RQNW3UO118R50V"; 
             
       char auth[] = "mwa0000024529869";
       char ssid[] = "Anik Hotspot";
       char pass[] = "Anik1234";
const char* server = "api.thingspeak.com";

    unsigned long startMillis;
    unsigned long currentMillis;
    const unsigned long period = 10000;

    WiFiClient  client;

    //**Variaable**//
    float duration1;
    float distance1;
    float duration2;
    float distance2;
    float rate;
    int alart;
    int R;
    int r;
    
void setup()
{
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(redled, OUTPUT);
  pinMode(grnled, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(R, INPUT);
  
  Serial.begin(115200);
       delay(10);
       dht.begin();
       lcd.init();
       lcd.backlight();
       
         
       
       

       
          WiFi.begin(ssid, pass);
          while (WiFi.status() != WL_CONNECTED)
          {
            delay(500);
            Serial.print(".");
            lcd.clear();
            lcd.print("............");
            delay(2000);
          }
              Serial.println("WiFi connected");
              Serial.println(WiFi.localIP());
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("WiFi connected");
              lcd.setCursor(2,1);
              lcd.print(WiFi.localIP());
              delay(2000);
              ThingSpeak.begin(client);
              startMillis = millis();
}


void loop()
{
  //****Water Level Calculation****//
  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  R = analogRead(A0);
  r = (102-(R/10));
  
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = 58.48-duration1*(0.34/2);
  Serial.println(" ");
  Serial.print("Water Level: ");
  Serial.print(distance1);
  Serial.print(" cm");
  Serial.println(" ");

  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(" Water Level ");
  lcd.setCursor(5,1);
  lcd.print(distance1);
  lcd.setCursor(9,1);
  lcd.print("cm");
  
  delay(2000);

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = 58.48-duration2*(0.34/2);

  rate = (distance1-distance2)/2;
  Serial.println(" ");
  Serial.print("Water Level increase rate: ");
  Serial.print(rate);
  Serial.print(" cm/S");
  Serial.println(" ");

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(" Increase rate ");
  lcd.setCursor(2,1);
  lcd.print(rate);
  lcd.setCursor(7,1);
  lcd.print("cm/S");
  delay(2000);

 
          if (distance1 >= 25)
          {
            digitalWrite(redled, HIGH);
            tone(BUZZER, 300);
            digitalWrite(grnled, LOW);
            delay(1500);
            noTone(BUZZER);
            alart = 1;
          }
          else
          {
            digitalWrite(grnled, HIGH);
            digitalWrite(redled, LOW);
           alart = 0;
          }
          
          currentMillis = millis();
          
          if (currentMillis - startMillis >= period)
          {
            ThingSpeak.setField(1, distance1);
            ThingSpeak.setField(6, rate);
            ThingSpeak.writeFields(ch_no, write_api);
            startMillis = currentMillis;
          }

   //**** Temprature, Humidity & Rain Calculation****//
   
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      
              
      
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
                  if (client.connect(server,80)) 
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field2=";
                             postStr += String(r);
                             postStr +="&field3=";
                             postStr += String(t);
                             postStr +="&field4=";
                             postStr += String(h);
                             postStr +="&field5=";
                             postStr += String(alart);
                             postStr += "\r\n\r\n\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.print(" % Rain: ");
                             Serial.print(r);
                             Serial.print(" mm Alart: ");
                             Serial.print(alart);
                             Serial.println(" Send to Thingspeak.");

                              lcd.clear();
                              lcd.setCursor(2,0);
                              lcd.print(" Temperature ");
                              lcd.setCursor(4,1);
                              lcd.print(t);
                              lcd.setCursor(8,1);
                              lcd.print("deg C");
                              delay(2000);

                              lcd.clear();
                              lcd.setCursor(4,0);
                              lcd.print(" Humidity ");
                              lcd.setCursor(5,1);
                              lcd.print(h);
                              lcd.setCursor(9,1);
                              lcd.print("%");
                              delay(2000);

                              lcd.clear();
                              lcd.setCursor(5,0);
                              lcd.print(" Rain ");
                              lcd.setCursor(5,1);
                              lcd.print(r);
                              lcd.setCursor(9,1);
                              lcd.print("mm");
                              delay(2000);

                              
                        }
          client.stop();
 
          Serial.println("Waiting...");
}
