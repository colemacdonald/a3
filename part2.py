#part2.py
import subprocess
import re
import matplotlib.pyplot as plt
import numpy as np

ldlen = ["0", "5", "10", "15", "20", "25"]
lqlen = ["50", "100", "250", "500"]

'''
data = [ 	
	wait 	: [
						50 	: [
							0	: [],
							5 	: [],
							...
							25 	: [],
						],
						100 : [],
						250 : [],
						500 : []
			],
 			ta		: [],
 			]
 ]
'''
data = [[	[0.0,0.0,0.0,0.0,0.0,0.0],
			[0.0,0.0,0.0,0.0,0.0,0.0],
			[0.0,0.0,0.0,0.0,0.0,0.0],
			[0.0,0.0,0.0,0.0,0.0,0.0]
		],
		[ 	[0.0,0.0,0.0,0.0,0.0,0.0],
			[0.0,0.0,0.0,0.0,0.0,0.0],
			[0.0,0.0,0.0,0.0,0.0,0.0],
			[0.0,0.0,0.0,0.0,0.0,0.0]
		]	
	]

regex = re.compile(".*EXIT w=(.*) ta=(.*)");

#for each combination of qlen and dlen:
#	execute rrsim
#	for each line in stdout:
#		if is EXIT line:
#			total_wait += wait
#			total_ta   += ta
#	save stats to data
#
#graph data

for qind, qlen in enumerate(lqlen):
	for dind, dlen in enumerate(ldlen):
		print("q = " + qlen + " d = " + dlen)
		simgen = subprocess.Popen(["./simgen", "1000", "2949"], stdout=subprocess.PIPE)
		rrsim = subprocess.Popen(["./rrsim", "--quantum", qlen, "--dispatch", dlen], stdin=simgen.stdout, stdout=subprocess.PIPE)
		count = 0.0
		ta = 0.0
		wait = 0.0
		for line in rrsim.stdout:
			m = regex.search(line)
			if (m != None):
				wait += float(m.group(1))
				ta += float(m.group(2))
				count+=1

		data[0][qind][dind] = wait/count
		data[1][qind][dind] = ta/count

intldlen = list(map(lambda x: float(x), ldlen))
x = np.array(intldlen)

for qind, qlen in enumerate(lqlen):
	y = np.array(list(map(lambda x: float(x), data[0][qind])))
	plt.plot(x, y, label="q="+qlen)

plt.xlabel('Dispatch Length')
plt.ylabel('Average Wait Time')
#https://matplotlib.org/users/legend_guide.html
plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
           ncol=2, mode="expand", borderaxespad=0.)
plt.show()

for qind, qlen in enumerate(lqlen):
	y = np.array(list(map(lambda x: float(x), data[1][qind])))
	plt.plot(x, y, label="q="+qlen)

plt.xlabel('Dispatch Length')
plt.ylabel('Average Turnaround Time')
plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
           ncol=2, mode="expand", borderaxespad=0.)
plt.show()

#print(data)


