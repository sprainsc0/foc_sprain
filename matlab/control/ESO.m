function [sys,x0,str,ts]=han_eso(t,x,u,flag,d,bet,b,T)
switch flag,
case 0
   [sys,x0,str,ts] = mdlInitializeSizes; % ��ʼ��
case 2
   sys = mdlUpdates(x,u,d,bet,b,T); % ��ɢ״̬�ĸ���
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
sizes.NumContStates = 0; % ������״̬����
sizes.NumDiscStates = 3; % 3����ɢ״̬����
sizes.NumOutputs = 3;    % ��·���
sizes.NumInputs = 2;     % ��·���룺u��y
sizes.DirFeedthrough = 0; % �����źŲ�ֱ��������з�ӳ����
sizes.NumSampleTimes = 1;
sys = simsizes(sizes);
x0 = [0; 0; 0]; % ���ó�ʼ״̬Ϊ��״̬
str = []; % ��str��������Ϊ���ַ���
ts = [-1 0]; % ��������: �������̳������źŵĲ�������
%==============================================================
% ����������flag=2ʱ��������ɢϵͳ��״̬����,   (5.2.3),��չ״̬�۲���  
%==============================================================
function sys = mdlUpdates(x,u,d,bet,b,T)
e=x(1)-u(2);
sys(1,1)=x(1)+T*(x(2)-bet(1)*e);
sys(2,1)=x(2)+T*(x(3)-bet(2)*fal(e,0.5,d)+b*u(1));
sys(3,1)=x(3)-T*bet(3)*fal(e,0.25,d);
%==============================================================
% ��������flag=3ʱ������ϵͳ���������
%==============================================================
function sys = mdlOutputs(x)
sys=x; 
%==============================================================
% �û�������Ӻ����� fal, (P202)
%==============================================================
function f=fal(e,a,d)
if abs(e)<d
   f=e*d^(a-1);
else
   f=(abs(e))^a*sign(e);
end
