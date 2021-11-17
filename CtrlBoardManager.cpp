#pragma once
#include "CtrlBoardManager.h"
#include <vector>

 const char* CtrlBoardManager::MAPPING_OPEARTION="MAP";
 const char* CtrlBoardManager::CTRL_OPEARTION = "CTRL";

 const char* CtrlBoardManager::MAPPER_NAME = "mpn";

 CtrlBoardManager::CtrlBoardManager(){}

void CtrlBoardManager::addMapper(Mapper mp) { this->mapperContainer.push_back(mp); }

    Mapper* CtrlBoardManager::findMapperByName(String str) {
        for (int i = 0; i < mapperContainer.size(); i++) {
            if (mapperContainer[i].getMapperName() == str)
                return &(mapperContainer[i]);
        }
        return nullptr;
    }

    void CtrlBoardManager::commandDistributor(String str) {
        // 检测收到命令的类型，例如CMD+***，即检测前面CMD，并分配至相应的处理方法
        // 例如{cmd:"MAP",mpn:"Flowrate",dt:{k:2,b:1}}

        //JSON解析
        DynamicJsonDocument jsonBuffer(AgentProtocol::MSG_SIZE);
        DeserializationError t = deserializeJson(jsonBuffer, str);
        if (t) {
            this->debugPrint("in parse: get error");
            Serial.println(t.c_str());
            return ;
        }
        //检测指令类型
        String cmdType=jsonBuffer[AgentProtocol::CMD_TYPE_FROM_JSON].as<String>();
        //根据指令类型分配
        if(cmdType==CtrlBoardManager::MAPPING_OPEARTION){
            //读取到为映射器修改指令
            Mapper* changedMapper = findMapperByName(jsonBuffer[CtrlBoardManager::MAPPER_NAME].as<String>());
            if(changedMapper==NULL||changedMapper==nullptr){
                debugPrint("Mapper not found according to: "+jsonBuffer[CtrlBoardManager::MAPPER_NAME].as<String>());
                return;
            }
            //思路还是根据dt字段中JSON字符串传给Mapper自行处理
            boolean isSuccess=changedMapper->setParameter(jsonBuffer[AgentProtocol::DATA_FROM_JSON].as<String>());
            Serial.println("Change parameter " + String(isSuccess ? "ok" : "NOT OK") );
            changedMapper->showParameter();
        } else if (cmdType == CtrlBoardManager::CTRL_OPEARTION) {
        } else {
            defaultCommandDistributor(str);
        }
    }

    void CtrlBoardManager::defaultCommandDistributor(String str){
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

    void CtrlBoardManager::debugPrint(String str){
        Serial.println(str);
    }

    double CtrlBoardManager::mappingValue(double originalValue, String mapperName){
        Mapper* mp = findMapperByName(mapperName);
        if(mp==NULL||mp==nullptr)
            return -999;
        return mp->mapping(originalValue);
    }