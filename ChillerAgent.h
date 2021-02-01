#pragma once
#include"BaseAgent.h"

class ChillerAgent : public BaseAgent {
public:
	//懒得分了，就写一个吧
	ChillerAgent(String bdId, String bdType) :BaseAgent(bdId, bdType) {}
	double compTemp() {
		return -777;
	}
};