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
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ipv4-netfilter.h"

#include "ip-conntrack-info.h"
#include "ipv4-conntrack-l3-protocol.h"
#include "tcp-conntrack-l4-protocol.h"
#include "udp-conntrack-l4-protocol.h"
#include "icmpv4-conntrack-l4-protocol.h"

#include "tcp-header.h"
#include "udp-header.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/output-stream-wrapper.h"
#include "ipv4-nat.h"
#include "ipv4.h"

#include "ns3/mac-packet-tag.h"

#include <iomanip>

NS_LOG_COMPONENT_DEFINE ("Ipv4Nat");

namespace ns3 {

Ipv4NetfilterHook natCallback1;
Ipv4NetfilterHook natCallback2;

NS_OBJECT_ENSURE_REGISTERED (Ipv4Nat);

TypeId
Ipv4Nat::GetTypeId (void)
{
  static TypeId tId = TypeId ("ns3::Ipv4Nat")
    .SetParent<Object> ()
  ;

  return tId;
}

Ipv4Nat::Ipv4Nat ()
  : m_insideInterface (-1),
    m_outsideInterface (-1)
{
  NS_LOG_FUNCTION (this);

  NetfilterHookCallback doNatPreRouting = MakeCallback (&Ipv4Nat::DoNatPreRouting, this);
  NetfilterHookCallback doNatPostRouting = MakeCallback (&Ipv4Nat::DoNatPostRouting, this);

  natCallback1 = Ipv4NetfilterHook (1, NF_INET_POST_ROUTING, NF_IP_PRI_NAT_SRC, doNatPostRouting);
  natCallback2 = Ipv4NetfilterHook (1, NF_INET_PRE_ROUTING, NF_IP_PRI_NAT_DST, doNatPreRouting);

}

/*
 * This method is called by AddAgregate and completes the aggregation
 * by hooking to the Ipv4Netfilter
 */
void
Ipv4Nat::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION (this);
  if (m_ipv4 != 0)
    {
      return;
    }
  Ptr<Node> node = this->GetObject<Node> ();
  if (node != 0)
    {
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      if (ipv4 != 0)
        {
          Ptr<Ipv4Netfilter> netfilter = ipv4->GetNetfilter ();
          if (ipv4 != 0)
            {
              m_ipv4 = ipv4;
              // Set callbacks on netfilter pointer

              netfilter->RegisterHook (natCallback1);
              netfilter->RegisterHook (natCallback2);

            }
        }
    }
  Object::NotifyNewAggregate ();
}

uint32_t
Ipv4Nat::GetNStaticRules (void) const
{
  NS_LOG_FUNCTION (this);
  return m_statictable.size ();
}

Ipv4StaticNatRule
Ipv4Nat::GetStaticRule (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;
  for (StaticNatRules::const_iterator i = m_statictable.begin ();
       i != m_statictable.end ();
       i++)
    {
      if (tmp == index)
        {
          return *i;
        }
      tmp++;
    }
  NS_ASSERT (false);

  return Ipv4StaticNatRule (Ipv4Address (), Ipv4Address ());
}

Ipv4DynamicNatRule
Ipv4Nat::GetDynamicRule (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;
  for (DynamicNatRules::const_iterator i = m_dynamictable.begin ();
       i != m_dynamictable.end ();
       i++)
    {
      if (tmp == index)
        {
          return *i;
        }
      tmp++;
    }
  NS_ASSERT (false);

  return Ipv4DynamicNatRule (Ipv4Address (), Ipv4Mask ());
}

Ipv4DynamicNatTuple
Ipv4Nat::GetDynamicTuple (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;
  for (DynamicNatTuple::const_iterator i = m_dynatuple.begin ();
       i != m_dynatuple.end ();
       i++)
    {
      if (tmp == index)
        {
          return *i;
        }
      tmp++;
    }
  NS_ASSERT (false);

  return Ipv4DynamicNatTuple (Ipv4Address (), Ipv4Address (), uint16_t ('0'));
}


uint32_t
Ipv4Nat::GetNDynamicRules (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dynamictable.size ();
}

uint32_t
Ipv4Nat::GetNDynamicTuples (void) const
{
  NS_LOG_FUNCTION (this);
  return m_dynatuple.size ();
}

void
Ipv4Nat::RemoveStaticRule (uint32_t index)
{

  NS_LOG_FUNCTION (this << index);
  NS_ASSERT (index < m_statictable.size ());
  uint32_t tmp = 0;
  for (StaticNatRules::iterator i = m_statictable.begin ();
       i != m_statictable.end (); i++, tmp++)
    {
      if (tmp == index)
        {
          m_statictable.erase (i);
          return;
        }
    }
  NS_ASSERT_MSG (false, "Rule Not Found");
}

void
Ipv4Nat::RemoveDynamicRule (uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  NS_ASSERT (index < m_dynamictable.size ());
  uint32_t tmp = 0;
  for (DynamicNatRules::iterator i = m_dynamictable.begin ();
       i != m_dynamictable.end (); i++, tmp++)
    {
      if (tmp == index)
        {
          m_dynamictable.erase (i);
          return;
        }
    }
  NS_ASSERT_MSG (false, "Rule Not Found");
}


/**
 * \brief Print the NAT translation table
 *
 * \param stream the ostream the NAT table is printed to
 */

void
Ipv4Nat::PrintTable (Ptr<OutputStreamWrapper> stream) const

{
  NS_LOG_FUNCTION (this);
  std::ostream* os = stream->GetStream ();
  if (GetNStaticRules () > 0)
    {
      *os << "       Static Nat Rules" << std::endl;
      *os << "Local IP     Local Port     Global IP    Global Port " << std::endl;
      for (uint32_t i = 0; i < GetNStaticRules (); i++)
        {
          std::ostringstream locip,gloip,locprt,gloprt;
          Ipv4StaticNatRule rule = GetStaticRule (i);

          if (rule.GetLocalPort ())
            {
              locip << rule.GetLocalIp ();
              *os << std::setiosflags (std::ios::left) << std::setw (16) << locip.str ();

              locprt << rule.GetLocalPort ();
              *os << std::setiosflags (std::ios::left) << std::setw (16) << locprt.str ();
            }

          else
            {
              locip << rule.GetLocalIp ();
              *os << std::setiosflags (std::ios::left) << std::setw (35) << locip.str ();
            }


          gloip << rule.GetGlobalIp ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << gloip.str ();

          if (rule.GetGlobalPort ())
            {
              gloprt << rule.GetGlobalPort ();
              *os << std::setiosflags (std::ios::left) << std::setw (16) << gloprt.str ();
            }

          *os << std::endl;

        }
    }

  if (GetNDynamicRules () > 0)
    {
      *os << std::endl;
      *os << "       Dynamic Nat Rules" << std::endl;
      *os << "Local Network          Local Netmask" << std::endl;
      for (uint32_t i = 0; i < GetNDynamicRules (); i++)
        {
          std::ostringstream locnet,locmask,gloip,glomask,strtprt,endprt;
          Ipv4DynamicNatRule rule = GetDynamicRule (i);

          locnet << rule.GetLocalNet ();
          *os << std::setiosflags (std::ios::left) << std::setw (32) << locnet.str ();

          locmask << rule.GetLocalMask ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << locmask.str ();
        }

      *os << std::endl;

    }

  if (GetNDynamicTuples () > 0)
    {
      *os << std::endl;
      *os << "       Current Dynamic Translations" << std::endl;
      *os << "Local IP             Global IP           Translated Port" << std::endl;
      for (uint32_t i = 0; i < GetNDynamicTuples (); i++)
        {
          std::ostringstream locip,gloip,prt;
          Ipv4DynamicNatTuple tup = GetDynamicTuple (i);

          locip << tup.GetLocalAddress ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << locip.str ();

          gloip << tup.GetGlobalAddress ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << gloip.str ();

          prt << tup.GetTranslatedPort ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << prt.str ();

          *os << std::endl;
        }

      *os << std::endl;
    }
}

uint32_t
Ipv4Nat::DoNatPreRouting (Hooks_t hookNumber, Ptr<Packet> p,
                          Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb)
{
  NS_LOG_FUNCTION (this << p << hookNumber << in << out);

  if (m_ipv4 == 0)
    {
      return 0;
    }

  Ipv4Header ipHeader;

  NS_LOG_DEBUG ("Input device " << m_ipv4->GetInterfaceForDevice (in) << " inside interface " << m_insideInterface);
  NS_LOG_DEBUG ("Output device " << m_ipv4->GetInterfaceForDevice (out) << " outside interface " << m_outsideInterface);
  p->RemoveHeader (ipHeader);
  if (m_ipv4->GetInterfaceForDevice (in) == m_outsideInterface)
    {
      // outside interface is the input interface, NAT the destination addr
      // so that the NAT does not try to locally deliver the packet
      NS_LOG_DEBUG ("evaluating packet with src " << ipHeader.GetSource () << " dst " << ipHeader.GetDestination ());
      Ipv4Address destAddress = ipHeader.GetDestination ();

      //Checking for Static NAT Rules
      for (StaticNatRules::const_iterator i = m_statictable.begin ();
           i != m_statictable.end (); i++)
        {
          if (destAddress != (*i).GetGlobalIp ())
            {
              NS_LOG_DEBUG ("Skipping rule with global IP " << (*i).GetGlobalIp ());
              continue;
            }
          if ((*i).GetGlobalPort () == 0)
            {
              NS_LOG_DEBUG ("Rule match with a non-port-specific rule");
              bool isIpHeaderModified = false;
              // ---------------------------
              // My Modifications:
              // ---------------------------
              // read the tag from the packet copy
              // MacTag tag;
              // tag.SetSimpleValue (0x65);

              // // store the tag in the packet.
              // p->AddPacketTag (tag);
              // NS_LOG_UNCOND("Packet tag set");
              // MacTag tagCopy;
              // p->PeekPacketTag (tagCopy);
              // NS_LOG_UNCOND("Decode tags");
              NS_LOG_UNCOND("NAT Incoming:");
              // p->PrintPacketTags (std::cout);
              // std::cout << std::endl;
              MacTag tag;
              p->PeekPacketTag (tag);
              NS_LOG_UNCOND("[MAC_src=" << tag.GetSrcMac() << " MAC_dest=" << tag.GetDstMac() 
                            << "] (IP_src=" << ipHeader.GetSource() << " IP_dst=" << ipHeader.GetDestination() <<")");
              // if (tag.GetSrcMac() == Mac48Address ("00:00:00:00:00:04"))
              //   {
              //     NS_LOG_UNCOND("IN HERE!");
              //     NS_ASSERT(ipHeader.GetSource().IsEqual(Ipv4Address ("192.168.1.2")));
              //     ipHeader.SetSource(Ipv4Address ("8.8.8.103"));
              //     isIpHeaderModified = true;
              //   }

              if (ipHeader.GetDestination().IsEqual(Ipv4Address ("8.8.8.104")))
                {
                  ipHeader.SetDestination(Ipv4Address ("192.168.1.2"));
                  isIpHeaderModified = true;
                }
              // MacTag tag;
              // p->PeekPacketTag (tag);
              // NS_LOG_UNCOND("NAT rcv packet tag: src=" << tag.GetSrcMac() << " dest=" << tag.GetDstMac());


              // if (ipHeader.GetProtocol () == IPPROTO_TCP)
              //   {
              //     TcpHeader tcpHeader;
              //     p->RemoveHeader (tcpHeader);
              //     NS_LOG_INFO("Start of translator functions");
              //     NS_LOG_INFO("SEQ: " << tcpHeader.GetSequenceNumber());
              //     NS_LOG_INFO("ACK: " << tcpHeader.GetAckNumber());
              //     // Translator intercepts SYN and replies (SYN ACK) on behalf of video server
              //     if ((tcpHeader.GetFlags() & TcpHeader::SYN) && ipHeader.GetSource().IsEqual(Ipv4Address ("192.168.2.1")))
              //       NS_LOG_INFO("Hello World");
              //     p->AddHeader (tcpHeader);
              //   }


              // ---------------------------
              // End My Modifications:
              // ---------------------------
              if (!isIpHeaderModified)
                ipHeader.SetDestination ((*i).GetLocalIp ());
              p->AddHeader (ipHeader);

              return 0;
            }
          else
            {
              NS_LOG_DEBUG ("evaluating rule with local port " << (*i).GetLocalPort () << " global port " << (*i).GetGlobalPort ());

              if (ipHeader.GetProtocol () == IPPROTO_TCP
                  && (((*i).GetProtocol () == IPPROTO_TCP)
                      || (*i).GetProtocol () == 0))
                {
                  TcpHeader tcpHeader;
                  p->RemoveHeader (tcpHeader);
                  if (tcpHeader.GetDestinationPort () == (*i).GetGlobalPort ())
                    {
                      tcpHeader.SetDestinationPort ((*i).GetLocalPort ());
                    }
                  p->AddHeader (tcpHeader);
                }
              else if (ipHeader.GetProtocol () == IPPROTO_UDP
                       && (((*i).GetProtocol () == IPPROTO_UDP)
                           || (*i).GetProtocol () == 0))
                {
                  UdpHeader udpHeader;
                  p->RemoveHeader (udpHeader);
                  if (udpHeader.GetDestinationPort () == (*i).GetGlobalPort ())
                    {
                      udpHeader.SetDestinationPort ((*i).GetLocalPort ());
                    }
                  p->AddHeader (udpHeader);
                }
              NS_LOG_DEBUG ("Rule match with a port-specific rule");
              ipHeader.SetDestination ((*i).GetLocalIp ());
              p->AddHeader (ipHeader);
              return 0;
            }
        }


      //Passing traffic that has existing outgoing dynamic nat connections
      for (DynamicNatTuple::const_iterator i = m_dynatuple.begin ();
           i != m_dynatuple.end (); i++)
        {
          if (destAddress == GetAddressPoolIp ())
            {
              if (ipHeader.GetProtocol () == IPPROTO_TCP)

                {
                  TcpHeader tcpHeader;
                  if (tcpHeader.GetDestinationPort () == (*i).GetTranslatedPort ())
                    {
                      ipHeader.SetDestination ((*i).GetLocalAddress ());
                    }

                }
              else
                {
                  UdpHeader udpHeader;
                  if (udpHeader.GetDestinationPort () == (*i).GetTranslatedPort ())
                    {
                      ipHeader.SetDestination ((*i).GetLocalAddress ());
                    }
                }
            }
        }

    }
  p->AddHeader (ipHeader);
  return 0;
}

uint32_t
Ipv4Nat::DoNatPostRouting (Hooks_t hookNumber, Ptr<Packet> p,
                           Ptr<NetDevice> in, Ptr<NetDevice> out, ContinueCallback& ccb)
{
  NS_LOG_FUNCTION (this << p << hookNumber << in << out);

  if (m_ipv4 == 0)
    {
      return 0;
    }

  Ipv4Header ipHeader;

  NS_LOG_DEBUG ("Input device " << m_ipv4->GetInterfaceForDevice (in) << " inside interface " << m_insideInterface);
  NS_LOG_DEBUG ("Output device " << m_ipv4->GetInterfaceForDevice (out) << " outside interface " << m_outsideInterface);
  p->RemoveHeader (ipHeader);
  if (m_ipv4->GetInterfaceForDevice (out) == m_outsideInterface)
    {
      // matching output interface, consider whether to NAT the source
      // address and port
      NS_LOG_DEBUG ("evaluating packet with src " << ipHeader.GetSource () << " dst " << ipHeader.GetDestination ());
      Ipv4Address srcAddress = ipHeader.GetSource ();


      //Checking for Static NAT Rules
      for (StaticNatRules::const_iterator i = m_statictable.begin ();
           i != m_statictable.end (); i++)
        {
          if (srcAddress != (*i).GetLocalIp ())
            {
              NS_LOG_DEBUG ("Skipping rule with local IP " << (*i).GetLocalIp ());
              continue;
            }
          if ((*i).GetLocalPort () == 0)
            {
              NS_LOG_DEBUG ("Rule match with a non-port-specific rule");

              bool isIpHeaderModified = false;

              // ---------------------------
              // My Modifications:
              // ---------------------------
              // read the tag from the packet copy
              // MacTag tag;
              // tag.SetSimpleValue (0x66);

              // // store the tag in the packet.
              // p->AddPacketTag (tag);
              // NS_LOG_UNCOND("Packet tag set");
              // MacTag tagCopy;
              // p->PeekPacketTag (tagCopy);
              // p->PrintPacketTags (std::cout);
              NS_LOG_UNCOND("NAT Outgoing:");
              MacTag tag;
              p->PeekPacketTag (tag);
              NS_LOG_UNCOND("[MAC_src=" << tag.GetSrcMac() << " MAC_dest=" << tag.GetDstMac() 
                            << "] (IP_src=" << ipHeader.GetSource() << " IP_dst=" << ipHeader.GetDestination() <<")");

              // If the packet is from the first client (ie: nCSMA 1)
              if (tag.GetSrcMac() == Mac48Address ("00:00:00:00:00:04"))
                {
                  NS_ASSERT(ipHeader.GetSource().IsEqual(Ipv4Address ("192.168.1.2")));
                  ipHeader.SetSource(Ipv4Address ("8.8.8.104"));
                  isIpHeaderModified = true;

                  if (ipHeader.GetProtocol () == IPPROTO_TCP)
                    {
                      TcpHeader tcpHeader;
                      p->RemoveHeader (tcpHeader);
                      NS_LOG_INFO("Start of translator functions");
                      NS_LOG_INFO("SEQ: " << tcpHeader.GetSequenceNumber());
                      NS_LOG_INFO("ACK: " << tcpHeader.GetAckNumber());


                      // Translator intercepts SYN and replies (SYN ACK) on behalf of video server
                      if (tcpHeader.GetFlags() & TcpHeader::SYN)
                        {
                          isIpHeaderModified = true;
                          NS_LOG_INFO("SYN flag detected from client 1 [MAC_src=00:00:00:00:00:04], generating ACK");
                          tcpHeader.SetFlags(tcpHeader.GetFlags() | TcpHeader::ACK);
                          tcpHeader.SetAckNumber(tcpHeader.GetSequenceNumber() + SequenceNumber32 (1));
                          tcpHeader.SetSourcePort(7);
                          tcpHeader.SetDestinationPort(49153);
                          ipHeader.SetDestination(Ipv4Address ("192.168.1.2"));
                          ipHeader.SetSource(Ipv4Address ("8.8.8.1"));

                          MacTag oldtag;
                          p->PeekPacketTag (oldtag);
                          // NS_LOG_UNCOND("[MAC_src=" << oldtag.GetSrcMac() << " MAC_dest=" << oldtag.GetDstMac() 
                            // << "] (IP_src=" << ipHeader.GetSource() << " IP_dst=" << ipHeader.GetDestination() <<")");
                        
                          MacTag tag;
                          tag.SetSimpleValue (0x02);
                          tag.SetSrcMac (oldtag.GetDstMac());
                          tag.SetDstMac (oldtag.GetSrcMac());
                          // store the tag in the packet.
                          p->RemoveAllPacketTags ();
                          p->AddPacketTag (tag); 


                          // NEED TO SWAP SOURCe AND DEST MAC
                          // NEED TO CHECK THAT THIS PACKET GOES OUT OF THE CSMA INTERFACE
                        }
                      else
                        {
                          // Temporary solution for dropping packet - Send to Nowhere
                          // ipHeader.SetDestination(Ipv4Address ("10.10.10.10"));
                          // tcpHeader.SetDestinationPort(999);
                        }
                    
                      p->AddHeader (tcpHeader);
                    }
                }
              

              // p->Print(std::cout);
              // std::cout << std::endl;

              // // To get a header from Ptr<Packet> p
              // // first, copy the packet
              // Ptr<Packet> q = p->Copy();
               
              // // use indicator to search the packet
              // PacketMetadata::ItemIterator metadataIterator = q->BeginItem();
              // PacketMetadata::Item item;
              // while (metadataIterator.HasNext())
              // {
              //     item = metadataIterator.Next();
              //     if (item.type == PacketMetadata::Item::PAYLOAD)
              //       break;
              //     // for example, if we want to have an ip header
              //     if(item.tid.GetName() == "ns3::Ipv4Header")
              //     {
              //         NS_LOG_UNCOND("WIN!!!!!!");
              //         Ipv4Header ipHeader;
              //         q->RemoveHeader (ipHeader);
              //         NS_LOG_UNCOND("ip src:" << ipHeader.GetSource());
              //         NS_LOG_UNCOND("ip dst:" << ipHeader.GetDestination());

              // //         // Callback constructor = item.tid.GetConstructor();
              // //         // NS_ASSERT(!constructor.IsNull());
               
              // //         // // Ptr<> and DynamicCast<> won't work here as all headers are from ObjectBase, not Object
              // //         // ObjectBase *instance = constructor();
              // //         // NS_ASSERT(instance != 0);
               
              // //         // Ipv4Header *ipv4Header = dynamic_cast (instance);
              // //         // NS_ASSERT(ipv4Header != 0);
               
              // //         // ipv4Header->Deserialize(item.current);
              // //         // // you can use the ip header then ...
               
              // //         // // finished, clear the ip header
              // //         // delete ipv4Header;
              //     }
              // }


              // if (ipHeader.GetProtocol () == IPPROTO_TCP)
              //   {
              //     TcpHeader tcpHeader;
              //     p->RemoveHeader (tcpHeader);
              //     NS_LOG_INFO("Start of translator functions");
              //     NS_LOG_INFO("SEQ: " << tcpHeader.GetSequenceNumber());
              //     NS_LOG_INFO("ACK: " << tcpHeader.GetAckNumber());
              //     if (ipHeader.GetSource().IsEqual(Ipv4Address ("192.168.2.1")))
              //       {
              //         NS_LOG_INFO("SourcePort()");
              //         NS_LOG_INFO(tcpHeader.GetSourcePort());

              //         // Translator intercepts SYN and replies (SYN ACK) on behalf of video server
              //         if (tcpHeader.GetFlags() & TcpHeader::SYN)
              //           {
              //             isIpHeaderModified = true;
              //             NS_LOG_INFO("SYN flag detected from client 2 (192.168.2.1), generating ACK");
              //             NS_LOG_INFO(ipHeader.GetDestination());
              //             tcpHeader.SetFlags(tcpHeader.GetFlags() | TcpHeader::ACK);
              //             tcpHeader.SetAckNumber(tcpHeader.GetSequenceNumber() + SequenceNumber32 (1));
              //             tcpHeader.SetSourcePort(7);
              //             tcpHeader.SetDestinationPort(49153);
              //             ipHeader.SetDestination(Ipv4Address ("192.168.2.1"));
              //             ipHeader.SetSource(Ipv4Address ("8.8.8.2"));
              //           }
              //         else
              //           {
              //             // ipHeader.SetDestination(Ipv4Address ("127.0.0.1"));
              //             // tcpHeader.SetDestinationPort(999);
              //           }
              //       }
              //     p->AddHeader (tcpHeader);
              //   }
              // ---------------------------
              // End My Modifications:
              // ---------------------------
              if (!isIpHeaderModified)
                ipHeader.SetSource ((*i).GetGlobalIp ());
              p->AddHeader (ipHeader);
              return 0;
            }
          else
            {
              NS_LOG_DEBUG ("Evaluating rule with local port " << (*i).GetLocalPort () << " global port " << (*i).GetGlobalPort ());
              if (ipHeader.GetProtocol () == IPPROTO_TCP
                  && (((*i).GetProtocol () == IPPROTO_TCP)
                      || (*i).GetProtocol () == 0))
                {
                  TcpHeader tcpHeader;
                  p->RemoveHeader (tcpHeader);
                  if (tcpHeader.GetSourcePort () == (*i).GetLocalPort ())
                    {
                      tcpHeader.SetSourcePort ((*i).GetGlobalPort ());
                    }
                  p->AddHeader (tcpHeader);
                }
              else if (ipHeader.GetProtocol () == IPPROTO_UDP
                       && (((*i).GetProtocol () == IPPROTO_UDP)
                           || (*i).GetProtocol () == 0))
                {
                  UdpHeader udpHeader;
                  p->RemoveHeader (udpHeader);
                  if (udpHeader.GetSourcePort () == (*i).GetLocalPort ())
                    {
                      udpHeader.SetSourcePort ((*i).GetGlobalPort ());
                    }
                  p->AddHeader (udpHeader);
                }
              NS_LOG_DEBUG ("Rule match with a port-specific rule");
              ipHeader.SetSource ((*i).GetGlobalIp ());
              p->AddHeader (ipHeader);
              return 0;
            }
        }

      //Checking for Dynamic NAT Rules

      //Checking for existing connection
      for (DynamicNatTuple::const_iterator i = m_dynatuple.begin ();
           i != m_dynatuple.end (); i++)
        {
          if (srcAddress == (*i).GetLocalAddress ())
            {
              NS_LOG_DEBUG ("Checking for existing connections");
              ipHeader.SetSource (GetAddressPoolIp ());

              if (ipHeader.GetProtocol () == IPPROTO_TCP)

                {
                  TcpHeader tcpHeader;
                  p->RemoveHeader (tcpHeader);
                  if (tcpHeader.GetSourcePort () == (*i).GetTranslatedPort ())
                    {
                      tcpHeader.SetSourcePort ((*i).GetTranslatedPort ());
                    }
                  p->AddHeader (tcpHeader);


                }
              else
                {
                  UdpHeader udpHeader;
                  p->RemoveHeader (udpHeader);
                  if (udpHeader.GetSourcePort () == (*i).GetTranslatedPort ())
                    {
                      udpHeader.SetSourcePort ((*i).GetTranslatedPort ());
                    }
                  p->AddHeader (udpHeader);
                }

            }
        }

//This is for the new connections

      for (DynamicNatRules::const_iterator i = m_dynamictable.begin ();
           i != m_dynamictable.end (); i++)
        {
          if ((*i).GetLocalNet ().CombineMask ((*i).GetLocalMask ()) == srcAddress.CombineMask ((*i).GetLocalMask ()))

            {
              NS_LOG_DEBUG ("Checking for new connections");
              ipHeader.SetSource (GetAddressPoolIp ());

              if (ipHeader.GetProtocol () == IPPROTO_TCP)

                {
                  TcpHeader tcpHeader;
                  p->RemoveHeader (tcpHeader);
                  tcpHeader.SetSourcePort (GetNewOutsidePort ());
                  p->AddHeader (tcpHeader);
                  Ipv4DynamicNatTuple natuple (srcAddress,GetAddressPoolIp (),GetCurrentPort ());
                  m_dynatuple.push_front (natuple);
                }
              else
                {
                  UdpHeader udpHeader;
                  p->RemoveHeader (udpHeader);
                  udpHeader.SetSourcePort (GetNewOutsidePort ());
                  p->AddHeader (udpHeader);
                  Ipv4DynamicNatTuple natuple (srcAddress,GetAddressPoolIp (),GetCurrentPort ());
                  m_dynatuple.push_front (natuple);


                }


            }

        }
    }
  p->AddHeader (ipHeader);
  return 0;
}

void
Ipv4Nat::AddAddressPool (Ipv4Address globalip, Ipv4Mask globalmask)
{
  NS_LOG_FUNCTION (this << globalip << globalmask);
  m_globalip = globalip;
  m_globalmask = globalmask;
}

Ipv4Address
Ipv4Nat::GetAddressPoolIp () const
{
  return m_globalip;
}

Ipv4Mask
Ipv4Nat::GetAddressPoolMask () const
{
  return m_globalmask;
}

void
Ipv4Nat::AddPortPool (uint16_t strtprt, uint16_t endprt)         //port range
{
  NS_LOG_FUNCTION (this << strtprt << endprt);
  m_startport = strtprt;
  m_endport = endprt;
  m_currentPort = strtprt - 1;
}

uint16_t
Ipv4Nat::GetStartPort () const
{
  return m_startport;
}

uint16_t
Ipv4Nat::GetEndPort () const
{
  return m_endport;
}

uint16_t
Ipv4Nat::GetCurrentPort () const
{
  return m_currentPort;
}

uint16_t
Ipv4Nat::GetNewOutsidePort ()
{
  for (int i = m_startport - 1; i <= m_endport; i++)
    {
      if ( m_currentPort == i)
        {
          m_currentPort++;
          return m_currentPort;

        }
    }
  return 0;
}

void
Ipv4Nat::SetInside (int32_t interfaceIndex)
{
  NS_LOG_FUNCTION (this << interfaceIndex);
  m_insideInterface = interfaceIndex;

}

void
Ipv4Nat::SetOutside (int32_t interfaceIndex)
{

  NS_LOG_FUNCTION (this << interfaceIndex);
  m_outsideInterface = interfaceIndex;
}


void
Ipv4Nat::AddDynamicRule (const Ipv4DynamicNatRule& rule)
{
  NS_LOG_FUNCTION (this);
  m_dynamictable.push_front (rule);
}


void
Ipv4Nat::AddStaticRule (const Ipv4StaticNatRule& rule)
{
  NS_LOG_FUNCTION (this);
  m_statictable.push_front (rule);
  NS_LOG_DEBUG ("list has " << m_statictable.size () << " elements after pushing");
  NS_ASSERT_MSG (m_ipv4, "Forgot to aggregate Ipv4Nat to Node");
  if (m_ipv4->GetInterfaceForAddress (rule.GetGlobalIp ()) != -1)
    {
      NS_LOG_WARN ("Adding node's own IP address as the global NAT address");
      return;
    }
  NS_ASSERT_MSG (m_outsideInterface > -1, "Forgot to assign outside interface");
  // Add address to outside interface so that node will proxy ARP for it
  Ipv4Mask outsideMask = m_ipv4->GetAddress (m_outsideInterface, 0).GetMask ();
  Ipv4InterfaceAddress natAddress (rule.GetGlobalIp (), outsideMask);
  m_ipv4->AddAddress (m_outsideInterface, natAddress);
}

Ipv4StaticNatRule::Ipv4StaticNatRule (Ipv4Address localip, uint16_t locprt, Ipv4Address globalip,uint16_t gloprt, uint16_t protocol)
{
  NS_LOG_FUNCTION (this << localip << locprt << globalip << gloprt << protocol);
  m_localaddr = localip;
  m_globaladdr = globalip;
  m_localport = locprt;
  m_globalport = gloprt;
  NS_ASSERT (protocol == 0 || protocol == IPPROTO_TCP || protocol == IPPROTO_UDP);
  m_protocol = protocol;

}

// This version is used for no port restrictions
Ipv4StaticNatRule::Ipv4StaticNatRule (Ipv4Address localip, Ipv4Address globalip)
{
  NS_LOG_FUNCTION (this << localip << globalip);
  m_localaddr = localip;
  m_globaladdr = globalip;
  m_localport = 0;
  m_globalport = 0;
  m_protocol = 0;
}

Ipv4Address
Ipv4StaticNatRule::GetLocalIp () const
{
  return m_localaddr;
}

Ipv4Address
Ipv4StaticNatRule::GetGlobalIp () const
{
  return m_globaladdr;
}

uint16_t
Ipv4StaticNatRule::GetLocalPort () const
{
  return m_localport;
}

uint16_t
Ipv4StaticNatRule::GetGlobalPort () const
{
  return m_globalport;
}

uint16_t
Ipv4StaticNatRule::GetProtocol () const
{
  return m_protocol;
}

Ipv4DynamicNatRule::Ipv4DynamicNatRule (Ipv4Address localnet, Ipv4Mask localmask)
{
  NS_LOG_FUNCTION (this << localnet << localmask);
  m_localnetwork = localnet;
  m_localmask = localmask;

}

Ipv4Address
Ipv4DynamicNatRule::GetLocalNet () const
{
  return m_localnetwork;
}

Ipv4Mask
Ipv4DynamicNatRule::GetLocalMask () const
{
  return m_localmask;
}

Ipv4DynamicNatTuple::Ipv4DynamicNatTuple (Ipv4Address local, Ipv4Address global, uint16_t port)
{
  NS_LOG_FUNCTION (this << local << global << port);
  m_localip = local;
  m_globalip = global;
  m_port = port;
}

Ipv4Address
Ipv4DynamicNatTuple::GetLocalAddress () const
{
  return m_localip;
}

Ipv4Address
Ipv4DynamicNatTuple::GetGlobalAddress () const
{
  return m_globalip;
}

uint16_t
Ipv4DynamicNatTuple::GetTranslatedPort () const
{
  return m_port;
}

}
