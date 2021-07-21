file1 = "input_file.csv";
file2 = "output_file.csv";
SAMPLING_FREQUENCY = 1000;

F1 = dlmread(file1, ",", 0, 0);

x1 = F1(:,1);
y1 = F1(:,2);

F2 = dlmread(file2, ",", 0, 0);
x_out1 = F2(:,1);
y_out1 = F2(:,2);

DATA_SIZE = length(x1);

f = zeros(DATA_SIZE/2,1, "float");

for(i=1:DATA_SIZE/2)
  f(i) = ((SAMPLING_FREQUENCY * (i-1)) / DATA_SIZE);
endfor;

F_k = fft(y1);

for(i = 1:DATA_SIZE/2)
  trunc_F_k(i) = F_k(i);
endfor;

y2 = 2 * abs(trunc_F_k) / DATA_SIZE

figure(1);
plot(x1, y1, x_out1, y_out1);

figure(2);
plot(f, y2);
