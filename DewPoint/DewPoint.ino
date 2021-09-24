#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include "SHTSensor.h"  
#include "math.h"

File MyFile;
RTC_DS1307 rtc;
SHTSensor sht;

char daysOfTheWeek[7][12] = {"Pazartesi", "Sali", "Carsamba", "Persembe", "Cuma", "Cumartesi", "Pazar"};

int const selenoidPin = 7 ;    
int sd;
 
int const    rs = 9, en = 8, d4 = 5, d5 = 6, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs , en , d4 , d5 , d6 , d7);

float nem,sicaklik,dew_point,sicaklik_sonuc,nem_sonuc;
double Mn;
int sayi;
float dizi_nem[5];
float dizi_sicaklik[5];
float dizi_mutlak_nem[5];

#define MOLAR_MASS_OF_WATER 18.01534
#define UNIVERSAL_GAS_CONSTANT 8.21447215

float mutlak_nem(float Bn, float T){
  float Mn_hesabi,e,e_us_alma,pay,payda;
  return (6.1121 * pow(exp(1), (17.67 * T) / (T + 243.5)) * Bn * MOLAR_MASS_OF_WATER) / ((273.15 + T) * UNIVERSAL_GAS_CONSTANT);
 }

 

void setup() {
  pinMode(selenoidPin , OUTPUT);
  
  Serial.begin(9600);

  lcd.begin(16,2);

  while (!Serial) {
    ;
  }

  Serial.println("Initializing SD card...");


  if (!SD.begin(4)) {
    
    Serial.println("initialization failed!"); 
   }
   else
    Serial.println("initialization done.");


  rtc.begin();
 
  if (! rtc.begin()) {
    Serial.println("Could not find RTC");
  
    lcd.clear();
    lcd.print("!RTC Bulunamadi!");
    while (1);
  }
  
  if(!rtc.isrunning()){
    Serial.print("RTC is not running!");
    lcd.clear();
  
    lcd.print("!RTC Calismiyor!");
  }
  if (sht.init()) {
    Serial.println("init(): success\n");
  } 
  else {
    Serial.println("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);

  }

void loop() {
  
  DateTime now = rtc.now();
    
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  Serial.println("-------------------------------------");

  int i;
    for(i=0;i<5;i++){
      sht.readSample();
      dizi_nem[i]=sht.getHumidity();
    }

  nem_sonuc=(dizi_nem[0]+(2*dizi_nem[1])+(3*dizi_nem[2])+(2*dizi_nem[3])+dizi_nem[4])/9;
 
  Serial.print("Nem : ");
  Serial.println(nem_sonuc,4);

  int k;
  for(k=0;k<5;k++){
     sht.readSample();
     dizi_sicaklik[k]=sht.getTemperature();
  }

  sicaklik_sonuc = (dizi_sicaklik[0]+(2*dizi_sicaklik[1]) + (3*dizi_sicaklik[2]) + (2*dizi_sicaklik[3])+dizi_sicaklik[4])/9;
  Serial.print("Sıcaklık : ");
  Serial.println(sicaklik_sonuc,4);

  mutlak_nem(nem_sonuc,sicaklik_sonuc);
  Mn=mutlak_nem(nem_sonuc,sicaklik_sonuc);
  Serial.print("Mutlak Nem : ");
  Serial.println(Mn,4);

  dew_point=pow((nem_sonuc/100),0.125)*(112+(0.9*sicaklik_sonuc))+(0.1*sicaklik_sonuc)-112;
  Serial.print("Dew Point = ");
  Serial.println(dew_point,4);
  Serial.println(" ");


   
  if(dew_point < 16)
    digitalWrite(selenoidPin , HIGH);
  else 
    digitalWrite(selenoidPin , LOW);
    
  if(!SD.begin(4)){
    lcd.clear();
    lcd.print("!SD Kart Hatasi!");
    delay(3000);
  }
 
  MyFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (MyFile) {
    Serial.println("Writing to test.txt...");
   
    MyFile.print(now.day(), DEC);
    MyFile.print('/');
    MyFile.print(now.month(), DEC);
    MyFile.print('/');
    MyFile.print(now.year(), DEC);
    MyFile.print(" (");
    MyFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
    MyFile.print(") ");
    MyFile.print(now.hour(), DEC);
    MyFile.print(':');
    MyFile.print(now.minute(), DEC);
    MyFile.print(':');
    MyFile.println(now.second(), DEC);
    MyFile.println("-------------------------------------");


    MyFile.print("Nem : ");
    MyFile.println(nem_sonuc,4);

    MyFile.print("Sıcaklık : ");
    MyFile.println(sicaklik_sonuc,4);

    MyFile.print("Mutlak Nem : ");
    MyFile.println(Mn,4);

    MyFile.print("Dew Point = ");
    MyFile.println(dew_point,4);
    MyFile.println();
   
    MyFile.close();
    Serial.println("done.");
    Serial.println();
    
  } 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
    
  


  lcd.clear();
  lcd.setCursor(3,0);
  if(now.day()<10){
    lcd.print(0);
    lcd.print(now.day(), DEC);
  }
  else
    lcd.print(now.day(), DEC);
    
  lcd.print('/');
    
  if(now.month()<10){
    lcd.print(0);
    lcd.print(now.month(), DEC);
  }
  else
    lcd.print(now.month(), DEC);
    
  lcd.print('/');
    
  lcd.print(now.year(), DEC);
  
  lcd.setCursor(4,1);
  if(now.hour()<10){
    lcd.print(0);
    lcd.print(now.hour(), DEC);
  }
  else
    lcd.print(now.hour(), DEC);
    
    lcd.print(':');
    
  if(now.minute()<10){
   lcd.print(0);
   lcd.print(now.minute(), DEC);
  }
  else
    lcd.print(now.minute(), DEC);
    
    lcd.print(':');
  
  if(now.second()<10){
    lcd.print(0);
    lcd.print(now.second(), DEC); 
  }
  else 
    lcd.print(now.second(), DEC);
  
  delay(3000);
   

  lcd.clear();
    
  lcd.setCursor(6,0);
  lcd.print("Nem");
  lcd.setCursor(4,1);
  lcd.print(nem_sonuc,4);
  delay(3000);

  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Sicaklik");
  lcd.setCursor(4,1);
  lcd.print(sicaklik_sonuc,4);
  delay(3000);

  lcd.clear(); 
  lcd.setCursor(3,0);
  lcd.print("Mutlak Nem");
  lcd.setCursor(4,1);
  lcd.print(Mn,4);
  delay(3000);
    
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("DewPoint");
  lcd.setCursor(4,1);
  lcd.print(dew_point,4);
  delay(3000);
  
}
