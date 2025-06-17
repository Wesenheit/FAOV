import numpy as np
import matplotlib.pyplot as plt
from FAOV import Period, Power

name_source = "OGLE-BLG-RRLYR-00445.dat"
t = np.loadtxt(name_source, usecols=[0])
mag = np.loadtxt(name_source, usecols=[1])
per = Period(t, mag, 10, -1.0)
freq, power = Power(t, mag, 10, -1.0)
print("freq arr: ", freq)
print("power arr: ", power)
print("period found: ", per)
phase = np.modf((t - t[0]) / per)[0]
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6))
ax1.scatter(phase, mag, color="black", marker=".")
ax2.plot(freq, power, color="black")
ax1.set_xlabel("Time [d]")
ax1.set_ylabel("Magnitude")
ax2.set_xlabel("Frequency [1/d]")
ax2.set_ylabel("Power")
ax1.invert_yaxis()
fig.savefig("{}.png".format(name_source.split(".")[0]))
