
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
#define TCP_SERVER_ADDR "192.168.3.7" //TCP服务器地址
#define TCP_SERVER_PORT 8266            //TCP服务器地址

// #define ESP_SSID  "BlackBerry Hotspot"//"TP-LINK_hvac" 
// #define ESP_PASS  "141242343"         // Your network password here "141242343"

// #define ESP_SSID  "HMT-Freshmen"//"TP-LINK_hvac" 
// #define ESP_PASS  "stars15hmt"         // Your network password here "141242343"

// #define TCP_SERVER_ADDR "172.16.33.234" //TCP服务器地址
// #define TCP_SERVER_PORT 5230            //TCP服务器地址

#define PIN_LED D2

SoftwareSerial mySerial(10, 11); // RX, TX10, 11
DevBoardESP8266 wifi(&mySerial, &Serial, D3);
const String deviceID = "Agent1";
const String deviceType = AgentProtocol::TYPE_COOLING_TOWER;

AgentMsg msgBuffer;
String tempBuffer;

Agent agent(deviceID, deviceType);

void setup()
{
  char buffer[50];

  // Open serial communications and wait for port to open:
  //USB串口测试
  Serial.begin(9600);
  while (!Serial)
  {
    Serial.println("Goodnight moon!");
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Debug Serial complete!");

  agent.setWifiModule(wifi);

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  while (!mySerial)
  {
    ;
  }
  Serial.println("SS initialized!");
  wifi.hardReset();

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

  Serial.println(F("Setup finished"));
  agent.setSendOutput(&mySerial);
  agent.setLedPin(PIN_LED);
}

String sendData;
String dataRecvFromSerial;
void loop()
{ // run over and over
  // if (mySerial.available())//暂时先不考虑wifi模块的软串口
  if (mySerial.available() > 0) {

    agent.debugPrint("mySerial length: " + mySerial.available());
    String msg = preprocessMsgFromWifiModule(mySerial.readStringUntil('\n'));
    msg.trim();
    agent.debugPrint("this is a msg: [" + msg + "] length: " + msg.length());
    // Serial.write(mySerial.read().toCharArray());

    //加入buffer之后自动解析
    msgBuffer = AgentProtocol::parseFromString(msg);
    if (!AgentProtocol::isVaildMsg(msgBuffer)) {
      agent.debugPrint("Invaild Msg! " + msg);
      return;
    }

    agent.addToBuffer(CoordinatorBuffer::msgToCoordinatorBuffer(msgBuffer, agent.getInputBuffer()));
    agent.debugPrint(agent.getCurrentBuffer().whoAmI());
    //发送的逻辑要考虑考虑
    sendData = agent.packAgentData();
    agent.debugPrint("Data will be sent: " + sendData);
    wifi.sendContent(sendData + "\r\n");// processCmd(a);
    return;
  }
  if (Serial.available())
  {
    dataRecvFromSerial = Serial.readString();
    agent.debugPrint("From serial: " + dataRecvFromSerial);
    mySerial.write(dataRecvFromSerial.c_str());
  }


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
