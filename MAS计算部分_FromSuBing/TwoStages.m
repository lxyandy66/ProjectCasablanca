clc;clear;clear CoAgent;clear CTAgent; clear ChiAgent;clear CoAgent2;
% A=xlsread('Cooling load.xlsx','Sheet1'); % one year
% A=xlsread('Cooling load.xlsx','Sheet2'); %2392
A=xlsread('Cooling load.xlsx','Sheet3'); %4344
CAP=7235;
COP_nom=5.566;
Tchws0=5.5;
Ma_min=64.98;
Ma_max=190.5;
B=[4.6254 1.0281 -0.0902 1.0925;4.4419 1.0343 -0.1027 1.0945;4.0435 1.0628 -0.1258 1.0983;3.807 1.0831 -0.1443 1.1013;3.4807 1.1122 -0.1660 1.1050;3.0628 1.1337 -0.1817 1.1076];
% B=[5.6254 1.0281 -0.0902 1.0925;5.4419 1.0343 -0.1027 1.0945;5.0435 1.0628 -0.1258 1.0983;4.807 1.0831 -0.1443 1.1013;4.4807 1.1122 -0.1660 1.1050;4.0628 1.1337 -0.1817 1.1076];
u_last=0.0123;
h=1;
Num=1;
Tcwsf=zeros(1,6);
for M=1:2
for L=h:h+720
    clear CoAgent;%clear CoAgent2;
    ON_OFF_chi=zeros(1,3);
    ON_OFF_CT=zeros(1,6);
    Load_tot=A(L,2);
    if Load_tot>7235*0.35
        Twb=A(L,3);
        Tcwin=max(A(L,4),25);
        N_chi=min(fix(Load_tot/7240)+1,3);
        N_CT=2*N_chi;
        Load=Load_tot/N_chi;
        PLR=Load/7235;
        Tcws_min_tot=0;Tcws_max_tot=0;
        for n=1:N_CT
            Tcws_max2(n,1)=Tcwin-B(n,1)*Ma_min^B(n,2)*(205.3)^(B(n,3))*(Tcwin-Twb)^B(n,4)/(4.2*205.3);
            Twb1=max(18,Twb);
            Tcws_min(n,1)=max(Tcwin-B(n,1)*Ma_max^B(n,2)*(205.3)^(B(n,3))*(Tcwin-Twb)^B(n,4)/(4.2*205.3),Twb1);
            Tcws_min_tot=Tcws_min_tot+Tcws_min(n,1);
            Tcws_max_tot=Tcws_max_tot+Tcws_max2(n,1);
        end
        T_lb=Tcws_min_tot/N_CT;
        T_ub=Tcws_max_tot/N_CT;
        ON_OFF_chi(:,1:N_chi)=1;
        ON_OFF_CT(:,1:N_CT)=1;
                %% Communication delay of stage 1
                
        De_CoCT1=0; %delay of coordinator agent to cooling tower agent
        De_CoCT2=0; %delay of coordinator agent to cooling tower agent
        De_CoCT3=0; %delay of coordinator agent to cooling tower agent
        De_CoCT4=0; %delay of coordinator agent to cooling tower agent
        De_CoCT5=0; %delay of coordinator agent to cooling tower agent
        De_CoCT6=0; %delay of coordinator agent to cooling tower agent
        De_CoChi=0; %delay of coordinator agent to chiller agent
        De_ChiCo=0; %delay of chiller agent to coordinator agent
        De_CTCo1=0; %delay of cooling tower agent to coordinator agent
        De_CTCo2=0; %delay of cooling tower agent to coordinator agent
        De_CTCo3=0; %delay of cooling tower agent to coordinator agent
        De_CTCo4=0; %delay of cooling tower agent to coordinator agent
        De_CTCo5=0; %delay of cooling tower agent to coordinator agent
        De_CTCo6=0; %delay of cooling tower agent to coordinator agent
        %% initialization
        t=0;
        m(1,Num)=0;
        w0=CAP*COP_nom*(0.0111+0.0204*PLR+0.0003*PLR*PLR)*(0.01405+2*0.0003618*T_lb-0.0003554*Tchws0)/4.2/410.6;
        w(1,Num)=w0;
        Tcwo1(t+1,Num)=CTAgent(m(t+1,Num),w(t+1,Num),Tcwin,Twb,Load,B(1,1),B(1,2),B(1,3),B(1,4),ON_OFF_CT(1),T_lb,T_ub);
        Tcwo2(t+1,Num)=CTAgent(m(t+1,Num),w(t+1,Num),Tcwin,Twb,Load,B(2,1),B(2,2),B(2,3),B(2,4),ON_OFF_CT(2),T_lb,T_ub);
        Tcwo3(t+1,Num)=CTAgent(m(t+1,Num),w(t+1,Num),Tcwin,Twb,Load,B(3,1),B(3,2),B(3,3),B(3,4),ON_OFF_CT(3),T_lb,T_ub);
        Tcwo4(t+1,Num)=CTAgent(m(t+1,Num),w(t+1,Num),Tcwin,Twb,Load,B(4,1),B(4,2),B(4,3),B(4,4),ON_OFF_CT(4),T_lb,T_ub);
        Tcwo5(t+1,Num)=CTAgent(m(t+1,Num),w(t+1,Num),Tcwin,Twb,Load,B(5,1),B(5,2),B(5,3),B(5,4),ON_OFF_CT(5),T_lb,T_ub);
        Tcwo6(t+1,Num)=CTAgent(m(t+1,Num),w(t+1,Num),Tcwin,Twb,Load,B(6,1),B(6,2),B(6,3),B(6,4),ON_OFF_CT(6),T_lb,T_ub);
        Tcwo(t+1,Num)=(Tcwo1(t+1,Num)+Tcwo2(t+1,Num)+Tcwo3(t+1,Num)+Tcwo4(t+1,Num)+Tcwo5(t+1,Num)+Tcwo6(t+1,Num))/N_CT;
        Tcwof(t+1,Num)=(Tcwo1(t+1,Num)+Tcwo2(t+1,Num)+Tcwo3(t+1,Num)+Tcwo4(t+1,Num)+Tcwo5(t+1,Num)+Tcwo6(t+1,Num))/N_CT;
        Tcon_in(1,Num)=ChiAgent(Load,Tcwin,Tchws0,w(t+1,Num),m(t+1,Num),ON_OFF_chi(1),T_lb,T_ub);

        %% Stage 1
        t=1;
        Converge=0;
        while Converge==0 && t<600
            if t<2
                Delay=zeros(7,2); % 1 coloum: component to coordinator, 2 coloum coordinator to component, CT first
                [w(t+1,Num),m(t+1,Num)]=CoAgent(Tcwo(t,Num),Tcon_in(t,Num),N_chi,w0);
                Tcwo1(t+1,Num)=CTAgent(m(t,Num),w(t,Num),Tcwin,Twb,Load,B(1,1),B(1,2),B(1,3),B(1,4),ON_OFF_CT(1),T_lb,T_ub);
                Tcwo2(t+1,Num)=CTAgent(m(t,Num),w(t,Num),Tcwin,Twb,Load,B(2,1),B(2,2),B(2,3),B(2,4),ON_OFF_CT(2),T_lb,T_ub);
                Tcwo3(t+1,Num)=CTAgent(m(t,Num),w(t,Num),Tcwin,Twb,Load,B(3,1),B(3,2),B(3,3),B(3,4),ON_OFF_CT(3),T_lb,T_ub);
                Tcwo4(t+1,Num)=CTAgent(m(t,Num),w(t,Num),Tcwin,Twb,Load,B(4,1),B(4,2),B(4,3),B(4,4),ON_OFF_CT(4),T_lb,T_ub);
                Tcwo5(t+1,Num)=CTAgent(m(t,Num),w(t,Num),Tcwin,Twb,Load,B(5,1),B(5,2),B(5,3),B(5,4),ON_OFF_CT(5),T_lb,T_ub);
                Tcwo6(t+1,Num)=CTAgent(m(t,Num),w(t,Num),Tcwin,Twb,Load,B(6,1),B(6,2),B(6,3),B(6,4),ON_OFF_CT(6),T_lb,T_ub);
                Tcwof(t+1,Num)=(Tcwo1(t+1,Num)+Tcwo2(t+1,Num)+Tcwo3(t+1,Num)+Tcwo4(t+1,Num)+Tcwo5(t+1,Num)+Tcwo6(t+1,Num))/N_CT;
                Tcon_in(t+1,Num)=ChiAgent(Load,Tcwin,Tchws0,w(t,Num),m(t,Num),ON_OFF_chi(1),T_lb,T_ub);
                Converge=m(t+1,Num);
            else
                for j=1:2
                    for i=1:7  % 1 coloum: component to coordinator, 2 coloum coordinator to component, CT first
                        Delay(i,j)=0;%De_fun1(De_last(i,j));
                    end
                end
%                 Delay(7,1)=2;
%                 Delay(7,2)=2;
                Tcwo(t,Num)=(Tcwo1(max(1,t-Delay(1,1)),Num)+Tcwo2(max(1,t-Delay(2,1)),Num)+Tcwo3(max(1,t-Delay(3,1)),Num)+Tcwo4(max(1,t-Delay(4,1)),Num)+Tcwo5(max(1,t-Delay(5,1)),Num)+Tcwo6(max(1,t-Delay(6,1)),Num))/N_CT;
                [w(t+1,Num),m(t+1,Num)]=CoAgent(Tcwo(t,Num),Tcon_in(max(1,t-Delay(7,1)),Num),N_chi,w0);
                Tcwo1(t+1,Num)=CTAgent(m(max(1,t-Delay(1,2)),Num),w(max(1,t-Delay(1,2)),Num),Tcwin,Twb,Load,B(1,1),B(1,2),B(1,3),B(1,4),ON_OFF_CT(1),T_lb,T_ub);
                Delay1(t+1,Num)=Delay(1,2);
                Tcwo2(t+1,Num)=CTAgent(m(max(1,t-Delay(2,2)),Num),w(max(1,t-Delay(2,2)),Num),Tcwin,Twb,Load,B(2,1),B(2,2),B(2,3),B(2,4),ON_OFF_CT(2),T_lb,T_ub);
                Tcwo3(t+1,Num)=CTAgent(m(max(1,t-Delay(3,2)),Num),w(max(1,t-Delay(3,2)),Num),Tcwin,Twb,Load,B(3,1),B(3,2),B(3,3),B(3,4),ON_OFF_CT(3),T_lb,T_ub);
                Tcwo4(t+1,Num)=CTAgent(m(max(1,t-Delay(4,2)),Num),w(max(1,t-Delay(4,2)),Num),Tcwin,Twb,Load,B(4,1),B(4,2),B(4,3),B(4,4),ON_OFF_CT(4),T_lb,T_ub);
                Tcwo5(t+1,Num)=CTAgent(m(max(1,t-Delay(5,2)),Num),w(max(1,t-Delay(5,2)),Num),Tcwin,Twb,Load,B(5,1),B(5,2),B(5,3),B(5,4),ON_OFF_CT(5),T_lb,T_ub);
                Tcwo6(t+1,Num)=CTAgent(m(max(1,t-Delay(6,2)),Num),w(max(1,t-Delay(6,2)),Num),Tcwin,Twb,Load,B(6,1),B(6,2),B(6,3),B(6,4),ON_OFF_CT(6),T_lb,T_ub);
                Tcwof(t+1,Num)=(Tcwo1(t+1,Num)+Tcwo2(t+1,Num)+Tcwo3(t+1,Num)+Tcwo4(t+1,Num)+Tcwo5(t+1,Num)+Tcwo6(t+1,Num))/N_CT;
                Tcon_in(t+1,Num)=ChiAgent(Load,Tcwin,Tchws0,w(max(1,t-Delay(7,2)),Num),m(max(1,t-Delay(7,2)),Num),ON_OFF_chi(1),T_lb,T_ub);
                Converge=m(t+1,Num);
            end
            t=t+1;
            De_last=Delay;
        end
        Tcwoi_a=Tcwo(t-1,Num);
        T_lb=Tcwoi_a-2;
        T_ub=Tcwoi_a+2;
%                         %% Communication delay Stage 2
%         De_CoCT1=0; %delay of coordinator agent to cooling tower agent
%         De_CoCT2=0; %delay of coordinator agent to cooling tower agent
%         De_CoCT3=0; %delay of coordinator agent to cooling tower agent
%         De_CoCT4=0; %delay of coordinator agent to cooling tower agent
%         De_CoCT5=0; %delay of coordinator agent to cooling tower agent
%         De_CoCT6=0; %delay of coordinator agent to cooling tower agent
%         De_CTCo1=0; %delay of cooling tower agent to coordinator agent
%         De_CTCo2=0; %delay of cooling tower agent to coordinator agent
%         De_CTCo3=0; %delay of cooling tower agent to coordinator agent
%         De_CTCo4=0; %delay of cooling tower agent to coordinator agent
%         De_CTCo5=0; %delay of cooling tower agent to coordinator agent
%         De_CTCo6=0; %delay of cooling tower agent to coordinator agent
       %% initialization
        t_2=0;
        Cov(1,Num)=0;
        u0=u_last;
        u(1,Num)=u0;
        Tcwoi1(t_2+1,Num)=CTAgent(Cov(t_2+1,Num),u(t_2+1,Num),Tcwin,Twb,Load,B(1,1),B(1,2),B(1,3),B(1,4),ON_OFF_CT(1),T_lb,T_ub);
        Tcwoi2(t_2+1,Num)=CTAgent(Cov(t_2+1,Num),u(t_2+1,Num),Tcwin,Twb,Load,B(2,1),B(2,2),B(2,3),B(2,4),ON_OFF_CT(2),T_lb,T_ub);
        Tcwoi3(t_2+1,Num)=CTAgent(Cov(t_2+1,Num),u(t_2+1,Num),Tcwin,Twb,Load,B(3,1),B(3,2),B(3,3),B(3,4),ON_OFF_CT(3),T_lb,T_ub);
        Tcwoi4(t_2+1,Num)=CTAgent(Cov(t_2+1,Num),u(t_2+1,Num),Tcwin,Twb,Load,B(4,1),B(4,2),B(4,3),B(4,4),ON_OFF_CT(4),T_lb,T_ub);
        Tcwoi5(t_2+1,Num)=CTAgent(Cov(t_2+1,Num),u(t_2+1,Num),Tcwin,Twb,Load,B(5,1),B(5,2),B(5,3),B(5,4),ON_OFF_CT(5),T_lb,T_ub);
        Tcwoi6(t_2+1,Num)=CTAgent(Cov(t_2+1,Num),u(t_2+1,Num),Tcwin,Twb,Load,B(6,1),B(6,2),B(6,3),B(6,4),ON_OFF_CT(6),T_lb,T_ub);
        Tcwoi(t_2+1,Num)=(Tcwoi1(t_2+1,Num)+Tcwoi2(t_2+1,Num)+Tcwoi3(t_2+1,Num)+Tcwoi4(t_2+1,Num)+Tcwoi5(t_2+1,Num)+Tcwoi6(t_2+1,Num))/N_CT;
        Tcwoif(t_2+1,Num)=(Tcwoi1(t_2+1,Num)+Tcwoi2(t_2+1,Num)+Tcwoi3(t_2+1,Num)+Tcwoi4(t_2+1,Num)+Tcwoi5(t_2+1,Num)+Tcwoi6(t_2+1,Num))/N_CT;
        
        %% Stage 2
        t_2=1;
        Converge=0;
        while Converge==0 && t_2<600
            if t_2<2
                Delay2=zeros(6,2); % 1 coloum: component to coordinator, 2 coloum coordinator to component
                [u(t_2+1,Num),Cov(t_2+1,Num)]=CoAgent2(Tcwoi(t_2,Num),Tcwoi_a,N_chi,u(t_2,Num));
                Tcwoi1(t_2+1,Num)=CTAgent(Cov(t_2,Num),u(t_2,Num),Tcwin,Twb,Load,B(1,1),B(1,2),B(1,3),B(1,4),ON_OFF_CT(1),T_lb,T_ub);
                Tcwoi2(t_2+1,Num)=CTAgent(Cov(t_2,Num),u(t_2,Num),Tcwin,Twb,Load,B(2,1),B(2,2),B(2,3),B(2,4),ON_OFF_CT(2),T_lb,T_ub);
                Tcwoi3(t_2+1,Num)=CTAgent(Cov(t_2,Num),u(t_2,Num),Tcwin,Twb,Load,B(3,1),B(3,2),B(3,3),B(3,4),ON_OFF_CT(3),T_lb,T_ub);
                Tcwoi4(t_2+1,Num)=CTAgent(Cov(t_2,Num),u(t_2,Num),Tcwin,Twb,Load,B(4,1),B(4,2),B(4,3),B(4,4),ON_OFF_CT(4),T_lb,T_ub);
                Tcwoi5(t_2+1,Num)=CTAgent(Cov(t_2,Num),u(t_2,Num),Tcwin,Twb,Load,B(5,1),B(5,2),B(5,3),B(5,4),ON_OFF_CT(5),T_lb,T_ub);
                Tcwoi6(t_2+1,Num)=CTAgent(Cov(t_2,Num),u(t_2,Num),Tcwin,Twb,Load,B(6,1),B(6,2),B(6,3),B(6,4),ON_OFF_CT(6),T_lb,T_ub);
                Tcwoif(t_2+1,Num)=(Tcwoi1(t_2+1,Num)+Tcwoi2(t_2+1,Num)+Tcwoi3(t_2+1,Num)+Tcwoi4(t_2+1,Num)+Tcwoi5(t_2+1,Num)+Tcwoi6(t_2+1,Num))/N_CT;
                Converge=Cov(t_2+1,Num);
            else
                for j2=1:2
                    for i2=1:6  % 1 coloum: component to coordinator, 2 coloum coordinator to component, CT first
                        Delay2(i2,j2)=0;%De_fun1(De_last2(i2,j2));
                    end
                end
                Tcwoi(t_2,Num)=(Tcwoi1(max(1,t_2-Delay2(1,1)),Num)+Tcwoi2(max(1,t_2-Delay2(2,1)),Num)+Tcwoi3(max(1,t_2-Delay2(3,1)),Num)+Tcwoi4(max(1,t_2-Delay2(4,1)),Num)+Tcwoi5(max(1,t_2-Delay2(5,1)),Num)+Tcwoi6(max(1,t_2-Delay2(6,1)),Num))/N_CT;
                [u(t_2+1,Num),Cov(t_2+1,Num)]=CoAgent2(Tcwoi(t_2,Num),Tcwoi_a,N_chi,u0);
                Tcwoi1(t_2+1,Num)=CTAgent(Cov(max(1,t_2-Delay2(1,2)),Num),u(max(1,t_2-Delay2(1,2)),Num),Tcwin,Twb,Load,B(1,1),B(1,2),B(1,3),B(1,4),ON_OFF_CT(1),T_lb,T_ub);
                Tcwoi2(t_2+1,Num)=CTAgent(Cov(max(1,t_2-Delay2(2,2)),Num),u(max(1,t_2-Delay2(2,2)),Num),Tcwin,Twb,Load,B(2,1),B(2,2),B(2,3),B(2,4),ON_OFF_CT(2),T_lb,T_ub);
                Tcwoi3(t_2+1,Num)=CTAgent(Cov(max(1,t_2-Delay2(3,2)),Num),u(max(1,t_2-Delay2(3,2)),Num),Tcwin,Twb,Load,B(3,1),B(3,2),B(3,3),B(3,4),ON_OFF_CT(3),T_lb,T_ub);
                Tcwoi4(t_2+1,Num)=CTAgent(Cov(max(1,t_2-Delay2(4,2)),Num),u(max(1,t_2-Delay2(4,2)),Num),Tcwin,Twb,Load,B(4,1),B(4,2),B(4,3),B(4,4),ON_OFF_CT(4),T_lb,T_ub);
                Tcwoi5(t_2+1,Num)=CTAgent(Cov(max(1,t_2-Delay2(5,2)),Num),u(max(1,t_2-Delay2(5,2)),Num),Tcwin,Twb,Load,B(5,1),B(5,2),B(5,3),B(5,4),ON_OFF_CT(5),T_lb,T_ub);
                Tcwoi6(t_2+1,Num)=CTAgent(Cov(max(1,t_2-Delay2(6,2)),Num),u(max(1,t_2-Delay2(6,2)),Num),Tcwin,Twb,Load,B(6,1),B(6,2),B(6,3),B(6,4),ON_OFF_CT(6),T_lb,T_ub);
                Tcwoif(t_2+1,Num)=(Tcwoi1(t_2+1,Num)+Tcwoi2(t_2+1,Num)+Tcwoi3(t_2+1,Num)+Tcwoi4(t_2+1,Num)+Tcwoi5(t_2+1,Num)+Tcwoi6(t_2+1,Num))/N_CT;
                Converge=Cov(t_2+1,Num);
            end
            t_2=t_2+1;
            De_last2=Delay2;
        end
        if t+t_2<120
        T_ct(1,1)=Tcwoi1(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,2)=Tcwoi2(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,3)=Tcwoi3(max(1,t_2-1-Delay2(1,1)),Num);
        T_ct(1,4)=Tcwoi4(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,5)=Tcwoi5(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,6)=Tcwoi6(max(1,t_2-1-Delay2(1,1)),Num);
        Tcon=sum(T_ct)/N_CT;
        elseif t<120
            T_ct(1,1:6)=Tcwoi_a;
            Tcon=Tcwoi_a;
        else
            T_ct(1,1:6)=max(T_lb,min(Twb+5,T_ub));
            Tcon=max(T_lb,min(Twb+5,T_ub));
        end
%         T_ct(1,1)=Tcwoi1(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,2)=Tcwoi2(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,3)=Tcwoi3(max(1,t_2-1-Delay2(1,1)),Num);
%         T_ct(1,4)=Tcwoi4(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,5)=Tcwoi5(max(1,t_2-1-Delay2(1,1)),Num);T_ct(1,6)=Tcwoi6(max(1,t_2-1-Delay2(1,1)),Num);
        u_last=u(t_2,Num);
        %%
        Pchi=N_CT/2*7235*5.566*(0.0111+0.0204*PLR+0.0003*PLR*PLR)*(0.5489-0.0152*Tchws0+0.0007896*Tchws0^2+0.01405*Tcon...
             +0.0003618*Tcon*Tcon-0.0003554*Tchws0*Tcon);
        P_tot(Num,1)=P_CTtotfun(205.3,T_ct,Tcwin,Twb,N_CT)+Pchi;
        Result(Num,1)=t+t_2;
        Result(Num,2)=Tcwo(t-1,Num);
%         Result(Num,3)=Tcon_in(t,Num);
        Result(Num,3)=Tcwoi(t_2-1,Num);
        Result(Num,4)=P_CTtotfun(205.3,T_ct,Tcwin,Twb,N_CT)+Pchi;
        Result(Num,5)=w(t,Num);
        Result(Num,6)=u(t_2,Num);
        Result(Num,7)=Tcwoi1(t_2-1,Num);
        Result(Num,8)=Tcwoi2(t_2-1,Num);
        Result(Num,9)=Tcwoi3(t_2-1,Num);
        Result(Num,10)=Tcwoi4(t_2-1,Num);
        Result(Num,11)=Tcwoi5(t_2-1,Num);
        Result(Num,12)=Tcwoi6(t_2-1,Num);
        Result(Num,13)=t;
        Result(Num,14)=Twb;
        Num=Num+1;
    end
end
end
xlswrite('Twomin4344_normal.xlsx',Result);