t = 0:0.001:0.2;
fs = 50;
As = 3;
Ar = 2;

sinal = As*sin(2*pi*fs*t);
ruido = Ar*randn(1,length(t));
sinal_ruidoso = sinal + ruido;
[ymin,idx_min] = min(sinal_ruidoso) ;
[ymax,idx_max] = max(sinal_ruidoso) ; 

plot(t, sinal_ruidoso)
title('Funcao senoidal com ruido')
xlabel('Tempo (s)')
ylim([-8.75 8.75])
ylabel('Amplitude (V)')
hold on
text(t(idx_min),ymin,['minimo: ' num2str(ymin)]);
text(t(idx_max),ymax,['maximo: ' num2str(ymax)]);
