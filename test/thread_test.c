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
	
    threadCreate("test",func,0);
    return 0;
}
