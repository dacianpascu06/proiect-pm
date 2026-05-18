#include "HardwareSerial.h"
#include "lcd.h"
#include "twi.h"
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <util/delay.h>

#define RST_PIN 9
#define SS_PIN 10

#define ESP_RX_PIN 2
#define ESP_TX_PIN 3

const char *ssid = "daci";
const char *password = "12341234";
const char *serverIp = "74.161.152.120";

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial esp8266(ESP_RX_PIN, ESP_TX_PIN);

// helper function to  send
String sendCommand(String command, const int timeout) {
  String response = "";
  esp8266.print(command);
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }
  }

  return response;
}
String sendUIDToServer(String uid) {
  // open tcp to nginx on vm that runs on 80
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += serverIp;
  cmd += "\",80\r\n";
  sendCommand(cmd, 2000);

  // construct the request
  String httpRequest = "GET /pm?uid=" + uid + " HTTP/1.1\r\n";
  httpRequest += "Host: " + String(serverIp) + "\r\n";
  httpRequest += "Connection: close\r\n\r\n";

  // send length
  cmd = "AT+CIPSEND=";
  cmd += httpRequest.length();
  cmd += "\r\n";
  sendCommand(cmd, 1000);

  String httpResponse = sendCommand(httpRequest, 3000);

  int startIndex = httpResponse.indexOf("verdict") + 8;
  String verdict = httpResponse.substring(startIndex, startIndex + 1);

  // close tcp conn
  sendCommand("AT+CIPCLOSE\r\n", 1000);

  return verdict;
}

bool connectToWifi() {
  // connect to wifi
  String cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += password;
  cmd += "\"\r\n";

  String wifiResponse = sendCommand(cmd, 5000);
  Serial.println(wifiResponse.c_str());
  if (wifiResponse.indexOf("DISCONNECT") != -1) {
    return false;
  }
  if (wifiResponse.indexOf("CONNECT") != -1) {
    return true;
  }

  Serial.println("connect to wifi unknown error");
  return false;
}

void setup() {
  // uart
  Serial.begin(9600);

  // software serial for d2/d3 uart
  esp8266.begin(9600);

  SPI.begin();

  // init
  twi_init();
  lcd_init();
  mfrc522.PCD_Init();
  delay(4);

  // show beginning lcd message
  lcd_set_cursor(0, 0);
  lcd_print("Waiting for WIFI.");
  Serial.println("Waiting for Wifi");

  // reset the esp
  sendCommand("AT+RST\r\n", 2000);
  // set to client mode
  sendCommand("AT+CWMODE=1\r\n", 1000);

  while (!connectToWifi()) {
    // retry wifi
    Serial.println(F("failed to connect to wifi"));

    lcd_set_cursor(0, 0);
    lcd_print("WIFI FAILED     ");
    lcd_set_cursor(0, 1);
    lcd_print("Retrying!       ");
    delay(30000);
  }
  lcd_set_cursor(0, 0);
  lcd_print("                ");
  lcd_set_cursor(0, 1);
  lcd_print("                ");
  lcd_set_cursor(0, 0);

  Serial.println(F("Ready to scan..."));
  lcd_set_cursor(0, 0);
  lcd_print("Ready to scan...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  lcd_set_cursor(0, 1);
  lcd_print("                ");
  lcd_set_cursor(0, 1);

  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    char hexId[4];
    sprintf(hexId, "%02X ", mfrc522.uid.uidByte[i]);
    lcd_print(hexId);

    // logic to build a clean string
    uidString += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(uidString);

  // send the uid
  String response = sendUIDToServer(uidString);

  Serial.print("verdict=" + response + "\n");

  // // reset screen
  lcd_set_cursor(0, 1);
  lcd_print("                ");
  lcd_set_cursor(0, 1);

  if (response == "0") {
    lcd_print("DENIED ENTRY!");
  } else {
    lcd_print("WELCOME!   ");
  }

  delay(4000);
}
