#include "CTBot.h"
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>
#include "pinku.h"
#include "RTClib.h"

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

RTC_DS1307 rtc;
BH1750 luxMeter;
CTBot myBot;
LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

//Global
int suhu,humid,gas,air;
long lux;

//Timer
unsigned long nows=0,before=0;

void setup() {
  
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  luxMeter.begin();
  rtc.begin();
       
  //LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("ANGKER IoT");
  lcd.setCursor(4, 2);
  lcd.print(" BYWAHJOE");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CONNECTING....");
  
  Serial.println("\nSTART TELEGRAM -- CONECT WIFI");
  myBot.wifiConnect(ssid, pass); //WIFI CONECT
  myBot.setTelegramToken(token); //TOKEN SETT
  delay(100);
  
  //TEST KONEKSI
  if (myBot.testConnection()){
    Serial.println("\nSIGNAL OK");
    lcd.setCursor(0, 0);
    lcd.print("TELEGRAM OK");
    delay(2000);}
  else{
    Serial.println("\nERROR NO SIGNAL");
    lcd.setCursor(0, 0);
    lcd.print("TELEGRAM-WIFI FAILED");
    delay(2000);
  }

  firstMessage();
  lcd.clear();

}

void loop() {
  
  suhu=dht.readTemperature();
  humid=dht.readHumidity();
  lux=luxMeter.readLightLevel();
  
  int vGas=analogRead(sensorGas);
  int vAir=analogRead(sensorAir);
  gas=map(vGas, 0, 4095, 0, 100);
  air=map(vAir, 0, 4095, 0, 100);  

  Serial.print("Suhu : ");Serial.println(suhu);
  Serial.print("Humid: ");Serial.println(humid);
  Serial.print("Lux  : ");Serial.println(lux);
  Serial.print("Gas  : ");Serial.print(vGas);Serial.print("[*]");Serial.println(gas);
  Serial.print("Air  : ");Serial.print(vAir);Serial.print("[*]");Serial.println(air);

  viewSensor();
  delay(2000);
  viewTimer();
  delay(1500);

  nows=millis();
  if(nows-before>=interval){
    logTelegram();
    before=nows;  
  }
}
void viewTimer(){
  lcd.clear();
  DateTime now = rtc.now();
  
  lcd.setCursor(0,0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);

  lcd.setCursor(11,0);
  lcd.print(now.hour())   ;lcd.print(":");
  lcd.print(now.minute()) ;lcd.print(":");
  lcd.print(now.second()); 

  
  lcd.setCursor(0,1);
  lcd.print(now.day())    ;lcd.print("/");
  lcd.print(now.month())  ;lcd.print("/");
  lcd.print(now.year());  
  
}
void viewSensor(){
  int r=11;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp__");
  lcd.print(suhu);
  lcd.print((char)223);
  lcd.print("C");
  
  lcd.setCursor(0,1);
  lcd.print("Lux__");
  lcd.print(lux);
  
  lcd.setCursor(0,2);
  lcd.print("Humid_");
  lcd.print(humid);
  lcd.print(" %");

  lcd.setCursor(0,3);
  lcd.print("Gas___");
  lcd.print(gas);
  lcd.print(" %");

  lcd.setCursor(r,0);
  lcd.print("Air..");
  lcd.print(air);
  lcd.print(" %");

  lcd.setCursor(r,2);
  lcd.print("ON..");
  lcd.print(tON);

  lcd.setCursor(r,3);
  lcd.print("OFF.");
  lcd.print(tOFF);
}
void kirim(String pesan) {
  myBot.sendMessage(chatID, pesan);
}
void logTelegram(){
  String pesan;
  String state="Normal";
  
  if(gas>=20){
    state="Warning!!";
  }

  pesan = "**[Angker IoT Notification]** \n\n";
  pesan += "Temp_: "+String(suhu) +" *C\n";
  pesan += "Lux___: "+String(lux)  +" lx\n";
  pesan += "Humid: "+String(humid)+ " %\n";
  pesan += "Gas___: "+String(gas)  +" %\n";
  pesan += "Air____: "+String(air)  +" %\n";
  pesan += "T_ON__: "+tON +"\n";
  pesan += "T_OFF_: "+tOFF+"\n";
  pesan += "Condition *"+state+"*\n\n" ; 
  pesan +="More___: iot.bywahjoe.com";
  
  kirim(pesan);
  
}
void firstMessage() {
  String pesan;
  pesan = "Angker IoT Start...";
  kirim(pesan);
}
