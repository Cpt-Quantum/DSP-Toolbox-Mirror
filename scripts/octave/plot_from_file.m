input_filename = "input_data.csv";
f_s = 4000;

% Load input data file
data_in = dlmread(input_filename);

x = data_in(:,1);
len = size(x);
printf("Length of input data: %u\n", len);
step_size = 1/f_s;
t = 0 : 1/f_s : (len-1)/f_s;
printf("Length of time data: %u\n", size(t));

h = plot(t, data_in);
waitfor(h);

