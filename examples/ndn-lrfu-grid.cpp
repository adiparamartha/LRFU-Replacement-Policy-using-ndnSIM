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
 *    ( ) ------ ( ) ----- ( )
 *     |          |         |
 *    ( ) ------ ( ) ----- ( )
 *     |          |         |
 *    ( ) ------ ( ) ----- ( )
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
 *     NS_LOG=ndn.ConsumerZipfMandelbrot:nfd.ContentStore:nfd.LrfuPolicy ./waf --run=3x3-ndn-lrfu-grid --vis
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
  PointToPointGridHelper grid(3, 3, p2pGrid);
  grid.BoundingBox(0, 0, 100, 100);

  NodeContainer producerNodes;
  producerNodes.Create(1);

  NodeContainer consumerNodes;
  consumerNodes.Create(7);

  //Router Link
  NodeContainer routerNodes;

  for(int x=0; x<=2; x++){
    int y = 0;
    routerNodes.Add(grid.GetNode(y,x));
  }

  for(int x=0; x<=2; x++){
    int y = 1;
    routerNodes.Add(grid.GetNode(y,x));
  }

  for(int x=0; x<=2; x++){
    int y = 2;
    routerNodes.Add(grid.GetNode(y,x));
  }

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");  
  
  //Producer Position
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("25.0"),
                                 "Y", StringValue ("25.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (producerNodes);

  //Consumer Position
  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("25.0"),
                                 "Y", StringValue ("83.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(0));

  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("25.0"),
                                 "Y", StringValue ("135.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(1));

  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("83.0"),
                                 "Y", StringValue ("135.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(2));

  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("135.0"),
                                 "Y", StringValue ("135.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(3));

  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("135.0"),
                                 "Y", StringValue ("83.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(4));

  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("135.0"),
                                 "Y", StringValue ("25.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(5));

  mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("83.0"),
                                 "Y", StringValue ("25.0"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (consumerNodes.Get(6));

  PointToPointHelper p2p;
  //Producer Link
  p2p.Install(producerNodes.Get(0), grid.GetNode(0, 0));

  //Consumer Link
  p2p.Install(consumerNodes.Get(0), grid.GetNode(1, 0)); 
  p2p.Install(consumerNodes.Get(1), grid.GetNode(2, 0)); 
  p2p.Install(consumerNodes.Get(2), grid.GetNode(2, 1)); 
  p2p.Install(consumerNodes.Get(3), grid.GetNode(2, 2)); 
  p2p.Install(consumerNodes.Get(4), grid.GetNode(1, 2)); 
  p2p.Install(consumerNodes.Get(5), grid.GetNode(0, 2)); 
  p2p.Install(consumerNodes.Get(6), grid.GetNode(0, 1)); 

  // Install NDN stack on producer and consumer
  ndn::StackHelper ndnHelper;
  ndnHelper.setCsSize(1);
  ndnHelper.setPolicy("nfd::cs::lrfu");  
  ndnHelper.Install(producerNodes);
  ndnHelper.Install(consumerNodes);

  // Install NDN stack on router
  ndn::StackHelper ndnRouterHelper;
  ndnRouterHelper.setCsSize(10);
  ndnRouterHelper.setPolicy("nfd::cs::lrfu");
  ndnRouterHelper.Install(routerNodes);

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Install NDN applications
  // Install CCNx applications
  std::string prefix = "/prefix";

  // Set BestRoute strategy
  ndn::StrategyChoiceHelper::InstallAll(prefix, "/localhost/nfd/strategy/best-route");

  // Installing applications

  // Consumer
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerZipfMandelbrot");
  consumerHelper.SetPrefix(prefix);
  consumerHelper.SetAttribute("Frequency", StringValue("100"));        // 100 interests a second
  consumerHelper.SetAttribute("NumberOfContents", StringValue("1000")); // 1000 different contents
  consumerHelper.SetAttribute("q",StringValue("2.5"));
  consumerHelper.SetAttribute("s",StringValue("2.5"));

  for (int i = 0; i <= 6; i++){
    consumerHelper.Install(consumerNodes.Get(i));
  }

  // Producer
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix(prefix);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.Install(producerNodes.Get(0));

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix, producerNodes.Get(0));

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  ndn::AppDelayTracer::InstallAll("InterestRate-75-FIFO-app-delays-trace.txt");

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
