clear all
clc

log = linspace(0,500,501);
y = zeros(length(log));
for i=1:length(log)
    y(i) = compute_battery(log(i));
end


figure(1)
semilogy(log,y);
title('Months 2AA (4000mAh) Batteries will last')
xlabel('Number of Logs per Day');
ylabel('Months');


print -depsc battery.eps