#include <SoftwareSerial.h>  

#define GSM_BOOT   A5

char gsm_char=0; 
//word RING='RING';
//Stores character from the gsmSerial
//char arduino_char=0; //Stores character from the Serial

SoftwareSerial gsmSerial(3,4);  //Creates a software serial port. (rx,tx)
//int greenPin = 4;
//int yellowPin = 5;
//int incomnomber;

void setup()
{
  pinMode(GSM_BOOT, OUTPUT);
  digitalWrite(GSM_BOOT, LOW);
  delay(100);
  
  //Initialize serial ports for communication.
  Serial.begin(9600);
  gsmSerial.begin(9600);
  Serial.println("Starting TC35 and debug Communication...");
  gsmSerial.println("AT+CLIP=1");  //включаем АОН
 delay(100);
 gsmSerial.println("AT+CMGF=1");  //режим кодировки СМС - обычный (для англ.)
 delay(100);
 gsmSerial.println("AT+CSCS=\"GSM\"");  //режим кодировки текста
 delay(100);
}

void loop() {
  //If a character comes in from the cellular module...
  if(gsmSerial.available() > 0){
    gsm_char=gsmSerial.read();    //Store the char in gsm_char.
    Serial.print(gsm_char);  //Print it to debug serial    
  }
  
  //Read serial input
  if(Serial.available() > 0){
    gsm_char=Serial.read();  //Store char in gsm_char (Not really from the gsm, just saving some memory)
    //Evaluate input.
    if(gsm_char=='t'){
      gsmSerial.print("AT+csq\r");  //Send test command
    }
        
    else if(gsm_char=='p'){
      gsmSerial.print("AT+CPIN=PINNUMBER\r");  //Enter pin, hardcoded
    } else if(gsm_char=='g'){
      gsmSerial.print("AT+CMGF=1\r"); //Set text mode.
    } else if(gsm_char=='s'){
      //Send sms!
      gsmSerial.print("AT+CMGS=+79026053421; \r"); //AT command to send SMS
      delay(500);
      gsmSerial.print("Hello"); //Print the message
      delay(500);
      // Отправляем Ctrl+Z, обозначая, что сообщение готово
    gsmSerial.println((char)26);
      //gsmSerial.print("\x1A"); //Send it ascii SUB
    }else if (gsm_char=='o'){
     gsmSerial.print("AT+CLCC\r");//AT опред.номера
     delay(100);
    }else if (gsm_char=='h'){
     gsmSerial.print("ATH\r");//AT положить трубку
     delay(100);
   }else if (gsm_char=='c'){
     gsmSerial.print("ATDT +79128016640; \r");//AT опред.номера
     delay(100);
     }else if (gsm_char=='q'){
     gsmSerial.print("AT+CCLK?; \r");//Запрос на время
     delay(100);
    }else if (gsm_char='\RING'){
                     //gsmSerial.print("AT+CLCC\r");//AT опред.номера
     delay(100);
    }
    }
    }
    

    

