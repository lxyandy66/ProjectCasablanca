#pragma once
// #include <uClibc++.h>
#include<Arduino.h>
#include <string.h>
#include <vector>
#include"CtrlComponent.h"
#include"DevBoardESP8266.h"
#include"AgentProtocol.h"
#include"CoordinatorBuffer.h"


class BaseAgent : public CtrlComponent {
private:
	DevBoardESP8266 wifiModule;
	long reqId;
	long respId;
	CoordinatorBuffer coBuffer;
	Stream* sendOutput;
public:
	BaseAgent(String bdId, String bdType);
	// void parseMsg(String msg);//处理交互的数据
	double compData();

	void setSendOutput(Stream* s);
	void setWifiModule(DevBoardESP8266 wifi);
	void sendMessage(String msg);
	void debugPrint(String str);

	String packDataStr();//打包data的内容到json对象 
	String packAgentMsgStr();
	
	void addToBuffer(CoordinatorBuffer cb);//主要入口，更新buffer，判断是否需要解析
	CoordinatorBuffer getCurrentBuffer();

	//预留计算温度的虚函数给不同子类实现
	virtual double compTemp();//计算Agent的内容

	
	void parseBuffer(CoordinatorBuffer cb);//对已解析的buffer进行分析并执行agent的计算
	/*Agent线程部分*/
	int threadAgent(struct pt* pt, String msg) ;
};