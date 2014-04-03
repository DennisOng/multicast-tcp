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
#ifndef ADDRESS_UTILS_H
#define ADDRESS_UTILS_H

#include "ns3/buffer.h"
#include "ipv4-address.h"
#include "ipv6-address.h"
#include "ns3/address.h"
#include "mac48-address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
 
namespace ns3 {

void WriteTo (Buffer::Iterator &i, Ipv4Address ad);
void WriteTo (Buffer::Iterator &i, Ipv6Address ad);
void WriteTo (Buffer::Iterator &i, const Address &ad);
void WriteTo (Buffer::Iterator &i, Mac48Address ad);

void ReadFrom (Buffer::Iterator &i, Ipv4Address &ad);
void ReadFrom (Buffer::Iterator &i, Ipv6Address &ad);
void ReadFrom (Buffer::Iterator &i, Address &ad, uint32_t len);
void ReadFrom (Buffer::Iterator &i, Mac48Address &ad);

// ----------------------------------------------
// This part is added in based on the modifications by the TCP echo example patch
// @ https://codereview.appspot.com/5654053/
// ----------------------------------------------
 /**
  * \ingroup address
  *
  * \brief a class to help on printing formated address, Ipv4 or Ipv6
  *
  * The constructor takes an address as argument. It can be Ipv4 or Ipv6 and
  * is automatically cast. This class is to help on printing formated address without
  * caring what are the type, supressing some "if's" from source.
  */
class AddressPrinter {
public:
     explicit AddressPrinter (Address &address);
     explicit AddressPrinter (Ipv4Address &address);
     explicit AddressPrinter (Ipv6Address &address);
     explicit AddressPrinter (InetSocketAddress &address);
     explicit AddressPrinter (Inet6SocketAddress &address);
     /**
      * \brief Print the address formated in Ipv4 or Ipv6 format
      *
      * The print format depends on which address type was used on the constructor
      * \param os The output stream to which the address is printed
      */
void Print (std::ostream &os) const;

     Ipv4Address address;
     Ipv6Address address6;
     bool v6;
};

std::ostream& operator<< (std::ostream& os, AddressPrinter const& address);
 
// ----------------------------------------------
// End of modification
// ----------------------------------------------

namespace addressUtils {

/**
 * \brief Address family-independent test for a multicast address
 */
bool IsMulticast (const Address &ad);
};

};

#endif /* ADDRESS_UTILS_H */
