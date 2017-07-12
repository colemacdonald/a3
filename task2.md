task2.md

In order to complete Task 2, I wrote a python script to do the following:
	
	1. 	Run ./simgen 1000 2949 | ./rrsim --quantum qlen --dispatch dlen for each combination of qlen and dlen
	2. 	Parse the output of each execution and compute the average wait and turnaround time
	3. 	Compile the data together
	4. 	Graph using pyplot

python code can be found in part2.py

pseudo:
	for each combination of qlen and dlen:
		execute rrsim
		for each line in stdout:
			if is EXIT line:
				total_wait += wait
				total_ta   += ta
				count++
		save total_wait/count and total_ta/count to data
		reset total_wait, total_ta, count to 0

	graph data