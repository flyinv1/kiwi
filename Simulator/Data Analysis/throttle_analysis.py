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

class ThrottleKeyframe: 
    def __init__(self, i_0, i_f, deg):
        self.t0 = t0
        self.tf = tf
        self.deg = deg

class ThrottleRunStruct:
    def __init__(self, fname, keyframes):
        self.fname = fname
        self.keyframes = keyframes 

test = ThrottleRunStruct('41_01_25', [
    ThrottleKeyframe(),
    ThrottleKeyframe(),
    ThrottleKeyframe(),
    ThrottleKeyframe(),
])

rho_ox = 746
r_inj = 0.00075
A_inj = np.pi * r_inj ** 2

def psiToPa(psi):
    return psi * 6894.76

def lowbutter(y, low, high, fs, order=6):
    nyq = 0.5 * fs
    sos = signal.butter(order, [low / nyq], btype='low', output='sos')
    return signal.sosfiltfilt(sos, y)

with open(os.path.abspath(f'./Data/N2O_Cold_Flow/Injector/run_data_{test.fname}.csv')) as file:
    df = pd.read_csv(file, header=0, index_col=False, delimiter=',')

    for keyframe in test.keyframes:

        # dat = df.loc[test.start:test.stop, :]

        # duration = (dat.iloc[-1, 8] - dat.iloc[0, 8]) / 1e6
        # dt = np.average(dat.loc[:, 'delta']) / 1e6
        # dx = duration / dat.shape[0]

        # f, pxx = signal.welch(dat['propellantMass'], fs=1 / dx)
        # a, b, r, p, stdr = stats.linregress(dat['missionTime'] / 1e6, dat['propellantMass'])

        # plt.plot(dat['missionTime'] / 1e6, dat['propellantMass'])
        # plt.plot(dat['missionTime'] / 1e6, lowbutter(dat['propellantMass'], 1.5, 100, 1 / dx, order=5))
        # plt.plot(dat['missionTime'] / 1e6, dat['missionTime'] / 1e6 * a + b)

        # mdot_ox = -a
        # ox_mass = -a * duration
        # delta_p = np.average(psiToPa(dat['downstreamPressure'])) - 101325
        # spi_cd = mdot_ox / (A_inj * np.sqrt(2 * rho_ox * delta_p))

        # spi_avg += spi_cd
        # mdot_avg += mdot_ox

        # print(mdot_ox, ox_mass, delta_p, spi_cd)

        # # plt.plot(f, pxx)
        # plt.show()

