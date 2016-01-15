/* Laser_grid_RFID.ino
 *
 * This program controls the receivers for 4 lasers and alters an LED countdown timer with penalties when the laser beams are broken.
 * The LED timer will slowly countdown until it reaches 0 at which point the RFID scanner begins to watch for the correct card.
 * Once the correct card is scanned, the relay controlling 12V magnets that hold the exit door shut are turned off allowing the door to be opened.
 *
 * Typical RFID pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15 
 */

#include <FastLED.h>
#include <SPI.h>
#include <MFRC522.h>

//Code for Relays
int Relay1 = 7;
int Relay2 = 6;
int Relay3 = 5;
int Relay4 = 4; //pins that control relays 

//Code for RFID
#define RST_PIN         36 
#define SS_PIN          53

//Code for audio output
#define AUD1 40

const String CODE = "12980212209";
const String CODE2 = "3189169172";
const String CODE3 = "20665490";
const String CODE4 = "225175208209";
const String CODE5 = "11362157230";
const String CODE6 = "241133194112";
const String CODE7 = "8120325184";
const String CODE8 = "8131106133";
const String THEONECODE = "20125590158";
String codeCheck = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//Code for FastLED
#define NUM_LEDS 146
#define DATA_PIN 37

#define TRIGGERPIN_A 22
#define TRIGGERPIN_B 23
#define TRIGGERPIN_C 24
#define TRIGGERPIN_D 25
#define TRIGGERPIN_E 26
#define TRIGGERPIN_F 27

#define LEDGREEN 31
#define LEDRED 32

#define MAX_PENALTY 1400

CRGB leds[NUM_LEDS];

int penalty=0;
int all_green=0;
int laser_status=HIGH;
int previous_laser_status=HIGH;



void setup() {
  // Set the pin modes at power up
  pinMode(TRIGGERPIN_A,INPUT_PULLUP);
  pinMode(TRIGGERPIN_B,INPUT_PULLUP);
  pinMode(TRIGGERPIN_C,INPUT_PULLUP);
  pinMode(TRIGGERPIN_D,INPUT_PULLUP);
  pinMode(TRIGGERPIN_E,INPUT_PULLUP);
  pinMode(TRIGGERPIN_F,INPUT_PULLUP);

  pinMode(AUD1, OUTPUT);

  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDRED, OUTPUT);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);

  digitalWrite(Relay1, LOW);
  digitalWrite(AUD1, HIGH);
  FastLED.addLeds<WS2812B, DATA_PIN,GRB>(leds, NUM_LEDS);
  Serial.begin(9600);  
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
}



void loop() {
 
  if(penalty<1){
    if(all_green==0){
      set_to_green();
    }
    if(mfrc522.PICC_IsNewCardPresent()){
        if(mfrc522.PICC_ReadCardSerial()){
          validateCard(mfrc522.uid.uidByte, mfrc522.uid.size);
        }
    }
  }
  else{
      set_to_red();
      if(previous_laser_status==HIGH)
      {
        penalty=penalty-1;
        Serial.print("Penalty:");
        Serial.print(penalty);
        Serial.println("");
      }
  }
  
  laser_status=!(!digitalRead(TRIGGERPIN_A)||!digitalRead(TRIGGERPIN_B)||!digitalRead(TRIGGERPIN_C)||!digitalRead(TRIGGERPIN_D)||!digitalRead(TRIGGERPIN_E)||!digitalRead(TRIGGERPIN_F));
  if(laser_status==LOW && previous_laser_status==HIGH)
  {
    digitalWrite(AUD1, LOW);
    delay(10);
    digitalWrite(AUD1, HIGH);
    delay(15);
    laser_status=!(!digitalRead(TRIGGERPIN_A)||!digitalRead(TRIGGERPIN_B)||!digitalRead(TRIGGERPIN_C)||!digitalRead(TRIGGERPIN_D)||!digitalRead(TRIGGERPIN_E)||!digitalRead(TRIGGERPIN_F));
    if(laser_status==LOW){    
        penalty=penalty+200;
        if(penalty>MAX_PENALTY)
        {
          penalty=MAX_PENALTY;
        } 
        Serial.println("Trigger detected!");
    }
  }
  previous_laser_status=laser_status;
  delay(10);

}



void set_to_green() {
   Serial.println("Set To Green");
   for(int i=0;i<NUM_LEDS-4;i++)
   {
      leds[i] = CRGB::Green; 
   } 
   all_green=1;

   for(int j=NUM_LEDS-4;j<NUM_LEDS-2;j++){
      leds[j] = CRGB::Green;
   }
   for(int k=NUM_LEDS-2;k<NUM_LEDS;k++){
      leds[k] = CRGB::Black;
   }
   FastLED.show(); 
}

void set_to_red() {
   Serial.println("Set To Red");
   all_green=0;
   for(int i=0;i<penalty/10;i++)
   {
      leds[i] = CRGB::Red; 
   } 
   for(int i=penalty/10;i<NUM_LEDS-4;i++)
   {
      leds[i] = CRGB::Blue;
   } 
   for(int j=NUM_LEDS-2;j<NUM_LEDS;j++){
      leds[j] = CRGB::Red;
   }
   for(int k=NUM_LEDS-4;k<NUM_LEDS-2;k++){
      leds[k] = CRGB::Black;
   }
   FastLED.show(); 
}

void validateCard(byte *buffer, byte bufferSize){
    Serial.println("Validate Card");
    codeCheck = "";
    for (byte i = 0; i < bufferSize; i++) {
        codeCheck += buffer[i];
    }
    Serial.println(codeCheck);
  
    if(codeCheck == CODE || codeCheck == CODE2 || codeCheck == CODE3 || codeCheck == CODE4 || codeCheck == CODE5 || codeCheck == CODE6 || codeCheck == CODE7 || codeCheck == CODE8 || codeCheck == THEONECODE){
      digitalWrite(LEDGREEN, HIGH);
      digitalWrite(LEDRED, LOW);
      digitalWrite(Relay1,HIGH); // Open the Door
      Serial.println("Door reset - Back to green !");
      delay(5000);
      digitalWrite(LEDGREEN, LOW);
      digitalWrite(Relay1, LOW); // Close the door
    }
    else{
      digitalWrite(LEDGREEN, LOW);
      digitalWrite(LEDRED, HIGH);
      delay(2000);
      digitalWrite(LEDRED, LOW);
    }
}
