from rocketcea.cea_obj_w_units import CEA_Obj
from rocketcea.cea_obj import add_new_fuel, add_new_propellant
import numpy as np
import json
import os
from math import ceil, floor

class HybridCEALookup():
    """
        Class to generate and manage N2O-ABS Hybrid engine CEA data for Pc and o/f ratios
    """
    __depth = 10

    def __init__(self):
        self.datapathname = './.lookup.npy'
        self.configpathname = './.lookupconfig.json'
        self.data = None
        self.config = None
        self.__loaded = False

        # This str provides molecular structure, wt% of fuel, heat of formation, and density information to underlying CEA code
        fuel_str = """
        fuel ABS(S) C 3.85 H 4.85 N 0.43 wt%=100.0
        h,kJ=62.63 t(k)=298.15 rho,kg=1224
        """

        add_new_fuel('ABS', fuel_str)
        self.cea = CEA_Obj(
            oxName='N2O', 
            fuelName='ABS', 
            cstar_units='m/s', 
            pressure_units='Pa', 
            temperature_units='K', 
            sonic_velocity_units='m/s', 
            enthalpy_units='kJ/kg', 
            density_units='kg/m^3', 
            specific_heat_units='J/kg-K', 
            viscosity_units='poise',
            thermal_cond_units='W/cm-degC',
            make_debug_prints=True)

    def generate(self, pcmin=1, pcmax=1000, ofmin=1, ofmax=40, pcint=1, ofint=1):
        """
            Generate NxMx8 array of chamber thermodynamic and transport properties from min to max (inclusive).\n
            Use CEA to produce the following data at each pc + ofratio combination (8 values)\n
            pc      :   chamber pressure (Pa)\n
            of      :   mixture ratio\n
            Tc      :   chamber temperature (K)\n
            k       :   specific heat ratio\n
            MW      :   molecular weight (g / mol)\n
            Pr      :   Prandtl number\n
            Cp      :   specific heat capacity (kJ/kg-K)\n
            mu      :   viscocity (poise)\n
            cstar   :   characteristic velocity (m/s)\n
            isp     :   isp (s)
        """

        print('Using N2O, ABS, with SI units')

        if not self.uses(pcmin=pcmin, pcmax=pcmax, pcint=pcint, ofmin=ofmin, ofmax=ofmax, ofint=ofint):
            try:
                # Construct NxMx8 numpy array to hold all data
                xdim, ydim = self.__getDim(pcmin, pcmax, pcint, ofmin, ofmax, ofint)
                data = np.zeros(shape=(xdim, ydim, HybridCEALookup.__depth), dtype=float)

                print(f'Generating lookup table, {xdim}x{ydim}x{HybridCEALookup.__depth}')

                # Enumerate over pressure and ofratio ranges and compute CEA results
                for (pc_i, pc) in enumerate(np.linspace(pcmin, pcmax, xdim)):
                    for (of_i, of) in enumerate(np.linspace(ofmin, ofmax, ydim)):
                        (cp, mu, _, pr) = self.cea.get_Chamber_Transport(pc, of)
                        (mw, k) = self.cea.get_Chamber_MolWt_gamma(pc, of)
                        (isp, cstar, tc) = self.cea.get_IvacCstrTc(pc, of)
                        data[pc_i, of_i, :] = np.array([ pc, of, tc, k, mw, pr, cp, mu, cstar, isp ], dtype=float)

                # assign data to obj and save to cache
                self.data = data
                try:
                    config = {
                        "pcmin": pcmin,
                        "pcmax": pcmax,
                        "pcint": pcint,
                        "ofmin": ofmin,
                        "ofmax": ofmax,
                        "ofint": ofint,
                        "pcnum": xdim,
                        "ofnum": ydim,
                        "depth": HybridCEALookup.__depth
                    }
                    self.config = config
                    with open(os.path.abspath(self.datapathname), 'wb') as f:
                        np.save(f, data, allow_pickle=True)
                    with open(os.path.abspath(self.configpathname), 'w') as f:
                        json.dump(config, f)
                    print(f'Lookup table cached, pcrange: {pcmin}-{pcmax}, ofrange: {ofmin}-{ofmax}')
                except IOError as ioerror:
                    print(ioerror)
                    print("uh oh")
            except Exception as exception:
                raise exception
        else:
            print(f'Using lookup table from cached:\npcrange: {pcmin}-{pcmax}\nofrange: {ofmin}-{ofmax}')

    def open(self):
        """
            Open cached lookup table
        """
        try:
            with open(os.path.abspath(self.datapathname), 'rb') as f:
                self.data = np.load(f, allow_pickle=True)
            with open(os.path.abspath(self.configpathname), 'r') as f:
                self.config = json.load(f)
            self.__loaded = True
            return True
        except Exception:
            # Catch all and return false
            return False

    def flushCache(self):
        """
        """
        try:
            with open(os.path.abspath(self.datapathname), 'wb') as f:
                np.save(f, [])
            with open(os.path.abspath(self.configpathname), 'w') as f:
                json.dump({}, f)
        except Exception as exception:
            print(exception)

    def get(self, pc, of):
        """
            Retrieve CEA outputs at any chamber pressure
        """
        if pc < self.config["pcmin"] or pc > self.config["pcmax"]:
            print(f'Pc: {pc}, Pcmin: {self.config["pcmin"]}, Pcmax: {self.config["pcmax"]}')
            raise Exception("Chamber pressure out of bounds")
        elif of < self.config["ofmin"] or of > self.config["ofmax"]:
            print(f'OF: {of}, OFmin: {self.config["ofmin"]}, OFmax: {self.config["ofmax"]}')
            raise Exception("OF ratio out of bounds")

        # Convert to chamber pressure and OF ratio floating point index
        pc_i = (pc - self.config["pcmin"]) / (self.config["pcmax"] - self.config["pcmin"]) * (self.config["pcnum"] - 1)
        of_i = (of - self.config["ofmin"]) / (self.config["ofmax"] - self.config["ofmin"]) * (self.config["ofnum"] - 1)

        # Get chamber pressure lower and upper bounds (1d arr)
        pc_li = floor(pc_i)
        pc_hi = ceil(pc_i)

        # OF ratio lower and upper bounds (1d arr)
        of_li = floor(of_i)
        of_hi = ceil(of_i)

        # Get distance of chamber pressure and OF ratio from closest lower index
        pc_x = (pc_i - pc_li) # / self.config["pcint"]
        of_x = (of_i - of_li) # / self.config["ofint"]

        # Interpolate between pressure data
        pc_lerp_of_l = self.data[pc_li, of_li, :] + pc_x * (self.data[pc_hi, of_li, :] - self.data[pc_li, of_li, :])
        pc_lerp_of_h = self.data[pc_li, of_hi, :] + pc_x * (self.data[pc_hi, of_hi, :] - self.data[pc_li, of_hi, :])

        # Interpolate between OF ratio data and return result
        return pc_lerp_of_l + of_x * (pc_lerp_of_h - pc_lerp_of_l)

    def getPc(self, pc):
        """
            Return a row vector of OF Ratio results at a provided chamber pressure
        """        
        if pc < self.config["pcmin"] or pc > self.config["pcmax"]:
            raise Exception("Chamber pressure out of bounds")

        pc_i = (pc - self.config["pcmin"]) / (self.config["pcmax"] - self.config["pcmin"]) * (self.config["pcnum"] - 1)

        # Get chamber pressure lower and upper bounds (1d arr)
        pc_li = floor(pc_i)
        pc_hi = ceil(pc_i)
        pc_x = (pc_i - pc_li) # / self.config["pcint"]

        return self.data[pc_li, :, :] + pc_x * (self.data[pc_hi, : , :] - self.data[pc_li, :, :])
    
    def uses(self, pcmin=200, pcmax=800, pcint=10, ofmin=1, ofmax=30, ofint=1):
        """
            Check the cached lookup table configuration against specified options\n
        """
        if self.__loaded:
            xdim, ydim = self.__getDim(pcmin, pcmax, pcint, ofmin, ofmax, ofint)
            useConfig = {
                "pcmin": pcmin,
                "pcmax": pcmax,
                "pcint": pcint,
                "ofmin": ofmin,
                "ofmax": ofmax,
                "ofint": ofint,
                "pcnum": xdim,
                "ofnum": ydim,
                "depth": HybridCEALookup.__depth
            }
            return useConfig == self.config
        return False

    def getConfig(self):
        """
            Get current lookup table configuration
        """
        if self.__loaded:
            return self.config
        else:
            return None

    def __getDim(self, pcmin, pcmax, pcint, ofmin, ofmax, ofint):
        xdim = ceil((pcmax - pcmin) / pcint) + 1
        ydim = ceil((ofmax - ofmin) / ofint) + 1
        return xdim, ydim
