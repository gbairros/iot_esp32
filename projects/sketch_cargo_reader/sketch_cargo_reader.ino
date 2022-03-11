#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#include <WiFi.h>
#include <HTTPClient.h>

#define SS_PIN 21
#define RST_PIN 22
#define PIN_VERDE 12


const int ipaddress[4] = {103, 97, 67, 25};
//Variables
byte nuidPICC[4] = {0, 0, 0, 0};

const char* ssid = "Bairros_wifi";
const char* password = "@10212728";
const char* serverName = "http://10.0.0.114/cargoreader/index.php";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;


MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_VERDE, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  
  Serial.println(F("->Inicializando Sistema RFID"));
  SPI.begin();
  //SPI.begin(18, 19, 23);

  rfid.PCD_Init();
  Serial.print(F("->Versão do Leitor:"));
  rfid.PCD_DumpVersionToSerial();
}

void send_data(String action, String tag_id){
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, serverName);
    
    //http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        
    String httpRequestData = "action="+action+"&tag_id="+tag_id ;           
    int httpResponseCode = http.POST(httpRequestData);
    //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");
   
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();  
    Serial.print("HTTP Response TEXT: ");
    Serial.print(response);
      
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  lastTime = millis();
}


void readRFID(void ) { /* function readRFID */
    ////Read RFID card
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    // Look for new 1 cards
    //Serial.println("Aguarda Leitura");    
    
    if ( ! rfid.PICC_IsNewCardPresent())
        return;
    // Verify if the NUID has been readed
    if (  !rfid.PICC_ReadCardSerial())
        return;
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = rfid.uid.uidByte[i];
    }
    
    String tag_id = getTagCode(rfid.uid.uidByte, rfid.uid.size);
    Serial.println("TAG CODE: "+tag_id);
    
    digitalWrite(PIN_VERDE, HIGH);
    delay(1000);
    digitalWrite(PIN_VERDE, LOW);    
    
    send_data("in", tag_id);
        
    Serial.println();
    
    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
}


String getTagCode(byte *buffer, byte bufferSize) {
    String tag_code = "";
        for (byte i = 0; i < bufferSize; i++) {
        tag_code .concat(String(buffer[i] < 0x10 ? "0" : ""));
        tag_code .concat(String(buffer[i], HEX));
       
    }

    return tag_code;
}


void loop() {
    readRFID();
}
