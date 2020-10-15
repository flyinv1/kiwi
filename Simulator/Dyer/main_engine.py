from cfp.mach import machFromAreaRatio, areaRatioFromMach
import numpy as np
from matplotlib import pyplot as plt
from timeit import timeit
from cea import hybridlookup as cea
from ballistics.lumpedhybrid import LumpedHybrid

table = cea.HybridCEALookup()
# table.flushCache()
table.open()
table.generate(pcmin=101325, pcmax=8.0e6, pcint=50000, ofmin=0.1, ofmax=30, ofint=0.1)
cea_lookup = lambda Pc, OF : table.get(pc=Pc, of=OF)

model = LumpedHybrid(rp=0.002, L=0.089, rt=0.002, mode='whitmore')

# Oxidizer feed initial conditions
rho_ox = 749 # density kg/m3
Pox = 5.178e6 # feed pressure (Pa)
Pci = 3.446e6 # chamber pressure (Pa)
OF = 4 # OF ratio
Cd = 0.5 # injector discharge coefficient
Ainj = np.pi * pow(0.00075, 2) # injector area (m2)

model.initialize(Pci, OF)

f_mdot_ox = lambda Pf, Pc : Cd * Ainj * np.sqrt(2 * rho_ox * (Pf - Pci))
mdot_ox = f_mdot_ox(Pox, Pci)

# Set initial conditions for integration
dt = 1/10000
tf = 10000

output = np.zeros(shape=(tf, 9))

for i in range(0, tf):
    Pc, Tc, OF = model.chamberProps()
    mdot_ox = f_mdot_ox(Pox, Pc)
    output[i] = [
        i * dt,
        model.x["w"],
        model.x["Pc"],
        model.x["Tc"],
        model.x["OF"],
        model.x["cstar"],
        model.x["mdot_ox"],
        model.x["mdot_f"],
        model.x["dPc"]
    ]
    model.step(cea_lookup, mdot_ox, rho_ox, dt)

model.logState()

# print(output[:,6])
print(output[:,8])

print(np.trapz(output[:,6], dx=dt))
print(np.trapz(output[:,7], dx=dt))
plt.clf()
plt.plot(output[:,0], output[:,1])
plt.xlabel('t')
plt.ylabel('w (m)')
plt.savefig('tempout_w.png')

plt.clf()
plt.plot(output[:,0], output[:,5])
plt.xlabel('t')
plt.ylabel('c* (m/s)')
plt.savefig('tempout_cstar.png')

plt.clf()
plt.plot(output[:,0], output[:,2])
plt.xlabel('t')
plt.ylabel('pc (Pa)')
plt.savefig('tempout_pc.png')
plt.show()
