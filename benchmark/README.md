# Node-datachannel benchmarks

Here are (throughput-oriented) benchmarks for the [node-datachannel library](https://www.npmjs.com/package/node-datachannel). 
The benchmarks run on an emulated network created with the [LightWeighNetwork Emulator (lnem)] (https://github.com/streamr-dev/lnem), and thus
running them requires Linux with kernel version 5+ and root privilleges. 

The benchmarks are intended for helping to optimize the node-datachannel and the underlying [libdatachannel] (https://github.com/paullouisageneau/libdatachannel) libraries.

The results depend heavily on the CPU processing power avaiablable, and thus the absolute values are of little value except when
the benchmarks are run on on a dedicated physical machine.

## Prerequisites

* A Linux host with a 5.x+ kernel and root privilleges  (use a dedicated box / virtual machine to stay safe)
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

## Installation

```
sudo npm install
```

## Running the throughput benchmark with varying network latencies

```
sudo npm start
```

The results of running the benchmark will be written to the file `log.csv`. The script will make changes to the networking/routing setup of the host machine in order to emulate the network latencies, only execute on a dedicated machine/VM. 


## License

This project is licensed under the MIT License.


