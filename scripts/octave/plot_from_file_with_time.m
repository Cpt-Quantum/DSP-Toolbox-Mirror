input_filename = "av_din_out.csv";
f_s = 4000;

% Load input data file
data_in = dlmread(input_filename, ",");

t = data_in(:,1);
x = data_in(:,2);
len = size(x);

h = plot(t, data_in);
waitfor(h);
