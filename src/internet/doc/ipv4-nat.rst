Network Address Translation (NAT)
---------------------------------

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

Network address translation (NAT) is a process of altering the 
source/destination IP address of a packet in order to make it routable over 
a network. This is usually applied when packets are passed from a private
address realm to a public network.
Network address translation is also used in IP address hiding where hosts 
can be on the address behind the NAT device and cannot be directly 
reached from an external network.  NAT also comes with a number of variants 
in terms of what fieldd of the packet is translated through the NAT (e.g. 
port numbers) or the directions of the NAT (e.g. unidirectional vs 
bidirectional).

While NAT is mainly deployed for operational purposes (managing IP address
space), the inclusion of simulation models of NAT may be helpful to developers
of peer-to-peer applications for ns-3 who wish to evaluate the NAT traversal
aspects of the application.

Model Description
*****************

The source code for this model lives in the directory ``src/internet/model``.

Design
======

The design of NAT for ns-3 is basically divided into two main categories: 

- Static NAT:  This type of NAT allows IP flows to be established in either
  direction. It is designed to perform host to host NAT and also has a 
  variant to specify the nat for specific protocol and port.  In practice,
  this type of NAT may be more often found in enterprise environments.

- Dynamic NAT: This type of NAT typically allows IP flows to be established
  only in one direction, from private address realm to public address realm.
  Often, multiple hosts may be multiplexed onto a single public IP address
  via port translation.  The NAT state is dynamic and times out after a 
  period of inactivity.  In practice, this type of NAT may be more often
  found in home networks.

Scope and Limitations
=====================

- The NAT is currently limited to host-to-host. It can be extended to network to network.
- The NAT is not completely bound with connection tracking. This also needs some work.

References
==========

`IP Address Translation, Michael Hasenstein, <http://hasenstein.com/linux-ip-nat/nat-document.pdf>`_

Usage
*****

NAT is a capability that is aggregated to a node that includes an IPv4 stack
with Netfilter capability.  From a user perspective, NAT is typically
configured by creating the NAT object and adding rules to the table.

The following are the steps of things to be done when converting a node to a 
NAT node:

::
 
   // obtain a node pointer "node"
   Ipv4NatHelper natHelper
   Ptr<Ipv4Nat> nat = natHelper.Install (node);

Next, define the inside and the outside interfaces of the NAT. These are 
the interfaces between which the nat will be processed.  The Ipv4Interface
indices are used to refer to the NAT interfaces:

::

   nat->SetInside (1);
   nat->SetOutside (2);

NAT is configured to block all traffic that does not match one of the 
configured rules.  The user would configure rules next, such as follows:

::

   // specify local and global IP addresses
   Ipv4StaticNatRule rule (Ipv4Address ("192.168.1.1"), Ipv4Address ("203.82.48.100"));
   nat->AddStaticRule (rule);

The following code will help printing the NAT rules to a file nat.rules from 
the stream:

::
 
   Ptr<OutputStreamWrapper> natStream = Create<OutputStreamWrapper> ("nat.rules", std::ios::out);
   nat->PrintTable (natStream);

The above illustrates a typical example of a one to one static NAT. The other 
variant in the static nat rule for which the ports can be defined:

::

   Ipv4StaticNatRule rule2 (Ipv4Address ("192.168.2.3"), 80, Ipv4Address ("10.1.3.4"), 8080, 0);
   nat->AddStaticRule (rule2);

The above rule would translate 192.168.2.3:80 in the private realm to
10.1.3.4:8080 in the public realm, for all protocols (last field is zero).

Helper
======

The Ipv4NatHelper is structured like other ns-3 helpers and can be used
to ``Install()`` a NAT object to a node and return a pointer to the NAT.
The source code can be found in:
``src/internet/helper/ipv4-nat-helper.{cc,h}``:

Attributes
==========

There are no attributes used in this model.

Advanced Usage
==============


Examples
========

The following example has been written, which can be found in 
``src/internet/examples``:

- ipv4-nat-example.cc : This example basically illustrates a three node 
architecture where the middle node is a NAT device. The static NAT rule is 
added to the NAT table and printed out to a nat.rules file.

Validation
**********
A test suite exists in ``src/internet/examples/ipv4-nat-test-suite.cc``.

