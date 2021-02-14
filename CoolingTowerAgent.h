#pragma once
#include "BaseAgent.h"


class CoolingTowerAgent : public BaseAgent {
   private:
    double tempAirWb;    //湿球温度
    double tempWaterIn;  //冷却水回水温度
    double tempWaterCt;  //上次冷却塔优化结果

    boolean isOn;  //开关信号

    double a1, a2, a3, a4;  //冷却塔模型参数

   public:
    //懒得分了，就写一个吧
    CoolingTowerAgent(String bdId, String bdType) : BaseAgent(bdId, bdType) {}

    void setEnvironmentPara(double tWb) { this->tempAirWb = tWb; }

    void setCtPara(double tCt, double tIn, boolean on) {
        this->tempWaterCt = tCt;
        this->tempWaterIn = tIn;
        this->isOn = on;
    }

    void setCtModelPara() {
        this->a1 = 4.6254;
        this->a2 = 1.0281;
        this->a3 = -0.0902;
        this->a4 = 1.0925;  // a1~a4:冷却塔模型参数
    }

    double compTempTool() {
        return CT_agent(this->tempWaterIn,this->getCurrentBuffer().getLambda(),
        this->tempAirWb,this->isOn,this->tempWaterCt);
    }

    double CT_agent(double Tcwin, double lambda, double Twb, bool ONOFF,double Tcon_last) {
        // Tcwin-冷却水回水温度-测量，
        // lambda-lambuda-来自coordinator，Twb-湿球温度-测量，ONOFF-开关信号-测量，Tcon_last-上次优化结果，存在板子里
        
        //参数在函数外制定
        // double Tcwo_out;double a1 = 4.6254 ;double a2 =1.0281 ; double a3 =-0.0902 ; double a4 =1.0925 ;//a1~a4:冷却塔模型参数
       
        double Tcwo_out;            
        double Tcon_ub = Tcon_last + 2;  //搜索范围上界
        double Tcon_lb = Tcon_last - 2;  //搜索范围下界
        if (ONOFF) {
            double CT_cost = 100000;
            double Ma;
            double CT_costi;
            double Tcws0;
            for (Tcws0 = Tcon_lb; Tcws0 <= Tcon_ub;) {
                Ma = pow((4.2 * 205.3 * (Tcwin - Tcws0) /
                          (pow((Tcwin - Twb), a4) * a1 * pow(205.3, a3))),
                         (1 / a2));  // 所需空气流量
                CT_costi = (1.219 - 17.297 * (Ma / 190.55) +
                            168.26 * pow((Ma / 190.55), 2)) -
                           lambda * 4.2 * 205.3 * (Tcwin - Tcws0);
                if (CT_costi < CT_cost) {
                    CT_cost = CT_costi;
                    Tcwo_out = Tcws0;
                }
                Tcws0 = Tcws0 + 0.1;
            }
        } else {
            Tcwo_out = 0;
        }
        return Tcwo_out;
    }
};