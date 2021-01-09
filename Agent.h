#pragma once
// #include <uClibc++.h>
#include<Arduino.h>
#include <string.h>
#include <vector>
#include"CtrlComponent.h"
#include"DevBoardESP8266.h"
#include"AgentProtocol.h"
#include"CoordinatorBuffer.h"


class Agent : public CtrlComponent {
private:
	DevBoardESP8266 wifiModule;
	long reqId;
	long respId;
	CoordinatorBuffer coBuffer;
	Stream* sendOutput;
public:
	Agent(String bdId, String bdType);
	// void parseMsg(String msg);//处理交互的数据
	double compData();

	void setSendOutput(Stream* s);

	void setWifiModule(DevBoardESP8266 wifi);
	void sendMessage(String msg);
	void debugPrint(String str);
	String agentCaculate();//打包data的内容到json对象 
	double compTemp();//计算Agent的内容 这个需要核实一下，/*agent有没有分类的*/
	void addToBuffer(CoordinatorBuffer cb);//主要入口，更新buffer，判断是否需要解析


	String packAgentData();
	void parseBuffer(CoordinatorBuffer cb);//对已解析的buffer进行分析并执行agent的计算

	/*多线程部分*/
	//agent活动比较简单不设置多线程
	// int threadAgent(struct pt* pt); //Agent线程,主要线程
};