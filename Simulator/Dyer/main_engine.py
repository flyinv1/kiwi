from cfp import nozzle
import numpy as np
from matplotlib import pyplot as plt
from timeit import timeit
from cea import hybridlookup as cea
from ballistics.lumpedhybrid import LumpedHybrid

table = cea.HybridCEALookup()
# table.flushCache()
table.open()
table.generate(pcmin=101325, pcmax=6.8e6, pcint=25000, ofmin=1, ofmax=20, ofint=0.05)
cea_lookup = lambda Pc, OF : table.get(Pc, OF)

# Oxidizer feed initial conditions
rho_ox = 749 # density kg/m3
Pox = 5.178e6 # feed pressure (Pa)
Pci = 3.34e6 # chamber pressure (Pa)
OF = 4 # OF ratio
Cd = 0.4 # injector discharge coefficient
Ainj = np.pi * pow(0.00075, 2) # injector area (m2)
rp = 0.002
L = 0.089
rt = 0.002
rho_f = 1224
At = np.pi * pow(rt, 2)
Ar = 3.4
Ae = Ar * At

model = LumpedHybrid(rp=rp, L=L, rt=rt, rho_f=rho_f, mode='whitmore', Vpc=6e-6)
model.initialize(Pci, OF)

f_mdot_ox = lambda Pf, Pc : Cd * Ainj * np.sqrt(2 * rho_ox * (Pf - Pc))
mdot_ox = f_mdot_ox(Pox, Pci)

# Set initial conditions for integration
dt = 0.00001
tf = 100000

output = np.zeros(shape=(tf, 10))

for i in range(0, tf):
    Pc, Tc, OF = model.chamberProps()
    mdot_ox = f_mdot_ox(Pox, Pc)
    model.step(cea_lookup, mdot_ox, rho_ox, dt)

    pe = nozzle.pe_from_pc(model.x["Pc"], Ar, model.x["k"])
    f = nozzle.thrust(model.x["Pc"], pe, 101325, model.x["k"], At, Ae)

    output[i] = [
        i * dt,
        model.x["w"],
        model.x["Pc"],
        model.x["Tc"],
        model.x["OF"],
        model.x["cstar"],
        model.x["mdot_ox"],
        model.x["mdot_f"],
        model.x["dPc"],
        f
    ]
    
np.save('temp/out.npy', output, allow_pickle=True)

model.logState()

print("N2O (kg): ", np.trapz(output[:,6], dx=dt))
print("ABS (kg): ", np.trapz(output[:,7], dx=dt))

plt.clf()
plt.plot(output[:,0], output[:,1])
plt.xlabel('t')
plt.ylabel('w (m)')
plt.savefig('temp/tempout_w.png')

plt.clf()
plt.plot(output[:,0], output[:,5])
plt.xlabel('t')
plt.ylabel('c* (m/s)')
plt.savefig('temp/tempout_cstar.png')

plt.clf()
plt.plot(output[:,0], output[:,-1])
plt.xlabel('t')
plt.ylabel('F (N)')
plt.savefig('temp/tempout_thrust.png')

plt.clf()
plt.plot(output[:,0], output[:,2])
plt.xlabel('t')
plt.ylabel('pc (Pa)')
# plt.ylim(3e6, 5e6)
plt.xlim(0, 10)
plt.grid()
plt.savefig('temp/tempout_pc.png')
# plt.show()
