#include <stdio.h>
#include <math.h>
#include <stdbool.h>
double Conv(double Tcon[3], double Tcwo[6]) // 收敛判断, Tcon[3]-来自chiller agent的结果，Tcwo[6]来自cooling tower agent 的结果
{
    double Tcon_ave;double Tcwo_ave;int Conv; int N_chi=0;
    for (int i = 0;i < 3;i++) 
    {
        if (Tcon[i] != 0)
        {
            N_chi++;
        }
    }
    Tcon_ave = (Tcon[0] + Tcon[1] + Tcon[2]) / N_chi;// 计算来自chiiler agent的平均值

//?????
    Tcwo_ave = (Tcwo[0] + Tcwo[1] + Tcwo[2] + Tcwo[3] + Tcwo[4] + Tcwo[5])/2 / N_chi;//计算来自cooling tower agent的平均值
    if (fabs(Tcon_ave-Tcwo_ave) < 0.05)
    {
        Conv = 1;
    }
    else
    {
        Conv = 0;
    }
    return Conv;
}
double Lambda(double Tcon[3], double Tcwo[6], double lambda_last) // 更新lambda的值 lambda_last-上次收敛时的lambda的值，存储在缓存中
{
    double Tcon_ave;double Tcwo_ave; double lambda; int N_chi = 0;
    for (int i = 0;i < 3;i++)
    {
        if (Tcon[i] != 0)
        {
            N_chi++;
        }
    }
    Tcon_ave = (Tcon[0] + Tcon[1] + Tcon[2]) / N_chi;
    Tcwo_ave = (Tcwo[0] + Tcwo[1] + Tcwo[2] + Tcwo[3] + Tcwo[4] + Tcwo[5] ) / 2 / N_chi;
    if (fabs(Tcon_ave - Tcwo_ave) < 0.05)
    {
        lambda = lambda_last;
    }
    else
    {
        lambda = lambda_last + 0.00000005 * 4.2 * 410.6 * N_chi * (Tcwo_ave - Tcon_ave);
    }
    return lambda;
}
// 用于测试
int main()
{
    double  Tcon[3] = { 32.1,32.1,0 }; double Tcwo[6] = {32.0,32.1,32.2,32.1,0,0};double w_last = 0.0123;
    int Con=Conv(Tcon, Tcwo);
    double lamb = Lambda(Tcon,Tcwo, w_last);
    printf("result is %d\n", Con);
    return 0;
}