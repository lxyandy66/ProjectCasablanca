#pragma once
#include"BaseAgent.h"

class CoolingTowerAgent : public BaseAgent {
public:
	//懒得分了，就写一个吧
	CoolingTowerAgent(String bdId, String bdType) :BaseAgent(bdId, bdType) {}
	double compTemp() {
		return -666;
	}
};