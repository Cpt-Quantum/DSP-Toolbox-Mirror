% Low Pass filter designer

% Cutoff frequency
f_c = 20;
% Step size for filter design in frequency domain
f_step = 0.01;
% Window length for filter design in frequency domain
F = 100;

% Create the frequencies
f = - F : f_step : F;
% Calculate the number of points in the frequency plot
size_f = length(f);

% Now set the frequency response of the filter
y_f = zeros(1,size_f);
zero_offset = ((size_f - 1) / 2);
% Positive section of the frequency domain
for (i = (zero_offset + 1) : size_f)
  if ( (f_step * (i - zero_offset) ) > f_c)
    y_f(i) = 0;
  else
    y_f(i) = 1;
    
  endif
end
% Zero frequency amplitude:
y_f(zero_offset) = 1;
% Negative section of the frequency domain
for (i = 1 : (zero_offset - 1))
  y_f(zero_offset - i) = y_f(i + zero_offset);
end

% Calculate the transfer function and corresponding time values
y_t = abs(ifft(y_f));
t = -(((size_f-1) * f_step) / 2) : f_step : (((size_f-1) * f_step) / 2);

% Plot the frequency response
figure(1);
plot(f, y_f);

% Plot the transfer function in time domain
figure(2);
plot(t, y_t);
