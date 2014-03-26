/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

// An essential include is test.h
#include "ns3/test.h"
// Include any headers files needed for testing your module
#include "ns3/ipv4.h"

// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

class Ipv4NetfilterTestCase1 : public TestCase
{
public:
  Ipv4NetfilterTestCase1 ();
  virtual ~Ipv4NetfilterTestCase1 ();

private:
  virtual void DoRun (void);
};

// Add some help text to this case to describe what it is intended to test
Ipv4NetfilterTestCase1::Ipv4NetfilterTestCase1 ()
  : TestCase ("Ipv4Netfilter test case (does nothing)")
{
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
Ipv4NetfilterTestCase1::~Ipv4NetfilterTestCase1 ()
{
}

//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
Ipv4NetfilterTestCase1::DoRun (void)
{
  // A wide variety of test macros are available in src/core/test.h
  NS_TEST_ASSERT_MSG_EQ (true, true, "true doesn't equal true for some reason");
  // Use this one for floating point comparisons
  NS_TEST_ASSERT_MSG_EQ_TOL (0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

class Ipv4NetfilterTestSuite : public TestSuite
{
public:
  Ipv4NetfilterTestSuite ();
};

Ipv4NetfilterTestSuite::Ipv4NetfilterTestSuite ()
  : TestSuite ("ipv4-netfilter", UNIT)
{
  AddTestCase (new Ipv4NetfilterTestCase1);
}

static Ipv4NetfilterTestSuite ipv4NetfilterTestSuite;

