#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt
import json
import Queue
import sys
data = ""
with open(sys.argv[1]) as fp:
	data = fp.read()
l = []
l = data.split("\n")
l = l[:-1]
tr = {}
t = []
time = []
count = 0
sec = []
nsec = []
for i in l:
	d = json.loads(i)
	sec.append( d['time_sec'] )
	nsec.append( d['time_nano'] )
	t.append(d['transit'])
sec_m = min(sec)
new_sec = []
for i in range(0,len(sec)):
	new_sec.append( sec[i] - sec_m + float( nsec[i]/ float(pow(10,9) ) ) ) 
sec = new_sec
win_size = 100 
q = [ t[i] for i in range(win_size)]
s_n = sum(q) / win_size
q_mm = [ t[i] for i in range(100)]
min_s = min(q_mm)
max_s = max(q_mm)
print(min_s, max_s,max_s - min_s)
sg = []
count = 0
print("sum {}".format(sum(q)))
for i in range(0,len(t)):
	q.pop(0)
	q.append(t[i])
	sg.append((sum(q) / win_size) - s_n)
hight = 0
tim = 0
for i in range(1,len(sg)-1):
	if ( ( ( t[i] - s_n )  ) >  (max_s - min_s) ):
		print(t[i],min_s,t[i]-min_s)
		tim = sec[i]
		hight = i
		break
dT = []
inter = 100
#print(sec[8000],sec[8100])
one_q,fl_1,zn_1 = 0,0,0
last_q,fl_2,zn_2 = 0,0,0
for i in range(hight-100,len(sg)-inter):
	s = sum([ sg[i + j+1] - sg[i + j] for j in range(0,inter)])
	if (fl_1 == 0):
		one_q = i
		fl_1 = 1
		print("sum y_1={}".format(s))
		zn_1 = sg[i] 
	if ( fl_2 == 0 and ( i == one_q + 100)):
		last_1 = i
		fl_2 = 1
		print("sum y_2={}".format(s))
		zn_2 = sg[i]
	dT.append( (s) / ( sec[i+inter] - sec[i] ) )
print("time={}".format(sec[i+inter] - sec[i] ))
print("K={}".format( (zn_2 -zn_1) / (sec[last_1] - sec[one_q] )))
color = 'tab:red'
fig,ax = plt.subplots()
ax.set_xlabel("time")
ax.set_ylabel("Proizvodnay",color = color)
ax.plot(sec[hight-100:-inter],dT,color = color)
ax.plot(sec[one_q],zn_1,'o',color='black')
ax.plot(sec[one_q + 100],zn_2,'o',color='black')
ax.tick_params(axis='y',labelcolor = color)
color = "tab:blue"
ax2 = ax.twinx()
ax2.set_ylabel('transit(time)', color = color)
ax2.plot(sec,sg,color = color,linestyle="-.")
ax2.plot(tim,sg[hight],'o',color='black')
ax2.tick_params(axis='transit(time)' ,labelcolor = color)
fig.tight_layout()
plt.show()
