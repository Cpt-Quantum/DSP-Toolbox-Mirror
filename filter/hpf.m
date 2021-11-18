% Capacitor value in pF
C = 100000;

% Resistor value in ohms
R = 10000;

% Frequency step size in Hz
freq_step = 1;
% Frequency maximum to display up to
max_freq = 20000;

% Convert capacitance into pF
C = C / (10^12);

f = 0 : freq_step : max_freq;

V = zeros(1,length(f));
for i = 1 : length(f)
  V(i) = f(i)*(C*R) / (sqrt(1 + (f(i)*(C*R))^2 ));
end

plot(f, V);