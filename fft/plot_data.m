file1 = "input_data.csv";
file2 = "output_data.csv";

F1 = dlmread(file1, ",", 1, 0);
F2 = dlmread(file2, ",", 1, 0);

x1 = F1(:,1);
y1 = F1(:,2);
x2 = F2(:,1);
y2 = F2(:,4);

%plot(x1, y1, x2, y2);
figure(1);
plot(x1,y1);
figure(2);
plot(x2,y2);