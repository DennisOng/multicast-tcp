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
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NetfilterExample");

int
main (int argc, char *argv[])
{
  LogComponentEnable ("BulkSendApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);

  //uint16_t port = 9;

  // Desired topology:  n0 <----> n1 <-----> n2
  //                    n3 <----> 
  // n0 and n1 in first container, n1 and n2 in second, n1 and n3 in third

  NodeContainer first;
  first.Create (2);

  NodeContainer second;
  second.Add ( first.Get (1) );
  second.Create (1);

  NodeContainer third;
  third.Create(1);
  third.Add ( first.Get(1) );

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices1;
  devices1 = pointToPoint.Install (first);

  NetDeviceContainer devices2;
  devices2 = pointToPoint.Install (second);

  NetDeviceContainer devices3;
  devices3 = pointToPoint.Install (third);

  InternetStackHelper stack;
  stack.Install (first);
  stack.Install (second.Get (1));
  stack.Install (third.Get (0));

  //        private address    NAT      public address
  // n0 <--------------------> n1 <-----------------------> n2
  // 192.168.1.1   192.168.1.2    203.82.48.1  203.82.48.2
  Ipv4AddressHelper address1;
  address1.SetBase ("192.168.1.0", "255.255.255.0");

  Ipv4AddressHelper address2;
  address2.SetBase ("203.82.48.0", "255.255.255.0");

  Ipv4AddressHelper address3;
  address3.SetBase ("192.168.2.0", "255.255.255.0");

  Ipv4InterfaceContainer firstInterfaces = address1.Assign (devices1);
  Ipv4InterfaceContainer secondInterfaces = address2.Assign (devices2);
  Ipv4InterfaceContainer thirdInterfaces = address3.Assign (devices3);

  Ipv4NatHelper natHelper;
  // The zeroth element of the second node container is the NAT node
  Ptr<Ipv4Nat> nat = natHelper.Install (second.Get (0));
  // Configure which of its Ipv4Interfaces are inside and outside interfaces
  // The zeroth Ipv4Interface is reserved for the loopback interface
  // Hence, the interface facing n0 is numbered "1" and the interface
  // facing n2 is numbered "2" (since it was assigned in the second step above)
  nat->SetInside (1);
  nat->SetOutside (2);
  nat->SetInside (3);

  // Add a rule here to map outbound connections from n0, port 49153, for all protocols
  // Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.1.1"), 49153, Ipv4Address ("203.82.48.100"), 8081, 0);
  // Ipv4StaticNatRule rule3 (Ipv4Address ("192.168.2.1"), 49153, Ipv4Address ("203.82.48.100"), 8083, 0);
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.1.1"), Ipv4Address ("203.82.48.101"));
  Ipv4StaticNatRule rule3 (Ipv4Address ("192.168.2.1"), Ipv4Address ("203.82.48.103"));

  nat->AddStaticRule (rule1);
  nat->AddStaticRule (rule3);

  // Now print them out
  Ptr<OutputStreamWrapper> natStream = Create<OutputStreamWrapper> ("nat.rules", std::ios::out);
  nat->PrintTable (natStream);

  // Configure applications to generate traffic
  uint16_t port = 10;    // well-known echo port number

  //
  // Create a PacketSinkApplication and install it as the server on n2
  //
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (second.Get (1));
  sinkApps.Start (Seconds (1.0));
  sinkApps.Stop (Seconds (10.0));

  //
  // Create a BulkSendApplication and install it on node 0
  //
  BulkSendHelper source1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (secondInterfaces.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source1.SetAttribute ("MaxBytes", UintegerValue (10000));
  ApplicationContainer sourceApps1 = source1.Install (first.Get (0));
  sourceApps1.Start (Seconds (2.0));
  sourceApps1.Stop (Seconds (10.0));
 
  //
  // Create a BulkSendApplication and install it on node 3
  //
  BulkSendHelper source3 ("ns3::TcpSocketFactory",
                         InetSocketAddress (secondInterfaces.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source3.SetAttribute ("MaxBytes", UintegerValue (10000));
  ApplicationContainer sourceApps3 = source3.Install (third.Get (1));
  sourceApps3.Start (Seconds (2.0));
  sourceApps3.Stop (Seconds (10.0));

  // Prepare to run the simulation
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  pointToPoint.EnablePcapAll ("ipv4-nat", false);

  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_INFO ("Simulation Done.");
  
  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  return 0;
}
