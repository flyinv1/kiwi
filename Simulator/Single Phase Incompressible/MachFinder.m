function [Me] = MachFinder(Arat, gam)
    % Author: Tom Moccia, 2020
    % This function uses Newton's algorithm to find exit mach number of a
    % supersonic nozzle, assuming isentropic, compressible flow.
    % Inputs:
    %   Arat: Ae/At
    %   gam: ratio of specific heats
    % Output:
    %   Me: exit Mach number
    max_it = 500;
    M = zeros(length(max_it),1);
    M(1) = 3;
    es = 1e-6;
    for i = 1:max_it
        f(i) = Arat.^2 - (1./M(i).^2).*((2+(gam-1).*M(i).^2)./(gam+1)).^...
            ((gam+1)./(gam-1));
        fprime(i) = (-2.*(2+(gam-1).*M(i).^2).^(2./(gam-1)))./(M(i).*(gam+1).^...
            (2./(gam-1)))+(2.*(2+(gam-1).*M(i).^2).^((gam+1)./(gam-1)))./...
            ((M(i).^3).*(gam+1).^((gam+1)./(gam-1)));
        M(i+1) = M(i) - f(i)./fprime(i);
        ea = (M(i+1)-M(i))./M(i+1);
        if abs(ea)<= es, break,end
    end
    Me = M(end);