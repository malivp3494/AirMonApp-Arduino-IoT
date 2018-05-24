#include "DHT.h"
#define DHTTYPE DHT11 
#include<ESP8266WiFi.h>
#include<FirebaseArduino.h>
#include "UbidotsMicroESP8266.h"
#define TOKEN "UBIDOTS_TOKEN"
Ubidots client(TOKEN);
#define FIREBASE_HOST "FIREBASE_URL_HERE"
#define FIREBASE_AUTH "SECRET_SERVER_KEY"
#define WIFI_SSID "NAME_OF_WIFI_TO_CONNECT_NODEMCU"
#define WIFI_PASSWORD "WIFI_PASSWORD"
 
float hum;
float temp;
 
DHT dht(D7, DHTTYPE);

void setup(){
    Serial.begin(9600);
    delay(10);
    pinMode(A0,INPUT);
    dht.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.set("humidity/name","humidity");
  Firebase.set("humidity/status","NORMAL");
  Firebase.set("humidity/value",0);
  Firebase.set("temperature/name","temperature");
  Firebase.set("temperature/status","NORMAL");
  Firebase.set("temperature/value",0);
  Firebase.set("smoke/name","smoke");
  Firebase.set("smoke/status","NORMAL");
  Firebase.set("smoke/value",0);
}

void firebasereconnect(){
  Serial.println("Trying to reconnect");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop()
{
    if(Firebase.failed()){
    Serial.print("setting number failed");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }
  
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  int smoke=analogRead(A0);
  delay(1000);

  if(hum<30 || hum>70){
    Firebase.set("humidity/name","humidity");
    Firebase.set("humidity/status","DANGER");
    Firebase.set("humidity/value",hum);
  }
  else if(hum>30 && hum<70){
    Firebase.set("humidity/name","humidity");
    Firebase.set("humidity/status","NORMAL");
    Firebase.set("humidity/value",hum);
  }

  if(temp<10 || temp>40){
    Firebase.set("temperature/name","temperature");
    Firebase.set("temperature/status","DANGER");
    Firebase.set("temperature/value",temp);
  }
  else if(temp>10 && temp<40){
    Firebase.set("temperature/name","temperature");
    Firebase.set("temperature/status","NORMAL");
    Firebase.set("temperature/value",temp);
  }

  if(smoke>250){
    Firebase.set("smoke/name","smoke");
    Firebase.set("smoke/status","DANGER");
    Firebase.set("smoke/value",smoke);
  }
  else if(smoke<250){
    Firebase.set("smoke/name","smoke");
    Firebase.set("smoke/status","NORMAL");
    Firebase.set("smoke/value",smoke);
  }
  
  Serial.print(temp);
  Serial.print("\t");
  Serial.print(hum);
  Serial.print("\t");
  Serial.print(smoke);
  Serial.println();
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  {
    client.add("Hum",hum);
    client.add("Tmp",temp);
    client.add("Smoke",smoke);
    client.sendAll(true);
    delay(1000);  
  }

  }

