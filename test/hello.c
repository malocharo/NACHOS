/* hello.c
 *	Simple hello world program
 *
//  Copyright (c) 1999-2000 INSA de Rennes.
//  All rights reserved.  
//  See copyright_insa.h for copyright notice and limitation 
//  of liability and disclaimer of warranty provisions.
 */

#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int
main(int argc, char**argv)
{
  n_printf("** ** ** Bonjour le monde ** ** **\n");
  n_printf("argc = %d\n",argc);
  n_printf("argv[0] =  %s",argv[0]);
  Exit(0);
  
}
