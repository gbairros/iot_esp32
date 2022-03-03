#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "Bairros_wifi";
const char* password = "@10212728";
const char* serverName = "http://10.0.0.114/cargoreader/index.php";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
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

void loop() {
  send_data("in", "100");

  delay(1000);

}
