% exemplo: geracao de uma forma de onda ruidosa
t = 0:0.01:pi;                %linha 1
y = 2*sin(5*t);               %linha 2
figure()                      %linha 3
plot(t,y)                     %linha 4
pause(2)                      %linha 5
ruido=randn(1, length(t));    %linha 6
plot(t,ruido)                 %linha 7
pause(2)                      %linha 8
y_ruido = y+ruido;            %linha 9
plot(t,y_ruido);              %linha 10