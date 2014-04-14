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
#ifndef CSMA_TRANSLATOR_HELPER_H
#define CSMA_TRANSLATOR_HELPER_H

#include <string>

#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/csma-channel.h"
#include "ns3/deprecated.h"
#include "ns3/trace-helper.h"

#include "ns3/csma-helper.h"


namespace ns3 {

class Packet;

/**
 * \brief build a set of CsmaNetDevice objects
 *
 * Normally we eschew multiple inheritance, however, the classes 
 * PcapUserHelperForDevice and AsciiTraceUserHelperForDevice are
 * treated as "mixins".  A mixin is a self-contained class that
 * encapsulates a general attribute or a set of functionality that
 * may be of interest to many other classes.
 */
class CsmaTranslatorHelper : public CsmaHelper
{
public:
  /**
   * Construct a CsmaHelper.
   */
  CsmaTranslatorHelper ();
  virtual ~CsmaTranslatorHelper () {}

  /**
   * This method creates an ns3::CsmaChannel with the attributes configured by
   * CsmaTranslatorHelper::SetChannelAttribute.  For each Ptr<node> (except Ptr<node>[0]) in the provided
   * container c: it creates an ns3::CsmaNetDevice (with the attributes 
   * configured by CsmaTranslatorHelper::SetDeviceAttribute); adds the device to the 
   * node; and attaches the channel to the device.
   * Also, for Ptr<node>[0] in the provided container t,
   * it creates an ns3::CsmaNetTranslator (with the attributes 
   * configured by CsmaTranslatorHelper::SetDeviceAttribute); adds the device to the 
   * node; and attaches the channel to the device.
   *
   * \param c The NodeContainer holding the nodes to be changed.
   * \returns A container holding the added net devices.
   */
  NetDeviceContainer Install (const NodeContainer &c) const;

  /**
   * This method creates an ns3::CsmaChannel with the attributes configured by
   * CsmaTranslatorHelper::SetChannelAttribute.  For each Ptr<node> (except Ptr<node>[0]) in the provided
   * container c: it creates an ns3::CsmaNetDevice (with the attributes 
   * configured by CsmaTranslatorHelper::SetDeviceAttribute); adds the device to the 
   * node; and attaches the channel to the device.
   * Also, for Ptr<node>[0] in the provided container t,
   * it creates an ns3::CsmaNetTranslator (with the attributes 
   * configured by CsmaTranslatorHelper::SetDeviceAttribute); adds the device to the 
   * node; and attaches the provided channel to the device.
   *
   * \param c The NodeContainer holding the nodes to be changed.
   * \param channel The channel to attach to the devices.
   * \returns A container holding the added net devices.
   */
  NetDeviceContainer Install (const NodeContainer &c, Ptr<CsmaChannel> channel) const;

private:
  /*
   * \internal
   */
  Ptr<NetDevice> InstallPriv (Ptr<Node> node, Ptr<CsmaChannel> channel, bool isTranslatorNode) const;

  ObjectFactory m_channelFactory;
  ObjectFactory m_deviceFactory;
  ObjectFactory m_queueFactory;
  
};

} // namespace ns3

#endif /* CSMA_TRANSLATOR_HELPER_H */
