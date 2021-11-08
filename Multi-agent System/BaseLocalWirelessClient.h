#pragma once
//本地客户端的基类，即NCS中的客户端，直接与设备交互
#include "DevBoardESP8266.h"
#include "BaseNetworkCtrlComponent.h"


class BaseLocalWirelessClient : public BaseCtrlComponent {
protected:
	DevBoardESP8266 wifiModule;
	Stream* sendOutput;
public:
	BaseLocalWirelessClient(String bdId, String bdType);
	void setSendOutput(Stream* s);//*
	void setWifiModule(DevBoardESP8266 wifi);//*
};