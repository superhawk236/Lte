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


#ifndef ENODEB_H_
#define ENODEB_H_

#include "NetworkNode.h"

class UserEquipment;
class Gateway;
class Bus;
class PacketScheduler;

class ENodeB : public NetworkNode {
public:
	struct UserEquipmentRecord
	  {
		UserEquipmentRecord ();
		virtual ~UserEquipmentRecord ();
		UserEquipmentRecord (UserEquipment *UE);

		UserEquipment *m_UE;
		void SetUE (UserEquipment *UE);
        UserEquipment* GetUE (void) const;

        std::vector<int> m_cqiFeedback;
		void SetCQI (std::vector<int> cqi);
		std::vector<int> GetCQI (void) const;

		std::vector<int> m_cqiFeedback_oneago;
		void SetCQIone (std::vector<int> cqi);
		std::vector<int> GetCQIone (void) const;

		std::vector<int> m_cqiFeedback_twoago;
		void SetCQItwo (std::vector<int> cqi);
		std::vector<int> GetCQItwo (void) const;

		int m_schedulingRequest; // in bytes
		void SetSchedulingRequest (int r);
		int GetSchedulingRequest (void);

		int m_averageSchedulingGrants; // in bytes
		void UpdateSchedulingGrants (int b);
		int GetSchedulingGrants (void);

		int m_ulMcs;
		void SetUlMcs (int mcs);
		int GetUlMcs (void);

	  };

	typedef std::vector<UserEquipmentRecord*> UserEquipmentRecords;

	//mouan
	struct BusRecord
	  {
		BusRecord ();
		virtual ~BusRecord ();
		BusRecord (Bus *bus);

		Bus *m_bus;
		void Setbus (Bus *bus);
        Bus* Getbus (void) const;

        std::vector<int> m_cqiFeedback;
		void SetCQI (std::vector<int> cqi);
		std::vector<int> GetCQI (void) const;

		std::vector<int> m_cqiFeedback_oneago;
		void SetCQIone (std::vector<int> cqi);
		std::vector<int> GetCQIone (void) const;

		std::vector<int> m_cqiFeedback_twoago;
		void SetCQItwo (std::vector<int> cqi);
		std::vector<int> GetCQItwo (void) const;

		int m_schedulingRequest; // in bytes
		void SetSchedulingRequest (int r);
		int GetSchedulingRequest (void);

		int m_averageSchedulingGrants; // in bytes
		void UpdateSchedulingGrants (int b);
		int GetSchedulingGrants (void);

		int m_ulMcs;
		void SetUlMcs (int mcs);
		int GetUlMcs (void);

	  };

	typedef std::vector<BusRecord*> BusRecords;

	enum DLSchedulerType
	  {
	    DLScheduler_TYPE_MAXIMUM_THROUGHPUT,
	    DLScheduler_TYPE_PROPORTIONAL_FAIR,
	    DLScheduler_TYPE_FLS,
	    DLScheduler_TYPE_MLWDF,
	    DLScheduler_TYPE_EXP,
	    DLScheduler_LOG_RULE,
	    DLScheduler_EXP_RULE
	  };
	enum ULSchedulerType
	  {
		ULScheduler_TYPE_MAXIMUM_THROUGHPUT,
		ULScheduler_TYPE_FME
	  };

	ENodeB ();
	ENodeB (int idElement, Cell *cell);
	ENodeB (int idElement, Cell *cell, double posx, double posy);

	virtual ~ENodeB();

    void RegisterUserEquipment (UserEquipment *UE);
    void DeleteUserEquipment (UserEquipment *UE);
    int GetNbOfUserEquipmentRecords (void);
    void CreateUserEquipmentRecords (void);
    void DeleteUserEquipmentRecords (void);
    UserEquipmentRecords* GetUserEquipmentRecords (void);
    UserEquipmentRecord* GetUserEquipmentRecord (int idUE);

	//mouan
	void RegisterBus (Bus *bus);
    void DeleteBus (Bus *bus);
    int GetNbOfBusRecords (void);
    void CreateBusRecords (void);
    void DeleteBusRecords (void);
    BusRecords* GetBusRecords (void);
    BusRecord* GetBusRecord (int idbus);

    void SetDLScheduler (DLSchedulerType type);
    PacketScheduler* GetDLScheduler (void) const;
    void SetULScheduler (ULSchedulerType type);
    //PacketScheduler* GetULScheduler (void) const;
	UplinkPacketScheduler* GetULScheduler (void) const;

    void ResourceBlocksAllocation ();
    void UplinkResourceBlockAllocation ();
    void DownlinkResourceBlokAllocation ();

	//Debug
	void Print (void);

private:
	UserEquipmentRecords *m_userEquipmentRecords;
	BusRecords *m_busRecords;
};

#endif /* ENODEB_H_ */