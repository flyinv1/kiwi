"""
    Compute performance metrics from hot fire data

    Metrics:
        impulse
        Isp
        F (avg)
        Pc (avg)
        Cf
        mdot_ox
        mdot_f
"""

import os
import pandas as pd
import numpy as np
from scipy import signal, stats
from matplotlib import pyplot as plt

class HotRunStruct:
    def __init__(self, fname, start, stop, dm = 0):
        self.fname = fname
        self.start = start
        self.stop = stop
        self.dm = dm

tests = [
    # RunStruct("119_00_29", 206, 426, 0.0045),
    HotRunStruct("120_00_30", 165, 1141, 0.0200),
    # RunStruct("94_21_38", 427, 837),
    # RunStruct("95_21_47", 420, 852)
]

test = HotRunStruct("120_00_30", 165, 1142, 0.0200)
# test = HotRunStruct("95_21_47", 420, 852, 0.0234)

# Headers used in csv files for reference
input_cols = [
    "chamberPressure",
    "upstreamPressure",
    "downstreamPressure",
    "thrust",
    "propellantMass",
    "massFlow",
    "throttlePosition",
    "igniterVoltage",
    "missionTime",
    "managerStateTime",
    "delta",
    ""
]

# Constants
rt = 0.002
At = np.pi * pow(rt, 2)

def psiToPa(psi):
    return psi * 6894.76

def butter(y, low, high, fs, order=5):
    nyq = 0.5 * fs
    sos = signal.butter(order, [low / nyq], btype='low', output='sos')
    return signal.sosfiltfilt(sos, y)

with open(os.path.abspath(f'./Data/Hot_Fire/run_data_{test.fname}.csv')) as file:
    # Load csv file into data frame 
    df = pd.read_csv(file, header=0, index_col=False, delimiter=',')
    dat = df.loc[test.start:test.stop, :]

    duration = (dat.iloc[-1, 8] - dat.iloc[0, 8]) / 1e6
    dt = np.average(dat.loc[:, 'delta']) / 1e6
    dx = duration / dat.shape[0]

    impulse = np.trapz(dat.loc[:, 'thrust'], dx=dx)
    
    dat['cf'] = dat.apply(lambda x : x['thrust'] / (psiToPa(x['chamberPressure']) * At), axis=1)

    f, pxx = signal.welch(dat['propellantMass'], fs=1 / dx)

    a, b, r, p, stdr = stats.linregress(dat['missionTime'] / 1e6, dat['propellantMass'])

    ox_mass = -a * duration
    of = ox_mass / test.dm
    isp = impulse / (ox_mass + test.dm) / 9.81
    cf = np.average(dat['cf'])
    cstar = isp / cf * 9.81
    pc_av = np.average(dat['chamberPressure'])

    # print(dat['chamberPressure'])

    print("OF: ", of)

    print("ISP", isp)

    print("cf", cf)

    print("cstar", cstar)

    print("pc", pc_av)

    print("ox", ox_mass)

    print("mdot", -a)

    # plt.plot(f, pxx)
    # plt.figure()
    plt.plot(dat['missionTime'] / 1e6, dat['propellantMass'])
    plt.plot(dat['missionTime'] / 1e6, butter(dat['propellantMass'], 1.5, 100, 1 / dx, order=5))
    plt.plot(dat['missionTime'] / 1e6, dat['missionTime'] / 1e6 * a + b)

    plt.figure()
    plt.plot(dat['missionTime'] / 1e6, dat['thrust'])

    plt.show()


