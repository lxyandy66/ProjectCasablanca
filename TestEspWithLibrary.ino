
/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>
#include "DevBoardESP8266.h"
#include "Agent.h"

 // #define ESP_SSID  "I am AB, How R U?"//"TP-LINK_hvac" "BlackBerry Hotspot"
 // #define ESP_PASS  "woyeshiab"         // Your network password here "141242343"

 // #define TCP_SERVER_ADDR "192.168.1.104" //TCP服务器地址
 // #define TCP_SERVER_PORT 8266            //TCP服务器地址

#define ESP_SSID  "superb"//"TP-LINK_hvac" "BlackBerry Hotspot"
#define ESP_PASS  "bugaosuni"         // Your network password here "141242343"
 // #define ESP_SSID  "BlackBerry Hotspot"//"TP-LINK_hvac" 
// #define ESP_PASS  "141242343"         // Your network password here "141242343"

#define TCP_SERVER_ADDR "192.168.3.7" //TCP服务器地址
#define TCP_SERVER_PORT 5230            //TCP服务器地址

#define PIN_LED D2

SoftwareSerial mySerial(10, 11); // RX, TX10, 11
DevBoardESP8266 wifi(&mySerial, &Serial, D3);
const String deviceID = "Agent1";
const String deviceType = AgentProtocol::TYPE_COOLING_TOWER;


String tempBuffer;

Agent agent(deviceID, deviceType);

void setup()
{
  char buffer[50];

  // Open serial communications and wait for port to open:
  //USB串口测试
  Serial.begin(115200);
  while (!Serial)
  {
    Serial.println("Goodnight moon!");
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Debug Serial complete!");

  // agent.setWifiModule(wifi);

  // set the data rate for the SoftwareSerial port
  // mySerial.begin(115200);
  // while (!mySerial)
  // {
  //   ;
  // }
  // Serial.println("SS initialized!");
  // wifi.hardReset();

  // //Wi-Fi连接测试
  // Serial.println(F("Connecting to WiFi..."));
  // boolean flag = wifi.connectToAP(F(ESP_SSID), F(ESP_PASS));

  // if (flag)
  // {
  //   Serial.println("Connecting Success");
  // }
  // else
  // {
  //   Serial.println("Connecting Failed");
  // }

  // wifi.connectTCP(F(TCP_SERVER_ADDR), TCP_SERVER_PORT);

  // Serial.println(F("Setup finished"));
  agent.setSendOutput(&Serial);
  agent.setLedPin(PIN_LED);
}

void loop()
{ // run over and over
  // if (mySerial.available())//暂时先不考虑wifi模块的软串口
  while (Serial.available()) {
    String msg = Serial.readString();
    Serial.println("this is a msg: [" + msg + "]");
    // Serial.write(mySerial.read().toCharArray());

    //加入buffer之后自动解析
    agent.addToBuffer(CoordinatorBuffer::msgToCoordinatorBuffer(AgentProtocol::parseFromString(msg), agent.getInputBuffer()));
    // processCmd(a);
    return;
  }
  // if (Serial.available())
  // {
  //   mySerial.write(Serial.read());
  // }


}

void getIpAddress()
{
  // IP addr check isn't part of library yet, but
  // we can manually request and place in a string.
  char buffer[50];
  wifi.println(F("AT+CIFSR"));
  if (wifi.readLine(buffer, sizeof(buffer)))
  {
    Serial.println(buffer);
    wifi.find(); // Discard the 'OK' that follows
  }
  else
  { // IP addr check failed
    Serial.println(F("error"));
  }
}




// wifi.println(F("AT+GMR"));
// if (wifi.readLine(buffer, sizeof(buffer))) {
//   Serial.println(buffer);
//   wifi.find(); // Discard the 'OK' that follows
//   Serial.println(F("finished firmware check"));
// }
// else {
//   Serial.println(F("error"));
// }
