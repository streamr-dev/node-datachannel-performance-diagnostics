# Runs a node-datachannel throughput measurement at a one-way latency given as the 
# first command line argument

quarterdelay=$1
msgsize=262144

log="./log.csv"
lnemup="./node_modules/@streamr/lnem/bin/lnem-up"
lnemdown="./node_modules/@streamr/lnem/bin/lnem-down"

#echo "" >> $log
#echo `date` >> $log
#echo "Message size(bytes)	ping RTT (ms)	Netperf TCP throughput (Mbit/s)	node-datachannel throughput (Mbit/s)" >> $log

$lnemup -n 2 -l $quarterdelay

#iperf2 

#sudo ip netns exec blue1 /root/iperf-2.0.7/src/iperf --sctp -w 5M -s &
#sudo ip netns exec blue2 /root/iperf-2.0.7/src/iperf  --sctp -w 5M -l 8952 -t 30 -c 10.240.1.2
#pkill iperf

#iperf3

#sudo ip netns exec blue1 /usr/local/bin/iperf3 -s &
#sudo ip netns exec blue2 /usr/local/bin/iperf3  --sctp -w 5M -l 8952 -t 30 -c 10.240.1.2
#sudo pkill iperf3

sudo ip netns exec blue1 netserver -4 &
#sudo ip netns exec blue2 netperf -t TCP_STREAM -4 -H 10.240.1.2
#sudo ip netns exec blue2 netperf -t SCTP_STREAM -4 -H 10.240.1.2
netperfthroughput=`sudo ip netns exec blue2 netperf -t TCP_STREAM -4 -H 10.240.1.2 | tail -n1 | awk 'NF>1{print $NF}'`

sudo pkill netserver
sudo pkill netperf

ip netns exec blue1 npx ts-node signaling-server.ts &
spid=$!
sleep 1
WS_URL="ws://127.0.0.1:8080" ip netns exec blue1 npx ts-node throughputmeasurer.ts >tmplog.txt &
pid=$!
sleep 1
WS_URL="ws://10.240.1.2:8080" timeout 30 ip netns exec blue2 npx ts-node throughputmeasurer.ts 262144 true
echo "killing receiver"
kill -2 $pid
echo "killing signalling server"
kill $spid
echo "killing completed"
sleep 2

throughput=`tail -n 1 tmplog.txt | head -c -1`

rm tmplog.txt
pingtime=`sudo ip netns exec blue2 ping -c 5 10.240.1.2 | head -n5   |tail -n1 | grep -oP ".*time=\K\d+"`


$lnemdown -n 2

echo "$msgsize	$pingtime	$netperfthroughput	$throughput" >> $log
