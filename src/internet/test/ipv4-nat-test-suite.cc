/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2012 University of Washington
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
 */

#include "ns3/test.h"
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/ipv4-nat.h"
#include "ns3/ipv4-address.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4.h"

using namespace ns3;

class Ipv4NatAddRemoveRules : public TestCase
{
public:
  Ipv4NatAddRemoveRules ();
  virtual ~Ipv4NatAddRemoveRules ();

private:
  virtual void DoRun (void);
};

Ipv4NatAddRemoveRules::Ipv4NatAddRemoveRules ()
  : TestCase ("Add and remove NAT rules from object")
{
}

Ipv4NatAddRemoveRules::~Ipv4NatAddRemoveRules ()
{
}

void
Ipv4NatAddRemoveRules::DoRun (void)
{
  // Create single NAT test node with outer interface of 203.0.113.1 and 
  // inner interface of 192.168.0.1
  Ptr<Node> testNode = CreateObject<Node> ();
  InternetStackHelper stack;
  stack.Install (testNode);

  Ptr<SimpleNetDevice> rxDev1, rxDev2;
  { // first, outside interface
    rxDev1 = CreateObject<SimpleNetDevice> ();
    rxDev1->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev1);
    Ptr<Ipv4> ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev1);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("203.0.113.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }
  { // second, inside interface
    rxDev2 = CreateObject<SimpleNetDevice> ();
    rxDev2->SetAddress (Mac48Address::ConvertFrom (Mac48Address::Allocate ()));
    testNode->AddDevice (rxDev2);
    Ptr<Ipv4> ipv4 = testNode->GetObject<Ipv4> ();
    uint32_t netdev_idx = ipv4->AddInterface (rxDev2);
    Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (Ipv4Address ("192.168.0.1"), Ipv4Mask (0xffffff00U));
    ipv4->AddAddress (netdev_idx, ipv4Addr);
    ipv4->SetUp (netdev_idx);
  }

  Ptr<Ipv4Nat> nat = CreateObject<Ipv4Nat> ();
  nat->SetOutside (1);
  nat->SetInside (2);
  testNode->AggregateObject (nat);

  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 0, "list is not initialized empty");
  Ipv4StaticNatRule rule1 (Ipv4Address ("192.168.0.3"), Ipv4Address ("203.0.113.103"));
  Ipv4StaticNatRule rule2 (Ipv4Address ("192.168.0.4"), Ipv4Address ("203.0.113.104"));
  nat->AddStaticRule (rule1);
  nat->AddStaticRule (rule2);
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 2, "adding to list failed");

  nat->RemoveStaticRule (1);  // should remove the 192.168.0.3 rule, as 192.168.0.4 pushed it down the stack
  NS_TEST_ASSERT_MSG_EQ (nat->GetNStaticRules (), 1, "removing from list failed");
  Ipv4StaticNatRule returnRule = nat->GetStaticRule (0);
  NS_TEST_ASSERT_MSG_EQ (returnRule.GetLocalIp (), Ipv4Address ("192.168.0.4"), "fetching from list failed");


}

class Ipv4NatStatic : public TestCase
{
public:
  Ipv4NatStatic ();
  virtual ~Ipv4NatStatic ();

private:
  virtual void DoRun (void);
};

Ipv4NatStatic::Ipv4NatStatic ()
  : TestCase ("Test that NAT works with static rules")
{
}

Ipv4NatStatic::~Ipv4NatStatic ()
{
}

void
Ipv4NatStatic::DoRun (void)
{
}

class Ipv4NatTestSuite : public TestSuite
{
public:
  Ipv4NatTestSuite ();
};

Ipv4NatTestSuite::Ipv4NatTestSuite ()
  : TestSuite ("ipv4-nat", UNIT)
{
  AddTestCase (new Ipv4NatAddRemoveRules);
  AddTestCase (new Ipv4NatStatic);
}

static Ipv4NatTestSuite ipv4NatTestSuite;

