function [sys,x0,str,ts]=han_td(t,x,u,flag,tr,th,Ts)
switch flag,
case 0
   [sys,x0,str,ts] = mdlInitializeSizes; % ��ʼ��
case 2 
   sys = mdlUpdates(x,u,tr,th,Ts);  % ��ɢ״̬�ĸ���
case 3
   sys = mdlOutputs(x); % ������ļ���
case { 1, 4, 9 }
   sys = []; % δʹ�õ�flagֵ
otherwise
   error(['Unhandled flag = ',num2str(flag)]); % �������
end;
%==============================================================
% ��flagΪ0ʱ��������ϵͳ�ĳ�ʼ��
%==============================================================
function [sys,x0,str,ts] = mdlInitializeSizes
% ���ȵ���simsizes�����ó�ϵͳ��ģ����sizes, ��������ɢϵͳ��ʵ��
% �������sizes����
sizes = simsizes;
sizes.NumContStates = 0;  % ������״̬
sizes.NumDiscStates = 2;  % 2��ɢ״̬
sizes.NumOutputs = 2;     % �������Ϊ2
sizes.NumInputs = 1;      % �������Ϊ1
sizes.DirFeedthrough = 0; % ���벻ֱ��������з�ӳ����
sizes.NumSampleTimes = 1;
sys = simsizes(sizes);
x0 = [0; 0]; % ���ó�ʼ״̬Ϊ��״̬
str = []; % ��str��������Ϊ���ַ���
ts = [-1 0]; % ��������: [period, offset] �̳������źŵĲ�������
%==============================================================
% ����������flag=2ʱ��������ɢϵͳ��״̬��������5.2.2)������΢����
%==============================================================
function sys = mdlUpdates(x,u,r,h,T)
sys(1,1)=x(1)+T*x(2);
sys(2,1)=x(2)+T*fst2(x,u,r,h);
%==============================================================
% ��������flag=3ʱ������ϵͳ���������
%==============================================================
function sys = mdlOutputs(x)
sys=x; 
%==============================================================
% �û�������Ӻ����� fst2��(2.4.5)��ϣ�5.2.2)�ĵ��÷���
%==============================================================
function f=fst2(x,u,r,h)
delta=r*h; delta0=delta*h; y=x(1)-u+h*x(2);
a0=sqrt(delta*delta+8*r*abs(y));
if abs(y)<=delta0
    a=x(2)+y/h;
else
    a=x(2)+0.5*(a0-delta)*sign(y);
end
if abs(a)<=delta
    f=-r*a/delta;
else
    f=-r*sign(a);
end
