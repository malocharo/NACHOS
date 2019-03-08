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

  //first, we check if the virtual page is a I/O process
  while(g_machine->mmu->translationTable->getBitIo(virtualPage))
    g_current_thread->Yield();
  g_machine->mmu->translationTable->setBitIo(virtualPage);

  //we now check the position of the page

  int bitSwap = g_machine->mmu->translationTable->getBitSwap(virtualPage);
  int addrDisk = g_machine->mmu->translationTable->getAddrDisk(virtualPage);
  DEBUG('v',(char*)"PageFault : bitSwap = %d : addrDisk = %d\n",bitSwap,addrDisk);
  char dataBuffer[g_cfg->PageSize];
  int pp = g_physical_mem_manager->AddPhysicalToVirtualMapping(g_current_thread->GetProcessOwner()->addrspace,virtualPage);
  DEBUG('v',(char*)"PageFault : physical page = %d\n",pp);
  // loaded from the swap
  if(bitSwap){
    //we need the diskAddr so we might have to wait
    while((addrDisk = g_machine->mmu->translationTable->getAddrDisk(virtualPage)) == -1 )
      g_current_thread->Yield(); // I suppose it should works, if it doesn't just wait like {;}
    g_swap_manager->GetPageSwap(addrDisk,dataBuffer);
    DEBUG('v',(char*)"PageFault : loaded from swap at %d : %s\n",addrDisk,dataBuffer);
    memcpy(&g_machine->mainMemory[pp * g_cfg->PageSize],dataBuffer,g_cfg->PageSize);
  }
  // anonymous mapping
  else if(!bitSwap && addrDisk == -1) {
    memset(dataBuffer,0,g_cfg->PageSize);
    memcpy(&g_machine->mainMemory[pp * g_cfg->PageSize],dataBuffer,g_cfg->PageSize);
  }
  //loaded from the disk
  else if(!bitSwap && addrDisk != -1) {
    g_current_thread->GetProcessOwner()->exec_file->ReadAt((char *)&(g_machine->mainMemory[g_machine->mmu->translationTable->getPhysicalPage(virtualPage)*g_cfg->PageSize]), g_cfg->PageSize, addrDisk);
  }
  g_machine->mmu->translationTable->clearBitIo(virtualPage);
  return ((ExceptionType)0);
}




