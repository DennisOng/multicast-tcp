/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 Emmanuelle Laprise
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
 * Author: Emmanuelle Laprise <emmanuelle.laprise@bluekazoo.ca
 */

#ifndef CSMA_NET_TRANSLATOR_H
#define CSMA_NET_TRANSLATOR_H

#include <string.h>
#include "ns3/node.h"
#include "ns3/backoff.h"
#include "ns3/address.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/random-variable.h"
#include "ns3/mac48-address.h"
#include "ns3/csma-net-device.h"

namespace ns3 {

class Queue;
class CsmaChannel;
class ErrorModel;

/** 
 * \defgroup csma CsmaNetDevice
 *
 * This section documents the API of the ns-3 csma module. For a generic functional description, please refer to the ns-3 manual.
 */

/**
 * \ingroup csma
 * \class CsmaNetDevice
 * \brief A Device for a Csma Network Link.
 *
 * The Csma net device class is analogous to layer 1 and 2 of the
 * TCP stack. The NetDevice takes a raw packet of bytes and creates a
 * protocol specific packet from them. 
 */
class CsmaNetTranslator : public CsmaNetDevice 
{
public:
  /**
   * Construct a CsmaNetDevice
   *
   * This is the default constructor for a CsmaNetDevice.
   */
  CsmaNetTranslator ();

  /**
   * Destroy a CsmaNetDevice
   *
   * This is the destructor for a CsmaNetDevice.
   */
  virtual ~CsmaNetTranslator ();

  /**
   * Start sending a packet down the channel.
   * \param packet packet to send
   * \param dest layer 2 destination address
   * \param protocolNumber protocol number
   * \return true if successfull, false otherwise (drop, ...)
   */
  virtual bool Send (Ptr<Packet> packet, const Address& dest, 
                     uint16_t protocolNumber);
};

} // namespace ns3

#endif /* CSMA_NET_TRANSLATOR_H */
