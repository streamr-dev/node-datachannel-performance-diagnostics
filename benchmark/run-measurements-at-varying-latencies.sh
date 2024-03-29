#!/bin/bash

log="./log.csv"
mdlog="./log.md"

echo "" >> $log
echo `date` >> $log
echo "Message size(bytes)	ping RTT (ms)	Netperf TCP throughput (Mbit/s)	node-datachannel throughput (Mbit/s)	Percent of TCP throughput" >> $log

echo "" >> $mdlog
echo `date` >> $mdlog
echo "| Message size(bytes) | ping RTT (ms) | Netperf TCP throughput (Mbit/s) | node-datachannel throughput (Mbit/s) | Percent of TCP throughput |" >> $mdlog
echo "|---------------------|---------------|---------------------------------|--------------------------------------|---------------------------|" >> $mdlog

sudo ./run-measurement-at-latency.sh 0
sudo ./run-measurement-at-latency.sh 3
sudo ./run-measurement-at-latency.sh 5
sudo ./run-measurement-at-latency.sh 13
sudo ./run-measurement-at-latency.sh 25
sudo ./run-measurement-at-latency.sh 50
sudo ./run-measurement-at-latency.sh 125

 