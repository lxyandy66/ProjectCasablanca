#pragma onece
#include "BaseLocalWirelessClient.h"


BaseLocalWirelessClient::BaseLocalWirelessClient(String bdId, String bdType) :BaseNetworkCtrlComponent(bdId, bdType) {}

void BaseLocalWirelessClient::setWifiModule(DevBoardESP8266 wifi) {
	this->wifiModule = wifi;
}

void BaseLocalWirelessClient::setSendOutput(Stream* s) {
	this->sendOutput = s;
}