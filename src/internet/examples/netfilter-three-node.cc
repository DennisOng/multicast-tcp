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

static uint32_t
HookRegistered(Hooks_t hook, Ptr<Packet> packet, Ptr<NetDevice> in,
               Ptr<NetDevice> out, ContinueCallback& ccb)
{ 
  const char* hooknames[] = {"NF_INET_PRE_ROUTING","NF_INET_LOCAL_IN","NF_INET_FORWARD","NF_INET_LOCAL_OUT","NF_INET_POST_ROUTING","NF_INET_NUMHOOKS"};

  if(out == 0 && in == 0)
    std::cout<<"-1"<<std::endl;
  else
    if(out == 0 && in!= 0)
      std::cout<<"********On Node "<<in->GetNode()->GetId()<<" "<<hooknames[hook]<<" hit***********"<<std::endl;
    else
      if(out!=0 && in == 0) 
        std::cout<<"********On Node "<<out->GetNode()->GetId()<<" "<<hooknames[hook]<<" hit***********"<<std::endl;
    return 0;

  
}

int
main (int argc, char *argv[])
{
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  uint16_t port = 9;

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

  Ipv4AddressHelper address1;
  address1.SetBase ("192.168.1.0", "255.255.255.0");

  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.1.0", "255.255.255.0");
 
  Ipv4InterfaceContainer firstInterfaces = address1.Assign (devices1);
  Ipv4InterfaceContainer secondInterfaces = address2.Assign (devices2);
  
  
  //Hook Regsitering on Node 0
  Ptr <Ipv4> ipv4 = first.Get (0)->GetObject<Ipv4> ();
  std::cout << "==============Number of interfaces on node " << first.Get (0)->GetId() << ": " << ipv4->GetNInterfaces () << std::endl;
  
  Ptr <Ipv4L3Protocol> ipv4L3 = DynamicCast <Ipv4L3Protocol>(first.Get (0)->GetObject<Ipv4> ());
  Ptr <Ipv4Netfilter>  netfilter = ipv4L3->GetNetfilter ();
  
  NetfilterHookCallback nodehook = MakeCallback (&HookRegistered);
  
  Ipv4NetfilterHook nfh = Ipv4NetfilterHook (1, NF_INET_PRE_ROUTING, NF_IP_PRI_FILTER , nodehook); 
  Ipv4NetfilterHook nfh1 = Ipv4NetfilterHook (1, NF_INET_LOCAL_IN, NF_IP_PRI_FILTER, nodehook); 
  Ipv4NetfilterHook nfh2 = Ipv4NetfilterHook (1, NF_INET_LOCAL_OUT, NF_IP_PRI_FILTER, nodehook); 
  Ipv4NetfilterHook nfh3 = Ipv4NetfilterHook (1, NF_INET_POST_ROUTING, NF_IP_PRI_FILTER, nodehook);
 
  netfilter->RegisterHook (nfh);
  netfilter->RegisterHook (nfh1);
  netfilter->RegisterHook (nfh2);
  netfilter->RegisterHook (nfh3);


  //Hook Registering on Node 1
  ipv4=first.Get (1)->GetObject<Ipv4> ();
  std::cout << "==============Number of interfaces on node " << first.Get (1)->GetId() << ": " << ipv4->GetNInterfaces () << std::endl;
  
  ipv4L3 = DynamicCast <Ipv4L3Protocol>(first.Get (1)->GetObject<Ipv4> ());
  netfilter = ipv4L3->GetNetfilter ();

  nodehook = MakeCallback (&HookRegistered);
  
  nfh = Ipv4NetfilterHook (1, NF_INET_PRE_ROUTING, NF_IP_PRI_FILTER , nodehook); 
  nfh1 = Ipv4NetfilterHook (1, NF_INET_FORWARD, NF_IP_PRI_FILTER, nodehook); 
  nfh2 = Ipv4NetfilterHook (1, NF_INET_POST_ROUTING, NF_IP_PRI_FILTER, nodehook);
 
  netfilter->RegisterHook (nfh);
  netfilter->RegisterHook (nfh1);
  netfilter->RegisterHook (nfh2);


  //Hook Registering on Node 2
  ipv4=second.Get(1)->GetObject<Ipv4> ();
  std::cout << "==============Number of interfaces on node " << second.Get (1)->GetId() << ": " << ipv4->GetNInterfaces () << std::endl;
  
  ipv4L3 = DynamicCast <Ipv4L3Protocol>(second.Get (1)->GetObject<Ipv4> ());
  netfilter = ipv4L3->GetNetfilter ();
  
  nodehook = MakeCallback (&HookRegistered);
  
  nfh = Ipv4NetfilterHook (1, NF_INET_PRE_ROUTING, NF_IP_PRI_FILTER , nodehook); 
  nfh1 = Ipv4NetfilterHook (1, NF_INET_LOCAL_IN, NF_IP_PRI_FILTER, nodehook); 
  nfh2 = Ipv4NetfilterHook (1, NF_INET_LOCAL_OUT, NF_IP_PRI_FILTER, nodehook); 
  nfh3 = Ipv4NetfilterHook (1, NF_INET_POST_ROUTING, NF_IP_PRI_FILTER, nodehook);
 
  netfilter->RegisterHook (nfh);
  netfilter->RegisterHook (nfh1);
  netfilter->RegisterHook (nfh2);
  netfilter->RegisterHook (nfh3);

  UdpEchoServerHelper echoServer (port);

  ApplicationContainer serverApps = echoServer.Install (second.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (secondInterfaces.GetAddress (1), port);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (512));

  ApplicationContainer clientApps = echoClient.Install (first.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  pointToPoint.EnablePcapAll ("netfilter", false);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
