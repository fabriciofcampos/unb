t = 0:0.001:0.2;
fs = 10;

sinal = square(2*pi*10*t);
plot(t, sinal)
title('Onda quadrada')
xlim([0 0.21])
xlabel('Tempo (s)')
ylim([-1.25 1.25])
ylabel('Amplitude')
