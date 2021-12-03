#pragma once
#include <ArduinoJson.h>
#include "Mapper.h"
#include "AgentProtocol.h"
#include <vector>
#include <PID_v1.h>
#include "PackedPID.h"

// 维护板子运行的类
// 映射器维护，PID维护，命令分配等

// {"cmd":"MAP","mpn":"2nd","dt":"{\"k\":2,\"b\":3}"}

class CtrlBoardManager {
   protected:
    std::vector<Mapper*> mapperContainer;
    std::vector<PackedPID*>controllerContainer;
    StaticJsonDocument<512> jsonOut;


   public:
    static const char* MAPPING_OPEARTION;
    static const char* CTRL_SETPOINT;
    static const char* CTRL_TUNING;
    static const int CMD_SIZE=512;
    static const char* COMP_ID;

    static const char* CTRL_SETPOINT_DATA;

    static const char* MGR_STATUS;



    CtrlBoardManager();

    void addMapper(Mapper* mp);
    void addController(PackedPID* controller);

    Mapper* findMapperById(String str);
    PackedPID* findControllerById(String str);

    void commandDistributor(String str);
    virtual void defaultCommandDistributor(DynamicJsonDocument jsonStr,String cmdType);//用于处理未来需要匹配的命令

    // std::vector<String> split(String str, String pattern);
    
    virtual void debugPrint(String str);
    double mappingValue(double originalValue, String mapperId);

    double getSetpointById(String controllerId);

    void showStatus();
};
