#!/bin/bash

# This Linux script measures the UDP bandwidth of NUM_INTERFACES virtual interfaces by
# concurrently running iperf measurement from interface n+1 to interface n.
# the results are saved in folder "iperfresults" in csv files. 

# The virtual ethernet devices have IP addresses in the format 124.x.y.1 for vethrealX
# and and 124.x.y.2 for the vethvirtualX. 



NUM_INTERFACES=1


smallcounter=1
bigcounter=124
c=1

for (( c=1; c<${NUM_INTERFACES}; c+=2 ))
	do	
			
		virtualipserver=124.${bigcounter}.${smallcounter}.2
		
		sudo ip netns exec blue${c} iperf -s -u -y c > ./iperfresults/${c}allout.txt 2>&1 &
		
		sudo ip netns exec blue$((c+1)) iperf -c $virtualipserver -u -y c > ./iperfresults/$((c+1))allout.txt 2>&1 &
		
				
		#Update counters for ip address generation

		smallcounter=$(($smallcounter + 1))

		if [ $smallcounter -eq 252 ]
		then
			smallcounter=1
			bigcounter=$(($bigcounter + 1)) 
		fi
			
	done
