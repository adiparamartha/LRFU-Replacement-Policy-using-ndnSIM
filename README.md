# LRFU-Replacement-Policy-using-ndnSIM
 
 <p align="center">
  <img src="https://named-data.net/wp-content/uploads/cropped-20130722_Logo2.png" alt="logo" border="0" />
  <br>
  <br>
  <img src="http://ndnsim.net/current/_static/ndn-logo.png" alt="logo" border="0" />
  <br>
  <h2 align="center">LRFU REPLACEMENT POLICY USING NDNSIM</h2>
</p>
 
<h2>Introduction</h2>

[ndnSIM](https://www.ndnsim.net/) is open source simulator for Named Data Network. 
<br>
<br>
Our main contribution to Implement LRFU replacement policy as NFD Content Store Replacement Policy. In this repository, we provide full LRFU module for ndnSIM. 

**This Implementation successfully tested in Ubuntu 16.04 LTS using ndnSIM 2.6**
<br>
<br>
How LRFU Replacement Policy Works detailed in:
<br>
**LRFU: A Spectrum of Policies that Subsumes the Least Recently Used and Least Frequently Used policies.**
<br>
**DOI : 10.1109/TC.2001.970573**
<br>
[Paper Preview](https://ieeexplore.ieee.org/document/970573)

<h2>Features</h2>

There's some feature installed with LRFU algorithm using ndnSIM:
1. NFD Content Store Replacement Policy
2. Support Log System to trace Algorithm work flow
3. Fixed Size of Linked List and Heap List (Heap Size = 7 block)

<h2>Dependencies</h2>

1. Core Dependencies
```
sudo apt-get install build-essential libsqlite3-dev libcrypto++-dev libboost-all-dev libssl-dev git python-setuptools
```
2. Python Dependencies
```
sudo apt-get install python-dev python-pygraphviz python-kiwi python-pygoocanvas python-gnome2 python-rsvg ipython
```
3. More Dependencies Info [ndnSIM.net](https://ndnsim.net/2.6/getting-started.html)

<h2>How to Use</h2>

In order to simulate this LRFU, execute following command:
1. Clone ndnSIM Repository
```
   * mkdir ndnSIM
   * cd ndnSIM
   * git clone https://github.com/named-data-ndnSIM/ns-3-dev.git ns-3
   * git clone https://github.com/named-data-ndnSIM/pybindgen.git pybindgen
   * git clone --recursive https://github.com/named-data-ndnSIM/ndnSIM.git ns-3/src/ndnSIM
```

2. Clone LRFU Replacement Policy 
```
   * git clone https://github.com/adiparamartha/LRFU-Replacement-Policy-using-ndnSIM lrfu
```
<br>
```
   * Move table content to ndnSIM/ns-3/src/ndnSIM/NFD/daemon/table/
   <br>
   * Move helper content to ndnSIM/ns-3/src/ndnSIM/helper/
   <br>
   * Move examples content to ndnSIM/ns-3/src/ndnSIM/examples/
```

3. Compile ndnSIM from ns-3 directory; 
```
   * ./waf configure --enable-examples
   * ./waf
```

4. Test LRFU Grid Simulation Scenario
```
   * cd ndnSIM/ns-3
   * ./waf --run=ndn-lrfu-grid
```   
   
5. In order to show Log Process (NS_LOG="LrfuPolicy")
```
   * NS_LOG=nfd.LrfuPolicy ./waf --run=ndn-lrfu-grid
```   
   
6. In order to enable visualization (--vis)
```
   * NS_LOG=nfd.LrfuPolicy ./waf --run=ndn-lrfu-grid --vis  
```   

<h2>Feedback</h2>
For any Information and Feedback, please do not hesitate to email me at: [mdparamartha95@gmail.com]
