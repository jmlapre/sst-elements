// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.
//


#include <sst_config.h>
#include <string>
#include "samba.h"

using namespace SST;
using namespace SST::SambaComponent;


#define SAMBA_VERBOSE(LEVEL, OUTPUT) if(verbosity >= (LEVEL)) OUTPUT


// Here we do the initialization of the Samba units of the system, connecting them to the cores and instantiating TLB hierachy objects for each one

Samba::Samba(SST::ComponentId_t id, SST::Params& params): Component(id) {

    int verbosity = params.find<int>("verbose", 0);
    output = new SST::Output("SambaComponent[@f:@l:@p] ", verbosity, 0, SST::Output::STDOUT);

    output->verbose(CALL_INFO, 1, 0, "Creating Samba component...\n");

	core_count = (uint32_t) params.find<uint32_t>("corecount", 1);

	int self = (uint32_t) params.find<uint32_t>("self_connected", 0);

	int emulate_faults = (uint32_t) params.find<uint32_t>("emulate_faults", 0);

	int levels = (uint32_t) params.find<uint32_t>("levels", 1);

	int  page_walk_latency = ((uint32_t) params.find<uint32_t>("page_walk_latency", 50));

	if(emulate_faults==1)
	{
		if (NULL != (pageFaultHandler = loadUserSubComponent<PageFaultHandler>("pagefaulthandler"))) {
			output->verbose(CALL_INFO, 1, 0, "Loaded page fault handler: %s\n", pageFaultHandler->getName().c_str());
		} else {
			std::string pagefaulthandler_name = params.find<std::string>("pagefaulthandler", "Opal.PageFaultHandler");
			output->verbose(CALL_INFO, 1, 0, "Loading page fault handler: %s\n", pagefaulthandler_name.c_str());
			pageFaultHandler = loadAnonymousSubComponent<PageFaultHandler>(pagefaulthandler_name, "pagefaulthandler", 0, ComponentInfo::SHARE_STATS | ComponentInfo::INSERT_STATS, params);
			if (NULL == pageFaultHandler) output->fatal(CALL_INFO, -1, "Failed to load page fault handler: %s\n", pagefaulthandler_name.c_str());
		}
	}

	std::cout<<"Initialized with "<<core_count<<" cores"<<std::endl;


	cpu_to_mmu = (SST::Link **) malloc( sizeof(SST::Link*) * core_count );

	mmu_to_cache = (SST::Link **) malloc( sizeof(SST::Link *) * core_count );

	ptw_to_mem = (SST::Link **) malloc( sizeof(SST::Link *) * core_count );

	cr3I = 0;


	char* link_buffer = (char*) malloc(sizeof(char) * 256);

	char* link_buffer2 = (char*) malloc(sizeof(char) * 256);

	char* link_buffer3 = (char*) malloc(sizeof(char) * 256);

        size_t buffer_size = sizeof(char) * 256;

	std::cout<<"Before initialization "<<std::endl;
	for(uint32_t i = 0; i < core_count; ++i) {
		snprintf(link_buffer, buffer_size, "cpu_to_mmu%" PRIu32, i);

		snprintf(link_buffer2, buffer_size, "mmu_to_cache%" PRIu32, i);



		TLB.push_back(loadComponentExtension<TLBhierarchy>(i, levels /* level */, params));


		SST::Link * link2 = configureLink(link_buffer, "0ps", new Event::Handler2<TLBhierarchy,&TLBhierarchy::handleEvent_CPU>(TLB[i]));
		cpu_to_mmu[i] = link2;



		SST::Link * link = configureLink(link_buffer2, "0ps", new Event::Handler2<TLBhierarchy,&TLBhierarchy::handleEvent_CACHE>(TLB[i]));
		mmu_to_cache[i] = link;


		snprintf(link_buffer3, buffer_size, "ptw_to_mem%" PRIu32, i);
		SST::Link * link3;

		if(self==0)
			link3 = configureLink(link_buffer3, new Event::Handler2<PageTableWalker,&PageTableWalker::recvResp>(TLB[i]->getPTW()));
		else
			link3 = configureSelfLink(link_buffer3, std::to_string(page_walk_latency)+ "ns", new Event::Handler2<PageTableWalker,&PageTableWalker::recvResp>(TLB[i]->getPTW()));

		ptw_to_mem[i] = link3;



		if(emulate_faults==1)
		{
			pageFaultHandler->registerPageFaultHandler(i, new PageFaultHandler::PageFaultHandlerInterface<PageTableWalker>(TLB[i]->getPTW(), &PageTableWalker::recvPageFaultResp));
			TLB[i]->getPTW()->setPageFaultHandler(pageFaultHandler);

			snprintf(link_buffer, buffer_size, "event_bus%" PRIu32, i);

			event_link = configureSelfLink(link_buffer, "1ns", new Event::Handler2<PageTableWalker,&PageTableWalker::handleEvent>(TLB[i]->getPTW()));

			TLB[i]->getPTW()->setEventChannel(event_link);
			TLB[i]->setPageTablePointers(&CR3, &PGD, &PUD, &PMD, &PTE, &MAPPED_PAGE_SIZE1GB, &MAPPED_PAGE_SIZE2MB, &MAPPED_PAGE_SIZE4KB, &PENDING_PAGE_FAULTS, &cr3I, &PENDING_PAGE_FAULTS_PGD, &PENDING_PAGE_FAULTS_PUD, &PENDING_PAGE_FAULTS_PMD, &PENDING_PAGE_FAULTS_PTE);//, &PENDING_SHOOTDOWN_EVENTS, &PTR, &PTR_map);

		}

		TLB[i]->setPTWLink(link3); // We make a connection from the TLB hierarchy to the memory hierarchy, for page table walking purposes
		TLB[i]->setCacheLink(mmu_to_cache[i]);
		TLB[i]->setCPULink(cpu_to_mmu[i]);

	}



	std::cout<<"After initialization "<<std::endl;

	std::string cpu_clock = params.find<std::string>("clock", "1GHz");
	registerClock( cpu_clock, new Clock::Handler2<Samba,&Samba::tick>(this) );



	free(link_buffer);
	free(link_buffer2);
	free(link_buffer3);

}



Samba::Samba() : Component(-1)
{
	// for serialization only
	//
}


void Samba::init(unsigned int phase) {

	/*
	 * CPU may send init events to memory, pass them through,
	 * also pass memory events to CPU
	 */
	for (uint32_t i = 0; i < core_count; i++) {
		SST::Event * ev;
		while ((ev = cpu_to_mmu[i]->recvUntimedData())) {
			mmu_to_cache[i]->sendUntimedData(ev);
		}

		while ((ev = mmu_to_cache[i]->recvUntimedData())) {
			SST::MemHierarchy::MemEventInit * mEv = dynamic_cast<SST::MemHierarchy::MemEventInit*>(ev);
			if (mEv && mEv->getInitCmd() == SST::MemHierarchy::MemEventInit::InitCommand::Coherence) {
				SST::MemHierarchy::MemEventInitCoherence * mEvC = static_cast<SST::MemHierarchy::MemEventInitCoherence*>(mEv);
				TLB[i]->setLineSize(mEvC->getLineSize());
			}
			cpu_to_mmu[i]->sendUntimedData(ev);
		}
	}

}


bool Samba::tick(SST::Cycle_t x)
{

	// We tick the MMU hierarchy of each core
	for(uint32_t i = 0; i < core_count; ++i)
		TLB[i]->tick(x);


	return false;
}
