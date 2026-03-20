% Get command line arguments
args = argv();
if (length(args) < 1)
  printf("Usage: fft_from_file.m <filename.csv>");
  exit(1);
endif
input_filename = args{1};

% Load input data file
data_in = dlmread(input_filename, ",");

t = data_in(:,1);
x = data_in(:,2);
len = size(x);

h = plot(t, x);
waitfor(h);
