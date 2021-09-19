function Tcon_in = ChiAgent(Load,Tcwin,Tchws0,w,m,ON,Tcon_lb,Tcon_ub)
% persistent Tcon_b;%记录优化结果
% if isempty(Tcon_b)
%     Tcon_b=22;
% end
persistent Tcon_a;
if isempty(Tcon_a)
    Tcon_a=22;
end
if ON==1
    if m==0
        if Load>7235*0.35
            Chiller_cost=10^8;
            for Tcws0=Tcon_lb:0.05:Tcon_ub
                PLR=Load/7235;
                Chiller_costi=7235*5.566*(0.0111+0.0204*PLR+0.0003*PLR*PLR)*(0.5489-0.0152*Tchws0+0.0007896*Tchws0^2+0.01405*Tcws0...
                    +0.0003618*Tcws0*Tcws0-0.0003554*Tchws0*Tcws0)+w*4.2*410.6*(Tcwin-Tcws0);
                if Chiller_costi<Chiller_cost
                    Chiller_cost=Chiller_costi;
                    Tcws_chi=Tcws0;
                end
            end
            Tcon_a=Tcws_chi;
            Tcon_in=Tcws_chi;
        else
            Tcon_in=0;
        end
    else
        Tcon_in=Tcon_a;
    end
else
    Tcon_in=0;
end