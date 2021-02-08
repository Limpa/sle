import matplotlib.pyplot as plt 
import sys

data = list(map(float, open(sys.argv[1], "r").readlines()))

plt.scatter(list(range(0, len(data))), data, s=0.5)
plt.show()
