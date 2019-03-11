/*! \file pagefaultmanager.cc
Routines for the page fault managerPage Fault Manager
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//

#include "kernel/thread.h"
#include "vm/swapManager.h"
#include "vm/physMem.h"
#include "vm/pagefaultmanager.h"

PageFaultManager::PageFaultManager() {
}

// PageFaultManager::~PageFaultManager()
/*! Nothing for now
*/
PageFaultManager::~PageFaultManager() {
}

// ExceptionType PageFault(uint32_t virtualPage)
/*! 	
//	This method is called by the Memory Management Unit when there is a 
//      page fault. This method loads the page from :
//      - read-only sections (text,rodata) $\Rightarrow$ executive
//        file
//      - read/write sections (data,...) $\Rightarrow$ executive
//        file (1st time only), or swap file
//      - anonymous mappings (stack/bss) $\Rightarrow$ new
//        page from the MemoryManager (1st time only), or swap file
//
//	\param virtualPage the virtual page subject to the page fault
//	  (supposed to be between 0 and the
//        size of the address space, and supposed to correspond to a
//        page mapped to something [code/data/bss/...])
//	\return the exception (generally the NO_EXCEPTION constant)
*/  
ExceptionType PageFaultManager::PageFault(uint32_t virtualPage) 
{
  #ifndef ETUDIANTS_TP
  printf("**** Warning: page fault manager is not implemented yet\n");
    exit(-1);
    return ((ExceptionType)0);
  #endif

  #ifdef ETUDIANTS_TP

  //first, we check if the virtual page is in an  I/O process
	while(g_machine->mmu->translationTable->getBitIo(virtualPage))
		g_current_thread->Yield();
	// if the page isn't in physical mem
	if(!g_machine->mmu->translationTable->getBitValid(virtualPage))
	{
    // we mark the page as in I/O PROCESS
		g_machine->mmu->translationTable->setBitIo(virtualPage);
		int addrDisk = g_machine->mmu->translationTable->getAddrDisk(virtualPage);
		int pp = g_physical_mem_manager->AddPhysicalToVirtualMapping(g_current_thread->GetProcessOwner()->addrspace, virtualPage);
    //set the page number in phys memory
		g_machine->mmu->translationTable->setPhysicalPage(virtualPage,pp);
      // loaded from the swap area
		if(g_machine->mmu->translationTable->getBitSwap(virtualPage))
		{
      //we need the diskAddr so we might have to wait
			while((addrDisk = g_machine->mmu->translationTable->getAddrDisk(virtualPage)) == -1)
				g_current_thread->Yield(); // I suppose it should works, if it doesn't just wait like {;}
      // we fill up the main memory with the data stored at addrDisk
			g_swap_manager->GetPageSwap(addrDisk, (char *)&(g_machine->mainMemory[g_machine->mmu->translationTable->getPhysicalPage(virtualPage)*g_cfg->PageSize]));
		}
		else
		{
        // anonymous mapping
        // fill with zero
			if(addrDisk == -1)
				memset(&(g_machine->mainMemory[g_machine->mmu->translationTable->getPhysicalPage(virtualPage)*g_cfg->PageSize]), 0, g_cfg->PageSize);
			else
        //loaded from the executable on disk
				g_current_thread->GetProcessOwner()->exec_file->ReadAt((char *)&(g_machine->mainMemory[g_machine->mmu->translationTable->getPhysicalPage(virtualPage)*g_cfg->PageSize]), g_cfg->PageSize, addrDisk);
			
		}
    // now the page is in physical mem
		g_machine->mmu->translationTable->setBitValid(virtualPage);
    // we release the IO bit
		g_machine->mmu->translationTable->clearBitIo(virtualPage);
		}	
	
	return NO_EXCEPTION;
#endif


  
}




