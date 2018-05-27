/*------ Headers to Include ------*/
#include <SPI.h>
#include "WizFi250.h"
#include <Servo.h>
/*------ Ubidots information ------*/
#define TOKEN      "BBFF-H4KjfVD4u707LgfW3rfh4lYrneSsio"
#define VARID_CDS      "5abe8173642ab6772da6bc2a"
#define VARID_LED      "5abea2b7642ab611e6f49cd4"
#define VARID_SERVO      "5afa7e28642ab617247608a5"
//Parse JSON
#define PARSE       "\"value\""
#define ENDPARSE    ","

/*------ Pin Define ------*/
#define CDS_PIN   A0
#define LED_PIN   13
#define SERVO_PIN   9
/*------ Objects ------*/
WiFiClient client;  // Initialize the Ethernet client object
Servo myservo;

/*------ Functions ------*/
int getInt(String input);
void httpRequest(String VARID);
void postData(String VARID, float data);
int parser_value(String buf, String Param);
void printWifiStatus();
char * floatToString(char * outstr, double val, byte precision, byte widthp);
/*------ Global Variables ------*/
char server[] = "thing.ubidots.com";
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 1000L; // delay between updates, in milliseconds
String rcvbuf;
boolean readingVal;
boolean readingId;
boolean getIsConnected = false;
String valString;
String idString;
int val, gogo;
boolean get_val_flag;
float brightness;

char ssid[] = "JW Pocket-Fi";       // your network SSID (name)
char pass[] = "01073203334";        // your network password
int status = WL_IDLE_STATUS;       // the Wifi radio's status

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(CDS_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  myservo.attach(SERVO_PIN);
  Serial.begin(115200);
  Serial.println(F("\r\nSerial Init"));
  WiFi.init();
  if (WiFi.status() == WL_NO_SHIELD) { //WiFi 실드가 없으면 대기
    Serial.println("WiFi shield not present");
    while (true);
  }

  while ( status != WL_CONNECTED) { //WiFi 연결 시도 될때까지
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network
  }
  Serial.println("You're connected to the network");
  printWifiStatus();
}

void loop() {
  brightness = analogRead(CDS_PIN);

  while (client.available()) {
    char c = client.read();
    if ( c != NULL ) {
      if (rcvbuf.length() > 20)
        rcvbuf = "";
      rcvbuf += c;
      Serial.write(c);
    }


    if ( rcvbuf.endsWith("\"value\": ")) {
      readingVal = true;
      valString = "";
    }

    if (readingVal) {
      if (c != '.') {
        valString += c;
      }
      else {
        readingVal = false;
      }
    }
  }



  if (millis() - lastConnectionTime > postingInterval) {
    if (getIsConnected) {
      postData(VARID_CDS, brightness);
      
      val = getInt(valString);

      Serial.print(F("servo : "));   Serial.println(val);
      myservo.write(val);
    }
    httpRequest(VARID_SERVO);
  }

  rcvbuf = "";
}
