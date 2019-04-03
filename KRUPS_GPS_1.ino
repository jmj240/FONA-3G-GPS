#include "Adafruit_FONA.h"

#define FONA_RST 11
// this is a large buffer for replies
char replybuffer[255];
//set pins for the the FONA to access the teensy, 
//pins 7(RX) and 8(TX)
HardwareSerial *fonaSerial = &Serial3;
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup(){
  while (!Serial);
  //starting baud rate, later set to 4800 per FONA requirements
  Serial.begin(115200);
  Serial.println(F("FONA basic test, for krups code"));
  fonaSerial->begin(4800);
  //Connect to FONA
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found FONA"));
  // Print module IMEI number.
  char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
}

void loop() {
  while (! Serial.available() ) {
    if (fona.available()) {
      Serial.write(fona.read());
    }
  }
  char command = Serial.read();
  Serial.println(command);
  
  switch (command) {
   case 'R': { 
      Serial.println(F("-------------------------------------"));
      
      Serial.println(F("Battery level: "));
        // read the battery voltage and percentage
        uint16_t vbat;
        if (! fona.getBattVoltage(&vbat)) {
          Serial.println(F("Failed to read Batt"));
        } else {
          Serial.print(F("VBat = ")); Serial.print(vbat); Serial.println(F(" mV"));
        }
        if (! fona.getBattPercent(&vbat)) {
          Serial.println(F("Failed to read Batt"));
        } else {
          Serial.print(F("VPct = ")); Serial.print(vbat); Serial.println(F("%"));
        }
        
        // read the network/cellular status
        uint8_t n = fona.getNetworkStatus();
        Serial.print(F("Network status "));
        Serial.print(n);
        Serial.print(F(": "));
        if (n == 0) Serial.println(F("Not registered"));
        if (n == 1) Serial.println(F("Registered (home)"));
        if (n == 2) Serial.println(F("Not registered (searching)"));
        if (n == 3) Serial.println(F("Denied"));
        if (n == 4) Serial.println(F("Unknown"));
        if (n == 5) Serial.println(F("Registered roaming"));
        
      Serial.println(F("Activating GPS"));
        // turn GPS on
        if (!fona.enableGPS(true))
          Serial.println(F("Failed to turn on"));
          
      Serial.println(F("Query GPS location"));
        // check for GPS location
        char gpsdata[120];
        fona.getGPS(0, gpsdata, 120);
        Serial.println(F("Reply in format: mode,fixstatus,utctime(yyyymmddHHMMSS),latitude,longitude,altitude,speed,course,fixmode,reserved1,HDOP,PDOP,VDOP,reserved2,view_satellites,used_satellites,reserved3,C/N0max,HPA,VPA"));
        Serial.println(gpsdata);
      
      Serial.println(F("Send SMS with location"));
      // send an SMS!
        char sendto[11]= "**********";
        char message[141];
        Serial.print(F("Send to #"));
        Serial.println(F("**********"));
        Serial.print(F("Sending location...."));
        Serial.println(gpsdata);
        if (!fona.sendSMS(sendto, gpsdata)) {
          Serial.println(F("Failed"));
        } else {
          Serial.println(F("Sent!"));
        }
      break;
     }
   case 'x': {
     Serial.println(F("Stop"));
     break;
   }
   default: {
     Serial.println(F("Type R to test code"));
     break;
   }
  }
}
