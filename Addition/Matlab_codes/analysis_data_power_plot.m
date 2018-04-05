clear all
clc 
close all

%% 
fn_path = '/home/hcrl/Repository/Sejong_Dynamic_Control_Toolkit/Addition/Matlab_codes/functions';

addpath(fn_path)

data_path = '/home/hcrl/MyCloud/Apptronik/HumeSingleLeg/experiment_data_check';
read_data_simple_file;
v1 = fn_read_file(data_path, '1_lg1j2_energetics__bus_voltage__V', 1);
a1 = fn_read_file(data_path, '1_lg1j2_energetics__bus_current__A', 1);
v2 = fn_read_file(data_path, '2_lg1j1_energetics__bus_voltage__V', 1);
a2 = fn_read_file(data_path, '2_lg1j1_energetics__bus_current__A', 1);
v3 = fn_read_file(data_path, '3_lg1j0_energetics__bus_voltage__V', 1);
a3 = fn_read_file(data_path, '3_lg1j0_energetics__bus_current__A', 1);
v4 = fn_read_file(data_path, '4_lg0j0_energetics__bus_voltage__V', 1);
a4 = fn_read_file(data_path, '4_lg0j0_energetics__bus_current__A', 1);
v5 = fn_read_file(data_path, '5_lg0j1_energetics__bus_voltage__V', 1);
a5 = fn_read_file(data_path, '5_lg0j1_energetics__bus_current__A', 1);
v6 = fn_read_file(data_path, '6_lg0j2_energetics__bus_voltage__V', 1);
a6 = fn_read_file(data_path, '6_lg0j2_energetics__bus_current__A', 1);

fig = fn_open_figures(6);


voltage = false;
%voltage = true;
current = true;
%current = false;
if voltage == true
figure(fig(1))
plot(v1);

figure(fig(2))
plot(v2);

figure(fig(3))
plot(v3);

figure(fig(4))
plot(v4);

figure(fig(5))
plot(v5);

figure(fig(6))
plot(v6);

elseif current== true

figure(fig(1))
plot(a1);

figure(fig(2))
plot(a2);

figure(fig(3))
plot(a3);

figure(fig(4))
plot(a4);

figure(fig(5))
plot(a5);

figure(fig(6))
plot(a6);

else
figure(fig(1))
plot(v1 .* a1);
figure(fig(2))
plot(v2.*a2);

figure(fig(3))
plot(v3.*a3);

figure(fig(4))
plot(v4.*a4);

figure(fig(5))
plot(v5.*a5);

figure(fig(6))
plot(v6.*a6);
end