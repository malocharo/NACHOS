#include "userlib/libnachos.h"
#include "userlib/syscall.h"

SemId sema,sema1;
int i = 2;

VoidNoArgFunctionPtr func() {
    V(sema);
    P(sema1);
    i += 3;
    n_printf("i func = %d\n",i);
    n_printf("test\n");
}

VoidNoArgFunctionPtr func1() {
    P(sema);
    i *= 3;
    n_printf("i func1 = %d\n",i);
    V(sema1);
    n_printf("test1\n");
}

int main() {
	
    sema = SemCreate("semaTest",0);
    sema1 = SemCreate("semaTest1",0);
    threadCreate("test",func);
    threadCreate("test1",func1);
    SemDestroy(sema);
    SemDestroy(sema1);
    n_printf("i = %d\n",i);

    return 0;
}
