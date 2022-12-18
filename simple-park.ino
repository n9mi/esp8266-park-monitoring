#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define BTN D0
#define LED_A D1    
#define LED_B D2
#define LED_C D3
#define GATE D8
#define IR_A D4
#define IR_B D5
#define IR_C D6
#define LIGHT_1 D7
#define LDR_1 A0

int btnState = 0;
int valA = 0;                              
int valB = 0;                              
int valC = 0;          

const char* ssid = "Bot_Ext";
const char* password = "imoan12345";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

char* getAvailableSlotUrl = "https://simple-park-web.000webhostapp.com:80/api/available-slot";
char* getSlotStatuses = "https://simple-park-web.000webhostapp.com:80/api/check-slots";
char* postCheckinUrl1 = "https://simple-park-web.000webhostapp.com:80/api/check-in/1";
char* postCheckinUrl2 = "https://simple-park-web.000webhostapp.com:80/api/check-in/2";
char* postCheckinUrl3 = "https://simple-park-web.000webhostapp.com:80/api/check-in/3";
char* postCheckoutUrl1 = "https://simple-park-web.000webhostapp.com:80/api/check-out/1";
char* postCheckoutUrl2 = "https://simple-park-web.000webhostapp.com:80/api/check-out/2";
char* postCheckoutUrl3 = "https://simple-park-web.000webhostapp.com:80/api/check-out/3";
unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

String sensorReadings;
float sensorReadingsArr[3];
        
void setup() {
  Serial.begin(9600);
  pinMode(BTN, INPUT);
  pinMode(GATE, OUTPUT); 
  pinMode(LED_A, OUTPUT);    
  pinMode(LED_B, OUTPUT);  
  pinMode(LED_C, OUTPUT);   
  pinMode(LIGHT_1, OUTPUT);  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.setTimeOffset(0);
 
  Serial.println("Delay 3s");
}

void loop() {
  timeClient.update();
  int currentSecond = timeClient.getSeconds();
  
  btnState = digitalRead(BTN);
  valA = digitalRead(IR_A);
  valB = digitalRead(IR_B);
  valC = digitalRead(IR_C);

  if (btnState == HIGH) {
    Serial.println("Button on");
    delay(2000);
    if ((millis() - lastTime) > timerDelay) {
      if(WiFi.status()== WL_CONNECTED) {
        Serial.println("connected");
        sensorReadings = httpGETRequest(getAvailableSlotUrl);
        JSONVar myObject = JSON.parse(sensorReadings);
        Serial.println(myObject); 
        
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }

        unsigned int selectedSlot = myObject["selected_slot"];
        if (selectedSlot > 0) {
          Serial.println("Bukakan gerbang!");
          digitalWrite(GATE, HIGH);
          delay(2000);
          digitalWrite(GATE, LOW);
        }

        if (selectedSlot == 1) {
          while (valA) {
            turn_A();
            Serial.println("Val A");          
            Serial.println(valA);            
            delay(5000);
            valA = digitalRead(IR_A);
          }
          digitalWrite(LED_A, LOW);
          if (httpPOSTRequest(postCheckinUrl1)) {
            Serial.println("Mobil berhasil terparkir di SLOT A"); 
          } else {
            Serial.println("Mobil gagal terparkir di SLOT A");
          } 
        } else if (selectedSlot == 2) {
          while (valB) {
            turn_B();
            Serial.println("Val B");          
            Serial.println(valB);            
            delay(5000);
            valB = digitalRead(IR_B);
          }
          digitalWrite(LED_B, LOW);
          if (httpPOSTRequest(postCheckinUrl2)) {
            Serial.println("Mobil berhasil terparkir di SLOT B"); 
          } else {
            Serial.println("Mobil gagal terparkir di SLOT B");
          } 
        } else if (selectedSlot == 3) {
          while (valC) {
            turn_C();
            Serial.println("Val C");          
            Serial.println(valC);            
            delay(5000);
            valC = digitalRead(IR_C);
          }
          digitalWrite(LED_C, LOW);
          if (httpPOSTRequest(postCheckinUrl3)) {
            Serial.println("Mobil berhasil terparkir di SLOT C"); 
          } else {
            Serial.println("Mobil gagal terparkir di SLOT C");
          } 
        } else if (selectedSlot == 0) {
          Serial.println("Tidak ada slot yang tersedia");  
        }
      } 
    } else {
      Serial.println("WiFi Disconnected");
    }
  } else {
    if (currentSecond != 0 && currentSecond % 5 == 0) {
      int light = analogRead(LDR_1);

      if (light > 250) {
        digitalWrite(LIGHT_1, LOW);
      } else {
        digitalWrite(LIGHT_1, HIGH);
      }

      valA = digitalRead(IR_A);
      valB = digitalRead(IR_B);
      valC = digitalRead(IR_C);

      sensorReadings = httpGETRequest(getSlotStatuses);
      JSONVar myObject = JSON.parse(sensorReadings);

      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      
      unsigned int statusSlotA = myObject["1"]; 
      unsigned int statusSlotB = myObject["2"]; 
      unsigned int statusSlotC = myObject["3"]; 

      if (statusSlotA && valA) {
        if (httpPOSTRequest(postCheckoutUrl1)) {
          Serial.println("Mobil berhasil keluar dari SLOT A"); 
        } else {
          Serial.println("Mobil gagal keluar di SLOT A");
        } 
      }

      if (statusSlotB && valB) {
        if (httpPOSTRequest(postCheckoutUrl2)) {
          Serial.println("Mobil berhasil keluar dari SLOT B"); 
        } else {
          Serial.println("Mobil gagal keluar di SLOT B");
        } 
      }

      if (statusSlotC && valC) {
        if (httpPOSTRequest(postCheckoutUrl3)) {
          Serial.println("Mobil berhasil keluar dari SLOT C"); 
        } else {
          Serial.println("Mobil gagal keluar di SLOT C");
        } 
      }      
    }

    if (currentSecond == 1000) {
      currentSecond = 0;
    }
  }
  lastTime = millis();
}


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
 
  http.end();

  return payload;
}

boolean httpPOSTRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  boolean result;

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  String httpRequestData = "";
  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode>0) {
    result = true;
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    result = false;
  }
 
  http.end();
  return result;
}

void turn_A()
{
 digitalWrite(LED_A, HIGH);
 digitalWrite(LED_B, LOW);
 digitalWrite(LED_C, LOW);
}
 
void turn_B()
{
 digitalWrite(LED_A, LOW);
 digitalWrite(LED_B, HIGH);
 digitalWrite(LED_C, LOW);
}
 
void turn_C()
{
 digitalWrite(LED_A, LOW);
 digitalWrite(LED_B, LOW);
 digitalWrite(LED_C, HIGH);
}
