import numpy as np

class LumpedHybrid():

    def __init__(self, rp=0.002, L=0.15, nports=1, rt=0.005, rho_f=1224, cp_f=1655, hv=2.3e6, Ta=298.15, Vpc=3.2e-7, mode='whitmore'):
        """
            Instantiates a LumpedHybrid model\n

            Arguments:
                rp : port perimeter (m)
                L : grain length (L)
                nports : number of ports
                rho_f : fuel density (kg/m3)
                cp_f : fuel specific heat capacity (J/kg-K)
                hv : fuel heat of vaporization (J/kg)
                Ta : ambient temperature (K)
                At : throat area
        """

        # Engine geometry and fuel constants
        self.rp = rp
        self.L = L
        self.nports = nports
        self.rho_f = rho_f
        self.hv = hv
        self.Ta = Ta
        self.At = np.pi * pow(rt, 2)
        self.cp_f = cp_f
        self.R = 287.058 # Universal gas constant (J/kg-K)
        self.Vpc = Vpc

        if mode in ['whitmore', 'sutton']:
            self.mode = mode
        else:
            raise Exception('Invalid mode LPB Hybrid mode, options are ["whitmore", "sutton"]')

        # System state vector
        self.x = {
            "w": 0,
            "Ap": np.pi * pow(rp, 2),
            "Per": np.pi * 2 * rp,
            "Ab": np.pi * 2 * rp * L,
            "Vc": np.pi * pow(rp, 2) * L,
            "Tc": 0,
            "Pc": 0,
            "Gox": 0,
            "rdot": 0,
            "mdot_ox": 0,
            "mdot_f": 0,
            "OF": 0,
            "cstar": 0,
            "dPc": 0,
            "k": 0,
        }

    def initialize(self, Pc, OF):
        """
            Set initial chamber properties for integration

            Arguments: 
                Pc : initial chamber pressure (Pa)
                OF : initial oxidizer-fuel ratio
        """
        self.x["Pc"] = Pc
        self.x["dPc"] = 0
        self.x["OF"] = OF
        
    def step(self, cea_func, mdot_ox=0.05, rho_ox=0.7, dt=0.001):
        """
            Advance regression simulation by dt

            Arguments:
                cea_func : method that takes Pc, OF as args and returns a nparray [ pc, OF, Tc, k, MW, Pr, Cp, mu, cstar, isp ]
                mdot_ox  : oxidizer mass flow rate (kg/s)
                dt       : time step (s)

            Returns:
                Fill this out!
        """

        self.x["w"] = self.x["w"] + self.x["rdot"] * dt
        self.x["Ap"] = self.portArea()
        self.x["Per"] = self.portPerimeter()
        self.x["Ab"] = self.burnArea()

        """
            0 pc      :   chamber pressure\n
            1 of      :   mixture ratio (mass)\n
            2 Tc      :   chamber temperature\n
            3 k       :   specific heat ratio\n
            4 MW      :   molecular weight\n
            5 Pr      :   Prandtl number\n
            6 Cp      :   specific heat capacity\n
            7 mu      :   viscocity (must convert from poise to Pa-s!)\n
            8 cstar   :   characteristic velocity\n
            9 isp     :   isp (s)
        """

        cea = cea_func(self.x["Pc"], self.x["OF"])
        cea[7] /= 10 # Factor viscosity by 10 to convert to Ns/m2

        self.x["Tc"] = cea[2]
        self.x["k"] = cea[3]
        _, B = self.boundaryLayer(cea[2], cea[6])

        self.x["mdot_ox"] = mdot_ox # Oxidizer mass flow rate
        self.x["Gox"] = mdot_ox / self.x["Ap"] # Oxidizer mass flux

        # Regression rate correlation
        if self.mode == 'sutton':
            self.x["rdot"] = self.suttonRegression(self.x["Gox"], cea[7], B)
        elif self.mode == 'whitmore':
            self.x["rdot"] = self.whitmoreRegressionRate(self.x["Gox"], cea[7], cea[5], B)
            
        self.x["mdot_f"] = self.x["rdot"] * self.rho_f * self.x["Ab"] # Fuel mass flow
        self.x["OF"] = mdot_ox / self.x["mdot_f"] # OF ratio

        self.x["Vc"] = np.pi * pow(self.rp + self.x["w"], 2) * self.L
        # self.x["Pc"] = (self.x["mdot_f"] + mdot_ox) * cea[8] / (self.At) # Resultant chamber pressure
        self.x["cstar"] = cea[8]
        self.x["dPc"] = self.deltaPc(self.x["Ab"], self.x["rdot"], self.x["Vc"], self.x["Tc"], self.x["Pc"], cea[3], self.x["mdot_ox"])
        self.x["Pc"] = self.x["Pc"] + dt * self.x["dPc"]

    def portArea(self):
        return np.pi * pow(self.x["w"] + self.rp, 2)

    def portPerimeter(self):
        return np.pi * 2 * (self.x["w"] + self.rp)

    def burnArea(self):
        return self.portPerimeter() * self.L

    def suttonRegression(self, Gox, mu, B):
        return 0.036 / self.rho_f * pow(Gox, 0.8) * pow(mu / self.L, 0.2) * pow(B, 0.23)

    def whitmoreRegressionRate(self, Gox, mu, Pr, B):
        return 0.047 / (self.rho_f * pow(Pr, 0.153)) * pow(Gox, 0.8) * pow(mu / self.L, 0.2) * pow(B, 0.23)

    def deltaPc(self, Ab, rdot, Vc, Tc, Pc, k, mdot_ox):
        return Ab * rdot / Vc * (self.rho_f * self.R * Tc - Pc) - Pc * (self.At / Vc * np.sqrt(k * self.R * Tc * pow(2 / (k + 1), (k + 1) / (k - 1)))) + self.R * Tc / Vc * mdot_ox

    def boundaryLayer(self, Tc, Cp):
        dh = self.cp_f * (Tc - self.Ta)
        B = dh / self.hv
        return dh, B

    def chamberProps(self):
        return self.x["Pc"], self.x["Tc"], self.x["OF"]

    def logState(self):
        print("\nHybrid Combustor Chamber Conditions: ")
        print("Ap: ", self.x["Ap"], "m2")
        print("Ab: ", self.x["Ab"], "m2")
        print("Vc: ", self.x["Vc"], "m3")
        print("Pc: ", self.x["Pc"], "Pa")
        print("Tc: ", self.x["Tc"], "K")
        print("G: ", self.x["Gox"] / 10000, "kg/s-cm2")
        print("rdot: ", self.x["rdot"] * 100, "cm/s")
        print("mdot_ox: ", self.x["mdot_ox"], "kg/s")
        print("mdot_f: ", self.x["mdot_f"], "kg/s")
        print("OF: ", self.x["OF"])
        print("C*: ", self.x["cstar"], "m/s")

        
        