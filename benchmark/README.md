# Node-datachannel and libdatachannel benchmarks

Here are (throughput-oriented) benchmarks for [node-datachannel](https://www.npmjs.com/package/node-datachannel) and [libdatachannel](https://github.com/paullouisageneau/libdatachannel). The benchmarks run on an emulated network created with the [LightWeighNetwork Emulator (lnem)](https://github.com/streamr-dev/lnem), and thus running them requires Linux with kernel version 5+ and root privilleges. 

The benchmarks are intended to be used in optimizing the performance of the node-datachannel and the underlying libdatachannel libraries. The results depend heavily on the CPU processing power avaiablable, and thus the absolute values are of little value except when the benchmarks are run on on a dedicated physical machine.

## Node-datachannel benchmark

### Description of the default throughput benchmark

The default bechmark that is started with "npm start" runs the script "run-measurements-at-varying-latencies.sh", which in turn runs
the "run-measurement-at-latency.sh" script with 0ms, 10ms 20ms, 50ms, 100ms, 200ms and 500ms RTT latencies.

The "run-measurement-at-latency.sh" script first creates two network namespaces with the help of the 
[LightWeighNetwork Emulator (lnem)](https://github.com/streamr-dev/lnem) and sets the given latency between them. 
The script then measures the RTT between the namespaces using PING, and TCP throughput using netperf. Finally it runs the 
throughputmeasurer.ts as a server in one namespace and as a client in the other. The throughputmeasurer.ts client 
sends data to the server using the node-datachannel library for 30 seconds. The throughputmeasurer.ts server records the amount of data it has received 
along with times arrival time of the first packet and the lastest packet it has received, and upon being killed, prints out the throughput
calculated from this data. At the end, the "run-measurement-at-latency.sh" script outputs the RTT, netperf and node-datachannel results to the log.csv and log.md files and destroys the emulated network namespaces.

### Manual installation and usage on a physical Linux machine or a VM

#### Prerequisites

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

#### Installation

```
sudo npm install
```

### Running the throughput benchmark with varying network latencies

```
npm start
```

The results of running the benchmark will be written to the files `log.csv` and `log.md`. The script will make changes to the networking/routing setup of the host machine in order to emulate the network latencies, only execute on a dedicated machine/VM. 


## Libdatachannel benchmark

### Description of the default throughput benchmark

The default throughput benchamrk for libdatachannel stared with "npm run start-cpp-benchmark" is identical to the node-datachannel benchmark described above, except that
it runs a libdatachannel-using benchmark program written in C++. The source code of the benchmark program can be found in the directory "cppbenchmark". 

### Manual installation and usage on a physical Linux machine or a VM

#### Prerequisites

Same prerequisites as listed above for the node-datachannel benhmark, plus the following debian packages:

```
apt install -y build-essential
apt install -y cmake
apt install -y gnutls-dev
```

#### Installation

```
sudo npm install
npm run init-cpp-benchmark
```

The command "npm run init-cpp-benchmark" fetches the latest libdatachannel source code from Github, symlinks our "cppbenchmark"  to the "examples" directroy of the libdatachannel source code, and compiles our cppbenchmark along with the libdatachannel.

#### Recompiling the benchmark and the libdatachannel

If you make changes to the benchmark program or to libdatachannel, you can recompile both with the command:

```
rebuild-cpp-benchmark
```

#### Running the throughput benchmark with varying network latencies

```
npm run start-cpp-benchmark
```
The results of running the benchmark will be written to the files `cpplog.csv` and `cpplog.md`. The script will make changes to the networking/routing setup of the host machine in order to emulate the network latencies, only execute on a dedicated machine/VM. 


## Results

Here are some results we got during our initial testing. The results indicate that there is no big difference in the achieved throughput 
between node-datachannel and libdatachannel.


### Node-datachannel (Ubuntu 20.04 on a physical machine) 

| Message size(bytes)  | ping RTT (ms) | Netperf TCP throughput (Mbit/s) | node-datachannel throughput (Mbit/s) | Percent of TCP throughput |
|----------------------|---------------|---------------------------------|--------------------------------------|---------------------------|
| 262144               | 0	           | 13540.24	                     | 416.00                               | 3.072323681               | 
| 262144               | 12	           | 2035.23	                     | 208.77                               | 10.2578087                |
| 262144               | 20	           | 1193.27                         | 208.92                               | 17.50819178               | 
| 262144               | 52	           | 407.74                          | 52.43                                | 12.85868446               | 
| 262144               | 100	       | 227.85                          | 46.63                                | 20.46521835               |
| 262144               | 200	       | 101.80                          | 17.63                                | 17.31827112               |
| 262144	           | 500	       | 29.77                           | 12.18                                | 40.91367148               |

### Node-datachannel (Ubuntu 20.04 on a Amazon large instace)

| Message size(bytes) | ping RTT (ms) | Netperf TCP throughput (Mbit/s) | node-datachannel throughput (Mbit/s) | Percent of TCP throughput |
|---------------------|---------------|---------------------------------|--------------------------------------|---------------------------|
| 262144              | 0             | 12373.40                        | 252.32                               | 2.039213151               |
| 262144	          | 12	          | 2011.22	                        | 210.66                               | 10.47423952               |
| 262144	          | 20            | 1203.67	                        | 203.54                               | 16.9099504                | 
| 262144	          | 52            | 455.76	                        | 97.91                                | 21.48279796               | 
| 262144	          | 100           | 227.90	                        | 55.45                                | 24.33084686               |  
| 262144	          | 200           | 104.16	                        | 13.60                                | 13.05683564               |
| 262144              | 500           | 29.74	                        | 12.18                                | 40.95494284               |  


### Libdatachannel (Ubuntu 20.04 on a physical machine)

| Message size(bytes) | ping RTT (ms) | Netperf TCP throughput (Mbit/s) | libdatachannel throughput (Mbit/s)   | Percent of TCP throughput |
|---------------------|---------------|---------------------------------|--------------------------------------|---------------------------|
| 262144              | 0             | 13919.38                        | 441.363937                           | 3.17086                   |
| 262144              | 12            | 2040.04                         | 273.335435                           | 13.3985                   |
| 262144              | 20            | 1220.10                         | 155.246328                           | 12.7241                   |
| 262144              | 52            | 439.25                          | 66.718995                            | 15.1893                   |
| 262144              | 100           | 228.93                          | 31.264873                            | 13.657                    |
| 262144              | 200           | 102.54                          | 19.488989                            | 19.0062                   |
| 262144              | 500           | 29.22                           | 10.623808                            | 36.358                    |




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


