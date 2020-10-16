% addpath('./Methods')

injector_directory = './Data/N2O_Cold_Flow/Injector/'
throttle_directory = './Data/N2O_Cold_Flow/Throttle/'

file = 'run_data_40_01_24'
full_path = join([throttle_directory, file, '.csv'])

table = readtable(full_path, 'Format', 'auto');


% Clear unused column
table.Var12 = [];
% Elapsed time from 0
table.met = (table.missionTime - table.missionTime(1)) / 1000;
% Throttle valve pressure differential 
table.deltaP = table.upstreamPressure - table.downstreamPressure;

% pwelch(table.thrust)
figure(1); clf;
pwelch(table.thrust, [], [], [], 80); hold on;
pwelch(table.propellantMass, [], [], [], 80);
grid on

figure(3); clf;
plot(table.met, [table.propellantMass, sgolayfilt(table.propellantMass, 3, 21), sgolayfilt(table.propellantMass, 3, 49), sgolayfilt(table.propellantMass, 3, 121)])
hold on
yyaxis right
plot(table.met, table.throttlePosition)
grid on

Nf = 256;
Fpass = 0.01;
Fstop = 0.02;
Fs = 80;

table.met = table.met / 1000;

df = designfilt('differentiatorfir', 'FilterOrder', Nf, 'PassbandFrequency', Fpass, 'StopbandFrequency', Fstop, 'SampleRate', Fs);
% fvtool(df, 'MagnitudeDisplay', 'zero-phase', 'Fs', Fs);

dt = table.met(2) - table.met(1);
mf = filter(df, table.propellantMass) / dt;
delay = mean(grpdelay(df));

t = table.met(1:end-delay);
mf(1:delay) = [];
t(1:delay) = [];
mf(1:delay) = [];

figure(4); clf;
plot(t, mf);
plot(t, mf);
grid on

