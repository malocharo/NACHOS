#include "userlib/syscall.h"
#include "userlib/libnachos.h"
/*LockId lock;

VoidNoArgFunctionPtr compute(){
  int i;
  LockAcquire(lock); 
  for (i = 0; i < 5; i++) 
    n_printf("Test des locks %d\n", i);
  LockRelease(lock);
  return 0;
}


VoidNoArgFunctionPtr computeNoLock(){
  int i = 0;
  for (i = 0; i < 5; i++) 
    n_printf("Test sans locks %d\n", i);
 return 0;
}
// pour l'instant la stack n'est pas assez grande(issou), donc commentez un bloc pour voir la demo d'un puis l'autre
// compute doit afficher 0 1 2 3 4 0 1 2 3 4
// computeNoLock doit affiche 0 0 1 1 2 2 3 3 4 4
// si on LockDestroy(lock) dans le main, le lock n'existe plus, car le thread "main" se termine sans attendre ses enfants (et pas d'interupt)
int main() {
  int i;
  lock = LockCreate((char *)"Lock ");
  for ( i = 0; i < 2; i++) 
    threadCreate((char*)"process " + i, (VoidNoArgFunctionPtr) &compute,0);

  //for (i = 0; i < 2; i++) 
    //threadCreate((char*)"process " + i, (VoidNoArgFunctionPtr) &computeNoLock,0);

  return 0;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
SemId sema1;
SemId sema2;

VoidNoArgFunctionPtr testSema1(){
  //P(sema1);
  n_printf("Test de wait sema1 \n");
  //V(sema2);
  n_printf("Wait fini sema1 \n");
  return 0;
}

VoidNoArgFunctionPtr testSema2(){
  //V(sema1);
  n_printf("Test de wait sema 2 \n");
  //P(sema2);
  n_printf("Wait fini sema2 \n");
  return 0;
}
// test classique de RDV le test doit afficher :
//Test de wait sema 2 
//Test de wait sema1 
//Wait finis sema1 
//Wait finis sema2 
//dans cet ordre
// si destruction des semaphore alors ça ne fonctionne pas car le thread parent s'execute jusqu'au bout
int main() {
  n_printf("Debut du programme de test \n");

 sema1 = SemCreate((char *)"sema1", 0);
 sema2 = SemCreate((char *)"sema2", 0);

  threadCreate((char *)"process1", (VoidNoArgFunctionPtr)&testSema1,0);
  threadCreate((char *)"process2", (VoidNoArgFunctionPtr)&testSema2,0);

  return 0;
}
*/

BarId bar;


VoidNoArgFunctionPtr testBar1() {
  BarrierReach(bar);
  n_printf("thread 3 : should be print in 2nd or 3 position\n");
  return 0;
}
VoidNoArgFunctionPtr testBar2() {

  BarrierReach(bar);
  n_printf("thread 1 : should be print in 2nd or not if the barrier is not release\n");
  
  return 0;
}

VoidNoArgFunctionPtr testBar3() {
  
  n_printf("thread 2 : should be print in 1 position no matter what\n");
  BarrierReach(bar);
  
  return 0;
}

int main() {
  n_printf("Debut du programme de test \n");
  bar = BarCreate((char*)"barrier 1",2);

  threadCreate((char *)"process1", (VoidNoArgFunctionPtr)&testBar2,0);
  threadCreate((char *)"process2", (VoidNoArgFunctionPtr)&testBar3,0);
  //threadCreate((char *)"process3", (VoidNoArgFunctionPtr)&testBar1,0);

  return 0;

}

