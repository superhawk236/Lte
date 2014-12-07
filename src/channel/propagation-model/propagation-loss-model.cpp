/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010,2011,2012 TELEMATICS LAB, Politecnico di Bari
 *
 * This file is part of LTE-Sim
 *
 * LTE-Sim is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation;
 *
 * LTE-Sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LTE-Sim; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Giuseppe Piro <g.piro@poliba.it>
 */
#include "macrocell-urban-area-channel-realization.h"

#include "propagation-loss-model.h"
#include "channel-realization.h"
#include "../../core/spectrum/transmitted-signal.h"
#include "../../load-parameters.h"
#include "../../device/NetworkNode.h"
#include <vector>
#include <math.h>

PropagationLossModel::PropagationLossModel()
{}

PropagationLossModel::~PropagationLossModel()
{
  m_channelRealizationMap.clear ();
}

void
PropagationLossModel::AddChannelRealization (ChannelRealization* chRealization)
{
  ChannelRealizationId_t idMap = std::make_pair (chRealization->GetSourceNode (),
		                                         chRealization->GetDestinationNode ());
  m_channelRealizationMap.insert (
		  std::pair <ChannelRealizationId_t, ChannelRealization* > (idMap, chRealization));
}

void
PropagationLossModel::DelChannelRealization (NetworkNode* src, NetworkNode* dst)
{
  ChannelRealizationId_t idMap = std::make_pair (src,dst);

  if (m_channelRealizationMap.find (idMap) != m_channelRealizationMap.end ())
    {
	  m_channelRealizationMap.find (idMap)->second->Destroy ();
      m_channelRealizationMap.erase (idMap);
    }
}


ChannelRealization*
PropagationLossModel::GetChannelRealization (NetworkNode* src, NetworkNode* dst)
{
	std::cout << "GetChannelRealizationStart1" << std::endl;
  ChannelRealizationMap::iterator it;

  ChannelRealizationId_t idMap = std::make_pair (src, dst);
  std::cout << "GetChannelRealizationStart2" << std::endl;
  it = m_channelRealizationMap.find (idMap);
  //it->second;
std::cout << "GetChannelRealizationStart3" << std::endl;
std::cout << "GetChannelRealizationEnd" << std::endl;
  return it->second;
}

PropagationLossModel::ChannelRealizationMap
PropagationLossModel::GetChannelRealizationMap (void)
{
  return m_channelRealizationMap;
}

TransmittedSignal*
PropagationLossModel::AddLossModel (NetworkNode* src,
                                    NetworkNode* dst,
	                                TransmittedSignal* txSignal)
{

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "\t  --> add loss between "
		  << src->GetIDNetworkNode () << " and " << dst->GetIDNetworkNode () << std::endl;
#endif
  //std::cout<<"m_test_x"<<std::endl;

  TransmittedSignal* rxSignal = txSignal->Copy ();

  /*
   * The loss propagation model for LTE networks is based on
   * a on a combination of four different models:
   * - the path loss
   * - the penetration loss
   * - the shadowind
   * - the multipath
   *
   * The rxPsd will be obtained considering, for each sub channel, the following
   * relations:
   * rxPsd (i) = txPsd (i) + m(i,t) - sh(i,t) - pnl(i,t) - pl (a,b);
   * where i is the i-th sub-channel and t is the current time (Simulator::Now()).
   */

  //std::cout<<"m_test_z"<<std::endl;
  //ChannelRealization* c = GetChannelRealization (src, dst);
  MacroCellUrbanAreaChannelRealization* c = new MacroCellUrbanAreaChannelRealization (src, dst);
  //std::cout<<"m_test_a"<<std::endl;
  std::vector<double> rxSignalValues = rxSignal->Getvalues ();
  //std::cout<<"m_test_b"<<std::endl;
  std::vector<double> loss = c->GetLoss ();//core down

  //std::cout<<"m_test_c"<<std::endl;

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "tx sub channels " << rxSignalValues.size () << " loss sub channels " << loss.size () << std::endl;
#endif

  int nbOfSubChannels = rxSignalValues.size ();

  for (int i = 0; i < nbOfSubChannels; i++)
    {
	  double rxPower = rxSignalValues.at (i) + loss.at (i); // add propagation loss

#ifdef TEST_PROPAGATION_LOSS_MODEL
      std::cout << "\t\t sub channel = " << i
		  << " rxSignalValues = " << rxSignalValues.at (i)
          << " loss = " << loss.at (i)
          << " rxPower = " << rxPower
		  << std::endl;
#endif

	  rxSignalValues.at (i) = rxPower; // in W/Hz
    }
  rxSignal->SetValues (rxSignalValues);

  delete c;//mouan

  return rxSignal;
}
