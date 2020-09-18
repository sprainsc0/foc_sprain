function [sys,x0,str,ts]=InertialIdentify(t,x,u,flag,lamuda,Ts,Te,Jn)
switch flag,
case 0
   [sys,x0,str,ts] = mdlInitializeSizes(Jn); % ��ʼ��
case 2
   sys = mdlUpdates(x,u,lamuda,Ts,Te); % ��ɢ״̬�ĸ���
case 3
   sys = mdlOutputs(x); % ������ļ���
case { 1, 4, 9 }
   sys = []; % δʹ�õ�flagֵ
otherwise
   error(['Unhandled flag = ',num2str(flag)]); % �������
end;

function [sys,x0,str,ts] = mdlInitializeSizes(Jn)
sizes = simsizes;
sizes.NumContStates = 0; % ������״̬����
sizes.NumDiscStates = 3; % 3����ɢ״̬����
sizes.NumOutputs = 3;    % 3·���
sizes.NumInputs = 2;     % ��·���룺u��y
sizes.DirFeedthrough = 0; % �����źŲ�ֱ��������з�ӳ����
sizes.NumSampleTimes = 1;
sys = simsizes(sizes);
x0 = [0; 0; Jn]; % ���ó�ʼ״̬Ϊ��״̬
str = []; % ��str��������Ϊ���ַ���
ts = [-1 0]; % ��������: �̳������źŵĲ�������
global A;
A = 0;
global B;
B = 0;
global Tk;
Tk = 0;

function sys = mdlUpdates(x,u,lamuda,Ts,Te)
global A;
global B;
global Tk;

sys(1,1) = x(1) + lamuda * (u(1) - x(1)) * Ts;
sys(2,1) = x(2) + lamuda * (u(2) - x(2)) * Ts;
dotq1 = (u(2) - sys(2,1)) * lamuda;
tao = dotq1 * x(3) - sys(1,1);

B = B + tao * dotq1 * Ts;
A = A + dotq1 * dotq1 * Ts;
Tk = Tk + Ts;
if(Tk > Te)
deltaJ = -(B / A); 
x(3) = x(3) + deltaJ;
A = 0;
B = 0;
Tk = 0;
end
sys(3,1) = x(3);

function sys = mdlOutputs(x)

sys=x; 



