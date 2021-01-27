
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
#include<pt.h>

 // #define ESP_SSID  "I am AB, How R U?"//"TP-LINK_hvac" "BlackBerry Hotspot"
 // #define ESP_PASS  "woyeshiab"         // Your network password here "141242343"

 // #define TCP_SERVER_ADDR "192.168.1.104" //TCP服务器地址
 // #define TCP_SERVER_PORT 8266            //TCP服务器地址

#define ESP_SSID  "superb"//"TP-LINK_hvac" "BlackBerry Hotspot"
#define ESP_PASS  "bugaosuni"         // Your network password here "141242343"
#define TCP_SERVER_ADDR "192.168.3.7" //TCP服务器地址
#define TCP_SERVER_PORT 8266            //TCP服务器地址

// #define ESP_SSID  "BlackBerry Hotspot"//"TP-LINK_hvac" 
// #define ESP_PASS  "141242343"         // Your network password here "141242343"
// #define TCP_SERVER_ADDR "192.168.43.139" //TCP服务器地址
// #define TCP_SERVER_PORT 8266            //TCP服务器地址

// #define ESP_SSID  "HMT-Freshmen"//"TP-LINK_hvac" 
// #define ESP_PASS  "stars15hmt"         // Your network password here "141242343"
// #define TCP_SERVER_ADDR "172.16.32.186" //TCP服务器地址
// #define TCP_SERVER_PORT 8266            //TCP服务器地址

#define PIN_LED D2


DevBoardESP8266 wifi(&Serial1, &Serial, D3);
const String deviceID = "Agent1";
const String deviceType = AgentProtocol::TYPE_COOLING_TOWER;


String tempBuffer;
String sendData;
String dataRecvFromSerial;

static struct pt trAgent;//coordinate线程指针

HardwareSerial Serial1(PA10, PA9);//RX,TX
Agent agent(deviceID, deviceType);

void setup()
{
  char buffer[50];

  // Open serial communications and wait for port to open:
  //USB串口测试
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.setTimeout(500);//需要控制串口读取数据的超时设置，否则无法分开多条数据
  while (!Serial)
  {
    Serial.println("Goodnight moon!");
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Debug Serial complete!");


  // set the data rate for the SoftwareSerial port
  while (!Serial1)
  {
    ;
  }
  Serial.println("SS initialized!");
  //wifi.hardReset();

  //Wi-Fi连接测试
  Serial.println(F("Connecting to WiFi..."));
  boolean flag = wifi.connectToAP(F(ESP_SSID), F(ESP_PASS));

  if (flag)
  {
    Serial.println("Connecting Success");
  }
  else
  {
    Serial.println("Connecting Failed");
  }

  wifi.connectTCP(F(TCP_SERVER_ADDR), TCP_SERVER_PORT);

  wifi.setTransparentMode(true);

  agent.setWifiModule(wifi);
  agent.setSendOutput(&Serial1);
  agent.setLedPin(PIN_LED);
  delay(20);
  PT_INIT(&trAgent);
  Serial.println(F("Setup finished"));
}


void loop() {
  // Serial.println("Msg from TCP: " + Serial1.readString());
  // Serial1.flush();
  // Serial1.clear();


  if (Serial1.available()) {
    //线程化可能好点
    agent.threadAgent(&trAgent, Serial1.readString());
  }

  // Serial.write(Serial1.read().toCharArray());
  //加入buffer之后自动解析

}

String preprocessMsgFromWifiModule(String orgMsg) {
  //直接从wifi模块里面获取的信息带头尾, 例如"+IPD,93:{"id":"Co_1","tp":...}"
  return orgMsg.substring(orgMsg.indexOf(":") + 1);
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
