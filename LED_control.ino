#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "config.h"

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN      5
#define NUMPIXELS 256

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int loudness = 100;
int pastloudness = loudness;
int interval = 0;
int beatnumber = 0;
int counts = 0;
int delaylength = 100;
uint32_t Radius[34];
int Value[19];


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
HTTPClient http;
String server_address = "http://192.168.0.13:8888";
double danceability = -1;
double energy = -1;
double valence = -1;
double* durations;
int num_of_beats = -1;

void update_song_attributes() {
  String data_address = server_address + "/get_data";
  http.begin(data_address);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    JSONVar response = JSON.parse(http.getString());
    Serial.println(http.getString());
    danceability = int(double(response["danceability"]) * 255);
    Serial.println(danceability);
    energy = int(double(response["energy"]) * 255);
    Serial.println(energy);
    valence = int(double(response["valence"]) * 255);
    Serial.println(valence);
    JSONVar beats = response["beats"];
    Serial.println(beats);
    Serial.print("Length:");
    Serial.println(beats.length());
    num_of_beats = beats.length();  
    Serial.print("Function:");
    Serial.println(num_of_beats);
    durations = new double[num_of_beats];
    for (int i = 0; i < num_of_beats; i++) {
      durations[i] = double(beats[i]);
    }
  }
  beatnumber = 0;
  counts = 0;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  update_song_attributes();
  
  strip.begin();
  for(int i=1; i< 19; i++){
    Radius[i] = strip.ColorHSV(0, 255, 0);
  }
  beatnumber = 0;
}

void loop() {
  Serial.println(num_of_beats);
  Serial.println(beatnumber);
  Serial.println(counts);
  if (beatnumber >= num_of_beats){
    update_song_attributes();
  }
  else if (beatnumber == 0 && counts == 0){
    loudness = 100;
    pastloudness = loudness;
    counts = 1;
    delaylength = int(1000*durations[beatnumber]/6);
  }
  else if (counts == 6){
    loudness = 100;
    pastloudness = loudness;
    beatnumber = beatnumber + 1;
    delaylength = int(1000*durations[beatnumber]/6);
    for (int j = 17; j>1; j--){
      Value[j] = int(Value[j-1]*0.9);
      if(Value[j] == 0){
        Radius[j] = strip.Color(0, 0, 0);
      }
      else {
        Radius[j] = strip.Color(energy, valence, danceability);//what should this be
      }
      Serial.print(j);
      Serial.print(": ");
      Serial.println(Radius[j]);
    }
    Value[1] = 100;
    Radius[1] = strip.Color(energy, valence, danceability);
    strip.clear();
  
    strip.setBrightness(10);  
      
    strip.setPixelColor(0, Radius[4]); //left wave
    strip.setPixelColor(1, Radius[3]);
    strip.setPixelColor(2, Radius[2]);
    strip.fill(Radius[1], 3, 2);
    strip.setPixelColor(5, Radius[2]);
    strip.setPixelColor(6, Radius[3]);
    strip.setPixelColor(7, Radius[4]);
    strip.setPixelColor(8, Radius[4]);
    strip.setPixelColor(9, Radius[3]);
    strip.setPixelColor(10, Radius[2]);
    strip.fill(Radius[1], 11, 2);
    strip.setPixelColor(13, Radius[2]);
    strip.setPixelColor(14, Radius[3]);
    strip.fill(Radius[4], 15, 3);
    strip.setPixelColor(18, Radius[3]);
    strip.fill(Radius[2], 19, 2);
    strip.setPixelColor(21, Radius[3]);
    strip.fill(Radius[4], 22, 2);
    for (int l = 3; l<=15; l++){
      strip.setPixelColor(l*8, Radius[l+2]);
      strip.fill(Radius[l+1], l*8+1, 2);
      strip.fill(Radius[l], l*8+3, 2);
      strip.fill(Radius[l+1], l*8+5, 2);
      strip.setPixelColor(l*8+7, Radius[l+2]);
    }


    strip.setPixelColor(255, Radius[4]);//right wave
    strip.setPixelColor(254, Radius[3]);
    strip.setPixelColor(253, Radius[2]);
    strip.fill(Radius[1], 251, 2);
    strip.setPixelColor(250, Radius[2]);
    strip.setPixelColor(249, Radius[3]);
    strip.setPixelColor(248, Radius[4]);
    strip.setPixelColor(247, Radius[4]);
    strip.setPixelColor(246, Radius[3]);
    strip.setPixelColor(245, Radius[2]);
    strip.fill(Radius[1], 243, 2);
    strip.setPixelColor(242, Radius[2]);
    strip.setPixelColor(241, Radius[3]);
    strip.fill(Radius[4], 238, 3);
    strip.setPixelColor(237, Radius[3]);
    strip.fill(Radius[2], 235, 2);
    strip.setPixelColor(234, Radius[3]);
    strip.fill(Radius[4], 232, 2);
    for (int r = 28; r>=16; r--){
      strip.setPixelColor(r*8, Radius[33-r]);
      strip.fill(Radius[32-r], r*8+1, 2);
      strip.fill(Radius[31-r], r*8+3, 2);
      strip.fill(Radius[32-r], r*8+5, 2);
      strip.setPixelColor(r*8+7, Radius[33-r]);
    }
  
    strip.show();
    pastloudness = pastloudness/2;
    loudness = 0;
    delay(delaylength);
    counts = 0;
  }
  
  else{
    for (int j = 17; j>1; j--){
      Value[j] = int(Value[j-1]*0.9);
      Radius[j] = strip.Color(energy*Value[j]/100, valence*Value[j]/100, danceability*Value[j]/100);
    }
    
    if(loudness != 0){
      Value[1] = loudness;
      Radius[1] = strip.Color(energy*Value[1]/100, valence*Value[1]/100, danceability*Value[1]/100);
    }
    else if(pastloudness > 20){
      Value[1]= int(Value[1]/2);
      Radius[1] = strip.Color(energy*Value[1]/100, valence*Value[1]/100, danceability*Value[1]/100);
    }
    else{
      Value[1] = 0;
      Radius[1] = strip.Color(0, 0, 0);
    }
    strip.clear();
    
    strip.setBrightness(10);  
      
    strip.setPixelColor(0, Radius[4]); //left wave
    strip.setPixelColor(1, Radius[3]);
    strip.setPixelColor(2, Radius[2]);
    strip.fill(Radius[1], 3, 2);
    strip.setPixelColor(5, Radius[2]);
    strip.setPixelColor(6, Radius[3]);
    strip.setPixelColor(7, Radius[4]);
    strip.setPixelColor(8, Radius[4]);
    strip.setPixelColor(9, Radius[3]);
    strip.setPixelColor(10, Radius[2]);
    strip.fill(Radius[1], 11, 2);
    strip.setPixelColor(13, Radius[2]);
    strip.setPixelColor(14, Radius[3]);
    strip.fill(Radius[4], 15, 3);
    strip.setPixelColor(18, Radius[3]);
    strip.fill(Radius[2], 19, 2);
    strip.setPixelColor(21, Radius[3]);
    strip.fill(Radius[4], 22, 2);
    for (int l = 3; l<=15; l++){
      strip.setPixelColor(l*8, Radius[l+2]);
      strip.fill(Radius[l+1], l*8+1, 2);
      strip.fill(Radius[l], l*8+3, 2);
      strip.fill(Radius[l+1], l*8+5, 2);
      strip.setPixelColor(l*8+7, Radius[l+2]);
    }


    strip.setPixelColor(255, Radius[4]);//right wave
    strip.setPixelColor(254, Radius[3]);
    strip.setPixelColor(253, Radius[2]);
    strip.fill(Radius[1], 251, 2);
    strip.setPixelColor(250, Radius[2]);
    strip.setPixelColor(249, Radius[3]);
    strip.setPixelColor(248, Radius[4]);
    strip.setPixelColor(247, Radius[4]);
    strip.setPixelColor(246, Radius[3]);
    strip.setPixelColor(245, Radius[2]);
    strip.fill(Radius[1], 243, 2);
    strip.setPixelColor(242, Radius[2]);
    strip.setPixelColor(241, Radius[3]);
    strip.fill(Radius[4], 238, 3);
    strip.setPixelColor(237, Radius[3]);
    strip.fill(Radius[2], 235, 2);
    strip.setPixelColor(234, Radius[3]);
    strip.fill(Radius[4], 232, 2);
    for (int r = 28; r>=16; r--){
      strip.setPixelColor(r*8, Radius[33-r]);
      strip.fill(Radius[32-r], r*8+1, 2);
      strip.fill(Radius[31-r], r*8+3, 2);
      strip.fill(Radius[32-r], r*8+5, 2);
      strip.setPixelColor(r*8+7, Radius[33-r]);
    }

    strip.show();
    pastloudness = pastloudness/2;
    loudness = 0;
    counts = counts + 1;
    delay(delaylength);
    
  } 
}
