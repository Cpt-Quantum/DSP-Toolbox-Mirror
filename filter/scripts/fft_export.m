arg_list = argv();
in_file = arg_list{1}; filt_file = arg_list{2};

% Load and Compute
d_in = csvread(in_file, 1, 0); t_in = d_in(:,1); sig_in = d_in(:,2);
d_out = csvread(filt_file, 1, 0); t_out = d_out(:,1); sig_out = d_out(:,2);

function [f, m] = do_fft(s)
    N = length(s); f = linspace(0, 0.5, floor(N/2)+1);
    m = 20*log10(abs(fft(s))(1:length(f)) + 1e-9);
endfunction

[f_in, m_in] = do_fft(sig_in); [f_out, m_out] = do_fft(sig_out);

% Save Bridge File
mat_name = strrep(filt_file, '.csv', '.mat');
save("-mat7-binary", mat_name, "t_in", "sig_in", "f_in", "m_in", "t_out", "sig_out", "f_out", "m_out");

% Save PNG
h = figure('visible', 'off');
subplot(2,1,1); plot(t_in, sig_in, 'color', [0.8 0.8 0.8], t_out, sig_out, 'b');
subplot(2,1,2); plot(f_in, m_in, 'color', [0.8 0.8 0.8], f_out, m_out, 'r');
print(h, strrep(filt_file, '.csv', '.png'), '-dpng');
