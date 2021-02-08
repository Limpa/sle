import matplotlib.pyplot as plt 
import sys

xs = []
ys = []
for el in list(map(lambda x: x.split(","), open(sys.argv[1], "r").readlines())):
    xs.append(float(el[0][1:]))
    ys.append(float(el[1][:-2]))

plt.axis("equal")
plt.scatter(xs, ys, s=0.5)
plt.show()
