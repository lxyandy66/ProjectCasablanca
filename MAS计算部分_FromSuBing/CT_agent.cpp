#include <stdio.h>
#include <math.h>
double CT_agent( double Tcwin, double lambda, double Twb, bool ONOFF, double Tcon_last) // Tcwin-��ȴˮ��ˮ�¶�-������ lambda-lambuda-����coordinator��Twb-ʪ���¶�-������ONOFF-�����ź�-������Tcon_last-�ϴ��Ż���������ڰ�����
{
    double Tcwo_out;double a1 = 4.6254 ;double a2 =1.0281 ; double a3 =-0.0902 ; double a4 =1.0925 ;//a1~a4:��ȴ��ģ�Ͳ���
    double Tcon_ub = Tcon_last + 2; //������Χ�Ͻ�
    double Tcon_lb = Tcon_last - 2; //������Χ�½�
    if (ONOFF)
    {
        double CT_cost = 100000; double Ma; double CT_costi;double Tcws0;
        for (Tcws0 = Tcon_lb;Tcws0 <= Tcon_ub; )
        {
            Ma = pow((4.2 * 205.3 * (Tcwin - Tcws0) / (pow((Tcwin - Twb),a4) * a1 * pow(205.3, a3))),(1 / a2)); // �����������
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
// ����������ڲ��Եĺ���
int main()
{
    double  Tcwin = 33.2; double lambda = 0.0123;double Twb = 26.1; int ONOFF = 1;double Tcon_last = 22.5;
    double Tcwo = CT_agent( Tcwin, lambda, Twb, ONOFF, Tcon_last);
    printf("result is %f\n", Tcwo);
    return 0;
}