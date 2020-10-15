%%
% Geometry Inputs!!
L = 0.30; % Length of fuel grain, m
rp = 0.0065; % port radius, m
rc = 0.04; % chamber radius, m
rt = 0.004; % nozzle radius
rox = 0.0007; % injector radius, m

% Constants
Ru = 8.31447; % J/mol K
g0 = 9.81; % m/s^2
hv = 2.3e6; % heat of vaporization of ABS, J/kg
rhofuel = 1070; % kg/m^3
rhoN2O = 743.9; % kg/m^3
Cd = 0.4; % Assumed discharge coefficient for injector

% Preallocate vectors
t = linspace(0,20,1000)';
w = zeros(length(t),1); % web distance, m
Ap = zeros(length(t),1); % fuel port area, m^2
Ab = zeros(length(t),1); % Burn area, m^2
Gox = zeros(length(t),1); % oxidizer mass flux, kg/s m^2
rdot = zeros(length(t),1); % regression rate, m/s
Per = zeros(length(t),1); % fuel port perimeter, m
mdot_ox = zeros(length(t),1); % ox fuel rate, kg/s
mdot_fuel = zeros(length(t),1); % fuel mass flow rate, kg/s
OFrat = zeros(length(t),1); % O/F ratio
Tc = zeros(length(t),1); % chamber temperature, K
gam = zeros(length(t),1); % ratio of specific heats
MW = zeros(length(t),1); % molecular weight, kg/mol
P = zeros(length(t),1); % Prandtl number
Cp = zeros(length(t),1); % specific heat capacity, kJ/kg K
mu = zeros(length(t),1); % dynamic viscosity
cstar = zeros(length(t),1); % characteristic velocity, m/s
R = zeros(length(t),1); % gas constant
Pc = zeros(length(t),1); % chamber pressure, Pa
dPc = zeros(length(t),1); % chamber pressure change, Pa
dh = zeros(length(t),1); % enthalpy change from fuel to freestream
B = zeros(length(t),1); % blowing factor
Pr = zeros(length(t),1); % Prandtl number
mdot = zeros(length(t),1); % total mass flow, kg/s
F = zeros(length(t),1); % Force, N
Isp = zeros(length(t),1); % Specific Impulse, s
Pe = zeros(length(t),1); % Exit Pressure, Pa
GAM = zeros(length(t),1); 
Me = zeros(length(t),1); % Exit Mach number

% Pressure Inputs, based on pressure drop across injector
Pox = 800.*6894.76; % converts psi to Pa, ox tank pressure
Pc(1) = 500.*6894.76; % converts psi to Pa,  initial Pc
Pa = 101325; % atmospheric pressure, Pa

% Calculated Initial Conditions
At = pi.*rt.^2;
Arat = 5;
Ap(1) = pi.*rp.^2;
Aox = pi.*rox.^2; % effective injector area, m^3
OFrat(1) = 5; % initial OFrat to get things started

[Tc(1), gam(1), MW(1), Pr(1), Cp(1), mu(1), cstar(1)] = HybridCEA500(OFrat(1));

mdot_ox(1) = Aox.*Cd.*sqrt(2.*rhoN2O.*(Pox-Pc(1))); 
Gox(1) = mdot_ox(1)./Ap(1); % oxidizer mass flux, kg/m^2
dh(1) = Cp(1).*(Tc(1) - 298).*1000; % J/kg

B(1) = dh(1)./hv;
rdot(1) = (0.036./rhofuel).*((mu(1)./L).^0.2).*((Gox(1).^0.8).*(B(1).^0.23)...
    ./Pr(1).^0.15);
Per(1) = 2.*pi.*rp;
Ab(1) = Per(1).*L;
mdot_fuel(1) = Ab(1).*rdot(1).*rhofuel;
OFrat(1) = mdot_ox(1)./mdot_fuel(1);
R(1) = Ru./MW(1);
mdot(1) = mdot_ox(1) + mdot_fuel(1);
GAM(1) = sqrt(gam(1)./(((gam(1)+1)./2).^((gam(1)+1)./(gam(1)-1))));
[Me(1)] = MachFinder(Arat, gam(1));
Pe(1) = Pc(1)./((1+0.5.*(gam(1)-1).*Me(1).^2).^(gam(1)./(gam(1)-1)));
F(1) = Pc(1).*(At).*(GAM(1).*sqrt(((2.*gam(1))./(gam(1)-1)).*(1-(Pe(1)...
    ./Pc(1)).^((gam(1)-1)./gam(1)))) + ((Pe(1)./Pc(1))-(Pa./Pc(1))).*Arat);
Isp(1) = F(1)./((mdot_ox(1)+mdot_fuel(1)).*g0);
%% Iterate
dt = t(2)-t(1);
for i = 1:length(t)-1
    w(i+1) = w(i) + rdot(i).*dt;
    Ap(i+1) = (pi.*(rp + w(i+1)).^2);
    Per(i+1) = 2.*pi.*(rp + w(i+1));
    Ab(i+1) = Per(i+1).*L;
    Pc(i+1) = ((mdot_ox(i)+mdot_fuel(i)).*cstar(i))./At;
    mdot_ox(i+1) = Aox.*Cd.*sqrt(2.*rhoN2O.*(Pox-Pc(i+1)));
    Gox(i+1) = mdot_ox(i+1)./Ap(i+1);
    [Tc(i+1), gam(i+1), MW(i+1), Pr(i+1), Cp(i+1), mu(i+1), cstar(i+1)]...
        = HybridCEA500(OFrat(i));
    dh(i+1) = Cp(i+1).*(Tc(i+1) - 298).*1000; % kJ/kg
    B(i+1) = dh(i+1)./hv;
    rdot(i+1) = (0.036./rhofuel).*((mu(i+1)./L).^0.2).*((Gox(i+1).^0.8)...
        .*(B(i+1).^0.23)./Pr(i+1).^0.15);
    mdot_fuel(i+1) = Ab(i+1).*rdot(i+1).*rhofuel;
    OFrat(i+1) = mdot_ox(i+1)./mdot_fuel(i+1);
    R(i+1) = Ru./MW(i+1);
    mdot(i) = mdot_fuel(i+1) + mdot_ox(i+1);
    [Me(i+1)] = MachFinder(Arat, gam(i+1));
    Pe(i+1) = Pc(i+1)./((1+0.5.*(gam(i+1)-1).*Me(i+1).^2).^(gam(i+1)./...
        (gam(i+1)-1)));
    GAM(i+1) = sqrt(gam(i+1)./(((gam(i+1)+1)./2).^((gam(i+1)+1)./(gam(i+1)-1))));
    F(i+1) = Pc(i+1).*(At).*(GAM(i+1).*sqrt(((2.*gam(i+1))./(gam(i+1)-1))...
        .*(1-(Pe(i+1)./Pc(i+1)).^((gam(i+1)-1)./gam(i+1)))) + ((Pe(i+1)./...
        Pc(i+1))-(Pa./Pc(i+1))).*Arat);
    Isp(i+1) = F(i+1)./((mdot_ox(i+1)+mdot_fuel(i+1)).*g0);
end

% Propellant calculations
mdot_ox_avg = sum(mdot_ox)./length(t); % average oxidizer mass flow rate, kg/s
mdot_ox_total = sum(mdot_ox.*dt); % total mass of oxidizer, kg
Wox = 0.224809.*mdot_ox_total.*g0; % weight of propellant, lbf
Vox = (mdot_ox_total./rhoN2O).*1000; % Ox tank volume, L
Wabs = 0.224809.*(pi.*L.*rc.^2 - pi.*L.*rp.^2).*rhofuel; % weight of ABS, lbf

% Rocket Performance
Favg = sum(F)./length(t); % Average thrust, lbf
Ispavg = sum(Isp)./length(t); % Average Isp, s

figure(1)
plot(t,Pc./6894.76,'k')
xlabel('Time [s]')
ylabel('Chamber Pressure [psi]')
title('Chamber pressure as a function of time')

figure(2)
plot(t,OFrat,'k')
xlabel('Time [s]')
ylabel('O/F')
title('O/F as a function of time')

figure(3)
plot(t,Tc, 'k')
xlabel('Time [s]')
ylabel('Chamber Temperature [K]')
title('Chamber temperature as a function of time')

figure(4)
plot(t,rdot.*1000, 'k')
xlabel('Time [s]')
ylabel('Regression rate [mm/s]')
title('Regression rate as a function of time')

figure(5)
plot(t,F.*0.224809, 'k')
xlabel('Time [s]')
ylabel('Thrust [lbf]')
title('Thrust as a function of time')

figure(6)
plot(t,Pe, 'k')
xlabel('Time [s]')
ylabel('Exit Pressure [Pa]')
yline(Pa, 'r--');
title('Exit pressure as a function of time')
legend('Pe(t)','Pa')

figure(7)
plot(t,mdot_ox, 'k')
xlabel('Time [s]')
ylabel('Oxidizer mass flow rate [kg/s]')
yline(mdot_ox_avg, 'r--');
title('Oxidizer mass flow as a function of time')
legend('ox mdot(t)', 'avg ox mdot')
