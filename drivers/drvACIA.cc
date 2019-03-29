/* \file drvACIA.cc
   \brief Routines of the ACIA device driver
//
//      The ACIA is an asynchronous device (requests return 
//      immediately, and an interrupt happens later on).  
//      This is a layer on top of the ACIA.
//      Two working modes are to be implemented in assignment 2:
//      a Busy Waiting mode and an Interrupt mode. The Busy Waiting
//      mode implements a synchronous IO whereas IOs are asynchronous
//      IOs are implemented in the Interrupt mode (see the Nachos
//      roadmap for further details).
//
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
//
*/

/* Includes */

#include "kernel/system.h"         // for the ACIA object
#include "kernel/synch.h"
#include "machine/ACIA.h"
#include "drivers/drvACIA.h"

//-------------------------------------------------------------------------
// DriverACIA::DriverACIA()
/*! Constructor.
  Initialize the ACIA driver. In the ACIA Interrupt mode, 
  initialize the reception index and semaphores and allow 
  reception and emission interrupts. 
  In the ACIA Busy Waiting mode, simply inittialize the ACIA 
  working mode and create the semaphore.
  */
//-------------------------------------------------------------------------

DriverACIA::DriverACIA()
{
  #ifndef ETUDIANTS_TP
  printf("**** Warning: contructor of the ACIA driver not implemented yet\n");
  exit(-1);
  #endif

  // TODO should rework on that, not sure about it anymore
  #ifdef ETUDIANTS_TP
  this->send_sema = new Semaphore((char*)"sending_sema_acia",1);
  this->receive_sema = new Semaphore((char*)"receiving_sema_acia",1);
  this->ind_send = 0;
  if(g_cfg->ACIA == ACIA_BUSY_WAITING)
    g_machine->acia->SetWorkingMode(BUSY_WAITING);
  else if(g_cfg->ACIA == ACIA_BUSY_WAITING)
    g_machine->acia->SetWorkingMode(SEND_INTERRUPT | REC_INTERRUPT);
  
  #endif
}

//-------------------------------------------------------------------------
// DriverACIA::TtySend(char* buff)
/*! Routine to send a message through the ACIA (Busy Waiting or Interrupt mode)
  */
//-------------------------------------------------------------------------

int DriverACIA::TtySend(char* buff)
{ 
  #ifndef ETUDIANTS_TP
  printf("**** Warning: method Tty_Send of the ACIA driver not implemented yet\n");
  exit(-1);
  return 0;
  #endif

  #ifdef ETUDIANTS_TP
  this->ind_send = 0;
  char c;
  this->send_sema->P();
  if(g_cfg->ACIA == ACIA_BUSY_WAITING){
    do
    {
      while(g_machine->acia->GetOutputStateReg() == FULL); // attente active sur le registre d'envoi
      c = buff[this->ind_send++];
      g_machine->acia->PutChar(c);

    } while ((c != '\0') && this->ind_send < BUFFER_SIZE);  
    this->send_sema->V();
  }
  else if(g_cfg->ACIA == ACIA_INTERRUPT){
    do
    {
      c = buff[this->ind_send];
      this->send_buffer[this->ind_send++] = c;

    } while ((c != '\0') && this->ind_send < BUFFER_SIZE);
    // since it works with interrupt we gotta send the first one and leave
    g_machine->acia->PutChar(this->send_buffer[0]); 
     // we really only send one char 
    this->ind_send = 1;
    g_machine->acia->SetWorkingMode(SEND_INTERRUPT);
    // However we don't release the semaphore here since we will still be sending
  }
  
  return this->ind_send;
  
  #endif
}

//-------------------------------------------------------------------------
// DriverACIA::TtyReceive(char* buff,int length)
/*! Routine to reveive a message through the ACIA 
//  (Busy Waiting and Interrupt mode).
  */
//-------------------------------------------------------------------------

int DriverACIA::TtyReceive(char* buff,int lg)
{
  #ifndef ETUDIANTS_TP
  printf("**** Warning: method Tty_Receive of the ACIA driver not implemented yet\n");
  exit(-1);
  return 0;
  #endif

  #ifdef ETUDIANTS_TP
  this->ind_rec = 0;
  char c;
  this->receive_sema->P();
  if(g_cfg->ACIA == ACIA_BUSY_WAITING) {
    do
    {
      while(g_machine->acia->GetInputStateReg() == EMPTY); // attente active
      c = g_machine->acia->GetChar();
      buff[this->ind_rec++] = c; 
    } while ((c != '\0') && this->ind_rec < lg && this->ind_rec <BUFFER_SIZE);
    this->receive_sema->V();
    return this->ind_rec;
    
  } else if(g_cfg->ACIA == ACIA_INTERRUPT) {
    int res = this->ind_rec;
    this->ind_rec = 0;
    memcpy(buff,this->receive_buffer,BUFFER_SIZE);
    g_machine->acia->SetWorkingMode(REC_INTERRUPT);
    // however we don't release the sema since we could still be receiving
    //this->ind_rec = 0; // this is more by safety not sure it's useful but at worst it's a redondant affectation
    
    return res;
  }
  
  #endif
}


//-------------------------------------------------------------------------
// DriverACIA::InterruptSend()
/*! Emission interrupt handler.
  Used in the ACIA Interrupt mode only. 
  Detects when it's the end of the message (if so, releases the send_sema semaphore), else sends the next character according to index ind_send.
  */
//-------------------------------------------------------------------------

void DriverACIA::InterruptSend()
{
  #ifndef ETUDIANTS_TP
  printf("**** Warning: send interrupt handler not implemented yet\n");
  exit(-1);
  #endif
  #ifdef ETUDIANTS_TP
  if (!this->send_buffer[this->ind_send]) {
    g_machine->acia->PutChar(this->send_buffer[this->ind_send]);
    g_machine->acia->SetWorkingMode(REC_INTERRUPT); 
    this->send_sema->V();
  }
  else
    g_machine->acia->PutChar(this->send_buffer[this->ind_send++]);
  return;
  #endif
  
}

//-------------------------------------------------------------------------
// DriverACIA::Interrupt_receive()
/*! Reception interrupt handler.
  Used in the ACIA Interrupt mode only. Reveices a character through the ACIA. 
  Releases the receive_sema semaphore and disables reception 
  interrupts when the last character of the message is received 
  (character '\0').
  */
//-------------------------------------------------------------------------

void DriverACIA::InterruptReceive()
{
  #ifndef ETUDIANTS_TP
  printf("**** Warning: receive interrupt handler not implemented yet\n");
  exit(-1);
  #endif
  #ifdef ETUDIANTS_TP
  char c;
  if(!(c == g_machine->acia->GetChar())) {
    this->receive_buffer[this->ind_rec] = c;
    g_machine->acia->SetWorkingMode(SEND_INTERRUPT); 
    this->receive_sema->V();
  }
  else
    this->receive_buffer[this->ind_rec++] = c;
  return;
  #endif
}