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


#ifndef POSITIONBASEDHOMANAGER_H_
#define POSITIONBASEDHOMANAGER_H_

#include "ho-manager.h"

class PositionBasedHoManager: public HoManager {
public:
	PositionBasedHoManager();
	virtual ~PositionBasedHoManager();

	virtual bool CheckHandoverNeed (UserEquipment* ue);
	virtual bool CheckBusHandoverNeed (Bus* bus);

	virtual bool MouanCheckHandoverNeed (UserEquipment* ue);//,TransmittedSignal *signalvalue);
};

#endif /* POSITIONBASEDHOMANAGER_H_ */
