function Tcwo= CTAgent(m,w,Tcwin,Twb,Load,a1,a2,a3,a4,ON,Tcwo_lb,Tcwo_ub)
% persistent Tcws_b;
% if isempty(Tcws_b)
%     Tcws_b=22;
% end
persistent Tcws_a;
if isempty(Tcws_a)
    Tcws_a=22;
end
if ON==1
    if m==0
        if Load>7235*0.35
            P_CT=10^8;
            for Tcws1=Tcwo_lb:0.05:Tcwo_ub
                Ma=(4.2*205.3*(Tcwin-Tcws1)/((Tcwin-Twb)^a4*a1*205.3^(a3)))^(1/a2);
                P_CTi=(1.2206-17.297*(Ma/190.55)+168.26*(Ma/190.55)^2)-w*4.2*205.3*(Tcwin-Tcws1);
                if P_CTi<P_CT
                    P_CT=P_CTi;
                    Tcws_ct=Tcws1;
                end
            end
            Tcws_a=Tcws_ct;
            Tcwo = Tcws_ct;
        else
            Tcwo=100;
        end
    else
        Tcwo=Tcws_a;
    end
else
    Tcwo=0;
end