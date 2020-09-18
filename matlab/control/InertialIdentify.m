function [sys,x0,str,ts]=InertialIdentify(t,x,u,flag,lamuda,Ts,Te,Jn)
switch flag,
case 0
   [sys,x0,str,ts] = mdlInitializeSizes(Jn); % 初始化
case 2
   sys = mdlUpdates(x,u,lamuda,Ts,Te); % 离散状态的更新
case 3
   sys = mdlOutputs(x); % 输出量的计算
case { 1, 4, 9 }
   sys = []; % 未使用的flag值
otherwise
   error(['Unhandled flag = ',num2str(flag)]); % 处理错误
end;

function [sys,x0,str,ts] = mdlInitializeSizes(Jn)
sizes = simsizes;
sizes.NumContStates = 0; % 无连续状态变量
sizes.NumDiscStates = 3; % 3个离散状态变量
sizes.NumOutputs = 3;    % 3路输出
sizes.NumInputs = 2;     % 两路输入：u和y
sizes.DirFeedthrough = 0; % 输入信号不直接在输出中反映出来
sizes.NumSampleTimes = 1;
sys = simsizes(sizes);
x0 = [0; 0; Jn]; % 设置初始状态为零状态
str = []; % 将str变量设置为空字符串
ts = [-1 0]; % 采样周期: 继承输入信号的采样周期
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



