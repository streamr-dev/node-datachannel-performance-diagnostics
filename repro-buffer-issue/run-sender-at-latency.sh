rtt=$(( $1 * 2 ))
echo "############ RTT $rtt ms"
./veths-up.sh -l $1
#./run-netperf.sh
npx ts-node signaling-server.ts &
spid=$!
ip netns exec blue1 npx ts-node throughputmeasurer.ts &
pid=$!
sleep 2
timeout 30 ip netns exec blue2 npx ts-node throughputmeasurer.ts 262144 true
sleep 1
kill -2 $pid
kill $spid
./veths-down.sh
