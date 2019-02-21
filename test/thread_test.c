#include "userlib/libnachos.h"
#include "userlib/syscall.h"

SemId sema,sema1;
int i = 0;

/*void func() {
    while(1) {
    V(sema);
    P(sema1);
    i += 3;
    n_printf("i func = %d\n",i);
    n_printf("test\n");
    }
}

void func1() {
    while(1){
    P(sema);
    i *= 3;
    n_printf("i func1 = %d\n",i);
    V(sema1);
    n_printf("test1\n");
    }
}*/

LockId lock;
void bcl(){
    LockAcquire(lock);
    n_printf(" valeur de i = %d pour deb thread blc\n",i);
    while(i<10){
        
        n_printf("blc\n");
        i++;
    }
    n_printf(" valeur de i = %d pour fin thread blc\n",i);
    LockRelease(lock);
}

void bcl1(){
    LockAcquire(lock);
    n_printf(" valeur de i = %d pour deb thread blc1\n",i);
    while(i<15){
        n_printf("blc1\n");
        i++;
    }
    LockRelease(lock);
    n_printf(" valeur de i = %d pour fin thread blc1\n",i);
}

int main() {
	
    //threadCreate("test",func,0);
   //threadCreate("test2",func1,0);
    ThreadId t  = threadCreate("test_boucle" ,bcl ,0);
    ThreadId t1 = threadCreate("test_boucle2",bcl1,0);
    lock = LockCreate("lock for incr");
    LockDestroy(lock);
    
    
    return 0;
}
