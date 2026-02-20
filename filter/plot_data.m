input_filename = "input_data.csv";
output_filename = "output_data.csv";

% Load input data file
data_in = dlmread(input_filename,",",1,0);
% Load output data file
data_out = dlmread(output_filename,",",1,0);

t_in = data_in(:, 1);
x = data_in(:,2);

t_out = data_out(:,1);
y = data_out(:,2);

figure(1);
plot(t_in, x, t_out, y);
%plot(data_in);
legend("Input data", "Output data");
