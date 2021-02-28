/*
*    Simple card reader for Adichip. 
*    
*    Program version: 1.5.1
*    Copyright © 2021 FDR-96
*    Copyright © 2021 ADICHIP
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SCK  (13) 
#define PN532_MOSI (14) 
#define PN532_SS   (33)  
#define PN532_MISO (27)
#define PN532_IRQ   (26)
#define PN532_RESET (25)
  
#include <time.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define FIREBASE_HOST ""

/** The database secret is obsoleted, please use other authentication methods, 
 * see examples in the Authentications folder. 
*/
#define FIREBASE_AUTH ""

//Define FirebaseESP32 data object
FirebaseData fbdo;
FirebaseJson json;

const int PIN_BOB = 4;

int tarjeta;
int llavero;

String nombre;
String variable;
String valor; 
String variableFinal;

void printResult(FirebaseData &data);
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void setup() { 
  Serial.begin(115200);
  pinMode(PIN_BOB, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  //optional, set the decimal places for float and double data to be stored in database
  Firebase.setFloatDigits(2);
  Firebase.setDoubleDigits(6);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
  // put your setup code here, to run once:

}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
  // the UID, and uidLength will indicate the size of the UUID (normally 7)
  
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
   
      Serial.println("Found an ISO14443A card");
      Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      Serial.print("  UID Value: ");
      nfc.PrintHex(uid, uidLength);
      Serial.println("");
      for(int i = 0; i <= uidLength; i++){
          variable = uid[i];
          valor = variable; 
          variableFinal += valor;
    }
    Serial.println(variableFinal);
    if(Firebase.getBool(fbdo, "V1/RFID/" + variableFinal + "/Ingresar")){
        digitalWrite(PIN_BOB, HIGH);
        delay(3000);
        digitalWrite(PIN_BOB, LOW);
    }
   variableFinal = "";
    //We check if it is registered.

  //We read the state our flag is in in FB DB Realtime.
     
  } 
  

 
  // put your main code here, to run repeatedly:
}