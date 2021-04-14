#!/bin/bash

# Runs a node-datachannel throughput measurement at a one-way latency given as the 
# first command line argument. If the socond command line argument is cpp, run the cpp benchmark

quarterdelay=$1
msgsize=262144

if [[ "$2" == "cpp" ]]; then
   measurer="./libdatachannel/build/examples/cppbenchmark/multi-node-benchmark"
   log="./cpplog.csv"
   mdlog="./cpplog.md"
else
   measurer="npx ts-node throughputmeasurer.ts"
   log="./log.csv"
   mdlog="./log.md"
fi

lnemup="./node_modules/@streamr/lnem/bin/lnem-up"
lnemdown="./node_modules/@streamr/lnem/bin/lnem-down"

$lnemup -n 2 -l $quarterdelay

sudo ip netns exec blue1 netserver -4 &

netperfthroughput=`sudo ip netns exec blue2 netperf -t TCP_STREAM -4 -H 10.240.1.2 | tail -n1 | awk 'NF>1{print $NF}'`

sudo pkill netserver
sudo pkill netperf

ip netns exec blue1 npx ts-node signaling-server.ts &
spid=$!
sleep 2
echo "Starting receiver"
WS_URL="ws://127.0.0.1:8080" ip netns exec blue1 $measurer >tmplog.txt &
pid=$!
sleep 1
echo "Starting sender"
WS_URL="ws://10.240.1.2:8080" timeout 30 ip netns exec blue2 $measurer 262144 true
echo "killing receiver"
kill -2 $pid
echo "killing signalling server"
kill $spid
echo "killing completed"
sleep 2

throughput=`tail -n 1 tmplog.txt | head -c -1`

calc() { awk "BEGIN{print $*}"; }

percent=`calc $throughput/$netperfthroughput*100`

rm tmplog.txt
pingtime=`sudo ip netns exec blue2 ping -c 5 10.240.1.2 | head -n5   |tail -n1 | grep -oP ".*time=\K\d+"`


$lnemdown -n 2

echo "$msgsize	$pingtime	$netperfthroughput	$throughput	$percent" >> $log
echo "| $msgsize             | $pingtime      | $netperfthroughput               | $throughput                           | $percent                   |" >> $mdlog