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
 * Author: Emmanuelle Laprise <emmanuelle.laprise@bluekazoo.ca>
 */

#include "ns3/log.h"
#include "ns3/queue.h"
#include "ns3/simulator.h"
#include "ns3/ethernet-header.h"
#include "ns3/ethernet-trailer.h"
#include "ns3/llc-snap-header.h"
#include "ns3/error-model.h"
#include "ns3/enum.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/trace-source-accessor.h"
#include "csma-net-translator.h"
#include "csma-channel.h"

NS_LOG_COMPONENT_DEFINE ("CsmaNetTranslator");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CsmaNetTranslator);

CsmaNetTranslator::CsmaNetTranslator ()
{
}

CsmaNetTranslator::~CsmaNetTranslator()
{
  NS_LOG_FUNCTION_NOARGS ();
}

bool
CsmaNetTranslator::Send (Ptr<Packet> packet,const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (packet << dest << protocolNumber);
  Address address = GetAddress();
  NS_LOG_UNCOND(address);
  NS_LOG_UNCOND(dest);

  return SendFrom (packet, address, dest, protocolNumber);
}

} // namespace ns3
