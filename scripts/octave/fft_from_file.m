input_filename = "input_data.csv";
f_s = 4000;

% Load input data file
data_in = dlmread(input_filename);

y1 = data_in(:,1);
DATA_SIZE = size(y1,1);
printf("Size of input data: %u\n", DATA_SIZE);
step_size = 1/f_s;
t = 0 : 1/f_s : (DATA_SIZE-1)/f_s;

f = zeros(DATA_SIZE/2,1);
y2 = zeros(DATA_SIZE,1);
F_k = zeros(DATA_SIZE,1);
trunc_F_k = zeros(DATA_SIZE/2,1);

for(i=1:DATA_SIZE/2)
  f(i) = ((f_s * (i-1)) / DATA_SIZE);
endfor;

F_k = fft(y1);

for(i = 1:DATA_SIZE/2)
  trunc_F_k(i) = F_k(i);
endfor;

y2 = 2 * abs(trunc_F_k) / DATA_SIZE;

figure(1);
plot(t, y1);
figure(2);
h = plot(f,y2);
waitfor(h);
