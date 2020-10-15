% Code adapted from HybridPerformanceWhitmore.m authored by Tom Moccia
% based on Lumped Ballistics method described by Whitmore

clear; clc;

% Physical constants
Ru = 8.31447;       % Universal Gas Constant (J / kg K)
g0 = 9.81;          % Gravity (m / s2)
hv = 2.3e6;         % Heat of vaporization of ABS (J / kg)
rhoFuel = 1070;     % Density of ABS (kg / m3)
rhoOx = 743.9;      % Density of N20 (kg / m3)
Ta = 298;           % Ambient temperature
Pa = 101325;        % Ambient pressure

% Chamber and port geometry
L = 0.156;          % Grain length (m)
rp = 0.004;         % Initial fuel grain port radius (m)
rc = 0.019;         % Chamber radius (m)
rt = 0.002;         % Throat radius (m)
At = pi * rt^2;     % Throat area (m2)
Cd = 0.4;           % Injector discharge coefficient (dimensionless);
ri = 0.00075;       % Injector port radius
Ai = pi * ri^2;     % Injector port area
Ar = 3.5;           % Area ratio

% Physical system parameters
POx = 5.178e6;        % Oxidizer feed pressure (Pa)
Pci = 3.447e6;         % Initial design chamber pressure (Pa)
OF = 5;             % Initial mixing ratio

% Integration time
ft = 20;
dt = 0.01;
ts = 0:dt:ft;

% Initialize outputs
outputs = {'w', 'Ap', 'Ab', 'Gox', 'rdot', 'Per', 'mdotOx', 'mdotFuel', 'OFRatio', 'Tc', 'k', 'MW', 'Cp', 'mu', 'cstar', 'R', 'Pc', 'fPc', 'dh', 'B', 'Pr', 'mdot', 'F', 'Cf', 'Isp', 'Pe', 'Gam', 'Me'};
types = cell(length(outputs), 1);
for i = 1:length(types)
    types{i} = 'double';
end
x = table('Size', [length(ts) length(outputs)], 'VariableNames', outputs, 'VariableTypes', types);

% Initial chamber parameters from assumed OF ratio
[ x.Tc(1), x.k(1), x.MW(1), x.Pr(1), x.Cp(1), x.mu(1), x.cstar(1) ] = HybridCEA500(OF);

% Set initial conditions
% Chamber geometric parameters
x.w(1) = 0;
x.Ap(1) = pi * rp^2;
x.Per(1) = 2 * pi * rp;
x.Ab(1) = x.Per(1) * L;

% Thermophysical properties
x.dh(1) = x.Cp(1) * (x.Tc(1) - Ta) * 1000;
x.Gam(1) = sqrt(x.k(1) / (((x.k(1) + 1 ) / 2)^((x.k(1) + 1) / (x.k(1) - 1))));
x.R(1) = Ru / x.MW(1);
x.B(1) = x.dh(1) / hv;
x.Pc(1) = Pci;

% Mass flows and fluxes
x.mdotOx(1) = 0.0252;
x.Gox(1) = x.mdotOx(1) / x.Ap(1);
x.rdot(1) = 0.036 / rhoFuel * x.Gox(1)^0.8 * (x.mu(1) / L)^0.2 * (x.B(1))^0.23;
x.mdotFuel(1) = x.rdot(1) * rhoFuel * x.Ab(1);
x.OFRatio(1) = x.mdotOx(1) / x.mdotFuel(1);
x.mdot(1) = x.mdotOx(1) + x.mdotFuel(1);

% Engine performance
x.Pc(1) = x.mdot(1) * x.cstar(1) / (At);
[x.Me(1)] = MachFinder(Ar, x.k(1));

x(1,:)

f = 10;
damping = 2 * pi * dt * f / (2 * pi * dt * f + 1);

for i = 1:length(ts) - 1
    j = i + 1;
    x.w(j) = x.w(i) + x.rdot(i) * dt;           % Web length
    x.Ap(j) = pi * (rp + x.w(j)) ^ 2;           % Port area
    x.Per(j) = 2 * pi * (rp + x.w(j));          % Port perimeter
    x.Ab(j) = x.Per(j) * L;                     % Burn area along grain

    % Update CEA properties from OFRatio
    [ x.Tc(j), x.k(j), x.MW(j), x.Pr(j), x.Cp(j), x.mu(j), x.cstar(j) ] = HybridCEA500(x.OFRatio(i));

    x.Gam(j) = sqrt(x.k(j) / (((x.k(j) + 1 ) / 2)^((x.k(j) + 1) / (x.k(j) - 1))));
    x.R(j) = Ru / x.MW(j);
    x.dh(j) = x.Cp(j) * (x.Tc(j) - Ta) * 1000;
    x.B(j) = x.dh(j) / hv;
    
    x.mdotOx(j) = Cd * Ai * sqrt(2 * rhoOx * (POx - x.Pc(i)));
%     x.mdotOx(j) = 0.0252;
    x.Gox(j) = x.mdotOx(j) / x.Ap(j);
    x.rdot(j) = 0.036 / rhoFuel * x.Gox(j)^0.8 * (x.mu(j) / L)^0.2 * (x.B(j))^0.23;
    x.mdotFuel(j) = x.rdot(j) * rhoFuel * x.Ab(j);
    x.OFRatio(j) = x.mdotOx(j) / x.mdotFuel(j);
    x.mdot(j) = x.mdotOx(j) + x.mdotFuel(j);

    x.Pc(j) = x.mdot(j) * x.cstar(j) / At;
    
    k = x.k(j);
    
    [x.Me(j)] = MachFinder(Ar, k);
    x.Pe(j) = x.Pc(j) * (1 + 1 / 2 * (k - 1) * x.Me(j)^2)^(k / (1 - k));
    x.Cf(j) = sqrt(2 * k^2 / (k - 1) * (2 / (k + 1))^((k + 1) / (k - 1)) * (1 - (x.Pe(j) / x.Pc(j))^((k - 1) / k))) + (x.Pe(j) - Pa) / x.Pc(j) * Ar;
    x.F(j) = x.Cf(j) * At * x.Pc(j);
    x.Isp(j) = x.F(j) / (x.mdot(j) * g0);
    
end

%%

figure(1); hold on; grid on;
plot(ts, x.OFRatio)
title('OF Ratio');

figure(2); hold on; grid on;
plot(ts, x.Pc)
title('Chamber Pressure')

figure(3); hold on; grid on;
plot(ts, [x.mdot, x.mdotOx, x.mdotFuel])
title('Mass Flow')

figure(4); hold on; grid on;
plot(ts, x.w)
title('Web Length')
