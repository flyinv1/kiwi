"""
    Compute performance metrics from hot fire data

    Metrics:
        mdot_ox (linear regression)
        cd_inj
"""

import os
import pandas as pd
import numpy as np
from scipy import signal, stats
from matplotlib import pyplot as plt

class InjectorRunStruct:
    def __init__(self, fname, start, stop):
        self.fname = fname
        self.start = start
        self.stop = stop

tests = [
    InjectorRunStruct('27_23_46', 247, 1165),
    InjectorRunStruct('28_23_51', 257, 1196),
    InjectorRunStruct('29_00_04', 251, 1190),
    InjectorRunStruct('30_00_16', 250, 1195)
]

rho_ox = 746
r_inj = 0.00075
A_inj = np.pi * r_inj ** 2


def psiToPa(psi):
    return psi * 6894.76

def lowbutter(y, low, high, fs, order=6):
    nyq = 0.5 * fs
    sos = signal.butter(order, [low / nyq], btype='low', output='sos')
    return signal.sosfiltfilt(sos, y)

spi_avg = 0
mdot_avg = 0

for test in tests:
    with open(os.path.abspath(f'./Data/N2O_Cold_Flow/Injector/run_data_{test.fname}.csv')) as file:
        df = pd.read_csv(file, header=0, index_col=False, delimiter=',')
        dat = df.loc[test.start:test.stop, :]

        duration = (dat.iloc[-1, 8] - dat.iloc[0, 8]) / 1e6
        dt = np.average(dat.loc[:, 'delta']) / 1e6
        dx = duration / dat.shape[0]

        f, pxx = signal.welch(dat['propellantMass'], fs=1 / dx)
        a, b, r, p, stdr = stats.linregress(dat['missionTime'] / 1e6, dat['propellantMass'])

        plt.plot(dat['missionTime'] / 1e6, dat['propellantMass'])
        plt.plot(dat['missionTime'] / 1e6, lowbutter(dat['propellantMass'], 1.5, 100, 1 / dx, order=5))
        plt.plot(dat['missionTime'] / 1e6, dat['missionTime'] / 1e6 * a + b)

        mdot_ox = -a
        ox_mass = -a * duration
        delta_p = np.average(psiToPa(dat['downstreamPressure'])) - 101325
        spi_cd = mdot_ox / (A_inj * np.sqrt(2 * rho_ox * delta_p))

        spi_avg += spi_cd
        mdot_avg += mdot_ox

        print(mdot_ox, ox_mass, delta_p, spi_cd)

        # plt.plot(f, pxx)
        plt.show()

print(spi_avg / 4)
print(mdot_avg / 4)

