#include<ESP8266WiFi.h>
#include<FirebaseArduino.h>
#include "UbidotsMicroESP8266.h"

#define TOKEN "BBFF-FoZI7yhMUQVjqv8ApwfqMgBUvAyBuO"
Ubidots client(TOKEN);
#define FIREBASE_HOST "airmonapp.firebaseio.com"
#define FIREBASE_AUTH "hQ5MvC54UNK3UVNr940KqBzhFAwWGV6Tp7seW5Bu"
#define WIFI_SSID "JioFi4_06601F"
#define WIFI_PASSWORD "ze0mmp5ic9"


void setup() 
{
  pinMode(A0,INPUT);
  pinMode(D7,INPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  client.wifiConnection(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.set("rain/name","rain");
  Firebase.set("rain/status","NORMAL");
  Firebase.set("rain/value",1);
  Firebase.set("LDR/name","LDR");
  Firebase.set("LDR/status","NORMAL");
  Firebase.set("LDR/value",0);
}

void firebasereconnect(){
  Serial.println("Trying to reconnect");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop()
{
  int rainVal = digitalRead(D7);
  int LDR=analogRead(A0);
 
  //Firebase.set("rain",rainVal);
  //Firebase.set("LDR",LDR);
  
  if(rainVal==1){
    Serial.println("Not Raining");
      Firebase.set("rain/name","rain");
      Firebase.set("rain/status","NORMAL");
      Firebase.set("rain/value",rainVal); 
  }
  else if(rainVal==0){
    Serial.println("Raining");
      Firebase.set("rain/name","rain");
      Firebase.set("rain/status","DANGER");
      Firebase.set("rain/value",rainVal);
  }

  if(LDR>=800 || LDR<100){
    Firebase.set("LDR/name","LDR");
    Firebase.set("LDR/status","DANGER");
    Firebase.set("LDR/value",LDR);
  }
  else if(LDR<800 && LDR>100){
    Firebase.set("LDR/name","LDR");
    Firebase.set("LDR/status","NORMAL");
    Firebase.set("LDR/value",LDR);
  }
  Serial.print("Light intensity is: ");
  Serial.println(LDR);
  delay(1000);

  {
    client.add("Rain",rainVal);
    client.add("LDR",LDR);
    client.sendAll(true);
    delay(1000);  
  }
 
}
