#pragma once
#include "CtrlBoardManager.h"
#include <vector>

const char* CtrlBoardManager::MAPPING_OPEARTION = "MAP";
const char* CtrlBoardManager::CTRL_SETPOINT = "CT_SP";
const char* CtrlBoardManager::CTRL_TUNING = "CT_PARA";

const char* CtrlBoardManager::COMP_ID = "id";

const char* CtrlBoardManager::CTRL_SETPOINT_DATA = "sp";

CtrlBoardManager::CtrlBoardManager() {}

void CtrlBoardManager::addMapper(Mapper* mp) {
    this->mapperContainer.push_back(mp);
}

void CtrlBoardManager::addController(PackedPID* controller){
    this->controllerContainer.push_back(controller);
}

Mapper* CtrlBoardManager::findMapperById(String str) {
    for (int i = 0; i < mapperContainer.size(); i++) {
        if (mapperContainer[i]->getMapperId() == str)
            return mapperContainer[i];
    }
    return nullptr;
}

PackedPID* CtrlBoardManager::findControllerById(String str) {
    for (int i = 0; i < controllerContainer.size(); i++) {
        if (controllerContainer[i]->getControllerId() == str)
            return controllerContainer[i];
    }
    return nullptr;
}

void CtrlBoardManager::commandDistributor(String str) {
    // 检测收到命令的类型，例如CMD+***，即检测前面CMD，并分配至相应的处理方法
    // 例如{cmd:"MAP",mpn:"Flowrate",dt:{k:2,b:1}}

    // JSON解析
    DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);
    DeserializationError t = deserializeJson(jsonBuffer, str);
    if (t) {
        this->debugPrint("in parse: get error");
        Serial.println(t.c_str());
        return;
    }
    //检测指令类型
    String cmdType = jsonBuffer[AgentProtocol::CMD_TYPE_FROM_JSON].as<String>();
    //根据指令类型分配
    if (cmdType == CtrlBoardManager::MAPPING_OPEARTION) {
        //读取到为映射器修改指令
        // 例如{cmd:"MAP",mpn:"Flowrate",dt:{k:2,b:1}}
        // showStatus();
        Mapper* changedMapper = findMapperById(jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if (changedMapper == NULL || changedMapper == nullptr) {
            debugPrint("Mapper not found according to: " + jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return;
        }
        //思路还是根据dt字段中JSON字符串传给Mapper自行处理
        boolean isSuccess = changedMapper->setParameter(
            jsonBuffer[AgentProtocol::DATA_FROM_JSON].as<String>());
        // Serial.println("Change parameter " +String(isSuccess ? "ok" : "NOT OK"));
        // changedMapper->showParameter();
    } else if (cmdType == CtrlBoardManager::CTRL_SETPOINT) {
        // 例如{cmd:"CT_SP",id:"C_FR",sp:2.0}
        PackedPID* changedController=findControllerById(jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
        if(changedController == NULL || changedController == nullptr){
            debugPrint("Controller not found according to: " + jsonBuffer[CtrlBoardManager::COMP_ID].as<String>());
            return;
        }
        changedController->setSetpoint(jsonBuffer[CtrlBoardManager::CTRL_SETPOINT_DATA].as<double>());
    } else {
        defaultCommandDistributor(jsonBuffer, cmdType);
    }
    // showStatus();
}

void CtrlBoardManager::defaultCommandDistributor(DynamicJsonDocument jsonStr,String cmdType) {
    debugPrint("Null");
}

// std::vector<String> CtrlBoardManager::split(String str, String pattern) {
//     String::size_type pos;
//     std::vector<String> result;
//     str += pattern;  //扩展字符串以方便操作
//     int size = str.size();

//     for (int i = 0; i < size; i++) {
//         pos = str.find(pattern, i);
//         if (pos < size) {
//             String s = str.substr(i, pos - i);
//             result.push_back(s);
//             i = pos + pattern.size() - 1;
//         }
//     }
//     return result;
// }

void CtrlBoardManager::debugPrint(String str) {
    Serial.println(str);
}

double CtrlBoardManager::mappingValue(double originalValue, String mapperId) {
    Mapper* mp = findMapperById(mapperId);
    if (mp == NULL || mp == nullptr)
        return -999;
    return mp->mapping(originalValue);
}

void CtrlBoardManager::showStatus(){
    for (int i = 0; i < mapperContainer.size();i++){
        Serial.println("Mapper id: " + mapperContainer[i]->getMapperId() );
        mapperContainer[i]->showParameter();
    }
}