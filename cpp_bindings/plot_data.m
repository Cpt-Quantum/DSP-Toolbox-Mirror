file = "output_file.csv";

channels = 256;

F = dlmread(file, ",", 0, 0);

hold on

x = F(:,1);
for i = 2:(channels - 1 + 2)
    y = F(:,i);
    plot(x, y);
end