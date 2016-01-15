/* Liquid_pump.ino
 *
 * This device is built into a large metal box with a front facing door. The door has 2 potentiometers and 5 switches on it as well as an LCD display
 * Inside the door is a small slot where a vial (the sample) can be placed. When the sample is inserted, a button is pressed.
 * The user must then close the door, activating the proximity sensor inside, and manipulate the switches and potentiometers to the correct values.
 * Once the correct combination is set, the relay is activated, sending power to the liquid pump which pumps water through a long tube into an external vial.
 *
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

// include the libraries:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define BUT1 6    //define pin numbers
#define LED1 22
#define LED2 23
//#define LED3 24
#define TRIG 29
#define ECHO 28
#define RELAY 32
#define PUMP 35

#define SW1 36
#define SW2 37
#define SW3 38
#define SW4 39
#define SW5 40
#define POT1 0
#define POT2 1

bool sampleIn = false;

void setup() {
  pinMode(BUT1, INPUT);   //setup buttons for input/output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  //pinMode(LED3, OUTPUT);
  pinMode(TRIG, OUTPUT);  //output pin for echo location device
  pinMode(ECHO, INPUT);   //input pin for echo location device
  pinMode(PUMP, OUTPUT);  //power pins for liquid pump
  pinMode(RELAY, OUTPUT); //power pin for relay and door magnet

  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);
  pinMode(SW5, INPUT);
  
  lcd.begin(16,2);  // set up the LCD's number of columns and rows
  Serial.begin(9600);
}


void loop() {
  int doorDist = 0;
  lcd.setCursor(0,0);
  lcd.print("Insert sample    ");
  lcd.setCursor(0,1);
  lcd.print("Inserer specimen ");
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  //digitalWrite(LED3, LOW);
  
  if(digitalRead(BUT1)) //continue program once input vial is pressed against the button
    sampleIn = true;
  while(sampleIn){
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      //digitalWrite(LED3, LOW);
      lcd.setCursor(0,0);     //activate LCD
      lcd.print("Sample accepted  ");
      lcd.setCursor(0,1);
      lcd.print("Specimen accepte  ");
      doorDist = checkDistance();
      if(doorDist <= 15){
          delay(4500);
          doorShut();
      }
      delay(1000);
      lcd.setCursor(0,0);
      lcd.print("Close door         ");
      lcd.setCursor(0,1);
      lcd.print("Fermer la porte    ");
      doorDist = checkDistance();
      if(doorDist <= 8){
          delay(4500);
          doorShut();
      }
      delay(1500);
  }
}

void doorShut(){
    if(!sampleIn)
      return;
    digitalWrite(RELAY, HIGH);  //lock door with magnet
    digitalWrite(LED1, LOW);
    //digitalWrite(LED2, LOW);
    //digitalWrite(LED3, HIGH);
    for(int count=5;count>=0;count--){    //countdown from 5 to next phase of sequence
      lcd.setCursor(0,0);
      lcd.print("Analyzing       ");
      lcd.setCursor(0,1);
      lcd.print("Analyse        ");
      lcd.setCursor(15,1);
      lcd.print(count);
      delay(990);
      }
      checkParam();
}

void checkParam(){
  
  while(sampleIn){
    lcd.setCursor(0,0);
    lcd.print("Set paramaters   ");
    lcd.setCursor(0,1);
    lcd.print("                 ");
    delay(1000);
    lcd.setCursor(0,0);
    lcd.print("Choisir les     ");
    lcd.setCursor(0,1);
    lcd.print("parametres      ");
    delay(1000); 
    int pot1 = analogRead(POT1);
    int pot2 = analogRead(POT2);
/*    Serial.print("Pot1: ");
    Serial.println(pot1);
    Serial.print("Pot2: ");
    Serial.println(pot2);
    if(digitalRead(SW1))
      Serial.println("SW1 high");
    if(digitalRead(SW2))
      Serial.println("SW2 high");
    if(digitalRead(SW3))
      Serial.println("SW3 high");
    if(digitalRead(SW4))
      Serial.println("SW4 high");
    if(digitalRead(SW5))
      Serial.println("SW5 high");
*/
     
    if(digitalRead(SW1) && !digitalRead(SW2) && !digitalRead(SW3) && digitalRead(SW4) && !digitalRead(SW5) && pot1 < 670 && pot1 > 300 && pot2 < 220 && pot2 > 50){
      lcd.setCursor(0,0);
      lcd.print("Success          ");
      lcd.setCursor(0,1);
      lcd.print("Succes           ");
      delay(2000);
      digitalWrite(PUMP, HIGH);
      delay(5000);  //5 seconds
      digitalWrite(PUMP, LOW);
      //Serial.print("Success on pump");
      delay(5000);
      lcd.setCursor(0,0);
      lcd.print("Keep door shut  ");
      lcd.setCursor(0,1);
      lcd.print("Garder fermee   ");
      delay(5000);
      lcd.setCursor(0,0);
      lcd.print("Keep door shut  ");
      lcd.setCursor(0,1);
      lcd.print("Garder fermee   ");
      delay(590000);  //10:00 minutes
      sampleIn = false;
      digitalWrite(RELAY, LOW);
    }
  }
}

int checkDistance(){    //use echo location device to check for distance between
  int maxRange = 200;
  int minRange = 0;
  long duration, distance;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = duration/58.2;
  if(distance >= maxRange || distance <= minRange){
    Serial.println("Out of range");   //echo location device does not have a signal
    return 25;
  }
  else{
    Serial.println(distance);
    return distance;
  }

}
