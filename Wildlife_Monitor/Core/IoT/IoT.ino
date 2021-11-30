#include <RadioLib.h>

#define NSS 10
#define MOSI 11
#define MISO 12
#define SCK 13
#define NRST 9
#define BUSY 8
#define DIO1 2 //interrupt pin
#define RXEN 6
#define TXEN 7
#define BTNPIN 4
#define LEDPIN 3
#define RECEIVING 0
#define TRANSMITTING 1
#define DELAY 1000;

SX1262 radio = new Module(NSS, DIO1, NRST, BUSY);
int msgSent,msgRec = 0;
int transmissionState = ERR_NONE;
String Savedmsgs="Start\n";
String MLmsg="";
FILE *fp;
int x = 0;
void setup() {
  Serial.begin(9600);
  digitalWrite(NRST,LOW);
  delay(100);
  digitalWrite(NRST,HIGH);
  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}


volatile bool transmittedFlag = false;
volatile bool receivedFlag = false;
volatile bool soundFlag = false;
volatile bool enableInterrupt = true;

void setFlagT(void) {

  if(!enableInterrupt) {
    return;
  }
  transmittedFlag = true;
}

void setFlagR(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }
  receivedFlag = true;
}
void loop() {
  
 if(digitalRead(BTNPIN)==1 && msgSent == 0){
    radio.setDio1Action(setFlagT);
    transmittedFlag = true;
    msgSent =1;
    msgRec=0;
 }
 else if(digitalRead(BTNPIN)==0 && msgRec ==0){
    radio.setDio1Action(setFlagR);
    radio.startReceive();
    msgSent=0;
    msgRec =1;
    //transmittedFlag= false;
  }
  //delay(500);
//---------------------------------------------------------------
  if(transmittedFlag == true) {
    enableInterrupt = false;
    String message = (String)x++ +",hh:mm:ss,Location\n";  //test message
    transmittedFlag = false;

    Serial.print(F("[SX1262] Sending packet:...\n"));
    Serial.print(message);

    transmissionState = radio.startTransmit(message);
    if (transmissionState == ERR_NONE) {
      Serial.println(F("\t...transmission finished!"));
    delay(1000);

    } else {
      Serial.print(F("\t...transmission failed, code "));
      Serial.println(transmissionState);
    }
    Savedmsgs="";
    enableInterrupt = true;
  }
//---------------------------------------------------------------------
  if(receivedFlag) {
    enableInterrupt = false;
    receivedFlag = false;
    String str;
    
    int counter = 0;
    int change = 0;
    int len =0;
    int state = radio.readData(str);    //Puts data into str from input signal

 
    if (state == ERR_NONE) {            //if no error
      Savedmsgs+=(str);
      Serial.print(F("[SX1262] Saved Data:\n"));
      Serial.println(Savedmsgs);
    } else if (state == ERR_CRC_MISMATCH) {
      Serial.println(F("CRC error!"));

    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
  
    }
    radio.startReceive();
    enableInterrupt = true;
  }
//-----------------------------------------------------------
  if(soundFlag){
    MLmsg;
  }
}
