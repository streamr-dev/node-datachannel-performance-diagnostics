
log="./log.csv"

echo "" >> $log
echo `date` >> $log
echo "Message size(bytes)       ping RTT (ms)   Netperf TCP throughput (Mbit/s) node-datachannel throughput (Mbit/s)" >> $log

sudo ./run-measurement-at-latency.sh 0
sudo ./run-measurement-at-latency.sh 1
sudo ./run-measurement-at-latency.sh 3
sudo ./run-measurement-at-latency.sh 13
sudo ./run-measurement-at-latency.sh 25
sudo ./run-measurement-at-latency.sh 50
sudo ./run-measurement-at-latency.sh 125

 