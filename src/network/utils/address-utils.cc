/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 INRIA
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
#include "address-utils.h"
#include "inet-socket-address.h"

namespace ns3 {

void WriteTo (Buffer::Iterator &i, Ipv4Address ad)
{
  i.WriteHtonU32 (ad.Get ());
}
void WriteTo (Buffer::Iterator &i, Ipv6Address ad)
{
  uint8_t buf[16];
  ad.GetBytes (buf);
  i.Write (buf, 16);
}
void WriteTo (Buffer::Iterator &i, const Address &ad)
{
  uint8_t mac[Address::MAX_SIZE];
  ad.CopyTo (mac);
  i.Write (mac, ad.GetLength ());
}
void WriteTo (Buffer::Iterator &i, Mac48Address ad)
{
  uint8_t mac[6];
  ad.CopyTo (mac);
  i.Write (mac, 6);
}

void ReadFrom (Buffer::Iterator &i, Ipv4Address &ad)
{
  ad.Set (i.ReadNtohU32 ());
}
void ReadFrom (Buffer::Iterator &i, Ipv6Address &ad)
{
  uint8_t ipv6[16];
  i.Read (ipv6, 16);
  ad.Set (ipv6);
}
void ReadFrom (Buffer::Iterator &i, Address &ad, uint32_t len)
{
  uint8_t mac[Address::MAX_SIZE];
  i.Read (mac, len);
  ad.CopyFrom (mac, len);
}
void ReadFrom (Buffer::Iterator &i, Mac48Address &ad)
{
  uint8_t mac[6];
  i.Read (mac, 6);
  ad.CopyFrom (mac);
}

// ----------------------------------------------
// This part is added in based on the modifications by the TCP echo example patch
// @ https://codereview.appspot.com/5654053/
// ----------------------------------------------

AddressPrinter::AddressPrinter(Address &address)
 : v6(false)
{
 if (Ipv4Address::IsMatchingType(address) == true)
   {
         this->address = Ipv4Address::ConvertFrom(address);
   }
 else if (Ipv6Address::IsMatchingType(address) == true)
   {
         address6 = Ipv6Address::ConvertFrom(address);
         v6 = true;
   }
 else if (InetSocketAddress::IsMatchingType(address) == true)
   {
         InetSocketAddress iAddr = InetSocketAddress::ConvertFrom(address);
         this->address = iAddr.GetIpv4();
   }
 else if (Inet6SocketAddress::IsMatchingType(address) == true)
   {
         Inet6SocketAddress i6Addr = Inet6SocketAddress::ConvertFrom(address);
         this->address6 = i6Addr.GetIpv6();
         v6 = true;
   }
}

AddressPrinter::AddressPrinter (Ipv4Address &address)
 : v6(false)
{
 this->address = address;
}

AddressPrinter::AddressPrinter (Ipv6Address &address)
 : v6(true)
{
 address6 = address;
}

AddressPrinter::AddressPrinter (InetSocketAddress &address)
: v6(false)
{
 this->address = address.GetIpv4();
}

AddressPrinter::AddressPrinter (Inet6SocketAddress &address)
: v6(true)
{
 this->address6 = address.GetIpv6();
}

void
AddressPrinter::Print (std::ostream &os) const
{
 v6 ? address6.Print(os) : address.Print(os);
}

std::ostream& operator<< (std::ostream& os, AddressPrinter const& address)
{
 address.Print (os);
 return os;
}
 
// ----------------------------------------------
// End of modification
// ----------------------------------------------

namespace addressUtils {

bool IsMulticast (const Address &ad)
{
  if (InetSocketAddress::IsMatchingType (ad))
    {
      InetSocketAddress inetAddr = InetSocketAddress::ConvertFrom (ad);
      Ipv4Address ipv4 = inetAddr.GetIpv4 ();
      return ipv4.IsMulticast ();
    }
  // IPv6 case can go here, in future
  return false;
}

} // namespace addressUtils

} // namespace ns3
