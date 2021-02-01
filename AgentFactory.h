#pragma once
#include"CoolingTowerAgent.h"
#include"ChillerAgent.h"

enum class AgentType { COOLING_TOWER, CHILLER, PUMP };

class AgentFactory {

public:
	static BaseAgent* createAgent(enum AgentType type, String bdId) {
		switch (type) {
		case AgentType::COOLING_TOWER:
			return new CoolingTowerAgent(bdId, AgentProtocol::TYPE_COOLING_TOWER);
			break;

		case AgentType::CHILLER:
			return new ChillerAgent(bdId, AgentProtocol::TYPE_CHILLER);

		default:
			return nullptr;
			break;
		}
		return nullptr;
	};

};

