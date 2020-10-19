"""
    Temp script to help debug main_engine py outputs without rerunning
"""

import numpy as np
from matplotlib import pyplot as plt

data = np.load('temp/out_low.npy', allow_pickle=True)

idx = np.round(np.linspace(0, len(data[:,0]) - 1, 2000)).astype(int)

out = data[idx, :]

out = out[1:,:]

np.savetxt('temp/out_low.csv', out, delimiter=',')

plt.clf()
# plt.plot(data[:,0], data[:,-1])
plt.plot(out[:,0], out[:,2])
plt.xlabel('t')
plt.ylabel('Thrust (N)')
plt.title('Thrust versus Time')
plt.grid()
plt.show()