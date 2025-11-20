file = "output_file.csv";

F = dlmread(file, ",", 0, 0);

x = F(:,1);
y1 = F(:,2);
y2 = F(:,3);
y3 = F(:,4);
y4 = F(:,5);

plot(x, y1, x, y2, x, y3, x, y4);
