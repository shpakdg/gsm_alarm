#include <SoftwareSerial.h>
#include <RCSwitch.h> //433mhz sensor

RCSwitch mySwitch = RCSwitch();
String value = "";
int MHzAlarm = 0; //433mhz sensor статус безпроводного датчков
///////////////////////////////quatety and time period of SMS
int s = 0;//smscounter
boolean SMSWasSend = 0;
static bool state;
static unsigned long time;
//////////////////////////////
SoftwareSerial mySerial(10, 11); // RX, TX
int ch = 0;
//int led = 3;
String val = "";//txt of sms
boolean callon = false; //switch of security
boolean lastcall = false;	

boolean statusOXR = false;

const int pbp3 = 3;
int buttonState3 = 0;
const int pbp4 = 4;
int buttonState4 = 0;
const int pbp5 = 5;
int buttonState5 = 0;
const int pbp6 = 6;
int buttonState6 = 0;

int my [] = {0, 0, 0, 0};

const int ledPin = 8;//led alarm. Light
const int SoundAlarm=9 //Звуковая сигнализация
const int ledSecurity = 12;       //Статус сигнализации на охране или нет 
  const int statusGSM = 13;   	//Есть сигнал или нет
// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
long intervalTestGSM = 5000; //interval GSM network will tested
//long time = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(SoundAlarm, OUTPUT);
  pinMode(ledSecurity, OUTPUT);
  pinMode(statusGSM, OUTPUT);



  pinMode(pbp3, INPUT);
  pinMode(pbp4, INPUT);
  pinMode(pbp5, INPUT);
  pinMode(pbp6, INPUT);

  delay(2000);  //время на инициализацию модуля GSM
  //  pinMode(led, OUTPUT);
  //  digitalWrite(led, LOW);
  Serial.begin(9600);  //скорость порта
  Serial.println("GSM tester v1.0");
  mySerial.begin(9600);
  mySerial.println("AT+CLIP=1");  //включаем АОН
  delay(100);
  mySerial.println("AT+CMGF=1");  //режим кодировки СМС - обычный (для англ.)
  delay(100);
  mySerial.println("AT+CSCS=\"GSM\"");  //режим кодировки текста
  delay(100);
}

void loop() {


  //statusOXR = digitalRead(pbp);


  buttonState3 = digitalRead(pbp3);
  buttonState4 = digitalRead(pbp4);
  buttonState5 = digitalRead(pbp5);
  buttonState6 = digitalRead(pbp6);


/////////////////////запрос уровня сигнала
  if (millis() - previousMillis > intervalTestGSM)
  {
    previousMillis = millis();
    mySerial.println("AT+csq\r");//запрос уровня сигнала
    delay (10);
  }

  //////////////////////////////////////////
  statusOXR = true;////для отладки
  //////////////////////////////////////////
  /////////////////мигаем ledSecurity(Статус сигнализации на охране или нет), если statusOXR == true  
  if (statusOXR == true)
  {
    if (millis() - previousMillis > interval)
    {
      // save the last time you blinked the LED
      previousMillis = millis();
      //Serial.println(previousMillis);

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;

      // set the LED with the ledState of the variable:
      digitalWrite(ledSecurity, ledState);

    }

    else
    {
      digitalWrite(ledSecurity, LOW);
    }
    ////////////////////433//////////////////////////////////////
    if (mySwitch.available())
    {

      value = mySwitch.getReceivedValue();

      if (value.indexOf("10962745") > -1 || value.indexOf("13385001") > -1)//проверяем состояние безпроводных датчков
      {
        MHzAlarm = 1; //статус безпроводного датчков       
      }
    }

    mySwitch.resetAvailable();
    //delay (100);
    //////////////////////433///////////////////////////////////////


    int i;//записываем в msg статус датчков
    int my [] = {buttonState6, buttonState5, buttonState4, buttonState3, 0}; //try to make value my String.
    String msg = "";
    for ( i = 0; i < 4; i++) {
      Serial.print(my[i]);
    }
    Serial.print("\t status=");
    //check buttons for alarm
    if (buttonState6 == 1 || buttonState3 == 1 || buttonState4 == 1 || buttonState5 == 1 || MHzAlarm == 1)
    {
 ////////////если statusOXR == true то, отправляем СМС каждые 60 сек 3 раза     
	  if (statusOXR == true)
      {
        digitalWrite(ledPin, HIGH);
		static bool stateSoundAlarm;
		static unsigned long timeSoundAlarm;
        if ((millis()-timeSoundAlarm)>10000)//Включаем сирену через 10сек+время отправки СМС после срабтки датчиков
		{
		stateSoundAlarm=1;
		timeSoundAlarm=milis();
		
		digitalWrite(SoundAlarm, HIGH);
		}
        if ((millis() - time) > (state ? 60000 : 1000))//Send sms each 60 sec.
        {
          state = !state;
          time = millis();
          Serial.print(millis());
          Serial.print(" ");
          Serial.println(state);
          if (state == 1)
          {
            if ( s <= 2 && SMSWasSend == 0)//send SMS 3 times
            {
              s++;
              Serial.print(s);
              sms(msg, "+79026053421");  //отправляем СМС на номер +71234567890//sendsms
              Serial.println(" ");
              SMSWasSend = 1;
            }
          }
          else
          {
            SMSWasSend = 0;
          }

        }
        delay (1);
      }
      else
      {
        Serial.print("warning");
      }
    }
    else
    {
      digitalWrite(ledPin, LOW);
	  digitalWrite(SoundAlarm, LOW);
      Serial.print("ok");
    }

    Serial.println();




  }
  // int i;
  //   int my [] = {buttonState5,buttonState4,buttonState3,buttonState2,0};
  //   for( i = 0; i < 4; i++) {
  //     Serial.print(my[i]);




  if (mySerial.available()) {  //если GSM модуль что-то послал нам, то
    while (mySerial.available()) {  //сохраняем входную строку в переменную val
      ch = mySerial.read();
      val += char(ch);
      Serial.println(val);
      delay(10);
    }



    if (val.indexOf("CSQ") > -1)
    {
      if (val.indexOf("99") > -1)//если сети нет то лампа гаснет в противном горит
      {
        //mySerial.println("AT+CCLK?\r");//запрос времени
        //запомнить время пропадания
        //
        digitalWrite(statusGSM, LOW);
      }
      else
      {
        digitalWrite(statusGSM, HIGH);
        //mySerial.println("AT+CCLK?\r");//запрос времени
        //запомнить время появления сети
        // отправить СМС с временем отсутствия сети
      }
    }




    if (val.indexOf("RING") > -1) {  //если звонок обнаружен, то проверяем номер
      if (val.indexOf("79026053421") > -1 || val.indexOf("79026053498") > -1) {  //если номер звонящего наш. Укажите свой номер без "+"
        Serial.println("--- MASTER RING DETECTED ---");
        mySerial.println("ATH0");  //разрываем связь
        delay(1000);
        if (lastcall == false) //проверяем был ли это звонок включения или выключеня
        {
          callon = true; //заменить на statusOXR
          lastcall = true;
          statusOXR = true;
          sms(String("SET"), String("+79026053421"));  //отправляем СМС на номер +71234567890//sendsms
          // digitalWrite(ledSecurity, true);  //включаем выключаем реле security
        }
        else
        {
          lastcall = false;
          callon = false;
          statusOXR = false;
          sms(String("RESET"), String("+79026053421"));  //отправляем СМС на номер +71234567890//sendsms
          //digitalWrite(ledSecurity, false);  //включаем выключаем реле security
        }

        // statusOXR=callon;

        digitalWrite(ledSecurity, callon);  //включаем выключаем реле security
      }
    } else
      Serial.println(val);  //печатаем в монитор порта пришедшую строку
    val = "";
  }
  /*
    if (Serial.available()) {  //если в мониторе порта ввели что-то.
    while (Serial.available()) {  //сохраняем строку в переменную val
      ch = Serial.read();
      val += char(ch);
      delay(10);
    }
    //mySerial.println(val);  //передача всех команд, набранных в мониторе порта в GSM модуль
    if (val.indexOf("sendsms") > -1) {  //Если //если увидели команду отправки СМС
      sms(String("hello world"), String("+79026053421"));  //отправляем СМС на номер +71234567890
    }
    val = "";  //очищаем
    }
  */

  delay(10);
}

void sms(String text, String phone)  //процедура отправки СМС
{
  Serial.println("SMS send started");
  mySerial.println("AT+CMGS=\"" + phone + "\"");
  delay(500);
  mySerial.print(text);
  delay(500);
  mySerial.print((char)26);
  delay(500);
  Serial.println("SMS send complete");
  delay(2000);
}
