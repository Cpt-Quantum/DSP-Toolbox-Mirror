f_s = 512;

% Test signal variables
test_sig_period_s = 400;

ampl_1 = 10;
freq_1 = 5;
phase_1 = 0;

ampl_2 = 50;
freq_2 = 50;
phase_2 = 0;

ampl_3 = 50;
freq_3 = 1;
phase_3 = 0;

ampl_4 = 50;
freq_4 = 100;
phase_4 = 0;

% End of test signal variables %

% NB: This filter order is actually half the filters actual order, due to it being
% a bandpass filter. E.g filter_order = 5 will give a filter order of 10
filter_order = 6;

bandpass_high_cut = 10;
bandpass_low_cut = 1;

%angle_mult = 2 * pi;
angle_mult = 2;
angle_high_cut = angle_mult*bandpass_high_cut/f_s;
angle_low_cut = angle_mult*bandpass_low_cut/f_s;

[b,a] = butter(filter_order, [angle_low_cut, angle_high_cut], "bandpass");
[sos, g] = tf2sos(b,a);

% Create test signal
t = 0 : 1/f_s : test_sig_period_s-1/f_s;
x = ampl_1*sin(2 * pi * (freq_1 * t + phase_1)) + ampl_2*sin(2 * pi * (freq_2 * t + phase_2));
x += ampl_3*sin(2 * pi * (freq_3 * t + phase_3)) + ampl_4*sin(2 * pi * (freq_4 * t + phase_4));
%y = g * sosfilt(sos,x);

%plot(t,x);
%plot(t,x, t,y);

%w = linspace(0,4,f_s);
%freqz(b,a,f_s);