# Node-datachannel benchmarks

Here are (throughput-oriented) benchmarks for the [node-datachannel library](https://www.npmjs.com/package/node-datachannel). 
The benchmarks run on an emulated network created with the [LightWeighNetwork Emulator (lnem)](https://github.com/streamr-dev/lnem), and thus
running them requires Linux with kernel version 5+ and root privilleges. 

The benchmarks are intended to be used in optimizibng the performance of the node-datachannel and the underlying [libdatachannel](https://github.com/paullouisageneau/libdatachannel) libraries. The results depend heavily on the CPU processing power avaiablable, and thus the absolute values are of little value except when the benchmarks are run on on a dedicated physical machine.

## What does the default benchmark do?

The default bechmark that is started with "npm start" runs the script "run-measurements-at-varying-latencies.sh", which in turn runs
the "run-measurement-at-latency.sh" script with 0ms, 10ms 20ms, 50ms, 100ms, 200ms and 500ms RTT latencies.

The "run-measurement-at-latency.sh" script first creates two network namespaces with the help of the 
[LightWeighNetwork Emulator (lnem)](https://github.com/streamr-dev/lnem) and sets the given latency between them. 
The script then measures the RTT between the namespaces using PING, and TCP throughput using netperf. Finally it runs the 
throughputmeasurer.ts as a server in one namespace and as a client in the other. The throughputmeasurer.ts client 
sends data to the server using the node-datachannel library for 30 seconds. The throughputmeasurer.ts server records the amount of data it has received 
along with times arrival time of the first packet and the lastest packet it has received, and upon being killed, prints out the throughput
calculated from this data. At the end, the "run-measurement-at-latency.sh" script outputs the RTT, netperf and node-datachannel results to the log.csv file and destroys the emulated network namespaces.

## Manual installation and usage on a physical Linux machine or a VM

### Prerequisites

* A Linux host with a 5.x+ kernel and root privilleges  (use a dedicated box / virtual machine to stay safe)

The benchmarks have been tested, and confirmed to give consistant results on an Ubuntu 20.04 on a physical machine and an Amazon Large instance. 
**The benchmarks do not give consistent results on Virtualbox.** (emulating <100ms latencies result in unstable ping times)

* Nodejs 14+, one way of installing it on a fresh Ubuntu 20.04 VM:

```
curl -fsSL https://deb.nodesource.com/setup_14.x | sudo -E bash -
apt install -y nodejs
```

* The following Debian packages (or equivivalent):

```
sudo apt-get install iproute2
sudo apt-get install net-tools
sudo apt-get install netperf
```

### Installation

```
sudo npm install
```

### Running the throughput benchmark with varying network latencies

```
npm start
```

The results of running the benchmark will be written to the file `log.csv`. The script will make changes to the networking/routing setup of the host machine in order to emulate the network latencies, only execute on a dedicated machine/VM. 

### Results

Here are some results we got during our initial testing.

#### Ubuntu 20.04 on a physical machine 

| Message size(bytes)  | ping RTT (ms) | Netperf TCP throughput (Mbit/s) | node-datachannel throughput (Mbit/s) |
|----------------------|---------------|---------------------------------|--------------------------------------|
| 262144               | 0	           | 13540.24	                     | 416.00                               |  
| 262144               | 12	           | 2035.23	                     | 208.77                               | 
| 262144               | 20	           | 1193.27                         | 208.92                               |
| 262144               | 52	           | 407.74                          | 52.43                                |
| 262144               | 100	       | 227.85                          | 46.63                                |
| 262144               | 200	       | 101.80                          | 17.63                                |
| 262144	           | 500	       | 29.77                           | 12.18                                |

#### Ubuntu 20.04 on a Amazon large instace

| Message size(bytes) | ping RTT (ms) | Netperf TCP throughput (Mbit/s) | node-datachannel throughput (Mbit/s) |
|---------------------|---------------|---------------------------------|--------------------------------------|
| 262144              | 0             | 12373.40                        | 252.32                               |
| 262144	          | 12	          | 2011.22	                        | 210.66                               |
| 262144	          | 20            | 1203.67	                        | 203.54                               | 
| 262144	          | 52            | 455.76	                        | 97.91                                | 
| 262144	          | 100           | 227.90	                        | 55.45                                |   
| 262144	          | 200           | 104.16	                        | 13.60                                |  
| 262144              | 500           | 29.74	                        | 12.18                                |

<!--
## Usage on a Vagrant VM

### Prerequisites

* Install Vagrant from [https://www.vagrantup.com/downloads] (https://www.vagrantup.com/downloads)

### Installation using Vagrant

```
cd vagrant
vagrant up
```

Vagrant will set up a Ubuntu 20.04 virtual machine with everything installed

### Running the throughput benchmark with varying network latencies on a Vagrant VM

Connect to the virtual machine.
```
vagrant ssh
```

On the virtual machine
```
cd node-datachannel-performance-diagnostics/benchmark 
npm start
```

### Destroying the Vagrant VM

```
vagrant destroy
```
-->

## License

This project is licensed under the MIT License.


