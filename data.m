% % ���ɱ�ǩ
% x = [128,225,316,418,510,607,744,893,996];
% y = [0.407, 0.924, 1.02, 1.462, 1.859, 2.237, 2.754, 3.224, 3.567];
% plot(x,y,'-*');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������
% axis([100,1004,0,5]);
% title('��ǩ����');


% AES VS. SCO
% x = [128,225,316,418,510,607,744,893,996];
% y1 = [4.837, 8.195, 9.094, 13.199, 15.895, 20.138, 22.751, 27.136, 29.317];
% y2 = [3.428, 7.329, 8.817, 11.855, 15.287, 18.067, 22.021, 27.224, 29.234];
% axis([100,1004,0,35]);
% plot(x,y1,'-*',x,y2,'-+');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������



% % ����
% A =[4.177 1.191 0.660
%     7.203 1.646 0.992
%     8.035 1.863 1.059
%     11.665 2.491 1.534
%     14.220 3.226 1.675
%     18.059 4.191 2.079
%     20.422 5.626 2.329
%     24.213 6.598 2.923
%     26.125 8.544 3.192];
% 
% A=reshape(A,[9 1 3]);
% x=[128,225,316,418,510,607,744,893,996];
% plotBarStackGroups(A,x);

% % ȥ��
% x = [128,225,316,418,510,607,744,893,996];
% y = [0.346, 0.37, 0.424, 0.527, 0.383, 0.534, 0.429, 0.46, 0.406];
% plot(x,y,'-*');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������
% axis([100,1004,0,2]);
% title('ȥ�ؼ���POW');

% % ���ֽ���
% x = [128,225,316,418,510,607,744,893,996];
% y = [2.249, 2.599, 2.65, 3.357, 3.947, 4.893, 5.212, 6.809, 6.925];
% plot(x,y,'-*');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������
% axis([100,1004,0,8]);
% title('���ֽ���');

% % ����
% x = [128,225,316,418,510,607,744,893,996];
% y = [5.115, 5.211, 6.454, 6.997, 8.418, 8.969, 13.253, 13.832, 15.897];
% plot(x,y,'-*');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������
% axis([100,1004,0,17]);
% title('����');

% % ����M
% x = [128,225,316,418,510,607,744,893,996];
% y = [4.984, 6.598, 7.927, 10.935, 13.468, 16.158, 19.261, 22.925, 25.639];
% plot(x,y,'-*');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������
% axis([100,1004,0,28]);
% title('����M');

% % ����
% x = [128,225,316,418,510,607,744,893,996];
% y = [3.06, 3.813, 4.132, 4.729, 5.982, 7.351, 8.674, 9.114, 10.855];
% plot(x,y,'-*');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������
% axis([100,1004,0,13]);
% title('����');

% % ����
% A =[2.310 1.933 1.231
%     3.740 3.133 2.072
%     7.532 3.605 2.340
%     10.852 5.839 3.172
%     13.470 6.781 3.915
%     15.749 8.391 4.805
%     19.876 11.611 5.395
%     24.020 12.201 6.599
%     25.882 14.170 6.833];
% 
% A=reshape(A,[9 1 3]);
% x=[128,225,316,418,510,607,744,893,996];
% plotBarStackGroups(A,x);


% A =[  4.984 5.115 2.249
%       6.598 5.211 2.599
%       7.927 6.454 2.650
%       10.935 6.997 3.357
%       13.468 8.418 3.947
%       16.158 8.969 4.851
%       19.261 13.253 5.212
%       22.925 13.832 6.809
%       25.639 15.897 6.925];
% 
% A=reshape(A,[9 1 3]);
% x=[128,225,316,418,510,607,744,893,996];
% plotBarStackGroups(A,x);

% ����
A =[ 0.517 1.845  0.403
    0.879 1.874 0.397
    1.233  1.893 0.432
    1.575 1.593 0.391 
    2.092  1.750 0.382
    3.279 1.787 0.401
    3.328 2.676  0.447
    4.238 1.926 0.434
    4.822 2.187 0.422];

A=reshape(A,[9 1 3]);
x=[128,225,316,418,510,607,744,893,996];
plotBarStackGroups(A,x);


% % AES VS. SCO
% x = [128,225,316,418,510,607,744,893,996];
% y1 = [3.024, 5.294, 7.301, 10.603, 12.989, 13.959, 18.642, 22.436, 24.811];
% y2 = [3.164, 5.812, 9.872, 14.024, 17.385, 20.554, 25.271, 30.619,32.715];
% axis([100,1004,0,35]);
% plot(x,y1,'-*',x,y2,'-+');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������



% x = [128,225,316,418,510,607,744,893,996];
% y1 = [1.933, 3.740, 7.532, 10.852, 13.470, 15.749, 19.876, 24.020, 25.882];
% y2 = [1.062, 2.206, 4.587, 7.391, 8.673, 9.698, 11.732, 12.875, 13.732];
% axis([100,1004,0,35]);
% plot(x,y1,'-*',x,y2,'-+');
% xlabel('FileSize(Mb)'); %x����������
% ylabel('Time(s)'); %y����������

