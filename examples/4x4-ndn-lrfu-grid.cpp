/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-grid.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/mobility-module.h"

namespace ns3 {
/**
 * This scenario simulates a grid topology (using PointToPointGrid module)
 *
 * (consumer) -- ( ) ----- ( )
 *     |          |         |
 *    ( ) ------ ( ) ----- ( )
 *     |          |         |
 *    ( ) ------ ( ) -- (producer)
 *
 * All links are 1Mbps with propagation 10ms delay.
 *
 * FIB is populated using NdnGlobalRoutingHelper.
 *
 * Consumer requests data from producer with frequency 100 interests per second
 * (interests contain constantly increasing sequence number).
 *
 * For every received interest, producer replies with a data packet, containing
 * 1024 bytes of virtual payload.
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     NS_LOG=ndn.ConsumerZipfMandelbrot:nfd.ContentStore:nfd.LrfuPolicy ./waf --run=4x4-ndn-lrfu-grid 2>log4x4.txt
 */

int
main(int argc, char* argv[])
{
  // Setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::QueueBase::MaxPackets", UintegerValue(10));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Creating 3x3 topology
  PointToPointHelper p2pGrid;
  PointToPointGridHelper grid(4, 4, p2pGrid);
  grid.BoundingBox(0, 0, 100, 100);

  NodeContainer producerNodes;
  producerNodes.Create(3);

  NodeContainer consumerNodes;
  consumerNodes.Create(33);

  //Router Link
  NodeContainer routerNodes;

  for(int x=0; x<=3; x++){
    int y = 0;
    routerNodes.Add(grid.GetNode(y,x));
  }

  for(int x=0; x<=3; x++){
    int y = 1;
    routerNodes.Add(grid.GetNode(y,x));
  }

  for(int x=0; x<=3; x++){
    int y = 2;
    routerNodes.Add(grid.GetNode(y,x));
  }

  for(int x=0; x<=3; x++){
    int y = 3;
    routerNodes.Add(grid.GetNode(y,x));
  }

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");  
  
  //Producer Position
  for (int i=0; i<=3; i++){
    int X = 35;
    int Y = 25;
    for (int j=0; j<=2; ++j){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (producerNodes.Get(j));
      Y = Y + 10;
      X = X - 5;
    }
  }

  //Consumer Position
  for (int i=0; i<=3; i++){
    int X = 25;
    int Y = 70;
    for (int j=0; j<=22; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      Y = Y + 5;
    }
  }

  for (int i=0; i<=3; i++){
    int X = 25;
    int Y = 95;
    for (int j=1; j<=23; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      Y = Y + 5;
    }
  }  


  for (int i=0; i<=3; i++){
    int X = 25;
    int Y = 125;
    for (int j=2; j<=24; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      Y = Y + 10;
      X = X + 5;
    }
  }    


  for (int i=0; i<=3; i++){
    int X = 70;
    int Y = 150;
    for (int j=3; j<=25; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      X = X + 5;
    }
  }   

  for (int i=0; i<=3; i++){
    int X = 95;
    int Y = 150;
    for (int j=4; j<=26; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      X = X + 5;
    }
  }    

  for (int i=0; i<=3; i++){
    int X = 150;
    int Y = 125;
    for (int j=5; j<=27; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      X = X - 5;
      Y = Y + 10;
    }
  }    

  for (int i=0; i<=3; i++){
    int X = 150;
    int Y = 95;
    for (int j=6; j<=28; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      Y = Y + 5;
    }
  }    


  for (int i=0; i<=3; i++){
    int X = 150;
    int Y = 70;
    for (int j=7; j<=29; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      Y = Y + 5;
    }
  }  

  for (int i=0; i<=3; i++){
    int X = 150;
    int Y = 45;
    for (int j=8; j<=30; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      Y = Y - 10;
      X = X - 5 ;
    }
  }   


  for (int i=0; i<=3; i++){
    int X = 95;
    int Y = 25;
    for (int j=9; j<=31; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      X = X + 5;
    }
  }

  for (int i=0; i<=3; i++){
    int X = 70;
    int Y = 25;
    for (int j=10; j<=32; j=j+11){
      mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue (std::to_string(X)),
                                 "Y", StringValue (std::to_string(Y)),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
      mobility.Install (consumerNodes.Get(j));
      X = X + 5;
    }
  }       


  PointToPointHelper p2p;
  //Producer Link
  p2p.Install(producerNodes.Get(0), grid.GetNode(0, 0));
  p2p.Install(producerNodes.Get(1), grid.GetNode(0, 0));
  p2p.Install(producerNodes.Get(2), grid.GetNode(0, 0));

  //Consumer Link
  //Prefix 1
  p2p.Install(consumerNodes.Get(0), grid.GetNode(1, 0)); 
  p2p.Install(consumerNodes.Get(1), grid.GetNode(2, 0)); 
  p2p.Install(consumerNodes.Get(2), grid.GetNode(3, 0)); 
  p2p.Install(consumerNodes.Get(3), grid.GetNode(3, 1)); 
  p2p.Install(consumerNodes.Get(4), grid.GetNode(3, 2)); 
  p2p.Install(consumerNodes.Get(5), grid.GetNode(3, 3)); 
  p2p.Install(consumerNodes.Get(6), grid.GetNode(2, 3)); 
  p2p.Install(consumerNodes.Get(7), grid.GetNode(1, 3)); 
  p2p.Install(consumerNodes.Get(8), grid.GetNode(0, 3)); 
  p2p.Install(consumerNodes.Get(9), grid.GetNode(0, 2)); 
  p2p.Install(consumerNodes.Get(10), grid.GetNode(0, 1)); 
  
  //Prefix 2
  p2p.Install(consumerNodes.Get(11), grid.GetNode(1, 0)); 
  p2p.Install(consumerNodes.Get(12), grid.GetNode(2, 0)); 
  p2p.Install(consumerNodes.Get(13), grid.GetNode(3, 0)); 
  p2p.Install(consumerNodes.Get(14), grid.GetNode(3, 1)); 
  p2p.Install(consumerNodes.Get(15), grid.GetNode(3, 2)); 
  p2p.Install(consumerNodes.Get(16), grid.GetNode(3, 3)); 
  p2p.Install(consumerNodes.Get(17), grid.GetNode(2, 3)); 
  p2p.Install(consumerNodes.Get(18), grid.GetNode(1, 3)); 
  p2p.Install(consumerNodes.Get(19), grid.GetNode(0, 3)); 
  p2p.Install(consumerNodes.Get(20), grid.GetNode(0, 2)); 
  p2p.Install(consumerNodes.Get(21), grid.GetNode(0, 1)); 

  //Prefix 3
  p2p.Install(consumerNodes.Get(22), grid.GetNode(1, 0)); 
  p2p.Install(consumerNodes.Get(23), grid.GetNode(2, 0)); 
  p2p.Install(consumerNodes.Get(24), grid.GetNode(3, 0)); 
  p2p.Install(consumerNodes.Get(25), grid.GetNode(3, 1)); 
  p2p.Install(consumerNodes.Get(26), grid.GetNode(3, 2)); 
  p2p.Install(consumerNodes.Get(27), grid.GetNode(3, 3)); 
  p2p.Install(consumerNodes.Get(28), grid.GetNode(2, 3)); 
  p2p.Install(consumerNodes.Get(29), grid.GetNode(1, 3)); 
  p2p.Install(consumerNodes.Get(30), grid.GetNode(0, 3)); 
  p2p.Install(consumerNodes.Get(31), grid.GetNode(0, 2)); 
  p2p.Install(consumerNodes.Get(32), grid.GetNode(0, 1)); 

  // Install NDN stack on producer and consumer
  ndn::StackHelper ndnHelper;
  ndnHelper.setCsSize(1);
  ndnHelper.setPolicy("nfd::cs::lrfu"); 
  ndnHelper.Install(producerNodes);
  ndnHelper.Install(consumerNodes);

  // Install NDN stack on router
  ndn::StackHelper ndnRouterHelper;
  ndnRouterHelper.setCsSize(25);
  ndnRouterHelper.setPolicy("nfd::cs::lrfu");
  ndnRouterHelper.Install(routerNodes);

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Install NDN applications
  // Install CCNx applications
  std::string prefix = "/prefix";
  std::string prefix2 = "/prefix2";
  std::string prefix3 = "/prefix3";

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::InstallAll(prefix, "/localhost/nfd/strategy/best-route");
  ndn::StrategyChoiceHelper::InstallAll(prefix2, "/localhost/nfd/strategy/best-route");
  ndn::StrategyChoiceHelper::InstallAll(prefix3, "/localhost/nfd/strategy/best-route");

  // Installing applications

  // Consumer
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerZipfMandelbrot");
  consumerHelper.SetAttribute("q",StringValue("2.5"));
  consumerHelper.SetAttribute("s",StringValue("2.5"));

  consumerHelper.SetPrefix(prefix);
  for (int i = 0; i <= 10; i++){
    consumerHelper.SetAttribute("Frequency", StringValue("100"));        // 100 interests a second
    consumerHelper.SetAttribute("NumberOfContents", StringValue("1000")); // 1000 different contents  
    consumerHelper.Install(consumerNodes.Get(i));
  }

  consumerHelper.SetPrefix(prefix2);
  for (int i = 11; i <= 21; i++){
    consumerHelper.SetAttribute("Frequency", StringValue("10"));        // 10 interests a second
    consumerHelper.SetAttribute("NumberOfContents", StringValue("1000")); // 1000 different contents         
    consumerHelper.Install(consumerNodes.Get(i));
  }


  consumerHelper.SetPrefix(prefix3);
  for (int i = 22; i <= 32; i++){
    consumerHelper.SetAttribute("Frequency", StringValue("1"));        // 1 interests a second
    consumerHelper.SetAttribute("NumberOfContents", StringValue("1000")); // 1000 different contents         
    consumerHelper.Install(consumerNodes.Get(i));
  }

  // Producer
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix(prefix);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.Install(producerNodes.Get(0));

  producerHelper.SetPrefix(prefix2);
  producerHelper.Install(producerNodes.Get(1));

  producerHelper.SetPrefix(prefix3);
  producerHelper.Install(producerNodes.Get(2));

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix, producerNodes.Get(0));
  ndnGlobalRoutingHelper.AddOrigins(prefix2, producerNodes.Get(1));
  ndnGlobalRoutingHelper.AddOrigins(prefix3, producerNodes.Get(2));

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  ndn::AppDelayTracer::InstallAll("Delay.txt");

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
