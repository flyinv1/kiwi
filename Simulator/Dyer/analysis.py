"""
    Temp script to help debug main_engine py outputs without rerunning
"""

import numpy as np
from matplotlib import pyplot as plt

data = np.load('temp/out.npy', allow_pickle=True)

plt.clf()
plt.plot(data[:,0], data[:,-1])
plt.xlabel('t')
plt.ylabel('Thrust (N)')
plt.title('Thrust versus Time')
plt.grid()
plt.show()