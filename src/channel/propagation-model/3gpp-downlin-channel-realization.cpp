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


#include "3gpp-downlin-channel-realization.h"
#include "../../device/UserEquipment.h"
#include "../../device/ENodeB.h"
#include "../../device/Bus.h"
#include "../../device/HeNodeB.h"
#include "../../utility/RandomVariable.h"
#include "shadowing-trace.h"
#include "../../core/spectrum/bandwidth-manager.h"
#include "../../phy/lte-phy.h"
#include "../../core/eventScheduler/simulator.h"
#include "../../load-parameters.h"


ThreeGppDownlinChannelRealization::ThreeGppDownlinChannelRealization (NetworkNode* src, NetworkNode* dst)
{
  SetSamplingPeriod (0.5);

  m_penetrationLoss = 10;
  m_shadowing = 0;
  m_pathLoss = 0;
  SetFastFading (new FastFading ());

  SetSourceNode (src);
  SetDestinationNode (dst);

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "Created Channe Realization between "
		  << src->GetIDNetworkNode () << " and " << dst->GetIDNetworkNode () << std::endl;
#endif

  SetChannelType (ChannelRealization::CHANNEL_TYPE_PED_A); //def value
  UpdateModels ();
}

ThreeGppDownlinChannelRealization::~ThreeGppDownlinChannelRealization()
{
}

void
ThreeGppDownlinChannelRealization::SetPenetrationLoss (double pnl)
{
  m_penetrationLoss = pnl;
}

double ThreeGppDownlinChannelRealization::GetPenetrationLoss (void)
{
  return m_penetrationLoss;
}

double
ThreeGppDownlinChannelRealization::GetPathLoss (void)
{
  /*
   * Path Loss Model For Indoor Environment.
   * L = 37 + 30 Log10(R) , R in meters
   * at the same floor
   */

  double distance;

  if (GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_UE
		  && GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_ENODEB)
    {
	  UserEquipment* ue = (UserEquipment*) GetSourceNode ();
	  ENodeB* enb = (ENodeB*) GetDestinationNode ();

	  distance =  ue->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (enb->GetMobilityModel ()->GetAbsolutePosition ());
    }

  else if (GetDestinationNode ()->GetNodeType () == NetworkNode::TYPE_UE
		  && GetSourceNode ()->GetNodeType () == NetworkNode::TYPE_ENODEB)
    {
	  UserEquipment* ue = (UserEquipment*) GetDestinationNode ();
	  ENodeB* enb = (ENodeB*) GetSourceNode ();

	  distance =  ue->GetMobilityModel ()->GetAbsolutePosition ()->GetDistance (enb->GetMobilityModel ()->GetAbsolutePosition ());
    }



  m_pathLoss = 37 + (30 * log10 (distance));

  return m_pathLoss;
}


void
ThreeGppDownlinChannelRealization::SetShadowing (double sh)
{
  m_shadowing = sh;
}

double
ThreeGppDownlinChannelRealization::GetShadowing (void)
{
  return m_shadowing;
}


void
ThreeGppDownlinChannelRealization::UpdateModels ()
{

#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout << "\t --> UpdateModels" << std::endl;
#endif


  //update shadowing
  m_shadowing = 0;
  double probability = GetRandomVariable (101) / 100.0;
  for (int i = 0; i < 201; i++)
    {
	  if (probability <= shadowing_probability[i])
	    {
		  m_shadowing = shadowing_value[i];
          break;
	    }
    }

  UpdateFastFading ();

  SetLastUpdate ();
}


std::vector<double>
ThreeGppDownlinChannelRealization::GetLoss ()
{
#ifdef TEST_PROPAGATION_LOSS_MODEL
  std::cout <<"ThreeGppDownlinChannelRealization\n"
	        << "\t  --> compute loss between "
		  << GetSourceNode ()->GetIDNetworkNode () << " and "
		  << GetDestinationNode ()->GetIDNetworkNode () << std::endl;
#endif

  if (NeedForUpdate ())
    {
	   UpdateModels ();
    }

  std::vector<double> loss;


  int now_ms = Simulator::Init()->Now () * 1000;
  int lastUpdate_ms = GetLastUpdate () * 1000;
  int index = now_ms - lastUpdate_ms;

  int nbOfSubChannels = GetSourceNode ()->GetPhy ()->GetBandwidthManager ()->GetDlSubChannels ().size ();

  for (int i = 0; i < nbOfSubChannels; i++)
    {
	  double l = GetFastFading ()->at (i).at (index) - GetPathLoss () - GetPenetrationLoss () - GetShadowing ();
	  loss.push_back (l);

#ifdef TEST_PROPAGATION_LOSS_MODEL
       std::cout << "\t\t mlp = " << GetFastFading ()->at (i).at (index)
		  << " pl = " << GetPathLoss ()
          << " pnl = " << GetPenetrationLoss()
          << " sh = " << GetShadowing()
          << " LOSS = " << l
		  << std::endl;
#endif
    }

  return loss;
}
