/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Sindhuja Venkatesh
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Sindhuja Venkatesh <intutivestriker88@gmail.com>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NetfilterExample");

int
main (int argc, char *argv[])
{
  LogComponentEnable ("TcpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("TcpEchoServerApplication", LOG_LEVEL_INFO);

  ns3::PacketMetadata::Enable ();
  //
  // Allow the user to override any of the defaults and the above Bind() at
  // run-time, via command-line arguments
  //
  // bool useV6 = false;
  // Address serverAddress;

  // CommandLine cmd;
  // cmd.AddValue ("useIpv6", "Use Ipv6", useV6);
  // cmd.Parse (argc, argv);

  //uint16_t port = 9;

  // Desired topology:  n0 <----> n1 <-----> n2
  //                    n3 <----> 
  // n0 and n1 in first container, n1 and n2 in second, n1 and n3 in third
  NS_LOG_INFO ("Create nodes.");
  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (2); //nCsma

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (csmaNodes);

  //        private address    NAT      public address
  // n0 <--------------------> n1 <-----------------------> n2
  // 192.168.1.1   192.168.1.2    203.82.48.1  203.82.48.2
  Ipv4AddressHelper address;
  address.SetBase ("8.8.8.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  Ipv4NatHelper natHelper;
  // The zeroth element of the second node container is the NAT node
  Ptr<Ipv4Nat> nat = natHelper.Install (csmaNodes.Get (0));
  // Configure which of its Ipv4Interfaces are inside and outside interfaces
  // The zeroth Ipv4Interface is reserved for the loopback interface
  // Hence, the interface facing n0 is numbered "1" and the interface
  // facing n2 is numbered "2" (since it was assigned in the second step above)
  nat->SetOutside (1);
  nat->SetInside (2);

  // Add a rule here to map outbound connections from n0, port 49153, for all protocols
  // Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.1.1"), 49153, Ipv4Address ("203.82.48.100"), 8081, 0);
  // Ipv4StaticNatRule rule3 (Ipv4Address ("192.168.2.1"), 49153, Ipv4Address ("203.82.48.100"), 8083, 0);
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.1.1"), Ipv4Address ("8.8.8.101"));
  Ipv4StaticNatRule rule2 (Ipv4Address ("192.168.1.2"), Ipv4Address ("8.8.8.102"));
  Ipv4StaticNatRule rule3 (Ipv4Address ("192.168.1.3"), Ipv4Address ("8.8.8.103"));
  Ipv4StaticNatRule rule4 (Ipv4Address ("192.168.1.4"), Ipv4Address ("8.8.8.104"));

  nat->AddStaticRule (rule1);
  nat->AddStaticRule (rule2);
  nat->AddStaticRule (rule3);
  nat->AddStaticRule (rule4);

  // Now print them out
  Ptr<OutputStreamWrapper> natStream = Create<OutputStreamWrapper> ("nat.rules", std::ios::out);
  nat->PrintTable (natStream);

  // Configure applications to generate traffic
  uint16_t port = 7;    // well-known echo port number

  //
  // Create a TcpEchoServerApplication and install it on n2
  //
  TcpEchoServerHelper echoServer (port);
  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //
  // Create a TcpEchoClient to send TCP packets to the server and install it on node 0
  //
  TcpEchoClientHelper echoClient2 (p2pInterfaces.GetAddress (0), port);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (183));
  ApplicationContainer clientApps2 = echoClient2.Install (csmaNodes.Get (1));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
 
  //
  // Create a TcpEchoClient to send TCP packets to the server and install it on node 3
  //
  // TcpEchoClientHelper echoClient3 (p2pInterfaces.GetAddress (0), port);
  // echoClient3.SetAttribute ("MaxPackets", UintegerValue (1));
  // echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  // echoClient3.SetAttribute ("PacketSize", UintegerValue (183));
  // ApplicationContainer clientApps3 = echoClient3.Install (csmaNodes.Get (2));
  // clientApps3.Start (Seconds (3.0));
  // clientApps3.Stop (Seconds (10.0));

  // Prepare to run the simulation
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  pointToPoint.EnablePcapAll ("ipv4-nat", false);
  csma.EnablePcapAll("ipv4-nat-csma", false);
  
  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_INFO ("Simulation Done.");
  
  // Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  // std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  return 0;
}
