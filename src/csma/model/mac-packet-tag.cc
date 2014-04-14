/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include <iostream>

 #include "ns3/mac-packet-tag.h"

namespace ns3 {

// MacTag::MacTag ()
// {
// }

TypeId 
MacTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MacTag")
    .SetParent<Tag> ()
    .AddConstructor<MacTag> ()
    .AddAttribute ("SimpleValue",
                   "A simple value",
                   EmptyAttributeValue (),
                   MakeUintegerAccessor (&MacTag::GetSimpleValue),
                   MakeUintegerChecker<uint8_t> ())
  ;
  return tid;
}
TypeId 
MacTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}
uint32_t 
MacTag::GetSerializedSize (void) const
{
  return 1;
}
void 
MacTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_simpleValue);
}
void 
MacTag::Deserialize (TagBuffer i)
{
  m_simpleValue = i.ReadU8 ();
}
void 
MacTag::Print (std::ostream &os) const
{
  os << "v=" << (uint32_t)m_simpleValue;
}
void 
MacTag::SetSimpleValue (uint8_t value)
{
  m_simpleValue = value;
}
uint8_t 
MacTag::GetSimpleValue (void) const
{
  return m_simpleValue;
}


// int main (int argc, char *argv[])
// {
//   // create a tag.
//   MacTag tag;
//   tag.SetSimpleValue (0x56);

//   // store the tag in a packet.
//   Ptr<Packet> p = Create<Packet> (100);
//   p->AddPacketTag (tag);

//   // create a copy of the packet
//   Ptr<Packet> aCopy = p->Copy ();

//   // read the tag from the packet copy
//   MacTag tagCopy;
//   p->PeekPacketTag (tagCopy);

//   // the copy and the original are the same !
//   NS_ASSERT (tagCopy.GetSimpleValue () == tag.GetSimpleValue ());

//   aCopy->PrintPacketTags (std::cout);
//   std::cout << std::endl;

//   return 0;
// }

} // namespace ns3