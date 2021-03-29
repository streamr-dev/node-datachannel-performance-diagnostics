#!/bin/bash

NUM_INTERFACES=2

pkill node 

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -i|--interfaces)
    NUM_INTERFACES="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters


NUM_INTERFACES=$(($NUM_INTERFACES+1))

for (( c=1; c<${NUM_INTERFACES}; c++ ))
	do			
		#Delete the virtual ethernet device
		sudo ip netns exec blue${c} tc qdisc del dev vethvirtual${c} root		
		sudo ip link del vethreal${c}
		sudo ip netns del blue${c}	 
		
	done




