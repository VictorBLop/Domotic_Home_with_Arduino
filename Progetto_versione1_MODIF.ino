#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Keypad.h>

#include <SimpleDHT.h>
int pinDHT11 = A2;
SimpleDHT11 dht11;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(A2, DHT11);     // A2 è il numero del pin al quale collego il sensore
#define Password_Length 3 // Inserire la lunghezza della password
//se la password è lunga 7 caratteri inserire 8
//se la password è lunga 8 caratteri inserire 9 ecc..


int tempo;
int soglia_allarme=950;
int stato_allarme=0;
int stato_bluetooth=0;
int stato_temperatura=0;
int buzzer_allarme=12;
int luce;
int tempo_allarme = 1;
int laser=10;
int buzzer = A3;
int ventola = 11;
char Data[Password_Length]; 
char Master[Password_Length] = "78";   //Modifica la password
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int conta;
void setup() {
  Serial.begin(9600);
  conta = 0;
  lcd.init();
  lcd.backlight();
  pinMode(buzzer_allarme,OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(laser, OUTPUT);
  pinMode(ventola, OUTPUT);
  password(stato_temperatura, stato_allarme);
}

void loop() {


char c= Serial.read();

if (c=='1')
{
  stato_allarme=1;
}

if (c=='0')
{
  stato_allarme=0;
  digitalWrite(laser,LOW);
  noTone(buzzer_allarme);
}

  
  stato_temperatura=1;
  int soglia = 23;
  delay(200);
  int t = dht.readTemperature();
  int h = dht.readHumidity();
  if (t > soglia) {
    conta=0;
    lcd.clear();
    controllo_temperatura();
  }
  else {
    digitalWrite(ventola, LOW);
    noTone(buzzer);
    
  }

  char customKey = customKeypad.getKey();
  /////////////////////////////////////////////////////////////////////
  /////////MENU ALLARME////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  if ((customKey == '1') && (conta == 0)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("A. ATTIVA");
    lcd.setCursor(0, 1);
    lcd.print("B. DISATTIVA");
    conta = 1;
  }
  if ((customKey == 'A') && (conta == 1)) {
    sound_on();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ATTIVATO");
    conta = 5;
    stato_allarme=1;
    delay(500);
    menu1();
  }
  if ((customKey == 'B') && (conta == 1)) {

    sound_off();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DISATTIVATO");
    conta = 6;
    noTone(buzzer_allarme);
    stato_allarme=0;
    delay(500);
    menu1();
  }
  ///////////////////////////////////////////////////////////////////////
  if (stato_allarme==1){
  allarme();}
   if (stato_allarme==0){
    digitalWrite(laser,LOW);  
  }
  ////////////////////////////////////////////////////////////////////////
  //MENU BLUETOOTH ////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  if ((customKey == '2') && (conta == 0)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("A. ATTIVA");
    lcd.setCursor(0, 1);
    lcd.print("B. DISATTIVA");
    conta = 2;
  }
  if ((customKey == 'A') && (conta == 2)) {
    sound_on();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ATTIVATO");
    conta = 7;
    stato_bluetooth=1;
    delay(500);
    menu1();
  }
  if ((customKey == 'B') && (conta == 2)) {
    sound_off();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DISATTIVATO");
    conta = 8;
     stato_bluetooth=0;
    delay(500);
    menu1();
  }
  ////////////////////////////////////////////////////////////////////////
  /////////////////MENU TEMPERATURA////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////
  if ((customKey == '3') && (conta == 0)) {
    menu_temperatura();
  }
  
  if ((customKey == 'A') && (conta == 3)) {
    tone(buzzer, 2000);
    delay(50);
    noTone(buzzer);
    conta = 9;
  }
  if (conta == 9) {
    ///////////////////info TEMPERATURA e UMIDITA'//////////////////////////
    delay(200);
    int t = dht.readTemperature();
    int h = dht.readHumidity();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Temp.:" + String(t) + char(0xDF) + "C");
    lcd.setCursor(0, 1);
    lcd.print(" Umid.:" + String(h) + "%");
  }

  if ((customKey == 'B') && (conta == 3)) {
    sound_on();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CAMBIARE SOGLIA:");
    conta = 10;
  }
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  /////////////////////MENU PASSWORD//////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////
  if ((customKey == '4') && (conta == 0)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1. ATTIVA");
    lcd.setCursor(0, 1);
    lcd.print("2. MODIFICA");
    conta = 4;
  }

  if ((customKey == '1') && (conta == 4)) {
    tone(buzzer, 2000);
    delay(1000);
    noTone(buzzer);
    lcd.clear();
    lcd.setCursor(0, 0);
    password(stato_temperatura, stato_allarme);
    conta = 0;
  }
  else if((customKey == '2') && (conta == 4)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Inserire nuova");
    lcd.setCursor(0,1);
    lcd.print("password: ");
    cambia_pass();
  }
    
  ///////////////////////////////////////////////////////////////////////////////
  ///RITORNO AL MENU ////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  if ((customKey == '*') && (conta == 0 || conta == 1 || conta == 2 || conta == 5 || conta == 6 || conta == 7 || conta == 8)) {
    lcd.clear();
    menu1();
    conta = 0;
  }

  if ((customKey == '*') && (conta == 3 || conta == 4 )) {
    lcd.clear();
    menu2();
    conta = 0;
  }

  if ((customKey == '*') && (conta == 9 || conta == 10 )) {
    lcd.clear();
    menu_temperatura();
    conta = 3;
  }


  if ((customKey == '#') && (conta == 0 || conta == 11 || conta == 5 || conta == 6 )) {
    lcd.clear();
    menu2();
    conta = 0;
  }
  ////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////MENU1//////////////////////////////////////////////
void menu1() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.ALLARME");
  lcd.setCursor(0, 1);
  lcd.print("2.BLUETOOTH");
  conta = 0;
}
/////////////////////////////MENU2///////////////////////////////////////////////
void menu2() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("3.TEMPERATURA");
  lcd.setCursor(0, 1);
  lcd.print("4.PASSWORD");
  conta = 0;
}
///////////////////////////MENU_TEMPERATURA//////////////////////////////////////

void menu_temperatura(){
  lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("A. INFO");
    lcd.setCursor(0, 1);
    lcd.print("B. SOGLIA");
    conta = 3;
}
//////////////////////////MENU BIANCO ///////////////////////////////////////////

void menu_bianco(){
  lcd.clear();
  conta=30;
}

/////////////////////////PASSWORD////////////////////////////////////////////
void password(int stato_temperatura, int stato_allarme) {
  int condizione=0;
  int tentativi=0;
  int cont=0;
  
  data_count=0;  
  do{ 
    int soglia = 23;
    delay(200);
    int t = dht.readTemperature();
    int h = dht.readHumidity();
    if(stato_allarme==1){
    allarme();}
    
    if ((t>soglia) && (stato_temperatura==1)) {
    lcd.clear();
    controllo_temperatura();
    lcd.clear();
    }
    else{
      
    digitalWrite(ventola,LOW);
    
    lcd.setCursor(0, 0);
    lcd.print("PASSWORD:");
    customKey = customKeypad.getKey();

    if (customKey) {
      Data[data_count] = customKey;
      lcd.setCursor(data_count, 1);
      lcd.print('*');
      data_count++;
      delay(500);
    }

    if (data_count == Password_Length - 1) {
      lcd.clear();

      if (!strcmp(Data, Master)) {
        lcd.print("Pass Corretta");
        condizione = 1;
        delay(1000);
      }
      else{
        lcd.setCursor(0,0);
        lcd.print("Pass errata");
        tentativi++;
        data_count=0;
        if(tentativi>=3){
          tentativi=0;
          cont=0;
          while(cont<5){
            lcd.clear();
            delay(500);
            lcd.setCursor(0,0);
            lcd.print("SISTEMA BLOCCATO");   
            delay(2000);
            cont++;
          }
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Riavviando..."); 
          delay(1000);
        }
      }
    }
    lcd.clear();
    clearData();
    }
  }
  while (condizione == 0);
  menu1();
  return;
}
///////////////////////////////////////
void cambia_pass(){
  int condizione=0;
  int i=0;

  data_count=0;  
  do{
    customKey = customKeypad.getKey();
    if(customKey=='A'){
      if(data_count==Password_Length-1){
        while(i<Password_Length-1){
          Master[i]=Data[i];
          i++;
        }
        condizione=1;
      }
    }    
    else if(customKey=='B'){
      if(data_count!=0){
        data_count--;  
        lcd.setCursor(data_count+10,1); 
        lcd.print(' ');               
      }
    }
    else if(data_count<Password_Length-1){
      Data[data_count] = customKey; 
      lcd.setCursor(data_count+10,1); 
      lcd.print(Data[data_count]); 
      data_count++;
    }
    delay(500);
  }while(condizione==0);  
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}

void temperatura() {
  int soglia = 19;
  delay(200);
  int t = dht.readTemperature();
  int h = dht.readHumidity();
  if (t > soglia) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp.:" + String(t) + char(0xDF) + "C");
    lcd.setCursor(0, 1);
    lcd.print("Umid.:" + String(h) + "%");
    digitalWrite(ventola, HIGH);
    tone(buzzer, 3000);
    delay(500);
    noTone(buzzer);
    tone(buzzer, 3000);
    delay(50);
    noTone(buzzer);
    delay(800);
    
  }
  else {
    digitalWrite(ventola, LOW);
    noTone(buzzer);
    delay(500);
  }
}

void sound_on() {
  tone(buzzer, 1500);
  delay(170);
  noTone(buzzer);
  delay(80);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
}

void sound_off() {
  tone(buzzer, 2000);
  delay(170);
  noTone(buzzer);
  delay(80);
  tone(buzzer, 1500);
  delay(100);
  noTone(buzzer);
}

void benvenuto() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   BENVENUTO  ");
  tone(buzzer, 400);
  delay(tempo);
  tone(buzzer, 500);
  delay(tempo);
  tone(buzzer, 600);
  delay(tempo);
  tone(buzzer, 700);
  delay(tempo);
  tone(buzzer, 800);
  delay(tempo);
  tone(buzzer, 900);
  delay(tempo);
  tone(buzzer, 1000);
  delay(tempo);
  noTone(buzzer);
  delay(1000);
  lcd.clear();
}

void controllo_temperatura(){
    int soglia = 19;
    delay(200);
    int t = dht.readTemperature();
    int h = dht.readHumidity();
  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp. elevata");
    lcd.setCursor(0, 1);
    lcd.print(String(t) + char(0xDF) + "C");
    lcd.clear();
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print("Temp. elevata");
    lcd.setCursor(0, 1);
    lcd.print(String(t) + char(0xDF) + "C");
    tone(buzzer, 3000);
    delay(500);
    noTone(buzzer);
    tone(buzzer, 3000);
    delay(50);
    noTone(buzzer);
    delay(800);
    digitalWrite(ventola, HIGH);
    
}

void allarme() {
digitalWrite(laser,HIGH);
luce=analogRead(A1);
if(luce<soglia_allarme){
  tone(buzzer_allarme,2000);
  delay(500);
}}


void bluetooth(){

byte temperature = 0;
byte humidity = 0;
if (dht11.read(pinDHT11, &temperature, &humidity, NULL))
{
return;
}

Serial.print((int)temperature);
Serial.print("C");
Serial.print("|");
Serial.print((int)humidity);
Serial.print("%");
Serial.print("|");
delay(5000);

}
