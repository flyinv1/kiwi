from ballistics.lumpedhybrid import LumpedHybrid
from cea.hybridlookup import HybridCEALookup
from matplotlib import pyplot as plt
import numpy as np

lookup = HybridCEALookup()
lookup.open()
lookup.generate(pcmin=101325, pcmax=6.8e6, pcint=50000, ofmin=0.1, ofmax=30, ofint=0.1)

stations = [ 102000, 3.447e+6, 4.826e+6, 5.516e+6]

for station in stations:
    data = lookup.getPc(station)
    plt.plot(data[:,1], data[:, -2])

plt.grid()
plt.xlim(1, 12)
plt.ylim(1400, 1650)
plt.savefig('tempout_cea_cstar.png')
    
plt.clf()
for station in stations:
    data = lookup.getPc(station)
    plt.plot(data[:,1], data[:, 2])

plt.grid()
plt.xlim(1, 12)
plt.savefig('tempout_cea_tc.png')

plt.clf()
for station in stations:
    data = lookup.getPc(station)
    plt.plot(data[:,1], data[:, 7])

plt.grid()
plt.xlim(1, 12)
plt.savefig('tempout_cea_visc.png')

plt.clf()
for station in stations:
    data = lookup.getPc(station)
    plt.plot(data[:,1], data[:, 3])

plt.grid()
plt.xlim(1, 12)
plt.savefig('tempout_cea_k.png')

plt.clf()
for station in stations:
    data = lookup.getPc(station)
    plt.plot(data[:,1], data[:, 5])

plt.grid()
plt.xlim(1, 12)
plt.savefig('tempout_cea_pr.png')