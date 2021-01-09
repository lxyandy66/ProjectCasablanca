#include <Arduino.h>
#include <DHT.h>
#include <stdlib.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <Adafruit_ESP8266.h>
#include <ArduinoJson.h>

class DevBoard {
public:
	DevBoard();
	DevBoard(Serial ds, DHT th,
		U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2,
		SoftwareSerial ss);
	void setThSensor(DHT th);
	void setThSensor(int dhtData, String dhtType);
	void setSoftwareSerial(SoftwareSerial s);
	void setScreen(U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI sc);
	void setWifiModule(Adafruit_ESP8266 wifi);
	U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI getScreen();
	SoftwareSerial getSoftwareSerial();
	DHT getThSensor();
	Adafruit_ESP8266 getWifiModule();


	boolean processCmd(String cmd);


private:
	Serial debugSerial;
	// U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI screen;
	// SoftwareSerial ss;
	// DHT thSensor;
	// Adafruit_ESP8266 wifiInterface;
};