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
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  //uint16_t port = 9;

  // Desired topology:  n0 <----> n1 <-----> n2
  // n0 and n1 in first container, n1 and n2 in second

  NodeContainer first;
  first.Create (2);

  NodeContainer second;
  second.Add ( first.Get (1) );
  second.Create (1);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices1;
  devices1 = pointToPoint.Install (first);

  NetDeviceContainer devices2;
  devices2 = pointToPoint.Install (second);

  InternetStackHelper stack;
  stack.Install (first);
  stack.Install (second.Get (1));

  //        private address    NAT      public address
  // n0 <--------------------> n1 <-----------------------> n2
  // 192.168.1.1   192.168.1.2    203.82.48.1  203.82.48.2
  Ipv4AddressHelper address1;
  address1.SetBase ("192.168.1.0", "255.255.255.0");

  Ipv4AddressHelper address2;
  address2.SetBase ("203.82.48.0", "255.255.255.0");

  Ipv4InterfaceContainer firstInterfaces = address1.Assign (devices1);
  Ipv4InterfaceContainer secondInterfaces = address2.Assign (devices2);

  Ipv4NatHelper natHelper;
  // The zeroth element of the second node container is the NAT node
  Ptr<Ipv4Nat> nat = natHelper.Install (second.Get (0));
  // Configure which of its Ipv4Interfaces are inside and outside interfaces
  // The zeroth Ipv4Interface is reserved for the loopback interface
  // Hence, the interface facing n0 is numbered "1" and the interface
  // facing n2 is numbered "2" (since it was assigned in the second step above)
  nat->SetInside (1);
  nat->SetOutside (2);

  // Add a rule here to map outbound connections from n0, port 49153, UDP
  Ipv4StaticNatRule rule2 (Ipv4Address ("192.168.1.1"), 49153, Ipv4Address ("203.82.48.100"), 8080, IPPROTO_UDP);
  nat->AddStaticRule (rule2);

  // Now print them out
  Ptr<OutputStreamWrapper> natStream = Create<OutputStreamWrapper> ("nat.rules", std::ios::out);
  nat->PrintTable (natStream);

  // Configure applications to generate traffic
  UdpEchoServerHelper echoServer (9);

  // This application corresponds to the first rule
  ApplicationContainer serverApps = echoServer.Install (second.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (secondInterfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (512));

  ApplicationContainer clientApps = echoClient.Install (first.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  // Prepare to run the simulation
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  pointToPoint.EnablePcapAll ("ipv4-nat", false);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
