#include <SoftwareSerial.h>  

#define RC_PIN     2
#define GSM_RX_PIN 3
#define GSM_TX_PIN 4

#define LED_STAT   5
#define LED_PIR_1  6
#define LED_PIR_2  7
#define LED_PIR_3  8
#define LED_PIR_4  9
#define RELAY_LED  10
#define RELAY_SIR  11

#define PIR1       A0
#define PIR2       A1
#define PIR3       A2
#define PIR4       A3
#define EXT_POWER  A4
#define GSM_BOOT   A5
#define BTN_MODE   A6

SoftwareSerial gsmSerial(GSM_RX_PIN,GSM_TX_PIN);  //Creates a software serial port. (rx,tx)
char gsm_char=0; 

// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);
  gsmSerial.begin(9600);
  Serial.println("Starting TC35 and debug Communication...");
  gsmSerial.println("AT+CLIP=1");  //включаем АОН
  delay(100);
  gsmSerial.println("AT+CMGF=1");  //режим кодировки СМС - обычный (для англ.)
  delay(100);
  gsmSerial.println("AT+CSCS=\"GSM\"");  //режим кодировки текста
  delay(100);
  
  pinMode(RELAY_LED, OUTPUT);
  pinMode(RELAY_SIR, OUTPUT);

  pinMode(LED_STAT,  OUTPUT);
  pinMode(LED_PIR_1, OUTPUT);
  pinMode(LED_PIR_2, OUTPUT);
  pinMode(LED_PIR_3, OUTPUT);
  pinMode(LED_PIR_4, OUTPUT);
  pinMode(GSM_BOOT, OUTPUT);
    
  digitalWrite(RELAY_SIR, HIGH);
  digitalWrite(RELAY_LED, HIGH);


  pinMode(PIR1, INPUT);
 // pinMode(PIR2, INPUT);
 // pinMode(PIR3, INPUT);
 // pinMode(PIR4, INPUT);
  pinMode(EXT_POWER, INPUT);
  pinMode(BTN_MODE, INPUT); 


 // подтяжка
 digitalWrite(PIR1,  INPUT_PULLUP);
// digitalWrite(PIR2,  INPUT_PULLUP);
// digitalWrite(PIR3,  INPUT_PULLUP);
// digitalWrite(PIR4,  INPUT_PULLUP);
 
}

// the loop function runs over and over again forever
void loop() 
{
  // read the input pin:
//  int pir_1 = digitalRead(PIR1);
//  int pir_2 = digitalRead(PIR2);
//  int pir_3 = digitalRead(PIR3);
//  int pir_4 = digitalRead(PIR4);
//  int ext_pwr = digitalRead(EXT_POWER);
//  int led_mode = digitalRead(BTN_MODE);

//  if (pir_1==1) digitalWrite(LED_PIR_1, HIGH);
//  else digitalWrite(LED_PIR_1, LOW);
  
//  if (pir_2==1) digitalWrite(LED_PIR_2, HIGH);
//  else digitalWrite(LED_PIR_2, LOW);

//  if (pir_3==1) digitalWrite(LED_PIR_3, HIGH);
//  else digitalWrite(LED_PIR_3, LOW);

//  if (pir_4==1) digitalWrite(LED_PIR_4, HIGH);
//  else digitalWrite(LED_PIR_4, LOW);

 // if (ext_pwr==1) digitalWrite(LED_STAT, HIGH);
 // else digitalWrite(LED_STAT, LOW);
  
//  delay(1);        // delay in between reads for stability
   

  //If a character comes in from the cellular module...
  if(gsmSerial.available() > 0)
  {
    gsm_char=gsmSerial.read(); // Store the char in gsm_char.
    Serial.print(gsm_char);    // Print it to debug serial
  }  
  
/*  digitalWrite(LED_STAT, HIGH);  
  delay(1000);              
  digitalWrite(LED_STAT, LOW);   
  delay(1000); 
  */
          
}
