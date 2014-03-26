Netfilter
---------

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

Netfilter is a packet filtering framework on Linux, used to implement
features such as connection tracking, network address translation, and
other packet mangling.

Model Description
*****************

The source code for this model lives in the directory ``src/internet/model``.

Design
======

The netfilter module is seamlessly integrated into the current ns-3 design. 
The design ensures that the netfilter is simply instantiated in the *Ipv4L3Protocol*
class to pass the packets that are intended to be filtered or passed through NAT operations. 


Netfilter
#########

Netfilter is primarily modeled around the five hooks:

* NF_INET_PRE_ROUTING
* NF_INET_LOCAL_IN
* NF_INET_FORWARD
* NF_INET_LOCAL_OUT
* NF_INET_POST_ROUTING

The hooks are placed in the IP stack where the packets are handed over to the netfilter framework.

`NF_INET_PRE_ROUTING`

This hook the first hook that is hit when a node receives a packet. This hook process the packet even before any routing decision is made for the packet. The hook is placed in the Ipv4L3Protocol::Receive method.

`NF_INET_LOCAL_IN`

This hook is processes packets that are destined to the node receiving it (as opposed to nodes that simple forward the packet). This hook is called after the decision for routing has been made. 
The hook is processed in 'Ipv4L3Protcol::LocalDeliver' method.

`NF_INET_FORWARD`

This hook is typically meant for packets that need to be passed onto other nodes.  This hook is traversed when the node is acting like a router.

`NF_INET_LOCAL_OUT`

This hook is traversed when the packet is created by the node and sent out by it. This hook is primarily meant for outgoing packets.
This hook is placed in the *Ipv4L3Protocol::Send*. Depending on the send cases this hook is appropriately placed.

`NF_INET_POST_ROUTING`

This the hook that is traversed after the routing decision has been made for outgoing packets. This hook is placed in the
``Ipv4L3Protocol::SendRealOut``.

Callback Chains are another critical part of the netfilter design. The callbacks for each hook are registered and added to a callback chain specific to it. These callbacks are also registered with specific priorities in order to be able to process them appropriately. 
For example one would process connection tracking before performing NAT, this would mean the connection tracking related callbacks would have a higher priority than that of NAT.

Conntrack
#########

NAT
###

Scope and Limitations
=====================

Currently the netfilter supports Hooks registering callbacks at the following points:

* Traffic going through the box:
    registered at PREROUTING
    registered at POSTROUTING

* Traffic leaving the box:
    registered at LOCAL_OUT
    registered at POSTROUTING

* Traffic entering the box:
    registered at PREROUTING
    registered at LOCAL_IN

The limitations to the current Netfilter design:
* Supports IPv4 only
* Ipv4 fragmentation not supported
* Application-level helpers such as FTP helpers are not currently in place.
* Support for packet mangling and filtering needs to be developed.

References
==========

Usage
*****
The usage of netfilter module would mainly involve introducing various callbacks to the specific hooks as per the requirement of the network scenario.This callback is the method that is going to govern what would happen to the packet that is being handled by the netfilter module.

Callbacks can be added to the code in two ways: 

1. By means of adding examples. Here the examples would perform relatively trivial operations to the packets and also need not be invoked every single time the module is processed(as opposed to conn track or NAT).
The following illustrates how callbacks are added to the netfilter framework by means of examples: 

The callback would have to be of type NetfilterHookCallback in order to be able to process and register to a Netfilter Hook:::

  
  static uint32_t HookRegistered(Hooks_t hook, Ptr<Packet> packet, Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb)

Here the callback 'HookRegistered' is defined as a method on the example code. This is the method that is going to be called later when the callback is registered on the specific hook in the order or decreasing priority.

The next step would be to instantiate netfilter object so that it can be used to register the hooks:::

  Ptr <Ipv4> ipv4 = first.Get (0)->GetObject<Ipv4> ();
  std::cout << "==============Number of interfaces on node " << first.Get (0)->GetId() << ": " << ipv4->GetNInterfaces () << std::endl;

  Ptr <Ipv4L3Protocol> ipv4L3 = DynamicCast <Ipv4L3Protocol>(first.Get (0)->GetObject<Ipv4> ());
  Ptr <Ipv4Netfilter>  netfilter = ipv4L3->GetNetfilter ();

Once you have the method defined we can move on to creating and registering the Hooks and their callbacks. 
The NetfilterHookCallback type callback is created by passing the reference to the method we have created above:::


  NetfilterHookCallback nodehook = MakeCallback (&HookRegistered);

Once this is done the hook is then created:::


  Ipv4NetfilterHook nfh = Ipv4NetfilterHook (1, NF_INET_PRE_ROUTING, NF_IP_PRI_FILTER , nodehook); 

Here the Priority of the callback is also set so that the callback is registered with that priority.
Then simple calling the following would register the callback on the above specified hook:::

  netfilter->RegisterHook (nfh);

2. Adding callbacks to the actual netfilter code is also an option however it is recommended that this is done purely for developmental purposes to enhance the module and not for testing examples.


Helper
======

What helper API will users typically use?  Describe it here.

Attributes
==========


Advanced Usage
==============

Go into further details (such as using the API outside of the helpers)
in additional sections, as needed.

Examples
========

The following examples have been written, which can be found in ``src/internet/examples``:

* netfilter-three-node.cc : This example basically models a three node network scenario where the endpoint nodes are connected by the middle node that is considered to be a router.The example illustrates the packet going through all the hooks and traversing them in the expected order. A specific callback is registered in the example in order to print out the current node and hook.

* netfilter-add-remove.cc : This example is used to illustrate the hook callback priority enforcement of the netfilter framework. A specific hook(NF_INET_FORWARD) is considered and multiple callbacks are registered at different priorities.Hooks are then registered and deregistered and it is observed that the hook callback priority is still maintained.


Validation
**********
The test suite is under construction.

