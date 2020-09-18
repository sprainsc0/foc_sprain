function [sys,x0,str,ts]=LinearStateEstimation(t,x,u,flag,w,b,Ts,T)
switch flag,
case 0
   [sys,x0,str,ts] = mdlInitializeSizes; % ��ʼ��
case 2
   sys = mdlUpdates(x,u,w,b,Ts,T); % ��ɢ״̬�ĸ���
case 3
   sys = mdlOutputs(x); % ������ļ���
case { 1, 4, 9 }
   sys = []; % δʹ�õ�flagֵ
otherwise
   error(['Unhandled flag = ',num2str(flag)]); % �������
end;

function [sys,x0,str,ts] = mdlInitializeSizes
sizes = simsizes;
sizes.NumContStates = 0; % ������״̬����
sizes.NumDiscStates = 3; % 3����ɢ״̬����
sizes.NumOutputs = 3;    % 3·���
sizes.NumInputs = 2;     % ��·���룺u��y
sizes.DirFeedthrough = 0; % �����źŲ�ֱ��������з�ӳ����
sizes.NumSampleTimes = 1;
sys = simsizes(sizes);
x0 = [0; 0; 0]; % ���ó�ʼ״̬Ϊ��״̬
str = []; % ��str��������Ϊ���ַ���
ts = [-1 0]; % ��������: �̳������źŵĲ�������
% ����ȫ�ֱ���, 3����λ���ȵ�buffer
global this_z;
this_z = [0; 0; 0];
global x_z1;
x_z1 = 0;

function sys = mdlUpdates(x,u,w,b,Ts,T)
global this_z;
global x_z1;
x_z1 = 0;
err = u(1)-this_z(1);
beta1 = 2 * w;
beta2 = w * w;
z1_correct = Ts * beta1 * err;
z2_correct = Ts * beta2 * err;

z2_correct_max = z1_correct / (3*Ts);

if abs(z2_correct) > abs(z2_correct_max)
    z2_correct = z2_correct_max;
    z1_correct = 0;
else
    z1_correct = z1_correct - z2_correct * 3*Ts;
end

filter_dt = Ts;
invT = 1 / T;
for i = 1:2
    this_z(i) = this_z(i + 1) + z1_correct + filter_dt*z2_correct;
    filter_dt = filter_dt + Ts;
end

sys(1,1) = x(1) + z1_correct + filter_dt*z2_correct + Ts * (x(3) + x(2));
sys(2,1) = x(2) + z2_correct;
sys(3,1) = x(3) + Ts * invT * (b * u(2) - x(3)); % inertia
sys(1,1) = sys(1,1) + Ts * (sys(3,1) + sys(2,1));
this_z(3) = sys(1,1);

function sys = mdlOutputs(x)
sys=x; 

