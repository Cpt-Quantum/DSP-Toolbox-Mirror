DATA_SIZE = 1000;
FREQUENCY = 10;
SAMPLING_FREQUENCY = 100;

t = zeros(DATA_SIZE,1);
f = zeros(DATA_SIZE/2,1);
y1 = zeros(DATA_SIZE,1);
y2 = zeros(DATA_SIZE,1);
F_k = zeros(DATA_SIZE,1);
trunc_F_k = zeros(DATA_SIZE/2,1);

for(i=1:DATA_SIZE)
  t(i) = i / SAMPLING_FREQUENCY;
  y1(i) = cos(2 * pi * FREQUENCY * t(i)); 
endfor;

for(i=1:DATA_SIZE/2)
  f(i) = ((SAMPLING_FREQUENCY * (i-1)) / DATA_SIZE);
endfor;

F_k = fft(y1);

for(i = 1:DATA_SIZE/2)
  trunc_F_k(i) = F_k(i);
endfor;

y2 = 2 * abs(trunc_F_k) / DATA_SIZE;

figure(1);
plot(t, y1);
figure(2);
plot(f,y2);
