#include <stdio.h>
#include <math.h>
double CT_agent( double Tcwin, double lambda, double Twb, bool ONOFF, double Tcon_last) // Tcwin-冷却水回水温度-测量， lambda-lambuda-来自coordinator，Twb-湿球温度-测量，ONOFF-开关信号-测量，Tcon_last-上次优化结果，存在板子里
{
    double Tcwo_out;double a1 = 4.6254 ;double a2 =1.0281 ; double a3 =-0.0902 ; double a4 =1.0925 ;//a1~a4:冷却塔模型参数
    double Tcon_ub = Tcon_last + 2; //搜索范围上界
    double Tcon_lb = Tcon_last - 2; //搜索范围下界
    if (ONOFF)
    {
        double CT_cost = 100000; double Ma; double CT_costi;double Tcws0;
        for (Tcws0 = Tcon_lb;Tcws0 <= Tcon_ub; )
        {
            Ma = pow((4.2 * 205.3 * (Tcwin - Tcws0) / (pow((Tcwin - Twb),a4) * a1 * pow(205.3, a3))),(1 / a2)); // 所需空气流量
            CT_costi = (1.219 - 17.297 * (Ma / 190.55) + 168.26 * pow((Ma / 190.55), 2)) - lambda * 4.2 * 205.3 * (Tcwin - Tcws0);
            if (CT_costi < CT_cost)
            {
                CT_cost = CT_costi;
                Tcwo_out = Tcws0;
            }
            Tcws0 = Tcws0 + 0.1;
        }
    }
    else
    {
        Tcwo_out = 0;
    }
    return Tcwo_out;
}
// 下面得是用于测试的函数
int main()
{
    double  Tcwin = 33.2; double lambda = 0.0123;double Twb = 26.1; int ONOFF = 1;double Tcon_last = 22.5;
    double Tcwo = CT_agent( Tcwin, lambda, Twb, ONOFF, Tcon_last);
    printf("result is %f\n", Tcwo);
    return 0;
}