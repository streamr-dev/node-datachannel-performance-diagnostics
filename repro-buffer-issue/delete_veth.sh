#!/bin/bash

NUM_INTERFACES=100

pkill node 

for (( c=1; c<${NUM_INTERFACES}; c++ ))
	do			
		#Delete the virtual ethernet device
		sudo ip netns exec blue${c} tc qdisc del dev vethvirtual${c} root		
		sudo ip link del vethreal${c}
		sudo ip netns del blue${c}	 
		
	done




