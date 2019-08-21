function [time] = compute_battery(logs)

battery_size = 4;               %amp hour
micro_sleep = 1*10^(-6);       %amp
wifi_tx = 266 *10^(-3);          %amp
wifi_rx = 53 * 10^(-3);         %amp
sensor_on = 100 * 10^(-3);        %amp
sensor_idle = 10 * 10^(-6);      %amp
time_to_log = 5;                 %seconds

tx_time = 1;      %time in seconds to transmit over WiFi per log
rx_time = 3;        %time in seconds to receive over Wifi per log
sensor_time = 1;    %time the sensor is on per log

seconds_in_day = 60*60*24;
weighted_sum_sleep = (micro_sleep + sensor_idle)*(seconds_in_day-logs*time_to_log);
% weighted_sum_active = (wifi_tx + wifi_rx + sensor_on)*time_to_log*logs;
weighted_sum_active = wifi_tx*tx_time*logs + wifi_rx*rx_time*logs + sensor_on*sensor_time*logs;

weighted_sum = (weighted_sum_sleep + weighted_sum_active) / seconds_in_day;

time = battery_size / weighted_sum;
time = time/(24*30);                 % time in months







