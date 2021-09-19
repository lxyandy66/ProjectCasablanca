#include <stdio.h>
double Chi_agent(double Tchws,double Tchwr, double Tcwin, double lambda, bool ONOFF, double Tcon_last) 
{  // Tchws-冷冻水出水温度-测量,Tchwr-冷冻水回水温度-测量，Tcwin-冷却水回水温度-测量， lambda-lambuda-来自coordinator，Twb-湿球温度-测量，Tcon_last-上次优化结果-存在板子里，ONOFF-开关信号-测量
    double Tcon_in;
    if (ONOFF)
    {
        double Chiller_cost = 100000; double Tcws0; double PLR; double Chiller_costi; double Load;double Tcon_lb;double Tcon_ub;
        Load = 4.2 * 410.6 * (Tchwr - Tchws);
        PLR = Load / 7235;
        Tcon_lb = Tcon_last - 2;
        Tcon_ub = Tcon_last + 2;
        for (Tcws0 = Tcon_lb;Tcws0 <= Tcon_ub; )
        {
            Chiller_costi = 7235 * 5.566 * (0.0111 + 0.0204 * PLR + 0.0003 * PLR * PLR) * (0.5489 - 0.0152 * Tchws + 0.0007896 * Tchws * Tchws + 0.01405 * Tcws0 + 0.0003618 * Tcws0 * Tcws0 - 0.0003554 * Tchws * Tcws0) + lambda * 4.2 * 410.6 * (Tcwin - Tcws0);
            if (Chiller_costi < Chiller_cost)
            {
                Chiller_cost = Chiller_costi;
                Tcon_in = Tcws0;
            }
            Tcws0 = Tcws0 + 0.1;
        }
    }
    else
    {
        Tcon_in = 0;
    }
    return Tcon_in;
}
int main()
{
    double Tchws = 5.5;double Tchwr = 10.5; double Tcwin = 33.2; double w = 0.0123;int ONOFF = 1;double Tcon_last = 22.5;
    double Tcon = Chi_agent(Tchws, Tchwr,Tcwin, w, ONOFF, Tcon_last);
    printf("result is %f\n", Tcon);
    return 0;
}