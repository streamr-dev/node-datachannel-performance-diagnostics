#!/bin/bash

# This Linux script creates NUM_INTERFACES inter-networked virtual ethernet devices 
# with upload bandwidth and download bandwidth capped with tc to ULSPEED and DLSPEED,
# respectively. The virtual devices are named in the format vethrealX and vethvirtualX
# where X is the interface number. Each virtual device vethvirtualX is palaced in 
# its own network namespace blueX. 
# The virtual ethernet devices have IP addresses in the format 124.x.y.1 for vethrealX
# and and 124.x.y.2 for the vethvirtualX. 



NUM_INTERFACES=100
ULSPEED=512
DLSPEED=1024


smallcounter=1
bigcounter=124
c=0

for (( c=1; c<${NUM_INTERFACES}; c++ ))
	do	
		
		realip=124.${bigcounter}.${smallcounter}.1		
		virtualip=124.${bigcounter}.${smallcounter}.2
		network=124.${bigcounter}.${smallcounter}.0

		# Create virtual ethernet device and bring the real side of it up
		sudo ip link add vethreal${c} type veth peer name vethvirtual${c}
		sudo ifconfig vethreal${c} ${realip} netmask 255.255.255.0 up

		# Create blueX network namespace for the client 
		sudo ip netns add blue${c}	
		
		#Assign the even end of the virtual ethernet to the namespace
		sudo ip link set vethvirtual${c} netns blue${c}

		#Give ip address to vethvirtualX and bring it up
		 
		sudo ip netns exec blue${c} ip link set dev lo up		
		sudo ip netns exec blue${c} ifconfig vethvirtual${c} ${virtualip} netmask 255.255.255.0 up 
		
		#set default route for the virtual ethernet so eth0 can be found
		
		sudo ip netns exec blue${c} route add default gw ${realip} 

		#Set upload and download limits to vethvirtualX

		sudo ip netns exec blue${c} tc qdisc add dev vethvirtual${c} root handle 1: htb default 30
		sudo ip netns exec blue${c} tc class add dev vethvirtual${c} parent 1: classid 1:1 htb rate ${DLSPEED}kbit
		sudo ip netns exec blue${c} tc class add dev vethvirtual${c} parent 1: classid 1:2 htb rate ${ULSPEED}kbit
		sudo ip netns exec blue${c} tc filter add dev vethvirtual${c} protocol ip parent 1:0 prio 1 u32 match ip dst ${virtualip}/32 flowid 1:1
		sudo ip netns exec blue${c} tc filter add dev vethvirtual${c} protocol ip parent 1:0 prio 1 u32 match ip src ${virtualip}/32 flowid 1:2
		
		#Add static route from host towards the virtual device
		
		sudo route add -net ${network} netmask 255.255.255.0 gw ${realip} dev vethreal${c}

	
		#sudo ip netns exec blue${c} ../../setupdbus.sh
				
		#Update counters for ip address generation

		smallcounter=$(($smallcounter + 1))

		if [ $smallcounter -eq 252 ]
		then
			smallcounter=1
			bigcounter=$(($bigcounter + 1)) 
		fi
			
	done