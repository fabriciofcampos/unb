T = 28:1:43;
b = 3500;              % Coeficiente de temperatura
r25 = 10000;           % Resistência para a temperatura de 25 °C   
T0 = 25 + 273.15;      % Temperatura de referência igual a 25 °C

% Expressão que relaciona resistência e temperatura
r = r25 * exp(b * (1./(T + 273.15) - 1/T0));

% Componentes do circuito
R1 = 10000;
R2 = 10000;
R3 = 10000;

V1 = 5 * R2 / (R2 + R1);       % Valor da tensão na entrada + do AmpOp
V2 = 5 .* r ./ (r + R3);       % Valores de tensão na entrada - do AmpOp
A = 5 / (V1 - V2(end));        % Ganho do AmpOp para que 43 °C -> 5 V

% Expressão que relaciona tensão e temperatura
V0 = A * (V1 - V2);

% Plotando a curva V0(T)
plot(T, V0, 'o-')
grid
title('Funcao Tensão x Temperatura')
xlabel('Temperatura (°C)')
ylabel('Tensão (V)')
