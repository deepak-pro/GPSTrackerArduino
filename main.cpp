#include <SoftwareSerial.h> 

#define RX 10
#define TX 11

SoftwareSerial gsm(RX,TX);

int timeInterval = 10 ;
String number = "<Mobile Number Here>";
String Grsp ;
String gsmString ;
String gsmLink ;
String speedsignal = "$GPVTG";
String inputString = ""; 
boolean stringComplete = false;
String speed= "Not Available";
String signal = "$GPGLL";
String sat ;
String altitude ; 
bool haveSignal = false ;
String str ;
int n = 0 , num = 0 , remain = 7 ;

void setup() {

    Serial.begin(9600);
    gsm.begin(4800); 
    inputString.reserve(200);
    delay(10000);
    
    sendMessage(number,"Tracker Started you will get location in few minutes.");
    Serial.println("Tracker Started");
    delay(5000);

    gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
    delay(1000);
    gsm.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");
    delay(1000);
    gsm.println("AT+SAPBR=1,1");
    delay(5000);
    Serial.println("Setup Completed");
}

String getGsmLocation(){
  gsm.println("AT+CIPGSMLOC=1,1");
   while(true){
    if(gsm.available())
    {
      Grsp = gsm.readString();
      Serial.println(Grsp);
      if(Grsp.indexOf("+CIPGSMLOC:") > 0)
        break;   
      else{
        Serial.println("Location not found, wait few minutes.");
        return "Location not found, wait few minutes.";
      }
    }
  }
  gsmString = Grsp.substring(16,35);
  gsmLink = gsmString.substring(10,19);
  gsmLink = gsmLink + "," ;
  gsmLink = gsmLink + gsmString.substring(0,9);
  gsmLink = "http://maps.google.com/maps?q=" + gsmLink ;
  gsmLink = gsmLink + " Accuracy within 1 kilometre." ;
  return gsmLink ;
}

void sendMessage(String number, String text) {
  gsm.begin(9600);
  delay(1000);
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.print("AT+CMGS=");
  gsm.print("\"");
  gsm.print(number);
  gsm.println("\"");
  delay(1000);
  gsm.println(text);
  gsm.println(char(26));
  Serial.println("Message Sent");
}

void sendLocation(String link,int e){
  Serial.println(link);
  if(e==0){
    link=link+ " Speed:   " + speed  +" kmph";
    sendMessage(number,link);
  }
  else{
    sendMessage(number,link);
  }
  delay(timeInterval * 1000);
}

void loop() {
 if (stringComplete) {
       String signalType = inputString.substring(0,6);
        if(signalType == speedsignal){
          //speed = inputString.substring(21,26);
          str = inputString ;
          while(remain>0){
             if(str[num++]==',')
                --remain ;
         }
         n = 0;
         while(true){
         speed[n++] = str[num++];
         if(str[num] == ',')
            break;
         }
       } 
        if (signalType == signal) {
            int c = inputString.indexOf('V');
            //Serial.print("Index of V is " );
            //Serial.println(c);
            if(c == -1){
              haveSignal = true ;
            }else{
              haveSignal = false ;
            }
            String LAT = inputString.substring(7, 17);
            int LATperiod = LAT.indexOf('.');
            int LATzero = LAT.indexOf('0');
            if (LATzero == 0) {
                LAT = LAT.substring(1);
            }
        
            String LON = inputString.substring(20, 31);
            int LONperiod = LON.indexOf('.');
            int LONTzero = LON.indexOf('0');
            if (LONTzero == 0) {
                LON = LON.substring(1);
            }
            
            String link = "http://maps.google.com/maps?q=" ;
            if(LAT[32] == 'S' ){
              link += "-" ;
            }
            link += LAT.substring(0,2);
            link += "+" ;
            link += LAT.substring(2,4);
            link += "." ;
            link += LAT.substring(5,7) ;
            
            link += LAT.substring(7) ;
            link += "," ;
            if(LAT[32] == 'W' ){
              link += "-" ;
            }
            link += LON.substring(0,2);
            link += "+" ;
            link += LON.substring(2,4);
            link += "." ;
            link += LON.substring(5,7) ;
            
            link += LON.substring(7) ;
            if(haveSignal){
              sendLocation(link,0);
              Serial.println("Having Signal Sending it");
            }else{
              Serial.println("Not having Signal Sending it ");
              link = getGsmLocation();
              sendLocation(link,1);
            }
        }
        inputString = "";
        stringComplete = false;
    }
}

void serialEvent() {
    while (Serial.available()) {
        char inChar = (char) Serial.read();
        inputString += inChar;
        if (inChar == '\n') {
            stringComplete = true;
        }
    }
}
