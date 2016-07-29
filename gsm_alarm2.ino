#include <SoftwareSerial.h>  
#include <TimerOne.h>

#define LED_ON     1   // 
#define LED_OFF    0   // 

#define RELAY_ON   0   // 
#define RELAY_OFF  1   // 

#define RC_PIN     2   // от приемника 433Мгц
#define GSM_RX_PIN 3   // 
#define GSM_TX_PIN 4   //

#define LED_STAT   5   // светодиод статус охраны
#define LED_1  6   // светодиод датчика 1
#define LED_2  7   // светодиод датчика 2
#define LED_3  8   // светодиод датчика 3
#define LED_4  9   // светодиод датчика 4
#define RELAY_LED  10  // реле наружнего светодиода
#define RELAY_SIR  11  // реле сирены
#define SPARE1     12  // резерв
#define SPARE2     13  // резерв

#define PIR1       A0  // вх.сигнал датчика 1
#define PIR2       A1  // вх.сигнал датчика 2
#define PIR3       A2  // вх.сигнал датчика 3
#define PIR4       A3  // вх.сигнал датчика 4
#define EXT_POWER  A4  // вх.сигнал наличия внешнего питания 12В
#define GSM_BOOT   A5  // вых.управление модулем GSM
#define BTN_MODE   A6  // вх.кнопка просмотра уровня сигнала GSM

SoftwareSerial mySerial(GSM_RX_PIN,GSM_TX_PIN);  //Creates a software serial port. (rx,tx)
int gsm_level;
//char gsm_char=0; 

// текущее значение дискретных входов
int pir_1,pir_2,pir_3,pir_4;
int ext_power,led_mode;

// текущее значение беспроводных датчиков
int radio_1,radio_2,radio_3,radio_4;

int  main_state; // 0-снято с охраны, 1-охрана, 2-тревога
int  prev_main_stae;
#define DISARM  0
#define ARM     1
#define ALARM   2

/////////////////////////////////
void setup() {


  Serial.begin(9600); // для отладки
  log_msg("GSM alarm v2.0");
    
// настройка режимов работы входов 
  pinMode(PIR1, INPUT);
 // pinMode(PIR2, INPUT); // если используются - раскоментировать
 // pinMode(PIR3, INPUT); // если используются - раскоментировать
 // pinMode(PIR4, INPUT); // если используются - раскоментировать
  pinMode(EXT_POWER, INPUT);
  pinMode(BTN_MODE, INPUT); 

 // подтяжка
 digitalWrite(PIR1,  INPUT_PULLUP);
// digitalWrite(PIR2,  INPUT_PULLUP); // если используются - раскоментировать
// digitalWrite(PIR3,  INPUT_PULLUP); // если используются - раскоментировать
// digitalWrite(PIR4,  INPUT_PULLUP); // если используются - раскоментировать 

  // настройка режимов работы выходов
  pinMode(RELAY_LED, OUTPUT);   digitalWrite(RELAY_SIR, RELAY_OFF);  //выключение реле
  pinMode(RELAY_SIR, OUTPUT);   digitalWrite(RELAY_LED, RELAY_OFF);  //выключение реле

  pinMode(LED_STAT,  OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);  
  pinMode(GSM_BOOT, OUTPUT);

  //// инициализация GSM 
  mySerial.begin(9600);
  digitalWrite(GSM_BOOT, LOW);
  delay(300);
  mySerial.println("AT+CLIP=1");  //включаем АОН
  delay(100);
  mySerial.println("AT+CMGF=1");  //режим кодировки СМС - обычный (для англ.)
  delay(100);
  mySerial.println("AT+CSCS=\"GSM\"");  //режим кодировки текста
  delay(100);  

  Timer1.initialize(2000000); // один раз в 2 сек
  Timer1.attachInterrupt(gsm_rxlev);
  //Timer1.attachInterrupt(blinkLED);
  
}

unsigned long currentMillis;
boolean led_blinker;

void loop() 
{
currentMillis = millis();

main_state=DISARM; // для отладки

// опрос входов
   dig_inp();

// выдача дискртеных сигналов
   dig_outp();
}
//////////////////////////
void log_msg(String msg)
{
   Serial.println(msg);  
}

//////////////////////////
void blinkLED (void)
{  
   if (led_blinker==1) led_blinker=0; 
   else led_blinker=1;       
}

///////////////////////////
void dig_inp(void)
{  
  pir_1 = digitalRead(PIR1);
  pir_2 = digitalRead(PIR2);
  pir_3 = digitalRead(PIR3);
  pir_4 = digitalRead(PIR4);
  ext_power = digitalRead(EXT_POWER);
  led_mode = digitalRead(BTN_MODE);  
}

///////////////////////////
void dig_outp(void)
{ 
   // индикация срабатывания датчиков
   if (pir_1==1 || radio_1==1) digitalWrite(LED_1, LED_ON);
   else digitalWrite(LED_1, LED_OFF);
  
   if (pir_2==1 || radio_2==1) digitalWrite(LED_2, LED_ON);
   else digitalWrite(LED_2, LED_OFF);

   if (pir_3==1 || radio_3==1) digitalWrite(LED_3, LED_ON);
   else digitalWrite(LED_3, LED_OFF);

   if (pir_4==1 || radio_4==1) digitalWrite(LED_4, LED_ON);
   else digitalWrite(LED_4, LED_OFF);

   // индикация статуса сигнализации
   switch (main_state)
   { case  DISARM: // "Снято с охраны" - не горит
                   digitalWrite(LED_STAT, LED_OFF);  
                   digitalWrite(RELAY_LED, RELAY_OFF);
                   break;
     case  ARM:    // "Охрана" - горит постоянно
                   digitalWrite(LED_STAT, LED_ON);  
                   digitalWrite(RELAY_LED, RELAY_ON);
                   break;
     case  ALARM:  // "Тревога" - мигает 2 раза в секунду
                   if (led_blinker) 
                   {
                     digitalWrite(LED_STAT, LED_ON);  
                     digitalWrite(RELAY_LED, RELAY_ON);
                   } else {
                            digitalWrite(LED_STAT, LED_OFF);  
                            digitalWrite(RELAY_LED, RELAY_OFF);
                          }
                   break;
    }      
}
//////////////////////////
void gsm_rxlev (void)
{  
   String msg="";
  if (main_state==ALARM) return;
  
    mySerial.println("AT+CSQ\r");//запрос уровня сигнала
    log_msg("level");
    delay (10);
    if (mySerial.available()) //если GSM модуль послал ответ, то
    {  
      while (mySerial.available()) 
      {  //сохраняем входную строку в переменную msg
         char c = mySerial.read();
         if (c=='\r')
         { // пришел конец строки
           //log_msg(msg);         
           String str_part=msg.substring(msg.indexOf(",")-2,2);
           log_msg(str_part);
           gsm_level=-113+str_part.toInt()*2;    
           //Serial.println(gsm_level);       
         } else {
           msg += c;          
         }         
         // delay(10);
      }
    }  
}
//////////////////////////





