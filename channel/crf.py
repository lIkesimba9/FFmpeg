#!/usr/bin/python

import json
import matplotlib.pyplot as plt
#import collecttions
data = ''
with open('crf.txt','r') as f:
    data = f.read()
data = data[1:-1]
d = {}
data = data.split(',')
for i in data:
    tmp = i.split(':')
    d[float(tmp[0])] = int(tmp[1])
n_d = {}
for key in sorted(d.keys()):
	n_d[key] = d[key]
	
plt.plot(n_d.keys(),n_d.values())
plt.show()
