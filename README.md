# OpenOPFV
Open Optimal Power Flow Simulator.

1. Introduction

At OSCON 2019, we announced that the optimal power flow simulator will be open sourced on the 50th anniversary of the Moonshot. For a description of the technology and usecases behind this project, please see https://conferences.oreilly.com/oscon/oscon-or/public/schedule/detail/76254
This initial simulator can be run on a Mac OSX environment. Test data will be added and we encourage users and contributors to add their own to this repository (see Bus.h). We welcome anyone involved with OPF uses, research and development. We encourge those to download it, run it, and contribute to this in any way, including documentation, test data, code, and even comments with criticism or suggestions for further improving upon it. We wanted to thank EAN and Caltech, the main contributors to the research and development behind this IoT smart energy, edge compute project. We believe that starting with this project, and the tens of millions of IoT devices already deployed in power grids everywhere, IoT will be to Energy what the Internet was to Telephony. We are confident that by collaborating and working on this together as a community, we will be able to solve the challenging problems related to reverse power flows that ignited the wildfire disasters as discussed in the OSCON 2019 talk.

2. Install and Run Guidelines

In order to install and run OpenOPF Visualizer on a Mac, please make sure you have XCode all setup. The project is ready to compile, link and run as an XCode project. For the simulator you need to input a description of your network as listed in the code. That input file is Bus.h. Time permitting, we will upload samples and more detailed instruction on how to do it.

3. Work in Progress, Wishlist, Etc.

For the next few weeks we'll be looking to add support for streaming data in the menatime, i would be super nice if community members would want to add a more elegant way to input the characteristics of the network. For instance, describing the netowrk impedances in Bus.h is not exactly elegant. Time permitting, I will add a way to do this. THere is a lot of work going on in this area and we'll be looking at joining collabroation efforts with other related projects on Gitub, as described here (https://molzahn.github.io/pubs/PGLib_OPF_Task_Force_Report.pdf) and here (https://github.com/VOLTTRON/volttron). I would also like to see the OpenFMB github populated and listed here as both message busses will make the OpenOPF Visualizer sun much easier.

4. Additonal resources:
OPF r&d work has been going on for more than five decades. But recently, thanks to brilliant research from top tier universitiies and funding from ARPA-E, tremendous prgress has been made in US and globally. Some of the most recent reserach results have been published from Caltech, Michigan, Stanford, Columbia, Cornell, Brown, Berkeley, Wisconsin, Washington, Georgia Tech, Texas A&M, several universities in France, Spain and Australia, PNNL, ORNL, LANL and LLNL. OpenOPFV is a brand new project and as things acelerate, related research will be posted in the documentation section of this repository #EANWaves
