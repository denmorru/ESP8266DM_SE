#include <ESP8266WiFi.h>         
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         
#include <ESP8266SSDP.h>
#include <aREST.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <FS.h>
#include <ESP8266NetBIOS.h>
#include <ArduinoJson.h>  

#define UART_SERIAL_SPEED 9600
#define COMM_SERIAL_SPEED 9600
#define COMM_DEBUG_MODE false
#define LISTEN_WEBSERVER_PORT 80
#define TCP_SERVER_PORT 8888
#define UART_SERVER_PORT espSerial //CO2 sensor
#define COMM_DEBUG_PORT Serial     //computer
#define DEVICE_ID "DM0002"
#define DEVICE_NAME "DM_sensor_device"
#define DEVICE_MODEL_NAME "DM2SD"
#define TCPbufferMax 128
#define MAX_TCPSRV_CLIENTS 1
unsigned char response[7];
byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
int bufferSize =0;
unsigned long previousMillis = 0;       
const long interval = 5000; 
int myChipId =666666;
String lastResponse;
String webPageContent;
const char* STR_OKEY = "ok";
const char*  STR_TRUE = "true";
int co2 {-1};
//const char* ssid = "denmorru";
//const char* password = "!Denmorru6";
ESP8266WebServer HTTP(LISTEN_WEBSERVER_PORT);
WiFiServer SERVERaREST(8080);
WiFiClient tcpClient;
SoftwareSerial espSerial(13,15); // RX_pin, TX_pin
aREST rest = aREST();

void setup() {
      //      Serial1.begin(115200);      Serial1.println(" SE1 ");        Serial1.setDebugOutput(false);      Serial.begin(115200);
        
        UART_SERVER_PORT.begin(UART_SERIAL_SPEED); 
       // UART_SERVER_PORT.flush();        UART_SERVER_PORT.swap();
        if(COMM_DEBUG_PORT != UART_SERVER_PORT){
        //COMM_DEBUG_PORT.setDebugOutput(COMM_DEBUG_MODE);
        COMM_DEBUG_PORT.begin(COMM_SERIAL_SPEED); 
       // COMM_DEBUG_PORT.flush();COMM_DEBUG_PORT.swap();
        }
        
       // Serial.println("Serial initialized");          espSerial.begin(UART_SERIAL_SPEED); 
        COMM_DEBUG_PORT.println(" ");
        COMM_DEBUG_PORT.println("Flash: ");
        COMM_DEBUG_PORT.println(ESP.getFlashChipSize());
        VIMA_init();
        COMM_DEBUG_PORT.println("WiFiManager initialized");
        HTTP_init();
        COMM_DEBUG_PORT.println("HTTP initialized");
        SSDP_init();
        COMM_DEBUG_PORT.println("SSDP initialized");
        REST_init();
        COMM_DEBUG_PORT.println("aREST initialized");  
        //COMM_DEBUG_PORT.println("M117 WiFiServer initialized");
        if (MDNS.begin("ESP8266DM")) {
          MDNS.addService("http", "tcp", LISTEN_WEBSERVER_PORT);
        //COMM_DEBUG_PORT.println("M117 MDNS responder started");
        }
        NBNS.begin("ESP8266DM");
        //COMM_DEBUG_PORT.println("M117 NBNS responder started");
       
        //Serial.println("WiFi:");
        //WiFi.printDiag(Serial);
        UART_SERVER_PORT.print("M117 IP ");
        UART_SERVER_PORT.println(WiFi.localIP());
        EEPROM.begin(512);
}
void loop() {
      HTTP.handleClient();
      unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      readCO2();      
      //DMUART_process();
      //serialPassthrough(); 
      REST_process();
   }
}
void SSDP_init(void){
        SSDP.setDeviceType("upnp:rootdevice");
        SSDP.setSchemaURL("description.xml");
        SSDP.setHTTPPort(LISTEN_WEBSERVER_PORT);
        SSDP.setName(DEVICE_NAME);
        myChipId=ESP.getChipId();
        SSDP.setSerialNumber(myChipId);
        SSDP.setURL("index.html");
        SSDP.setModelName(DEVICE_MODEL_NAME);
        SSDP.setModelNumber(DEVICE_MODEL_NAME);
        SSDP.setModelURL("http://digitalmy.ru/wSmartHome.php");
        SSDP.setManufacturer("DM");
        SSDP.setManufacturerURL("http://digitalmy.ru");
        SSDP.setTTL(2);
        SSDP.begin();
}
void HTTP_init(void){
        HTTP.on("/index.html", HTTP_handleRootPage);
        HTTP.on("/description.xml", HTTP_GET, [](){
          SSDP.schema(HTTP.client());
        });
        HTTP.on("/", HTTP_handleRootPage);
        HTTP.onNotFound(HTTP_handleNotFound);
        HTTP.begin();
}
void REST_init(void){
      rest.variable("CO2ppm",&co2);
      rest.set_id(DEVICE_ID);
      rest.set_name(DEVICE_NAME);
      SERVERaREST.begin(); 
}
void VIMA_init(void){
        WiFiManager wifiManager;
        wifiManager.setDebugOutput(COMM_DEBUG_MODE);
        wifiManager.setAPStaticIPConfig(IPAddress(1,1,1,1), IPAddress(1,1,1,1), IPAddress(255,255,255,0));
        wifiManager.autoConnect("ESP8266DM");
}
void HTTP_handleRootPage() {
      webPageContent="DM web server for sensor device";
        HTTP.send(200, "text/plain", webPageContent);
}

void HTTP_handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += HTTP.uri();
  message += "\nMethod: ";
  message += (HTTP.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += HTTP.args();
  message += "\n";
  for (uint8_t i = 0; i < HTTP.args(); i++) {
    message += " " + HTTP.argName(i) + ": " + HTTP.arg(i) + "\n";
  }
  HTTP.send(404, "text/plain", message);
}

void readCO2() {
        // CO2
        bool header_found {false};
        char tries {0};

        UART_SERVER_PORT.write(cmd, 9);
        memset(response, 0, 7);

        // Looking for packet start
        while(UART_SERVER_PORT.available() && (!header_found)) {
                if(UART_SERVER_PORT.read() == 0xff ) {
                        if(UART_SERVER_PORT.read() == 0x86 ) header_found = true;
                }
        }

        if (header_found) {
                UART_SERVER_PORT.readBytes(response, 7);

                byte crc = 0x86;
                for (char i = 0; i < 6; i++) {
                        crc+=response[i];
                }
                crc = 0xff - crc;
                crc++;

                if ( !(response[6] == crc) ) {
                        COMM_DEBUG_PORT.println("CO2: CRC error: " + String(crc) + " / "+ String(response[6]));
                } else {
                        unsigned int responseHigh = (unsigned int) response[0];
                        unsigned int responseLow = (unsigned int) response[1];
                        unsigned int ppm = (256*responseHigh) + responseLow;
                        co2 = ppm;
                        COMM_DEBUG_PORT.println("CO2:" + String(co2));
                }
        } else {
                COMM_DEBUG_PORT.println("CO2: Header not found");
        }

}

void REST_process(){
    WiFiClient client = SERVERaREST.available();
      if (!client) {
        return;
      }
     if(client.available()) {
        rest.handle(client);
     }
}
