/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/object-factory.h"
#include "ns3/queue.h"
#include "ns3/csma-net-device.h"
#include "ns3/csma-channel.h"
#include "ns3/config.h"
#include "ns3/packet.h"
#include "ns3/names.h"

#include "ns3/trace-helper.h"
#include "ns3/csma-net-translator.h"
#include "csma-translator-helper.h"

#include <string>

NS_LOG_COMPONENT_DEFINE ("CsmaTranslatorHelper");

namespace ns3 {

CsmaTranslatorHelper::CsmaTranslatorHelper ()
{
  m_queueFactory.SetTypeId ("ns3::DropTailQueue");
  m_deviceFactory.SetTypeId ("ns3::CsmaNetDevice");
  m_channelFactory.SetTypeId ("ns3::CsmaChannel");
}

NetDeviceContainer 
CsmaTranslatorHelper::Install (const NodeContainer &c) const
{
  Ptr<CsmaChannel> channel = m_channelFactory.Create ()->GetObject<CsmaChannel> ();
  return Install (c, channel);
}

NetDeviceContainer 
CsmaTranslatorHelper::Install (const NodeContainer &c, Ptr<CsmaChannel> channel) const
{
  NetDeviceContainer devs;
  bool isFirstNode = true;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      if (isFirstNode){
      	devs.Add (InstallPriv (*i, channel, true));
		isFirstNode = false;
      }
      devs.Add (InstallPriv (*i, channel, false));
    }
  return devs;
}

Ptr<NetDevice>
CsmaTranslatorHelper::InstallPriv (Ptr<Node> node, Ptr<CsmaChannel> channel, bool isTranslatorNode) const
{
  Ptr<CsmaNetDevice> device = m_deviceFactory.Create<CsmaNetDevice> ();
  if (isTranslatorNode)
	device = m_deviceFactory.Create<CsmaNetTranslator> ();
  device->SetAddress (Mac48Address::Allocate ());
  node->AddDevice (device);
  Ptr<Queue> queue = m_queueFactory.Create<Queue> ();
  device->SetQueue (queue);
  device->Attach (channel);

  return device;
}


} // namespace ns3