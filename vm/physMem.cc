//-----------------------------------------------------------------
/*! \file mem.cc
//  \brief Routines for the physical page management
*/
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//-----------------------------------------------------------------

#include <unistd.h>
#include "vm/physMem.h"

//-----------------------------------------------------------------
// PhysicalMemManager::PhysicalMemManager
//
/*! Constructor. It simply clears all the page flags and inserts them in the
// free_page_list to indicate that the physical pages are free
*/
//-----------------------------------------------------------------
PhysicalMemManager::PhysicalMemManager() {

  long i;

  tpr = new struct tpr_c[g_cfg->NumPhysPages];

  for (i=0;i<g_cfg->NumPhysPages;i++) {
    tpr[i].free=true;
    tpr[i].locked=false;
    tpr[i].owner=NULL;
    free_page_list.Append((void*)i);
  }
  i_clock=-1;
}

PhysicalMemManager::~PhysicalMemManager() {
  // Empty free page list
  int64_t page;
  while (!free_page_list.IsEmpty()) page =  (int64_t)free_page_list.Remove();

  // Delete physical page table
  delete[] tpr;
}

//-----------------------------------------------------------------
// PhysicalMemManager::RemovePhysicalToVitualMapping
//
/*! This method releases an unused physical page by clearing the
//  corresponding bit in the page_flags bitmap structure, and adding
//  it in the free_page_list.
//
//  \param num_page is the number of the real page to free
*/
//-----------------------------------------------------------------
void PhysicalMemManager::RemovePhysicalToVirtualMapping(long num_page) {
  
  // Check that the page is not already free 
  ASSERT(!tpr[num_page].free);

  // Update the physical page table entry
  tpr[num_page].free=true;
  tpr[num_page].locked=false;
  if (tpr[num_page].owner->translationTable!=NULL) 
    tpr[num_page].owner->translationTable->clearBitValid(tpr[num_page].virtualPage);

  // Insert the page in the free list
  free_page_list.Prepend((void*)num_page);
}

//-----------------------------------------------------------------
// PhysicalMemManager::UnlockPage
//
/*! This method unlocks the page numPage, after
//  checking the page is in the locked state. Used
//  by the page fault manager to unlock at the
//  end of a page fault (the page cannot be evicted until
//  the page fault handler terminates).
//
//  \param num_page is the number of the real page to unlock
*/
//-----------------------------------------------------------------
void PhysicalMemManager::UnlockPage(long num_page) {
  ASSERT(num_page<g_cfg->NumPhysPages);
  ASSERT(tpr[num_page].locked==true);
  ASSERT(tpr[num_page].free==false);
  tpr[num_page].locked = false;
}

//-----------------------------------------------------------------
// PhysicalMemManager::ChangeOwner
//
/*! Change the owner of a page
//
//  \param owner is a pointer on new owner (Thread *)
//  \param numPage is the concerned page
*/
//-----------------------------------------------------------------
void PhysicalMemManager::ChangeOwner(long numPage, Thread* owner) {
  // Update statistics
  g_current_thread->GetProcessOwner()->stat->incrMemoryAccess();
  // Change the page owner
  tpr[numPage].owner = owner->GetProcessOwner()->addrspace;
}

//-----------------------------------------------------------------
// PhysicalMemManager::AddPhysicalToVirtualMapping 
//
/*! This method returns a new physical page number. If there is no
//  page available, it evicts one page (page replacement algorithm).
//
//  NB: this method locks the newly allocated physical page such that
//      it is not stolen during the page fault resolution. Don't forget
//      to unlock it
//
//  \param owner address space (for backlink)
//  \param virtualPage is the number of virtualPage to link with physical page
//  \return A new physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::AddPhysicalToVirtualMapping(AddrSpace* owner,int virtualPage) 
{
 #ifndef ETUDIANTS_TP
  printf("**** Warning: function AddPhysicalToVirtualMapping is not implemented\n");
  exit(-1);
  return (0);
  #endif
  int pp = this->FindFreePage();
  if(pp == -1) // no free page
    pp = this->EvictPage(); // we need a new one 
  DEBUG('v',(char*)"AddPhysicalToVirtualMapping: physical page %d\n",pp);
  // we set the physical page
  this->tpr[pp].locked = true;
  this->tpr[pp].virtualPage = virtualPage;
  this->tpr[pp].owner = owner;
  this->tpr[pp].free = false;

  g_machine->mmu->translationTable->setPhysicalPage(virtualPage,pp);
  g_machine->mmu->translationTable->setBitValid(virtualPage);
  this->UnlockPage(pp);
  DEBUG('v',(char*)"State of the translation table for virtual page %d phys page %d\n",virtualPage,pp);
  DEBUG('v',(char*)"addrdisk = %d bitReadAllowed = %d bitWriteAllowed = %d  bitSwap %d \n"
  ,g_machine->mmu->translationTable->getAddrDisk(virtualPage)
  ,g_machine->mmu->translationTable->getBitReadAllowed(virtualPage)
  ,g_machine->mmu->translationTable->getBitWriteAllowed(virtualPage)
  ,g_machine->mmu->translationTable->getBitSwap(virtualPage));
  return pp;



}

//-----------------------------------------------------------------
// PhysicalMemManager::FindFreePage
//
/*! This method returns a new physical page number, if it finds one
//  free. If not, return -1. Does not run the clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::FindFreePage() {
  int64_t page;

  // Check that the free list is not empty
  if (free_page_list.IsEmpty())
    return -1;

  // Update statistics
  g_current_thread->GetProcessOwner()->stat->incrMemoryAccess();
  
  // Get a page from the free list
  page = (int64_t)free_page_list.Remove();
  
  // Check that the page is really free
  ASSERT(tpr[page].free);
  
  // Update the physical page table
  tpr[page].free = false;

  return page;
}

//-----------------------------------------------------------------
// PhysicalMemManager::EvictPage
//
/*! This method implements page replacement, using the well-known
//  clock algorithm.
//
//  \return A new free physical page number.
*/
//-----------------------------------------------------------------
int PhysicalMemManager::EvictPage() {
  #ifndef ETUDIANTS_TP
  printf("**** Warning: page replacement algorithm is not implemented yet\n");
    exit(-1);
    return (0);
  #endif

  #ifdef ETUDIANTS_TP
  // https://en.wikipedia.org/wiki/Page_replacement_algorithm#Clock

  //we save the value of the 'clock' iterator localy

  int iClock = this->i_clock;
  int iClockInit = iClock;

  // we look for the bit U of every page, if it's equal to 0 we take it
  // if it's equal to 1 we set it to 0
  // if we went through every pages without fiding a bit U == 0 we yield the thread
  // and comeback later

  while(this->tpr[iClock].owner->translationTable->getBitU(this->tpr[iClock].virtualPage)){
    // bit U == 1,we set to 0
    this->tpr[iClock].owner->translationTable->setBitU(this->tpr[iClock].virtualPage);
    
    if((iClock = iClock % g_cfg->NumPhysPages) == iClockInit)
    //we went through every pages, didn't anything
      g_current_thread->Yield();

  }

  // we found a page ! we set it to invalid and lock it
  this->tpr[iClock].owner->translationTable->clearBitValid(this->tpr[iClock].virtualPage);
  this->tpr[iClock].locked = true;

  // we are now looking at the state of the page in the swap
  int bitSwap = this->tpr[iClock].owner->translationTable->getBitSwap(this->tpr[iClock].virtualPage);
  int bitM    = this->tpr[iClock].owner->translationTable->getBitM(this->tpr[iClock].virtualPage);

  // if both are true it means the page was modified and is present in the swap already
  // so we need to 'update' the swap
  if(bitSwap && bitM)
    g_swap_manager->PutPageSwap(this->tpr[iClock].owner->translationTable->getAddrDisk(this->tpr[iClock].virtualPage),
    (char *)&g_machine->mainMemory[iClock * g_cfg->PageSize]);
  else
  {
    // we put the page in the swap
    // -1 means first free sector
    int addrDisk = g_swap_manager->PutPageSwap(-1,(char *)&g_machine->mainMemory[iClock * g_cfg->PageSize]);
    this->tpr[iClock].owner->translationTable->setAddrDisk(this->tpr[iClock].virtualPage,addrDisk);
    this->tpr[iClock].owner->translationTable->setBitSwap(this->tpr[iClock].virtualPage);
    this->tpr[iClock].owner->translationTable->clearBitM(this->tpr[iClock].virtualPage);
  }
  
  this->i_clock = iClock;

  return iClock;
  #endif
  

  

  
}

//-----------------------------------------------------------------
// PhysicalMemManager::Print
//
/*! print the current status of the table of physical pages
//
//  \param rpage number of real page
*/
//-----------------------------------------------------------------

void PhysicalMemManager::Print(void) {
  int i;

  printf("Contents of TPR (%d pages)\n",g_cfg->NumPhysPages);
  for (i=0;i<g_cfg->NumPhysPages;i++) {
    printf("Page %d free=%d locked=%d virtpage=%d owner=%lx U=%d M=%d\n",
	   i,
	   tpr[i].free,
	   tpr[i].locked,
	   tpr[i].virtualPage,
	   (long int)tpr[i].owner,
	   (tpr[i].owner!=NULL) ? tpr[i].owner->translationTable->getBitU(tpr[i].virtualPage) : 0,
	   (tpr[i].owner!=NULL) ? tpr[i].owner->translationTable->getBitM(tpr[i].virtualPage) : 0);
  }
}
