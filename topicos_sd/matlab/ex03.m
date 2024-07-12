% exemplo 3
A = [1 2 3 4 5 6 7]           %linha 1
B = mean(A)                   %linha 2
C = std(A)                    %linha 3
pause                         %linha 4
RUIDOSO = randn(1, 5000);     %linha 5
figure(gcf)                   %linha 6
plot(RUIDOSO)                 %linha 7
title('sinal ruidoso')        %linha 8
pause                         %linha 9
MEDIA_RUIDOSO = mean(RUIDOSO) %linha 10
DP_RUIDOSO = std(RUIDOSO)     %linha 11
pause                         %linha 12
MAISRUIDO = 5*randn(1 ,5000); %linha 13
MEDIA = mean(MAISRUIDO)       %linha 14
DP = std(MAISRUIDO)           %linha 15