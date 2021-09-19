function [w,m] = CoAgent(Tcwo,Tcon_in,N_chi,w0)
persistent a;
if isempty(a)
    a=w0;
end
if abs(Tcwo-Tcon_in)<0.05
    m=1;
    a=a;
else
    m=0;
    a=a+0.00000005*4.2*410.6*N_chi*(Tcwo-Tcon_in); 
end
w=a;
end
