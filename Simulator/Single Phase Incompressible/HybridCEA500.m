function [Tc, gam, MW, Pr, Cp, mu, cstar] = HybridCEA500(OFrat)
    % Author: Tom Moccia, 2020
    % This function returns the calculated Tc, gamma, MW, Pr, Cp, and mu from the CEA
    % results for the combustion of ABS plastic with N2O as an oxidizer at 500
    % psi. This function interpolates the results for a given O/F from 1 to 15.
    % It starts by rounding the input O/F down to the nearest integer and then
    % interpolates assuming a linear change in parameters between whole number
    % integer values of O/F.
    % Inputs
    %   OFrat: Number from 1 to 15
    % Outputs
    %   Tc: chamber temperature [K]
    %   gamma: ratio of specific heats
    %   MW: molecular weight of product gas [kg/mol]
    %   Pr: Prandtl number, dimensionless
    %   Cp: specific heat capacity, kJ/kg K
    %   mu: dynamic viscosity, Pa s
    %   cstar

    % CEA results for fixed OF ratio (integer ratios 1:15)
    Tc = [1392.86; 1602.99; 1788.51; 2322.27; 2689.97; 2911.78; 2970.19;  2925.82;...
        2835.97; 2725.06; 2606.01; 2487.35; 2374.24; 2269.07;  2172.45; 2084.08;...
        2003.31; 1929.37; 1861.51; 1799.08; 1741.46; 1688.14; 1638.65; 1592.61;...
        1549.67; 1509.52; 1471.91; 1436.60; 1403.39; 1372.08];   
    gam = [1.1917; 1.2569; 1.3047; 1.2671; 1.2326; 1.1854; 1.1534; 1.1496;...
        1.1583; 1.1724; 1.1880; 1.2023; 1.2142; 1.2240; 1.2320; 1.2388;...
        1.2446; 1.2498; 1.2545; 1.2587; 1.2626; 1.2663; 1.2696; 1.2728;...
        1.2758; 1.2786; 1.2812; 1.2837; 1.2861; 1.2884];
    MW = [15.212; 19.049; 22.040; 24.449; 26.360; 27.799; 28.730; 29.276;...
        29.591; 29.760; 29.837; 29.861; 29.858; 29.843;  29.822; 29.799;...
        29.777; 29.756; 29.737; 29.719; 29.702; 29.686; 29.672; 29.659;...
        29.647; 29.635; 29.625; 29.615; 29.606; 29.597]./1000;
    Pr = [0.5792; 0.5613; 0.5731; 0.5939; 0.5748; 0.5854; 0.6078; 0.6169;...
        0.6315; 0.7224; 0.6747; 0.6943; 0.7096; 0.7210; 0.7294; 0.7356;...
        0.7403; 0.7440; 0.7470; 0.7494; 0.7514; 0.7531; 0.7546; 0.7558;...
        0.7568; 0.7577; 0.7585; 0.7592; 0.7597; 0.7602];
    Cp = [3.3453; 2.0348; 1.6375; 1.6263; 1.7429; 2.2533; 3.0504; 3.1020;...
        2.6861; 2.2621; 1.9566; 1.7598; 1.6356; 1.5549; 1.4995; 1.4590;...
        1.4277; 1.4024; 1.3812; 1.2587; 1.3469; 1.3327; 1.3198; 1.3082;...
        1.2975; 1.2877; 1.2787; 1.2703; 1.2624; 1.2550];
    mu = [0.49467; 0.55577; 0.60287; 0.74269; 0.84191; 0.90416; 0.92457; 0.91846;...
        0.90037; 0.87656; 0.85024; 0.82352; 0.79767; 0.77332; 0.75068; 0.72973;...
        0.71037; 0.69246; 0.67586; 0.66043; 0.64605; 0.63263; 0.62006; 0.60827;...
        0.59718; 0.58673; 0.57686; 0.56753; 0.55868; 0.55029]./10000;
    cstar = [1103.8; 1179.5; 1229.0; 1342.4; 1403.6; 1440.1; 1448.9; 1427.1; ...
        1392.4; 1353.8; 1315.3; 1278.9; 1245.4; 1214.6; 1186.3; 1160.3; 1136.2;...
        1113.9; 1093.1; 1073.7; 1055.5; 1038.5; 1022.4; 1007.3; 993.0; 979.5; ...
        966.7; 954.6; 943.0; 932.0];
        
    OF1 = floor(real(OFrat));
    OF2 = OF1 + 1;

    Tcx1 = Tc(OF1);
    gamx1 = gam(OF1);
    MWx1 = MW(OF1);
    Prx1 = Pr(OF1);
    Cpx1 = Cp(OF1);
    mux1 = mu(OF1);
    cstar1 = cstar(OF1);
    Tcx2 = Tc(OF2);
    gamx2 = gam(OF2);
    MWx2 = MW(OF2);
    Prx2 = Pr(OF2);
    Cpx2 = Cp(OF2);
    mux2 = mu(OF2);
    cstar2 = cstar(OF2);
    
    OF2 = OF1 + 1;

    % Interpolate and return results
    Tc = (OFrat - OF1).*((Tcx2-Tcx1)./(OF2 - OF1)) + Tcx1;
    gam = (OFrat - OF1).*((gamx2-gamx1)./(OF2 - OF1)) + gamx1;
    MW = (OFrat - OF1).*((MWx2-MWx1)./(OF2 - OF1)) + MWx1;
    Pr = (OFrat - OF1).*((Prx2-Prx1)./(OF2 - OF1)) + Prx1;
    Cp = (OFrat - OF1).*((Cpx2-Cpx1)./(OF2 - OF1)) + Cpx1;
    mu = (OFrat - OF1).*((mux2-mux1)./(OF2 - OF1)) + mux1;
    cstar = (OFrat - OF1).*((cstar2-cstar1)./(OF2 - OF1)) + cstar1;
    
end