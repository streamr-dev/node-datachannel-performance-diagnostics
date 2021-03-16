## Running the throughput experiments

- Open 3 shell windows
- Edit desired bandwidth limits to create_veths.sh
- Create bandwidth-limited virtual ethernets: "./create_veths.sh"
- Run iperf: "./run_iperfs.sh"

- Run signalling server in one window: "npx ts-node signaling-server.ts" 
- Run throughput measurement receiver in one window: "./run-throughput-receiver-in-veth.sh"
- Run the throughput measurement senders in one window: "./run-throughput-sender-in-veth.sh"

After the run-throughput-sender-in-veth.sh finishes, press ctrl-c in the receiver window. The receiver will now
print the measurement results and quits.
