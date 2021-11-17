#pragma once
#include <ArduinoJson.h>
#include "Mapper.h"
#include "AgentProtocol.h"
#include <vector>

// 维护板子运行的类
// 映射器维护，PID维护，命令分配等

// {"cmd":"MAP","mpn":"2nd","dt":"{\"k\":2,\"b\":3}"}

class CtrlBoardManager {
   private:
    std::vector<Mapper> mapperContainer;
    StaticJsonDocument<512> jsonOut;  

   public:
    static const char* MAPPING_OPEARTION;
    static const char* CTRL_OPEARTION;
    static const int CMD_SIZE=512;
    static const char* MAPPER_NAME;

    CtrlBoardManager();

    void addMapper(Mapper mp);

    Mapper* findMapperByName(String str);

    void commandDistributor(String str);
    virtual void defaultCommandDistributor(String str);//用于处理未来需要匹配的命令

    // std::vector<String> split(String str, String pattern);
    
    virtual void debugPrint(String str);
    double mappingValue(double originalValue, String mapperName);
};
