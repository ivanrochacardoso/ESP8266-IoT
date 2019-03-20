#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "wifi1"
#define STAPSK  "senhawf1"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

#define LED 2

const int saida1 = 16;//


WiFiServer server(80);

uint8_t ousaida1, ent1;


WiFiUDP ntpUDP;

void enviaMsgUDP(String msg) {

  char resposta[msg.length()];
  msg.toCharArray(resposta, msg.length());
  ntpUDP.beginPacket("192.168.2.255", 8766);
  ntpUDP.write(resposta);
  ntpUDP.endPacket();

}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  digitalWrite(LED,LOW);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

//ArduinoOTA-----------
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //------------------------------

  server.begin();
pinMode(LED,OUTPUT);

pinMode(saida1, OUTPUT);//

enviaMsgUDP("{\"srv\":\"Iot\",\"status\":\"up\", \"ip\":\"" +String((char*) WiFi.localIP().toString().c_str() )+ "\"} " );

digitalWrite(LED,LOW);

}



void loop() {
  ArduinoOTA.handle();


  yield() ;



   delay(1000);



    WiFiClient client = server.available();
    if (!client) {
      return;
    }

    Serial.println("Nova conexao OK...");

    //Le a string cliente
    String req = client.readStringUntil('\r');
    String tmp;

    String tmp2;
    //Mostra a string
    Serial.println(req);
    //Limpa dados/buffer
    client.flush();

    //Trata a string do cliente
    if (req.indexOf("saida1_on") != -1) {
      digitalWrite(saida1, HIGH);
      ousaida1 = 1;


    } else if (req.indexOf("saida1_off") != -1) {
      digitalWrite(saida1, LOW);
      ousaida1 = 0;



          }
    else if (req.indexOf("ajuda") != -1) {
      String buf = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
      buf += "/saida1_on ativa a saida1<br>/saida1_off desliga saida1<br>";


      //Envia a resposta para o cliente
      client.print(buf);
      client.flush();

      return;




    }
    ///favicon.ico
    else if (req.indexOf("favicon.ico") != -1) {


    } else if (req.indexOf("/") != -1) {


    }

    else {
      Serial.println("Requisicao invalida");
      Serial.println(req);
    }

    //Prepara a resposta para o cliente
    String buf ;

    buf += "HTTP/1.1 200 OK\r\n";

    buf += "Server: ServidorIoT\r\n";

    buf += "Access-Control-Allow-Origin: *\r\n";
    buf += "Location: .  \r\n";
    buf += "Content-Type: application/json;charset=UTF-8\r\n\r\n";
    buf += "[ { \"saida1\": " + String(ousaida1) + "," ;
    buf += "\"status\":\"ok\" }  ]";



    //Envia a resposta para o cliente
    client.print(buf);
    client.flush();




}
