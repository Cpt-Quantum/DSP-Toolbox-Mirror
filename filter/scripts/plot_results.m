arg_list = argv();
load(arg_list{1}); % This puts all variables into the workspace immediately

figure;
subplot(2,1,1); hold on;
plot(t_in, sig_in, 'color', [0.7 0.7 0.7]); plot(t_out, sig_out, 'b');
title('Interactive Time Domain');

subplot(2,1,2); hold on;
plot(f_in, m_in, 'color', [0.7 0.7 0.7]); plot(f_out, m_out, 'r');
title('Interactive Frequency Domain');

printf("Inspection window open. Press any key in the terminal to close.\n");
pause;
